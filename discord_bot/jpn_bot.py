import discord
from discord.ext import commands
from dotenv import load_dotenv
import logging
import os
import subprocess
import asyncio
import sys

# Store active game sessions per user
active_games = {}

load_dotenv()
token = os.getenv('DISCORD_TOKEN')

handler = logging.FileHandler(filename='discord.log', encoding='utf-8', mode='w')
intents = discord.Intents.default()
intents.dm_messages = True
intents.members = True
intents.messages = True
intents.message_content = True

bot = commands.Bot(command_prefix='!', intents=intents)

@bot.event
async def on_ready():
    print(f"We are ready to go in, {bot.user.name}")

@bot.event
async def on_message(message):
    # Ignore bot's own messages
    if message.author == bot.user:
        return
    
    # Process commands first
    await bot.process_commands(message)
    
    # Check if this was a command - if so, don't treat it as game input
    ctx = await bot.get_context(message)
    if ctx.valid:
        return
    
    # Check if user has an active game session and message is a DM
    if message.author.id in active_games and isinstance(message.channel, discord.DMChannel):
        process = active_games[message.author.id]
        
        # Check if process is still alive
        if process.poll() is not None:
            await message.author.send("Game has ended. Type !play to start a new game.")
            del active_games[message.author.id]
            return
        
        # Send user input to the C program
        try:
            print(f"Sending to game: {message.content}")  # Debug
            process.stdin.write(message.content + '\n')
            process.stdin.flush()
            
            # Small delay to let the program process the input
            await asyncio.sleep(0.2)
            
            # Read the response from the game
            # Use longer timeout for responses that might include countdowns
            await read_game_output(process, message.author, initial_timeout=5.0, line_timeout=1.5)
            
        except BrokenPipeError:
            await message.author.send("Game connection lost. Type !play to start a new game.")
            if message.author.id in active_games:
                del active_games[message.author.id]
        except Exception as e:
            print(f"Error in on_message: {e}")  # Debug
            await message.author.send(f"Error communicating with game: {e}")
            # Clean up the dead process
            if message.author.id in active_games:
                try:
                    active_games[message.author.id].terminate()
                except:
                    pass
                del active_games[message.author.id]

async def read_game_output(process, user, initial_timeout=3.0, line_timeout=1.5):
    """Read output from the game process and send to user"""
    try:
        output_lines = []
        loop = asyncio.get_event_loop()
        
        # Wait longer for the first line
        first_line_timeout = initial_timeout
        consecutive_timeouts = 0
        max_consecutive_timeouts = 2  # Allow 2 consecutive timeouts before stopping
        
        while True:
            try:
                # Use longer timeout for first line, shorter for subsequent lines
                timeout = first_line_timeout if not output_lines else line_timeout
                
                # Read line with timeout
                line = await asyncio.wait_for(
                    loop.run_in_executor(None, process.stdout.readline),
                    timeout=timeout
                )
                
                if line == "":
                    # Process ended
                    if output_lines:
                        break
                    else:
                        return
                
                line = line.strip()
                if line:
                    output_lines.append(line)
                    consecutive_timeouts = 0  # Reset timeout counter
                    print(f"Read line: {line}")  # Debug
                    
            except asyncio.TimeoutError:
                consecutive_timeouts += 1
                print(f"Timeout {consecutive_timeouts}/{max_consecutive_timeouts} - collected {len(output_lines)} lines")  # Debug
                
                # If we've had multiple consecutive timeouts and have some output, send it
                if consecutive_timeouts >= max_consecutive_timeouts and output_lines:
                    break
                # If we've had timeouts but no output yet, keep waiting a bit more
                elif consecutive_timeouts >= max_consecutive_timeouts:
                    break
        
        # Send all collected output to user as one message
        if output_lines:
            # Discord has a 2000 character limit, so split if needed
            message_text = '\n'.join(output_lines)
            
            # Split into chunks if message is too long
            if len(message_text) <= 2000:
                await user.send(message_text)
                print(f"Sent {len(output_lines)} lines to user")  # Debug
            else:
                # Split into multiple messages
                chunks = []
                current_chunk = []
                current_length = 0
                
                for line in output_lines:
                    if current_length + len(line) + 1 > 2000:
                        chunks.append('\n'.join(current_chunk))
                        current_chunk = [line]
                        current_length = len(line)
                    else:
                        current_chunk.append(line)
                        current_length += len(line) + 1
                
                if current_chunk:
                    chunks.append('\n'.join(current_chunk))
                
                for chunk in chunks:
                    await user.send(chunk)
                    await asyncio.sleep(0.5)  # Small delay between messages
                
                print(f"Sent {len(chunks)} message chunks to user")  # Debug
            
    except Exception as e:
        print(f"Error in read_game_output: {e}")  # Debug
        import traceback
        traceback.print_exc()

@bot.command()
async def hello(ctx):
    await ctx.author.send(f"Hello {ctx.author.mention}!")

@bot.command()
async def play(ctx):
    # Check if user already has an active game
    if ctx.author.id in active_games:
        await ctx.author.send("You already have an active game! Type `!stop` to end it first.")
        return
    
    # Path to the bot script folder
    script_dir = os.path.dirname(os.path.abspath(__file__))

    # Go up one folder (to project root), then to src/output/main.exe
    exe_path = os.path.join(script_dir, "..", "src", "output", "main.exe")
    exe_path = os.path.abspath(exe_path)
    
    print(f"Looking for game at: {exe_path}")  # Debug

    try:
        # Set encoding based on platform
        encoding = 'utf-8' if sys.platform != 'win32' else 'cp1252'
        
        # Run the compiled C program
        process = subprocess.Popen(
            exe_path, 
            stdin=subprocess.PIPE, 
            stdout=subprocess.PIPE, 
            stderr=subprocess.PIPE, 
            encoding=encoding,
            errors='replace',  # Replace encoding errors instead of crashing
            text=True,
            bufsize=0,  # Unbuffered
            universal_newlines=True
        )
        
        print(f"Process started with PID: {process.pid}")  # Debug
        
        # Store the process for this user
        active_games[ctx.author.id] = process
        
        await ctx.author.send("Starting Japanese reading game! 🎮")
        
        # Small delay to let process initialize
        await asyncio.sleep(0.5)
        
        # Read initial game output with longer timeout to handle countdown
        # The countdown takes ~3 seconds (3...2...1) so we need longer timeouts
        await read_game_output(process, ctx.author, initial_timeout=5.0, line_timeout=1.5)
        
    except FileNotFoundError:
        await ctx.author.send(f"❌ Error: Could not find game executable at {exe_path}")
        print(f"File not found: {exe_path}")  # Debug
    except Exception as e:
        await ctx.author.send(f"❌ Error starting game: {e}")
        print(f"Error in play command: {e}")  # Debug
        import traceback
        traceback.print_exc()

@bot.command()
async def stop(ctx):
    """Stop the current game session"""
    if ctx.author.id in active_games:
        process = active_games[ctx.author.id]
        try:
            process.terminate()
            process.wait(timeout=2)
        except:
            process.kill()
        del active_games[message.author.id]
        await ctx.author.send("Game stopped! 🛑")
    else:
        await ctx.author.send("You don't have an active game.")

bot.run(token, log_handler=handler, log_level=logging.DEBUG)
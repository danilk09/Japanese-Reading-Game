import discord
from discord.ext import commands
from dotenv import load_dotenv
import logging
import os
import asyncio
import sys

sys.stdout.reconfigure(encoding='utf-8')
load_dotenv()
token = os.getenv('DISCORD_TOKEN')

active_games = {}

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
    if message.author == bot.user:
        return

    await bot.process_commands(message)

    ctx = await bot.get_context(message)
    if ctx.valid:
        return

    if message.author.id in active_games and isinstance(message.channel, discord.DMChannel):
        game = active_games[message.author.id]
        process = game['process']

        if process.returncode is not None:
            await message.author.send("Game has ended. Type !play to start a new game.")
            del active_games[message.author.id]
            return

        if game['waiting_for_output']:
            await message.author.send("⏳ Still waiting for the game to respond, please wait...")
            return

        game['waiting_for_output'] = True
        try:
            print(f"Sending to game: {message.content}")
            process.stdin.write((message.content + '\n').encode('utf-8'))
            await process.stdin.drain()

            received = await read_game_output(game['stdout_reader'], message.author, first_line_timeout=3.0, next_line_timeout=0.05)
            await check_and_cleanup(message.author.id, message.author)
            if not received and message.author.id in active_games:
                await message.author.send("⚠️ No response from game. The game may have ended.")

        except Exception as e:
            print(f"Error in on_message: {e}")
            await message.author.send(f"Error communicating with game: {e}")
            try:
                process.kill()
            except:
                pass
            del active_games[message.author.id]
        finally:
            if message.author.id in active_games:
                active_games[message.author.id]['waiting_for_output'] = False

async def check_and_cleanup(user_id, user):
    """Check if the game process has ended and clean up if so."""
    if user_id not in active_games:
        return
    process = active_games[user_id]['process']
    if process.returncode is not None:
        del active_games[user_id]
        await user.send("Game over! Thanks for playing. Type `!play` to start a new game. 🎮")

async def read_game_output(reader, user, first_line_timeout=3.0, next_line_timeout=0.3):
    """Read lines from asyncio StreamReader until idle, then send to user."""
    output_lines = []

    try:
        while True:
            timeout = first_line_timeout if not output_lines else next_line_timeout
            try:
                line = await asyncio.wait_for(reader.readline(), timeout=timeout)
                if not line:
                    break
                decoded = line.decode('utf-8', errors='replace').strip()
                if decoded:
                    output_lines.append(decoded)
                    print(f"Read line: {decoded}")
            except asyncio.TimeoutError:
                # No more output coming — break cleanly without consuming future data
                break

    except Exception as e:
        print(f"Error reading output: {e}")

    if output_lines:
        message_text = '\n'.join(output_lines)
        if len(message_text) <= 2000:
            await user.send(message_text)
        else:
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
                await asyncio.sleep(0.1)

        print(f"Sent {len(output_lines)} lines to user")
        return True

    return False


@bot.command()
async def hello(ctx):
    await ctx.author.send(f"Hello {ctx.author.mention}!")


@bot.command()
async def play(ctx):
    if ctx.author.id in active_games:
        await ctx.author.send("You already have an active game! Type `!stop` to end it first.")
        return

    script_dir = os.path.dirname(os.path.abspath(__file__))
    root_dir = os.path.abspath(os.path.join(script_dir, ".."))

    if sys.platform == "win32":
        exe_path = os.path.join(root_dir, "src", "output", "main.exe")
    else:
        exe_path = os.path.join(root_dir, "src", "output", "main")
    print(f"Looking for game at: {exe_path}")

    try:
        # Use asyncio.subprocess instead of subprocess.Popen
        process = await asyncio.create_subprocess_exec(
            exe_path,
            stdin=asyncio.subprocess.PIPE,
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE,
        )

        print(f"Process started with PID: {process.pid}")

        active_games[ctx.author.id] = {
            'process': process,
            'stdout_reader': process.stdout,  # This is a native asyncio StreamReader
            'waiting_for_output': False
        }

        await ctx.author.send("Starting Japanese reading game! 🎮")

        active_games[ctx.author.id]['waiting_for_output'] = True
        await read_game_output(process.stdout, ctx.author, first_line_timeout=2.0, next_line_timeout=0.05)
        await check_and_cleanup(ctx.author.id, ctx.author)
        active_games[ctx.author.id]['waiting_for_output'] = False

    except FileNotFoundError:
        await ctx.author.send(f"❌ Error: Could not find game executable at {exe_path}")
    except Exception as e:
        await ctx.author.send(f"❌ Error starting game: {e}")
        import traceback
        traceback.print_exc()


@bot.command()
async def stop(ctx):
    if ctx.author.id in active_games:
        process = active_games[ctx.author.id]['process']
        try:
            process.kill()
            await process.wait()
        except:
            pass
        del active_games[ctx.author.id]
        await ctx.author.send("Game stopped! 🛑")
    else:
        await ctx.author.send("You don't have an active game.")


bot.run(token, log_handler=handler, log_level=logging.DEBUG)
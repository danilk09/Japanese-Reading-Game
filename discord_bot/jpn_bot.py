import discord
from discord.ext import commands
from dotenv import load_dotenv
import logging
import os
import subprocess

load_dotenv()
token = os.getenv('DISCORD_TOKEN')

handler = logging.FileHandler(filename='discord.log', encoding='utf-8', mode='w')
intents = discord.Intents.default()
intents.dm_messages = True
intents.members = True #might not need
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

@bot.command()
async def hello(ctx):
    await ctx.author.send(f"Hello {ctx.author.mention}!")

@bot.command()
async def play(ctx):
    dm = await ctx.author.create_dm()
    await dm.send("Game starting!")

    script_dir = os.path.dirname(os.path.abspath(__file__))
    # Go up one folder (to project root), then to src/output/main.exe
    exe_path = os.path.join(script_dir, "..", "src", "output", "main.exe")
    exe_path = os.path.abspath(exe_path)

    process = subprocess.Popen(exe_path, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    
    async def game_loop():
        while True:
            print("Got here")
            line = process.stdout.readline()
            if not line:
                break
            await dm.send(line.strip())

    bot.loop.create_task(game_loop())


bot.run(token, log_handler=handler, log_level=logging.DEBUG)
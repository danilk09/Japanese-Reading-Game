# Japanese Reading Game

A speed-based quiz game for practicing hiragana and katakana. Answer questions as fast as possible to maximize your score — playable from the terminal or via a Discord bot.

## How It Works

Each game consists of 10 questions. A Japanese character or word is displayed and you type the romaji equivalent. Points are awarded based on how quickly you answer:

| Time | Points |
|------|--------|
| ≤ 2 seconds | 20 |
| 3–12 seconds | 20 − (seconds − 2) × 2 |
| > 12 seconds | 1 |
| Wrong answer | 0 |

Maximum possible score: **200 points**.

## Game Modes

| Mode | Input |
|------|-------|
| Hiragana Characters | `hiragana characters` |
| Hiragana Words | `hiragana words` |
| Katakana Characters | `katakana characters` |
| Katakana Words | `katakana words` |
| Both Characters | `both characters` |
| Both Words | `both words` |

## Project Structure

```
jpn_reading_game/
├── src/                  # C game source
│   ├── main.c
│   ├── functions.c / functions.h
│   └── node.h
├── storage/              # SQLite databases
│   ├── hiragana.txt / hiragana_sql.c
│   ├── katakana.txt / katakana_sql.c
│   └── output/           # Compiled .db files
├── discord_bot/
│   └── jpn_bot.py        # Discord bot wrapper
├── Dockerfile
└── requirements.txt
```

## Running Locally

### Build the C game

```bash
mkdir -p src/output
gcc src/main.c src/functions.c -I src -o src/output/main -lsqlite3
```

### Play in the terminal

```bash
./src/output/main
```

### Run the Discord bot

1. Create a `.env` file in `discord_bot/`:
   ```
   DISCORD_TOKEN=your_token_here
   ```
2. Install dependencies:
   ```bash
   pip install -r requirements.txt
   ```
3. Start the bot:
   ```bash
   python discord_bot/jpn_bot.py
   ```

## Discord Commands

| Command | Description |
|---------|-------------|
| `!play` | Start a new game in your DMs |
| `!stop` | End your current game |
| `!hello` | Test that the bot is online |

## Deployment (Railway)

The bot is hosted on [Railway](https://railway.app) and runs continuously. Railway builds the project using the `Dockerfile`, which compiles the C game and starts the Discord bot automatically.

### Setup

1. Create a Railway project and link the repository.
2. Set the `DISCORD_TOKEN` environment variable in the Railway service settings.
3. Deploy — Railway will build from the `Dockerfile` and keep the bot running.

### How the Dockerfile works

```dockerfile
# Installs gcc and libsqlite3-dev, then compiles the C game:
gcc src/main.c src/functions.c -I src -o src/output/main -lsqlite3

# Then starts the Discord bot:
python discord_bot/jpn_bot.py
```

## Dependencies

- **C**: gcc, libsqlite3-dev
- **Python**: discord.py, python-dotenv

# Japanese Reading Game

A speed-based quiz game for practicing hiragana and katakana. Answer questions as fast as possible to maximize your score — playable from the terminal or via a Discord bot.

> **Deployment:** The bot runs continuously on [Railway](https://railway.app) via the `railway` branch. This branch (`main`) is for local development and testing.

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

1. Create `discord_bot/.env`:
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

## Dependencies

- **C**: gcc, libsqlite3-dev
- **Python**: discord.py, python-dotenv

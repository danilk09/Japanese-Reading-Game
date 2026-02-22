FROM python:3.11-slim

# Install gcc and sqlite3 dev headers
RUN apt-get update && apt-get install -y \
    gcc \
    libsqlite3-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy everything
COPY . .

# Install Python dependencies
RUN pip install -r requirements.txt

# Compile the C game
RUN mkdir -p src/output && \
    gcc src/main.c src/functions.c -I src -o src/output/main -lsqlite3 && \
    chmod +x src/output/main

CMD ["python", "discord_bot/jpn_bot.py"]
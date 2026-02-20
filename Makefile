CC = gcc
TARGET = src/output/main
SRCS = src/main.c src/functions.c
LIBS = -lsqlite3

all:
	mkdir -p src/output
	$(CC) $(SRCS) -o $(TARGET) $(LIBS)
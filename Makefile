CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic
LDFLAGS = -lm

TARGET = whisky

all:
	$(CC) $(CFLAGS) whisky.c -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
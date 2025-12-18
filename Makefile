CC = gcc
CFLAGS = -Wall -std=c99
TARGET = scheduler

all: $(TARGET)

$(TARGET): scheduler.c
	$(CC) $(CFLAGS) -o $(TARGET) scheduler.c

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean

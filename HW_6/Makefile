CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = phonebook

SRCS = list.c main.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
%.o: %.c list.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)


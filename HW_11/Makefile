CC = gcc
CFLAGS = -Wall -pthread
LIBS = -pthread

all: program

program: main.c fun.c fun.h
	$(CC) $(CFLAGS) main.c fun.c -o program $(LIBS)

clean:
	rm -f program

CC = gcc
CFLAGS = -Wall -pthread
LIBS = -lrt -lncurses -pthread

all: recv send
	$(CC) $(CFLAGS) -o recv recv.c && $(CC) $(CFLAGS) -o send send.c

clean:
	rm -f recv send

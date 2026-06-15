CC = gcc
CFLAGS = -Wall -pthread
LIBS = -lrt -lncurses -pthread

all: recv send

recv: recv.c
	$(CC) $(CFLAGS) -o recv recv.c $(LIBS)

send: send.c
	$(CC) $(CFLAGS) -o send send.c $(LIBS)

clean:
	rm -f recv send

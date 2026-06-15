CC = gcc
CFLAGS = -Wall -pthread
LIB = -pthread

all: client server

client: client.c
	$(CC) $(CFLAGS) -o client client.c $(LIB)

server: server.c
	$(CC) $(CFLAGS) -o server server.c $(LIB)

clean:
	rm -f client server

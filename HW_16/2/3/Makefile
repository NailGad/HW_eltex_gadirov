CC = gcc
CFLAGS = -Wall -pthread
LIB = -pthread

all: client server

client: client.c
	$(CC) $(CFLAGS) -o client client.c $(LIB)

server: server_dir/main.c server_dir/fun.c server_dir/fun.h
	$(CC) $(CFLAGS)  server_dir/main.c server_dir/fun.c -o server $(LIB)

clean:
	rm -f client server

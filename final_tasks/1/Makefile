CC = gcc
CFLAGS = -Wall -Iinclude
SRC_DIR = src
FUN_DIR = fun
INC_DIR = include

all: server client

server: $(SRC_DIR)/server.c $(FUN_DIR)/server_fun.c $(FUN_DIR)/common.c
	$(CC) $(CFLAGS) -o server $^

client: $(SRC_DIR)/client.c $(FUN_DIR)/client_fun.c $(FUN_DIR)/common.c
	$(CC) $(CFLAGS) -o client $^

clean:
	rm -f server client

run-server: server
	sudo ./server

run-client: client
	./client
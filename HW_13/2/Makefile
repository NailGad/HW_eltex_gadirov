CC = gcc
CFLAGS = -Wall -Wextra 
LIBS_S = -lrt -pthread
LIBS_C = -lrt -lncurses -pthread

all: server_run client_run

server_run: server/server.c server/server_fun.h server/server_fun.c
	$(CC) $(CFLAGS) server/server.c server/server_fun.c -o server_run $(LIBS_S)

client_run: client/client.c client/client_fun.c client/client_fun.h
	$(CC) $(CFLAGS) client/client.c client/client_fun.c -o client_run $(LIBS_C)
	
clean:
	rm -f server_run client_run chat_history.txt

rserver: server_run
	./server_run

rclient: client_run
	./client_run
CC = gcc
CFLAGS = -Wall -pthread
LIBS = -lrt -lncurses -pthread

all: server_run client_run

server_run: server/server.c server/server_fun.c include/server_fun.h include/shared.h
	$(CC) $(CFLAGS) server/server.c server/server_fun.c -o server_run $(LIBS)

client_run: client/client.c client/client_fun.c include/client_fun.h include/shared.h
	$(CC) $(CFLAGS) client/client.c client/client_fun.c -o client_run $(LIBS)

clean:
	rm -f server_run client_run

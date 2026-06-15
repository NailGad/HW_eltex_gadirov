CC = gcc
CFLAGS = -Wall -pthread
LIB = -pthread

all: udp_client tcp_client 

tcp_client: tcp_client.c
	$(CC) $(CFLAGS) -o tcp_client tcp_client.c $(LIB)

udp_client: udp_client.c
	$(CC) $(CFLAGS) -o udp_client udp_client.c $(LIB)

select_server: servers/select_server.c
	$(CC) $(CFLAGS)  servers/select_server.c -o select_server $(LIB)

poll_server: servers/poll_server.c
	$(CC) $(CFLAGS)  servers/poll_server.c -o poll_server $(LIB)

epoll_server: servers/epoll_server.c
	$(CC) $(CFLAGS)  servers/epoll_server.c -o epoll_server $(LIB)

clean:
	rm -f tcp_client udp_client select_server poll_server epoll_server

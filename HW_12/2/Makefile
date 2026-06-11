CC = gcc
CFLAGS = -Wall -Wextra -Werror
TARGET1 = server
TARGET2 = client

$(TARGET1): server.c
	$(CC) $(CFLAGS) -o $(TARGET1) server.c
$(TARGET2): client.c
	$(CC) $(CFLAGS) -o $(TARGET2) client.c
clean:
	rm $(TARGET1) $(TARGET2)
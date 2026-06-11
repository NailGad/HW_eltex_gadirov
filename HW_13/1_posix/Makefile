CC = gcc
CFLAGS = -Wall -Wextra -Werror
TARGET1 = server
TARGET2 = client

$(TARGET1): server.c
	$(CC) $(CFLAGS) -o $(TARGET1) server.c -lrt
$(TARGET2): client.c
	$(CC) $(CFLAGS) -o $(TARGET2) client.c -lrt
clean:
	rm $(TARGET1) $(TARGET2)
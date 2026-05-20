CC = gcc
CFLAGS = -Wall -Wextra -Werror
TARGET = program

$(TARGET): 1.c
	$(CC) $(CFLAGS) -o $(TARGET) 1.c
run: $(TARGET)
	./$(TARGET)
clean:
	rm $(TARGET)
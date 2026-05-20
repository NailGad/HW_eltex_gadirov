CC = gcc
CFLAGS = -Wall -Wextra -Werror
TARGET = program

$(TARGET): main.c func.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c func.c

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
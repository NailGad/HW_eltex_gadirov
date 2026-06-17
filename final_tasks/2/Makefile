CC = gcc
CFLAGS = -Wall -Iinclude
LDFLAGS = -lrt -pthread

SRC_DIR = src
INC_DIR = include
TARGET = taxi

SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/driver.c $(SRC_DIR)/manager.c
HEADERS = $(INC_DIR)/common.h $(INC_DIR)/driver.h $(INC_DIR)/manager.h

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
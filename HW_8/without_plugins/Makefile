CC = gcc
TARGET = libmath.so
PROG = program
OBJS = add.o sub.o mul.o div.o

$(TARGET): $(OBJS)
	$(CC) -shared $^ -o $@

%.o: %.c
	$(CC) -c -fPIC $< -o $@

main.o: main.c
	$(CC) -c main.c -o main.o

$(PROG): main.o $(TARGET)
	$(CC) main.o -L. -lmath -o $(PROG)

clean:
	rm -f *.o *.so $(PROG)

run: $(PROG)
	LD_LIBRARY_PATH=. ./$(PROG)
CC = gcc
TARGET = libmath.so
PROG = program
SRCS = $(wildcard plugins/*.c)

# создания динамической библиотеки
$(TARGET): $(SRCS)
	$(CC) -shared -fPIC $^ -o $@

#компиляция программы с динамической загрузкой (через dlopen)
$(PROG): main.c
	$(CC) main.c -o $(PROG) -ldl

clean:
	rm -f $(TARGET) $(PROG)

run: $(PROG)
	LD_LIBRARY_PATH=. ./$(PROG)

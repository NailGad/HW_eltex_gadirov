CC = gcc
lib = -Wall -Wextra -Werror -Wpedantic -Wshadow -Wconversion -lncurses
PROG = program
SRCS = main.c fm_lib.c


$(PROG): $(SRCS) 
	$(CC) $^ -o $@ $(lib)
run: $(PROG)
	./$(PROG)
clean:
	rm $(PROG)
	

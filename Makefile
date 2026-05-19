CC = gcc

CFLAGS = -Iinclude

SRC = src/main.c
DEPS = include/config.h

myshell: $(SRC) $(DEPS)
	$(CC) $(CFLAGS) -o myshell $(SRC)

.PHONY: clean
clean:
	rm -f my_shell
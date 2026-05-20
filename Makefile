CC = gcc

CFLAGS = -Iinclude

SRC = src/main.c src/exec.c
DEPS = include/config.h include/exec.h

myshell: $(SRC) $(DEPS)
	$(CC) $(CFLAGS) -o myshell $(SRC)

.PHONY: clean
clean:
	rm -f my_shell
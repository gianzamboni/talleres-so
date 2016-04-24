CFLAGS=-Wall -Werror -std=c99

.PHONY: all clean cleanall

all: mini-shell

mini-shell: mini-shell.c
	gcc $(CFLAGS) -o mini-shell mini-shell.c

clean:
	rm -f mini-shell

cleanall:
	rm -f mini-shell

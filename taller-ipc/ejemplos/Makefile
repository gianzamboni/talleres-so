.PHONY: all clean

CC = gcc
CFLAGS = -g -Wall -Werror

BINARIES = 	sockets/echo-client \
			sockets/echo-server-dgram \
			sockets/echo-server1-stream \
			sockets/echo-server2-blocking \
			sockets/echo-server2-fcntl \
			sockets/echo-server2-select \
			pipes/pipe1 \
			pipes/pipe2 \
			shared-memory/main

all: $(BINARIES)

shared-memory/main: shared-memory/main.c
	$(CC) $(CFLAGS)	shared-memory/main.c -o shared-memory/main -lrt

clean:
	rm -f */*.o
	rm -f $(BINARIES)

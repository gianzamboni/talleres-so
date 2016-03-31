CFLAGS= -Wall -O0 -g -ggdb -m32
NASMFLAGS= -g -ggdb
LDFLAGS=-static -m32 -L'/usr/lib32'
CC=gcc

BINS=launch

OBJS=$(BINS:=.o)

all: $(BINS)

# Dependencias
launch.o: launch.c

clean:
	rm -rf $(OBJS) $(BINS)

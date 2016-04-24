CFLAGS=-Wall -Werror

.PHONY: all clean cleanall

all: mt_client mt_server

mt_client: mt_client.c
	gcc $(CFLAGS) -o mt_client mt_client.c

mt_server: mt_server.c
	gcc $(CFLAGS) -o mt_server mt_server.c

clean:
	rm -f mt_client
	rm -f mt_server

cleanall:
	rm -f mt_client
	rm -f mt_server

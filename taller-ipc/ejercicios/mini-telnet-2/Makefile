CFLAGS=-Wall -Werror

.PHONY: all clean cleanall

all: mt_client2 mt_server2

mt_client2: mt_client2.c
	gcc $(CFLAGS) -o mt_client2 mt_client2.c

mt_server2: mt_server2.c
	gcc $(CFLAGS) -o mt_server2 mt_server2.c

clean:
	rm -f mt_client2
	rm -f mt_server2

cleanall:
	rm -f mt_client2
	rm -f mt_server2

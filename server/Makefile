CC = gcc

CFLAGS = -Wall -Werror -O2 -Iinclude
LDFLAGS = -lpthread

all: bbroom-server

bbroom-server: bbroom-server.c lib/thpool.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	@rm -vf bbroom-server


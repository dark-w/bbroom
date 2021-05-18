all: client

CC = gcc

CFLAGS = -Wall -Werror -O2
LDFLAGS = -lpthread

client: client.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

phony := client
.PHONY: $(phony)

clean:
	rm client

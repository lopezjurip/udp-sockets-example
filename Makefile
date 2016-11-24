.PHONY: server client clean # force make

CC=gcc
CFLAGS=-Wall

default:
	make server & (sleep 1 && make client)

server:
	$(CC) $(CFLAGS) -o server server.c
	./server

client:
	$(CC) $(CFLAGS) -o client client.c
	./client

clean:
	rm server client

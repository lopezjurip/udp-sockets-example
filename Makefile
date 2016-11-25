.PHONY: server client clean # force make

CC=gcc
CFLAGS=-Wall

default:
	make server & make client

server: server.o common.o
	$(CC) $(CFLAGS) common.o server.o -o server

client: client.o common.o
	$(CC) $(CFLAGS) common.o client.o -o client

clean:
	rm server client

client.o: client.c common.h
	$(CC) $(CFLAGS) -c client.c

server.o: server.c common.h
	$(CC) $(CFLAGS) -c server.c

common.o: common.c common.h
	$(CC) $(CFLAGS) -c common.c

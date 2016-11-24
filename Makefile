CC=gcc
CFLAGS=-c -Wall

default:
	make server
	make client

server:
	$(CC) $(CFLAGS) -o server server.c

client:
	$(CC) $(CFLAGS) -o client client.c

clean:
	rm server client
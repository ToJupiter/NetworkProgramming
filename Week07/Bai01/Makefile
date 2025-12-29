CC = gcc
all: server client

server: tcpCaesarServer.c
	$(CC) -o server tcpCaesarServer.c

client: tcpCaesarClient.c
	$(CC) -o client tcpCaesarClient.c

clean:
	rm -f server client
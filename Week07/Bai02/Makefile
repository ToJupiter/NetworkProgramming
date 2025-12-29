CC = gcc

all: server client

server: tcpLoginServer.c
	$(CC) -o server tcpLoginServer.c

client: tcpLoginClient.c
	$(CC) -o client tcpLoginClient.c

clean:
	rm -f server client
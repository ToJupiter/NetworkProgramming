all: server client

server: tcpNonBlockServer.c
	gcc -Wall -Wextra -o server tcpNonBlockServer.c

client: tcpNonBlockClient.c
	gcc -Wall -Wextra -o client tcpNonBlockClient.c

clean:
	rm -f server client
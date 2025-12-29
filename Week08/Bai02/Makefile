all: server client

server: udpSignalServer.c
	gcc -Wall -Wextra -o server udpSignalServer.c

client: udpSignalClient.c
	gcc -Wall -Wextra -o client udpSignalClient.c

clean:
	rm -f server client
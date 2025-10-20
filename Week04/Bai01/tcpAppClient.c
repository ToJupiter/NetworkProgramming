#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define SERV_PORT 9000
#define MAXLINE 4096

int main(int argc, char **argv){
    int sockfd;
    struct sockaddr_in servaddr;
    char *sendline = (char *) malloc(MAXLINE * sizeof(char));
    char *recvline = (char *) malloc(MAXLINE * sizeof(char));

    if (argc != 2){
        perror("Usage: TCPClient <IP address of the server");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(SERV_PORT);

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
        perror("Problem in connecting to the server");
        exit(3);
    }

    while (fgets(sendline, MAXLINE, stdin) != NULL){
        send(sockfd, sendline, strlen(sendline), 0);
        
        if (recv(sockfd, recvline, MAXLINE,0) == 0){
            perror("The server terminated prematurely"); 
            exit(4);
        }

        
    }


}
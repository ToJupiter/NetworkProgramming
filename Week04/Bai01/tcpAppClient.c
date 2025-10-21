#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXLINE 4096

int main(int argc, char **argv){
    int sockfd;
    struct sockaddr_in servaddr;
    char *sendline = (char *) malloc(MAXLINE * sizeof(char));
    char *recvline = (char *) malloc(MAXLINE * sizeof(char));

    if (argc != 3){
        fprintf(stderr, "Usage: TCPClient <IP address of the server> <PortNumber>\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(atoi(argv[2]));

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
        perror("Problem in connecting to the server");
        exit(3);
    }

    while (1){
        if (fgets(sendline, MAXLINE, stdin) == NULL) break;
        if (sendline[0] == '\n') break;

        send(sockfd, sendline, strlen(sendline), 0);
        
        ssize_t n = recv(sockfd, recvline, MAXLINE -1, 0);
        if (n == 0){
            perror("The server terminated prematurely"); 
            exit(4);
        }
        recvline[n] = '\0';
        printf("%s", recvline);
        
    }
    free(sendline);
    free(recvline);
    close(sockfd);
    return 0;
}
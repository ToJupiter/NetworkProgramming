#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 1024

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[MAXLINE];
    char response[MAXLINE];
    
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IPAddress> <PortNumber>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    char *ip = argv[1];
    int port = atoi(argv[2]);
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0) {
        perror("invalid address");
        exit(EXIT_FAILURE);
    }
    
    while (1) {
        printf("Enter string: ");
        if (fgets(buffer, MAXLINE, stdin) == NULL) {
            break;
        }
        
        if (buffer[0] == '\n') {
            break;
        }
        
        sendto(sockfd, (const char *)buffer, strlen(buffer), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
        
        socklen_t len = sizeof(servaddr);
        int n = recvfrom(sockfd, (char *)response, MAXLINE, 0, (struct sockaddr *)&servaddr, &len);
        response[n] = '\0';
        
        if (strcmp(response, "Error") == 0) {
            printf("Error\n");
        } else {
            printf("%s\n", response);
            n = recvfrom(sockfd, (char *)response, MAXLINE, 0, (struct sockaddr *)&servaddr, &len);
            response[n] = '\0';
            printf("%s\n", response);
        }
    }
    
    close(sockfd);
    return 0;
}
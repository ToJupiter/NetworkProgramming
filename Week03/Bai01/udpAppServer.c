#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 1024

void process_string(const char* input, char *letters, char *digits){
    int i = 0, l = 0, d = 0;
    int has_invalid = 0;

    while (input[i] != '\0' && input[i] != '\n'){
        if (isalpha(input[i])){
            letters[l++] = input[i];
        }
        else if (isdigit(input[i])){
            digits[d++] = input[i];
        }
        else has_invalid = 1;
        i++;
    }
    letters[l] = '\0';
    digits[d] = '\0';

    if (has_invalid){
        strcpy(letters, "Error");
        digits[0] = '\0';
    }
}

int main(int argc, char **argv){
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char *buffer = (char*) malloc(MAXLINE * sizeof(char));
    char *letters = (char*) malloc(MAXLINE * sizeof(char));
    char *digits = (char*) malloc(MAXLINE * sizeof(char));

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <PortNumber>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Server running on port %d\n", port);
    
    while (1) {
        socklen_t len = sizeof(cliaddr);
        int n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';
        
        process_string(buffer, letters, digits);
        
        if (strcmp(letters, "Error") == 0) {
            sendto(sockfd, (const char *)letters, strlen(letters), 0, (const struct sockaddr *)&cliaddr, len);
        } else {
            sendto(sockfd, (const char *)digits, strlen(digits), 0, (const struct sockaddr *)&cliaddr, len);
            sendto(sockfd, (const char *)letters, strlen(letters), 0, (const struct sockaddr *)&cliaddr, len);
        }
    }
    
    close(sockfd);
    return 0;
}
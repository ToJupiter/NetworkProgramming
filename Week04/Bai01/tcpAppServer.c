#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define MAXLINE 4096
#define EXIT_FAILURE 1
#define LISTENQ 8

int process_string(const char* input, char *letters, char *digits){
    int i = 0, l = 0, d = 0;
    int has_invalid = 0;

    while (input[i] != '\0' && input[i] != '\n'){
        if (isalpha(input[i])) letters[l++] = input[i];
        else if (isdigit(input[i])) digits[d++] = input[i];
        else has_invalid = 1;
        i++;
    }
    letters[l] = '\0'; digits[d] = '\0';

    return has_invalid;
}

int main(int argc, char **argv){
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen;
    int listenfd, connfd, n;
    char *buffer = (char*) malloc(MAXLINE * sizeof(char));
    char *letters = (char*) malloc(MAXLINE * sizeof(char));
    char *digits = (char*) malloc(MAXLINE * sizeof(char));

    if (argc != 2){
        fprintf(stderr, "Usage: %s <PortNumber>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    listen(listenfd, LISTENQ);
    printf("%s\n", "Server running... waiting for connections.");

    for ( ; ; ){
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen);
        printf("%s\n", "Received request...");
        int is_error;
        while((n = recv(connfd, buffer, MAXLINE - 1, 0)) > 0){
            buffer[n] = '\0';
            is_error = process_string(buffer, letters, digits);
            printf("%s","String received from and resent to the client:");
            puts(buffer);
            
            if (is_error){
                send(connfd, "Error\n", 6, 0);
            } else {
                char response[MAXLINE * 2];
                sprintf(response, "Digits: %s\n Letters: %s\n", digits, letters);
                send(connfd, response, strlen(response), 0);
            }
        }

        if (n < 0){
            perror("Read error");
            exit(1);
        }
        close(connfd);
    }

    free(buffer);
    free(letters);
    free(digits);
    close(listenfd);

}


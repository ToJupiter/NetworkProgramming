#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 4096
#define EXIT_FAILURE 1
#define LISTENQ 8
#define SERV_PORT 9000

void process_string(const char* input, char *letters, char *digits){
    int i = 0, l = 0, d = 0;
    int has_invalid = 0;

    while (input[i] != '\0' && input[i] != '\n'){
        if (isalpha(input[i])) letters[l++] = input[i];
        else if (isdigit(input[i])) digits[d++] = input[i];
        else has_invalid = 1;
        i++;
    }
    letters[l] = '\0'; digits[d] = '\0';

    if (has_invalid){
        strcpy(letters, "Error");
        digits[0] = '\0';
    }
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
    servaddr.sin_port = htons(SERV_PORT);

    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    listen(listenfd, LISTENQ);
    printf("%s\n", "Server running... waiting for connections.");

    for ( ; ; ){
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen);
        printf("%s\n", "Received request...");
        while((n = recv(connfd, buffer, MAXLINE, 0)) > 0){
            process_string(&buffer, &letters, &digits);
            printf("%s","String received from and resent to the client:");
            puts(buffer);
            
            if (digits[0] != '\0'){
                printf("Resending the digits: \n");
                send(connfd, digits, strlen(digits), 0);
                printf("Resending the letters: \n");
                send(connfd, letters, strlen(letters), 0);
            }
            else if (digits[0] == '\0'){
                send(connfd, letters, strlen(buffer), 0);
            }
        }

        if (n < 0){
            perror("Read error");
            exit(1);
        }
        close(connfd);
    }

    close(listenfd);

}


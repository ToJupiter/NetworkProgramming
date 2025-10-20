#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 1024
#define EXIT_FAILURE 1

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
    char *buffer = (char*) malloc(MAXLINE * sizeof(char));
    char *letters = (char*) malloc(MAXLINE * sizeof(char));
    char *digits = (char*) malloc(MAXLINE * sizeof(char));

    if (argc != 2){
        fprintf(stderr, "Usage: %s <PortNumber>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
}


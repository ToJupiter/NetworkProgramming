#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 1000000

int main(int argc, char **argv){
    if (argc != 3){
        fprintf(stderr, "Usage: %s <IP> <Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(atoi(argv[2]));

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        perror("Connect failed");
        exit(EXIT_FAILURE);
    }
    
    char *filepath = malloc(MAXLINE * sizeof(char));
    char *filename = malloc(MAXLINE * sizeof(char));
    char *response = malloc(MAXLINE * sizeof(char));

    while(1){
        if (fgets(filepath, MAXLINE, stdin) == NULL) break;
        size_t len = strlen(filepath);
        if (len == 1 && filepath[0] == '\n') break;
        if (filepath[len - 1] == '\n') filepath[len - 1] = '\0';

        FILE *fp = fopen(filepath, "rb");
        if (!fp){
            printf("File not found");
            continue;
        }

        char* slash = strrchr(filepath, '/');
        char* backslash = strrchr(filepath, '\\');
        char *base = filepath;
        if (backslash && (!slash || backslash > slash)) base = backslash + 1;
        else if (slash) base = slash + 1;
        else base = filepath;

        strncpy(filename, base, MAXLINE - 1);
        send(sockfd, filename, strlen(filename), 0);

        ssize_t n = recv(sockfd, response, MAXLINE - 1, 0);
        if (n <= 0){
            printf("Error: Connection lost\n");
            fclose(fp);
            break;
        }
        response[n] = '\0';

        if (strncmp(response, "OK", 2) != 0){
            printf("%s", response);
            fclose(fp);
            continue;
        }

        char *buffer = malloc(MAXLINE * sizeof(char));
        while((n = fread(buffer, 1, MAXLINE, fp)) > 0){
            if (send(sockfd, buffer, n , 0) != n){
                printf("File transfering is interupted\n");
                fclose(fp);
                close(sockfd);
                return EXIT_FAILURE;
            }
        }

        fclose(fp);
        shutdown(sockfd, SHUT_WR);

        n = recv(sockfd, response, MAXLINE -1, 0);
        if (n > 0){
            response[n] = '\0';
            printf("%s", response);   
        }

        close(sockfd);
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
        free(buffer);
    }
    close(sockfd);
    return 0;

}
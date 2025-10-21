#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <dirent.h>

#define MAXLINE 1000000
#define UPLOAD_DIR "uploads"

int file_exists(const char *filename){
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

int main(int argc, char **argv){
    if (argc != 2){
        fprintf(stderr, "Usage: %s <PortNumber>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    listen(listenfd, 8);

    mkdir(UPLOAD_DIR, 0755);
    
    char *filename = malloc(MAXLINE * sizeof(char));
    char *response = malloc(MAXLINE * sizeof(char));
    char *filepath = malloc(MAXLINE * sizeof(char));

    for (;;){
        struct sockaddr_in cliaddr;
        socklen_t clilen = sizeof(cliaddr);
        int connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);

        ssize_t n = recv(connfd, filename, MAXLINE - 1, 0);
        if (n <= 0){
            close(connfd);
            continue;
        }
        filename[n] = '\0';
        if (filename[0] == '\n' || filename[0] == '\0') {
            close(connfd);
            continue;
        }

        snprintf(filepath, MAXLINE, "%s/%s", UPLOAD_DIR, filename);
        
        if (file_exists(filepath)){
            char *error_log = "File exists on server\n";
            send(connfd, error_log, sizeof(error_log), 0);
            close(connfd);
            continue;
        }

        FILE *fp = fopen(filepath, "wb");
        if (!fp){
            char *error_log = "Cannot create file on server";
            send(connfd, error_log, sizeof(error_log), 0);
            close(connfd);
            continue;        
        }

        send(connfd, "OK\n", 3, 0);
        char *buffer = malloc(MAXLINE * sizeof(char));
        while((n = recv(connfd, buffer, MAXLINE, 0)) > 0){
            fwrite(buffer, 1, n, fp);
        }
        fclose(fp);
        if (n==0)
            send(connfd, "Success\n", 8, 0);
        else {
            unlink(filepath);
            send(connfd, "Interrupted\n", 12, 0);
        }

        free(buffer);
        close(connfd);
    }
    
    free(filename);
    free(response);
    free(filepath);
    close(listenfd);

}

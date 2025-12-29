#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_LEN 4096
#define IP_ARG 1
#define PORT_ARG 2

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP_Addr> <Port_Number>\n", argv[0]);
        exit(1);
    }

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[PORT_ARG]));
    inet_pton(AF_INET, argv[IP_ARG], &serv_addr.sin_addr);

    if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        exit(1);
    }

    char input[MAX_LEN];
    char buffer[MAX_LEN * 4];

    while (1) {
        printf("Enter domain or IP: ");
        if (fgets(input, MAX_LEN, stdin) == NULL) break;

        if (strlen(input) == 1 && input[0] == '\n') {
            break;
        }
        input[strcspn(input, "\n")] = 0;

        send(sock_fd, input, strlen(input), 0);
        
        int n = recv(sock_fd, buffer, sizeof(buffer) - 1, 0);
        if (n > 0) {
            buffer[n] = '\0';
            printf("%s\n", buffer);
        }
    }

    close(sock_fd);
    return 0;
}
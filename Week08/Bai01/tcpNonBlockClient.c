#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_LEN 1024
#define IP_ARG 1
#define PORT_ARG 2

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP_Addr> <Port_Number>\n", argv[0]);
        exit(1);
    }

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[PORT_ARG]));
    if (inet_pton(AF_INET, argv[IP_ARG], &serv_addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(1);
    }

    if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        exit(1);
    }

    char input[MAX_LEN];
    while (1) {
        printf("Enter string: ");
        if (fgets(input, MAX_LEN, stdin) == NULL) break;
        
        if (strlen(input) == 1 && input[0] == '\n') {
            break;
        }

        send(sock_fd, input, strlen(input), 0);

        char status;
        recv(sock_fd, &status, 1, 0);

        if (status == '1') {
            uint16_t len_n;
            recv(sock_fd, &len_n, sizeof(uint16_t), 0);
            uint16_t len = ntohs(len_n);
            char msg[len + 1];
            recv(sock_fd, msg, len, 0);
            msg[len] = '\0';
            printf("%s\n", msg);
        } else {
            uint16_t len1_n, len2_n;
            recv(sock_fd, &len1_n, sizeof(uint16_t), 0);
            recv(sock_fd, &len2_n, sizeof(uint16_t), 0);

            int len1 = ntohs(len1_n);
            int len2 = ntohs(len2_n);

            char buf1[len1 + 1];
            char buf2[len2 + 1];
            memset(buf1, 0, len1 + 1);
            memset(buf2, 0, len2 + 1);

            struct iovec iov[2];
            iov[0].iov_base = buf1;
            iov[0].iov_len = len1;
            iov[1].iov_base = buf2;
            iov[1].iov_len = len2;

            readv(sock_fd, iov, 2);
            
            if (len1 > 0) printf("%s ", buf1);
            if (len2 > 0) printf("%s", buf2);
            printf("\n");
        }
    }

    close(sock_fd);
    return 0;
}
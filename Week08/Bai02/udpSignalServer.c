#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAX_LEN 1024

int sockfd;

void handle_sigio() {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[MAX_LEN];
    char response[MAX_LEN * 4];

    int n = recvfrom(sockfd, buffer, MAX_LEN - 1, 0, (struct sockaddr *)&client_addr, &addr_len);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Received request: %s\n", buffer);

        struct in_addr addr;
        int is_ip = inet_aton(buffer, &addr);

        memset(response, 0, sizeof(response));

        if (is_ip) {
            char hbuf[NI_MAXHOST];
            if (getnameinfo((struct sockaddr *)&client_addr, sizeof(client_addr), hbuf, sizeof(hbuf), NULL, 0, NI_NAMEREQD) == 0) {
                struct sockaddr_in temp_addr;
                memset(&temp_addr, 0, sizeof(temp_addr));
                temp_addr.sin_family = AF_INET;
                temp_addr.sin_addr = addr;
                temp_addr.sin_port = 0;
                
                if (getnameinfo((struct sockaddr *)&temp_addr, sizeof(temp_addr), hbuf, sizeof(hbuf), NULL, 0, NI_NAMEREQD) == 0) {
                    snprintf(response, sizeof(response), "Official name: %s", hbuf);
                } else {
                    strcpy(response, "IP address is invalid");
                }
            } else {
                strcpy(response, "IP address is invalid");
            }
        } else {
            struct addrinfo hints, *res, *p;
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;

            int status = getaddrinfo(buffer, NULL, &hints, &res);
            if (status != 0) {
                strcpy(response, "Not found information");
            } else {
                char ipstr[INET_ADDRSTRLEN];
                int count = 0;
                for (p = res; p != NULL; p = p->ai_next) {
                    struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
                    inet_ntop(p->ai_family, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
                    if (count == 0) {
                        snprintf(response, sizeof(response), "Official IP: %s", ipstr);
                    } else {
                        strcat(response, " Alias IP: ");
                        strcat(response, ipstr);
                    }
                    count++;
                }
                if (count == 0) strcpy(response, "Not found information");
                freeaddrinfo(res);
            }
        }

        sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)&client_addr, addr_len);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Port_Number>\n", argv[0]);
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    struct sigaction sa;
    sa.sa_handler = handle_sigio;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGIO, &sa, NULL);

    fcntl(sockfd, F_SETOWN, getpid());
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_ASYNC | O_NONBLOCK);

    printf("UDP Signal-driven server running on port %d...\n", atoi(argv[1]));

    while (1) {
        pause();
    }

    return 0;
}
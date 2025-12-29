#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_LEN 1024
#define PORT_ARG 1

void set_nonblocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        exit(1);
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Port_Number>\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[PORT_ARG]);
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(listen_fd, 5) < 0) {
        perror("listen");
        exit(1);
    }

    set_nonblocking(listen_fd);

    fd_set master_set, working_set;
    FD_ZERO(&master_set);
    FD_SET(listen_fd, &master_set);
    int max_sd = listen_fd;

    printf("Server started on port %d (Non-blocking mode)\n", port);

    while (1) {
        memcpy(&working_set, &master_set, sizeof(master_set));
        int activity = select(max_sd + 1, &working_set, NULL, NULL, NULL);

        if (activity < 0 && errno != EINTR) {
            perror("select");
            exit(1);
        }

        for (int i = 0; i <= max_sd; i++) {
            if (FD_ISSET(i, &working_set)) {
                if (i == listen_fd) {
                    struct sockaddr_in client_addr;
                    socklen_t addr_len = sizeof(client_addr);
                    int new_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
                    
                    if (new_fd < 0) {
                        if (errno != EWOULDBLOCK) {
                            perror("accept");
                        }
                    } else {
                        set_nonblocking(new_fd);
                        FD_SET(new_fd, &master_set);
                        if (new_fd > max_sd) {
                            max_sd = new_fd;
                        }
                        printf("New connection from %s:%d\n", 
                               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    }
                } else {
                    char buffer[MAX_LEN];
                    memset(buffer, 0, MAX_LEN);
                    int bytes_received = recv(i, buffer, MAX_LEN - 1, 0);

                    if (bytes_received <= 0) {
                        if (bytes_received == 0) {
                            printf("Client disconnected\n");
                        } else if (errno != EWOULDBLOCK) {
                            perror("recv");
                        }
                        close(i);
                        FD_CLR(i, &master_set);
                    } else {
                        buffer[bytes_received] = '\0';
                        
                        char digits[MAX_LEN] = {0};
                        char letters[MAX_LEN] = {0};
                        int has_invalid = 0;
                        int d_idx = 0, l_idx = 0;

                        for (int j = 0; j < bytes_received - 1; j++) {
                            if (buffer[j] >= '0' && buffer[j] <= '9') {
                                digits[d_idx++] = buffer[j];
                            } else if ((buffer[j] >= 'a' && buffer[j] <= 'z') || 
                                       (buffer[j] >= 'A' && buffer[j] <= 'Z')) {
                                letters[l_idx++] = buffer[j];
                            } else {
                                has_invalid = 1;
                                break;
                            }
                        }

                        if (has_invalid) {
                            char status = '1';
                            uint16_t len = htons(strlen("Error"));
                            char *msg = "Error";
                            send(i, &status, 1, 0);
                            send(i, &len, sizeof(uint16_t), 0);
                            send(i, msg, strlen(msg), 0);
                        } else {
                            char status = '0';
                            uint16_t len1 = htons(strlen(digits));
                            uint16_t len2 = htons(strlen(letters));
                            
                            send(i, &status, 1, 0);
                            send(i, &len1, sizeof(uint16_t), 0);
                            send(i, &len2, sizeof(uint16_t), 0);
                            if (strlen(digits) > 0) send(i, digits, strlen(digits), 0);
                            if (strlen(letters) > 0) send(i, letters, strlen(letters), 0);
                        }
                    }
                }
            }
        }
    }

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 4096

typedef struct {
    int socket;
    int key;
    int mode;
    int temp_file_fd;
    char temp_file_name[256];
    int state;
} ClientSession;

ClientSession clients[MAX_CLIENTS];

void init_clients() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].socket = -1;
    }
}

int find_free_client_slot() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket == -1) {
            return i;
        }
    }
    return -1;
}

int create_temp_file(char *filename) {
    mkdir("server_files", 0777);
    strcpy(filename, "server_files/tmpfileXXXXXX");
    return mkstemp(filename);
}

void caesar_cipher(char *data, int len, int key, int encrypt) {
    if (!encrypt) key = -key;
    for (int i = 0; i < len; i++) {
        if (data[i] >= 'a' && data[i] <= 'z') {
            data[i] = 'a' + (data[i] - 'a' + key) % 26;
            if (data[i] < 'a') data[i] += 26;
        } else if (data[i] >= 'A' && data[i] <= 'Z') {
            data[i] = 'A' + (data[i] - 'A' + key) % 26;
            if (data[i] < 'A') data[i] += 26;
        }
    }
}

int send_message(int sock, unsigned char opcode, unsigned short length, void *payload) {
    unsigned char header[3];
    header[0] = opcode;
    unsigned short net_len = htons(length);
    memcpy(header + 1, &net_len, 2);
    
    int sent = send(sock, header, 3, 0);
    if (sent < 0) return -1;
    
    if (length > 0) {
        sent = send(sock, payload, length, 0);
        if (sent < 0) return -1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <Port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    int server_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) return 1;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(server_sock);
        return 1;
    }

    listen(server_sock, 5);
    init_clients();

    fd_set readfds;
    int max_sd;

    printf("Server running on port %d\n", port);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_sock, &readfds);
        max_sd = server_sock;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sd = clients[i].socket;
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(server_sock, &readfds)) {
            int new_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);
            if (new_sock < 0) continue;

            int slot = find_free_client_slot();
            if (slot != -1) {
                clients[slot].socket = new_sock;
                clients[slot].state = 0;
                clients[slot].temp_file_fd = -1;
            } else {
                close(new_sock);
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sd = clients[i].socket;
            if (FD_ISSET(sd, &readfds)) {
                unsigned char header[3];
                int valread = recv(sd, header, 3, MSG_WAITALL);
                
                if (valread <= 0) {
                    close(sd);
                    if (clients[i].temp_file_fd != -1) close(clients[i].temp_file_fd);
                    if (strlen(clients[i].temp_file_name) > 0) remove(clients[i].temp_file_name);
                    clients[i].socket = -1;
                    continue;
                }

                unsigned char opcode = header[0];
                unsigned short length = ntohs(*(unsigned short*)(header + 1));
                char payload[BUFFER_SIZE];

                if (length > 0) {
                    recv(sd, payload, length, MSG_WAITALL);
                }

                if (opcode == 0 || opcode == 1) {
                    clients[i].mode = opcode;
                    clients[i].key = atoi(payload);
                    clients[i].state = 1;
                    clients[i].temp_file_fd = create_temp_file(clients[i].temp_file_name);
                } else if (opcode == 2) {
                    if (length > 0) {
                        if (clients[i].temp_file_fd != -1) {
                            write(clients[i].temp_file_fd, payload, length);
                        }
                    } else {
                        close(clients[i].temp_file_fd);
                        
                        int result_fd = open(clients[i].temp_file_name, O_RDWR);
                        char buffer[BUFFER_SIZE];
                        int bytes_read;
                        while ((bytes_read = read(result_fd, buffer, BUFFER_SIZE)) > 0) {
                            caesar_cipher(buffer, bytes_read, clients[i].key, clients[i].mode == 0);
                            lseek(result_fd, -bytes_read, SEEK_CUR);
                            write(result_fd, buffer, bytes_read);
                        }
                        close(result_fd);

                        result_fd = open(clients[i].temp_file_name, O_RDONLY);
                        while ((bytes_read = read(result_fd, buffer, BUFFER_SIZE)) > 0) {
                            send_message(sd, 2, bytes_read, buffer);
                        }
                        close(result_fd);
                        send_message(sd, 2, 0, NULL);

                        remove(clients[i].temp_file_name);
                        clients[i].temp_file_fd = -1;
                        clients[i].state = 0;
                    }
                }
            }
        }
    }
    close(server_sock);
    return 0;
}
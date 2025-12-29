#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096

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

int recv_message(int sock, unsigned char *opcode, unsigned short *length, void *payload) {
    unsigned char header[3];
    int recv_len = recv(sock, header, 3, MSG_WAITALL);
    if (recv_len <= 0) return -1;

    *opcode = header[0];
    *length = ntohs(*(unsigned short*)(header + 1));

    if (*length > 0) {
        recv(sock, payload, *length, MSG_WAITALL);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <IP> <Port>\n", argv[0]);
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection failed\n");
        return 1;
    }

    int mode;
    int key;
    char filename[256];
    char output_filename[512];

    printf("Select Mode (0: Encrypt, 1: Decrypt): ");
    scanf("%d", &mode);
    getchar(); 

    printf("Enter Key: ");
    scanf("%d", &key);
    getchar(); 

    printf("Enter Filename: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = 0;

    snprintf(output_filename, sizeof(output_filename), "%s_enc", filename);

    char key_str[16];
    sprintf(key_str, "%d", key);
    send_message(sock, mode, strlen(key_str), key_str);

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        close(sock);
        return 1;
    }

    char buffer[BUFFER_SIZE];
    int bytes_read;
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        send_message(sock, 2, bytes_read, buffer);
    }
    close(fd);
    send_message(sock, 2, 0, NULL);

    int out_fd = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    unsigned char opcode;
    unsigned short length;
    
    while (1) {
        if (recv_message(sock, &opcode, &length, buffer) < 0) break;
        if (opcode == 2) {
            if (length > 0) {
                write(out_fd, buffer, length);
            } else {
                break;
            }
        }
    }
    close(out_fd);
    close(sock);
    printf("Done. Output saved to %s\n", output_filename);
    return 0;
}
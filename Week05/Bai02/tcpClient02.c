#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 5501
#define BUFFER_SIZE 4096

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char filepath[256];
    ssize_t bytes_sent_total = 0, bytes_read, bytes_sent;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server.\nEnter file path to send: ");
    scanf("%255s", filepath);

    FILE *file = fopen(filepath, "rb");
    if (!file) {
        perror("File open failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        bytes_sent = send(client_socket, buffer, bytes_read, 0);
        if (bytes_sent < 0) {
            perror("Send failed");
            break;
        }
        bytes_sent_total += bytes_sent;
    }
    fclose(file);

    shutdown(client_socket, SHUT_WR);

    printf("\n--- File received from server ---\n");
    while ((bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        fwrite(buffer, 1, bytes_read, stdout);
    }
    printf("\n--- End of file ---\n");

    printf("Total bytes sent to server: %zd\n", bytes_sent_total);
    close(client_socket);
    return 0;
}
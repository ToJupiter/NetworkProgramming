#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 10002
#define BUFFER_SIZE 4096

typedef struct {
    uint8_t type;
    uint16_t length;
    char data[BUFFER_SIZE];
} Packet;

void handle_login(int client_sock, Packet *packet, char *username) {
    strncpy(username, packet->data, packet->length);
    username[packet->length] = '\0';
    
    Packet response = {0x81, 1, {0x00}};
    send(client_sock, &response, 3, 0);
    printf("User '%s' logged in\n", username);
}

void handle_message(int client_sock, Packet *packet, const char *username) {
    if(strlen(username) == 0) {
        Packet response = {0x82, 1, {0x01}};
        send(client_sock, &response, 3, 0);
        return;
    }
    
    char filename[256];
    snprintf(filename, sizeof(filename), "%s.log", username);
    
    FILE *file = fopen(filename, "a");
    if(file) {
        packet->data[packet->length] = '\0';
        fprintf(file, "%s\n", packet->data);
        fclose(file);
    }
    
    Packet response = {0x82, 1, {0x00}};
    send(client_sock, &response, 3, 0);
    printf("Message saved to %s\n", filename);
}

int main() {
    int server_fd, client_sock;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);
    
    printf("Server listening on port %d\n", PORT);
    
    client_sock = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    
    char username[256] = "";
    
    while(1) {
        uint8_t header[3];
        int bytes_read = recv(client_sock, header, 3, MSG_WAITALL);
        
        if(bytes_read <= 0) break;
        
        uint8_t type = header[0];
        uint16_t length = ntohs(*(uint16_t*)(header + 1));
        
        Packet packet;
        packet.type = type;
        packet.length = length;
        
        if(length > 0) {
            recv(client_sock, packet.data, length, MSG_WAITALL);
        }
        
        switch(packet.type) {
            case 0x01:
                handle_login(client_sock, &packet, username);
                break;
            case 0x02:
                handle_message(client_sock, &packet, username);
                break;
        }
    }
    
    close(client_sock);
    close(server_fd);
    return 0;
}
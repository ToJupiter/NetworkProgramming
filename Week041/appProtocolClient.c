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

void send_packet(int sock, uint8_t type, const char *data) {
    uint16_t data_len = strlen(data);
    uint8_t header[3];
    header[0] = type;
    *(uint16_t*)(header + 1) = htons(data_len);
    
    send(sock, header, 3, 0);
    if(data_len > 0) {
        send(sock, (void*)data, data_len, 0);
    }
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    
    char buffer[BUFFER_SIZE];
    
    printf("Enter username: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    send_packet(sock, 0x01, buffer);
    
    Packet response;
    recv(sock, &response, 3, MSG_WAITALL);
    
    if(response.type == 0x81 && response.data[0] == 0x00) {
        printf("Login successful!\n");
        
        while(1) {
            printf("Enter message (or 'quit' to exit): ");
            fgets(buffer, BUFFER_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            
            if(strcmp(buffer, "quit") == 0) break;
            
            send_packet(sock, 0x02, buffer);
            recv(sock, &response, 3, MSG_WAITALL);
        }
    } else {
        printf("Login failed!\n");
    }
    
    close(sock);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SERVER_IP "127.0.0.1"
#define PORT 5500
#define BUFFER_SIZE 1024

int total_bytes_sent = 0;

void *receive_messages(void *socket_desc) {
    int sock = *(int*)socket_desc;
    char buffer[BUFFER_SIZE];
    int read_size;
    
    while ((read_size = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[read_size] = '\0';
        printf("Server response: %s\n", buffer);
    }
    
    pthread_exit(NULL);
}

int main() {
    int sock;
    struct sockaddr_in server;
    char message[BUFFER_SIZE];
    pthread_t thread_id;
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket\n");
        return 1;
    }
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_port = htons(PORT);
    
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connect failed");
        return 1;
    }
    
    printf("Connected to server\n");
    
    if (pthread_create(&thread_id, NULL, receive_messages, (void*)&sock) < 0) {
        perror("Could not create thread");
        return 1;
    }
    
    while (1) {
        printf("Enter message: ");
        fgets(message, BUFFER_SIZE, stdin);
        message[strcspn(message, "\n")] = 0;
        
        if (strcmp(message, "q") == 0 || strcmp(message, "Q") == 0) {
            break;
        }
        
        int bytes_sent = send(sock, message, strlen(message), 0);
        if (bytes_sent < 0) {
            perror("Send failed");
            break;
        }
        
        total_bytes_sent += bytes_sent;
    }
    
    close(sock);
    printf("Total bytes sent: %d\n", total_bytes_sent);
    
    return 0;
}
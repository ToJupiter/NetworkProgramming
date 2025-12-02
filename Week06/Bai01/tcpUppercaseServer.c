#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <pthread.h>

#define PORT 5500
#define BUFFER_SIZE 1024

void *handle_client(void *socket_desc) {
    int sock = *(int*)socket_desc;
    char buffer[BUFFER_SIZE];
    int read_size;
    
    while ((read_size = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[read_size] = '\0';
        
        if (strcmp(buffer, "q") == 0 || strcmp(buffer, "Q") == 0) {
            break;
        }
        
        for (int i = 0; buffer[i]; i++) {
            buffer[i] = toupper(buffer[i]);
        }
        
        send(sock, buffer, strlen(buffer), 0);
    }
    
    close(sock);
    free(socket_desc);
    pthread_exit(NULL);
}

int main() {
    int server_fd, client_sock, *new_sock;
    struct sockaddr_in server, client;
    int c;
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Could not create socket");
        return 1;
    }
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        return 1;
    }
    
    listen(server_fd, 5);
    
    printf("Server is listening on port %d...\n", PORT);
    c = sizeof(struct sockaddr_in);
    
    while ((client_sock = accept(server_fd, (struct sockaddr *)&client, (socklen_t*)&c))) {
        pthread_t thread_id;
        new_sock = malloc(1);
        *new_sock = client_sock;
        
        if (pthread_create(&thread_id, NULL, handle_client, (void*)new_sock) < 0) {
            perror("Could not create thread");
            return 1;
        }
        
        pthread_detach(thread_id);
    }
    
    if (client_sock < 0) {
        perror("Accept failed");
        return 1;
    }
    
    return 0;
}
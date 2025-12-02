#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

typedef struct {
    int sock;
    int authenticated;
} ClientData;

void *receive_messages(void *data) {
    ClientData *client_data = (ClientData*)data;
    int sock = client_data->sock;
    char buffer[BUFFER_SIZE];
    int read_size;
    
    while (1) {
        read_size = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (read_size <= 0) {
            if (read_size == 0) {
                printf("Server closed connection\n");
            } else {
                perror("Receive failed");
            }
            break;
        }
        
        buffer[read_size] = '\0';
        
        // Handle server messages
        if (strstr(buffer, "Login successful") != NULL) {
            client_data->authenticated = 1;
            printf("%s\n", buffer);
            printf("Available commands: logout, connections, online\n");
        } else if (strstr(buffer, "Logout successful") != NULL) {
            client_data->authenticated = 0;
            printf("%s\n", buffer);
            printf("Please login again.\n");
        } else {
            printf("%s\n", buffer);
            
            // Check if account was locked
            if (strstr(buffer, "Account locked") != NULL || 
                strstr(buffer, "Account not found") != NULL) {
                client_data->authenticated = 0;
                printf("Please try again with new credentials.\n");
            }
        }
        
        // If logout was successful, break to allow re-login
        if (!client_data->authenticated && strstr(buffer, "Logout successful") != NULL) {
            break;
        }
    }
    
    pthread_exit(NULL);
}

int perform_login(int sock) {
    char username[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    char message[2 * BUFFER_SIZE + 1];
    
    printf("\nUsername: ");
    if (fgets(username, sizeof(username), stdin) == NULL) return 0;
    username[strcspn(username, "\n")] = 0;
    
    printf("Password: ");
    if (fgets(password, sizeof(password), stdin) == NULL) return 0;
    password[strcspn(password, "\n")] = 0;
    
    snprintf(message, sizeof(message), "%s %s", username, password);
    send(sock, message, strlen(message), 0);
    
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <server_ip> <port>\n", argv[0]);
        return 1;
    }
    
    int sock;
    struct sockaddr_in server;
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket\n");
        return 1;
    }
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(atoi(argv[2]));
    
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connect failed");
        close(sock);
        return 1;
    }
    
    printf("Connected to server\n");
    
    ClientData client_data;
    client_data.sock = sock;
    client_data.authenticated = 0;
    
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, receive_messages, (void*)&client_data) < 0) {
        perror("Could not create thread");
        close(sock);
        return 1;
    }
    
    // Initial login
    perform_login(sock);
    
    char command[BUFFER_SIZE];
    
    while (1) {
        if (!client_data.authenticated) {
            // Wait for authentication to complete or fail
            sleep(1);
            
            // If authentication failed, prompt for new credentials
            if (!client_data.authenticated) {
                printf("\nAuthentication failed or session ended. Please login again.\n");
                perform_login(sock);
                sleep(1); // Give time for server response
            }
        } else {
            printf("\nEnter command (logout, connections, online): ");
            if (fgets(command, sizeof(command), stdin) == NULL) {
                break;
            }
            command[strcspn(command, "\n")] = 0;
            
            if (strlen(command) > 0) {
                send(sock, command, strlen(command), 0);
                
                // Special handling for logout
                if (strcmp(command, "logout") == 0) {
                    // Wait for logout to complete
                    sleep(1);
                }
            }
        }
    }
    
    pthread_cancel(thread_id);
    close(sock);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024
#define ACCOUNT_FILE "account.txt"

typedef struct {
    char username[50];
    char password[50];
    int status;
    int failed_attempts;
} Account;

typedef struct {
    int socket;
    char username[50];
    char ip[INET_ADDRSTRLEN];
    time_t login_time;
    int is_logged_in;
} Client;

Account accounts[100];
int account_count = 0;
Client clients[MAX_CLIENTS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void load_accounts() {
    FILE *file = fopen(ACCOUNT_FILE, "r");
    if (file == NULL) {
        printf("Could not open account file\n");
        exit(1);
    }
    
    while (fscanf(file, "%s %s %d", accounts[account_count].username, 
                 accounts[account_count].password, &accounts[account_count].status) != EOF) {
        accounts[account_count].failed_attempts = 0;
        account_count++;
    }
    
    fclose(file);
}

void save_accounts() {
    FILE *file = fopen(ACCOUNT_FILE, "w");
    if (file == NULL) {
        printf("Could not open account file for writing\n");
        return;
    }
    
    for (int i = 0; i < account_count; i++) {
        fprintf(file, "%s %s %d\n", accounts[i].username, accounts[i].password, accounts[i].status);
    }
    
    fclose(file);
}

int find_account(const char *username) {
    for (int i = 0; i < account_count; i++) {
        if (strcmp(accounts[i].username, username) == 0) {
            return i;
        }
    }
    return -1;
}

int add_client(int socket, const char *ip) {
    pthread_mutex_lock(&mutex);
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket == -1) {
            clients[i].socket = socket;
            strcpy(clients[i].ip, ip);
            clients[i].username[0] = '\0';
            clients[i].login_time = 0;
            clients[i].is_logged_in = 0;
            
            pthread_mutex_unlock(&mutex);
            return i;
        }
    }
    
    pthread_mutex_unlock(&mutex);
    return -1;
}

void remove_client(int client_index) {
    pthread_mutex_lock(&mutex);
    clients[client_index].socket = -1;
    pthread_mutex_unlock(&mutex);
}

void update_client_login(int client_index, const char *username) {
    pthread_mutex_lock(&mutex);
    strcpy(clients[client_index].username, username);
    clients[client_index].login_time = time(NULL);
    clients[client_index].is_logged_in = 1;
    pthread_mutex_unlock(&mutex);
}

void get_active_connections_for_user(const char *username, char *response) {
    pthread_mutex_lock(&mutex);
    
    int count = 0;
    sprintf(response, "Active connections for %s:\n", username);
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket != -1 && clients[i].is_logged_in && 
            strcmp(clients[i].username, username) == 0) {
            char time_str[50];
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&clients[i].login_time));
            sprintf(response + strlen(response), "- IP: %s, Login time: %s\n", 
                   clients[i].ip, time_str);
            count++;
        }
    }
    
    if (count == 0) {
        sprintf(response + strlen(response), "No active connections\n");
    }
    
    pthread_mutex_unlock(&mutex);
}

void get_online_users(char *response) {
    pthread_mutex_lock(&mutex);
    
    sprintf(response, "Online users:\n");
    
    for (int i = 0; i < account_count; i++) {
        int is_online = 0;
        for (int j = 0; j < MAX_CLIENTS; j++) {
            if (clients[j].socket != -1 && clients[j].is_logged_in && 
                strcmp(clients[j].username, accounts[i].username) == 0) {
                is_online = 1;
                break;
            }
        }
        
        if (is_online) {
            sprintf(response + strlen(response), "- %s\n", accounts[i].username);
        }
    }
    
    pthread_mutex_unlock(&mutex);
}

void *handle_client(void *socket_desc) {
    int sock = *(int*)socket_desc;
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    getpeername(sock, (struct sockaddr*)&addr, &addr_size);
    
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    
    int client_index = add_client(sock, client_ip);
    if (client_index == -1) {
        close(sock);
        free(socket_desc);
        pthread_exit(NULL);
    }
    
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    int read_size;
    int is_authenticated = 0;
    
    while ((read_size = recv(sock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[read_size] = '\0';
        
        if (!is_authenticated) {
            char username[50], password[50];
            if (sscanf(buffer, "%s %s", username, password) == 2) {
                pthread_mutex_lock(&mutex);
                int account_index = find_account(username);
                
                if (account_index == -1) {
                    sprintf(response, "Account not found");
                } else if (accounts[account_index].status == 0) {
                    sprintf(response, "Account is locked");
                } else if (strcmp(accounts[account_index].password, password) == 0) {
                    accounts[account_index].failed_attempts = 0;
                    update_client_login(client_index, username);
                    is_authenticated = 1;
                    sprintf(response, "Login successful");
                } else {
                    accounts[account_index].failed_attempts++;
                    if (accounts[account_index].failed_attempts >= 5) {
                        accounts[account_index].status = 0;
                        save_accounts();
                        sprintf(response, "Account locked due to too many failed attempts");
                    } else {
                        sprintf(response, "Invalid password. Attempts left: %d", 
                               5 - accounts[account_index].failed_attempts);
                    }
                }
                pthread_mutex_unlock(&mutex);
            } else {
                sprintf(response, "Invalid login format. Use: username password");
            }
        } else {
            if (strcmp(buffer, "logout") == 0) {
                pthread_mutex_lock(&mutex);
                clients[client_index].is_logged_in = 0;
                pthread_mutex_unlock(&mutex);
                is_authenticated = 0;
                sprintf(response, "Logout successful. Please login again.");
            } else if (strcmp(buffer, "connections") == 0) {
                get_active_connections_for_user(clients[client_index].username, response);
            } else if (strcmp(buffer, "online") == 0) {
                get_online_users(response);
            } else {
                sprintf(response, "Unknown command. Available commands: logout, connections, online");
            }
        }
        
        send(sock, response, strlen(response), 0);
        
        // If logout was successful, break the loop to allow re-login
        if (!is_authenticated && strstr(response, "Logout successful") != NULL) {
            break;
        }
    }
    
    remove_client(client_index);
    close(sock);
    free(socket_desc);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }
    
    int port = atoi(argv[1]);
    int server_fd, client_sock, *new_sock;
    struct sockaddr_in server, client;
    int c;
    pthread_t thread_id;
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].socket = -1;
    }
    
    load_accounts();
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Could not create socket");
        return 1;
    }
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    
    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        return 1;
    }
    
    listen(server_fd, 5);
    
    printf("Server is listening on port %d...\n", port);
    c = sizeof(struct sockaddr_in);
    
    while (1) {
        client_sock = accept(server_fd, (struct sockaddr *)&client, (socklen_t*)&c);
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }
        
        new_sock = malloc(sizeof(int));
        *new_sock = client_sock;
        
        if (pthread_create(&thread_id, NULL, handle_client, (void*)new_sock) < 0) {
            perror("Could not create thread");
            free(new_sock);
            close(client_sock);
        }
        
        pthread_detach(thread_id);
    }
    
    return 0;
}
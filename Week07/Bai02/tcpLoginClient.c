#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) return 1;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) return 1;

    char username[50], password[50], buffer[BUFFER_SIZE];
    
    printf("Username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    printf("Password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    sprintf(buffer, "USER %s", username);
    send(sock, buffer, strlen(buffer), 0);
    
    int len = recv(sock, buffer, sizeof(buffer), 0);
    buffer[len] = '\0';
    
    if (strstr(buffer, "OK") == NULL) {
        printf("%s\n", buffer);
        close(sock);
        return 1;
    }

    sprintf(buffer, "PASS %s", password);
    send(sock, buffer, strlen(buffer), 0);
    
    len = recv(sock, buffer, sizeof(buffer), 0);
    buffer[len] = '\0';
    
    if (strcmp(buffer, "LOGIN_OK") == 0) {
        printf("Login Successful!\n");
        while (1) {
            printf("Enter command (logout): ");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            if (strcmp(buffer, "logout") == 0) {
                send(sock, "LOGOUT", 6, 0);
                break;
            }
        }
    } else {
        printf("%s\n", buffer);
    }
    
    close(sock);
    return 0;
}
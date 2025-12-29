#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024
#define MAX_ACCOUNTS 100
#define ACCOUNT_FILE "account.txt"

typedef struct {
    char username[50];
    char password[50];
    int status;
    int wrong_attempts;
} Account;

Account accounts[MAX_ACCOUNTS];
int account_count = 0;

void load_accounts() {
    FILE *fp = fopen(ACCOUNT_FILE, "r");
    if (!fp) return;
    char line[256];
    account_count = 0;
    while (fgets(line, sizeof(line), fp) && account_count < MAX_ACCOUNTS) {
        sscanf(line, "%s %s %d", accounts[account_count].username, accounts[account_count].password, &accounts[account_count].status);
        accounts[account_count].wrong_attempts = 0;
        account_count++;
    }
    fclose(fp);
}

void save_accounts() {
    FILE *fp = fopen(ACCOUNT_FILE, "w");
    if (!fp) return;
    for (int i = 0; i < account_count; i++) {
        fprintf(fp, "%s %s %d\n", accounts[i].username, accounts[i].password, accounts[i].status);
    }
    fclose(fp);
}

Account* get_account(char *username) {
    for (int i = 0; i < account_count; i++) {
        if (strcmp(accounts[i].username, username) == 0) {
            return &accounts[i];
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) return 1;

    load_accounts();

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(atoi(argv[1]));

    bind(server_sock, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_sock, 5);

    struct pollfd fds[MAX_CLIENTS + 1];
    char current_users[MAX_CLIENTS + 1][50];
    nfds_t nfds = 1;
    fds[0].fd = server_sock;
    fds[0].events = POLLIN;

    for (int i = 1; i <= MAX_CLIENTS; i++) {
        fds[i].fd = -1;
        current_users[i][0] = '\0';
    }

    printf("Server running on port %s\n", argv[1]);

    while (1) {
        poll(fds, nfds, -1);

        if (fds[0].revents & POLLIN) {
            int new_sock = accept(server_sock, NULL, NULL);
            for (int i = 1; i <= MAX_CLIENTS; i++) {
                if (fds[i].fd == -1) {
                    fds[i].fd = new_sock;
                    fds[i].events = POLLIN;
                    current_users[i][0] = '\0';
                    if (i >= nfds) nfds = i + 1;
                    break;
                }
            }
        }

        for (int i = 1; i < nfds; i++) {
            if (fds[i].fd != -1 && (fds[i].revents & POLLIN)) {
                char buf[BUFFER_SIZE];
                int len = recv(fds[i].fd, buf, sizeof(buf), 0);
                if (len <= 0) {
                    close(fds[i].fd);
                    fds[i].fd = -1;
                    current_users[i][0] = '\0';
                } else {
                    buf[len] = '\0';
                    char cmd[50], arg[50];
                    sscanf(buf, "%s %s", cmd, arg);

                    if (strcmp(cmd, "USER") == 0) {
                        Account *acc = get_account(arg);
                        if (acc) {
                            strcpy(current_users[i], arg);
                            send(fds[i].fd, "OK", 2, 0);
                        } else {
                            current_users[i][0] = '\0';
                            send(fds[i].fd, "ERR User not found", 18, 0);
                        }
                    } else if (strcmp(cmd, "PASS") == 0) {
                        if (strlen(current_users[i]) == 0) {
                            send(fds[i].fd, "ERR No user", 11, 0);
                            continue;
                        }
                        Account *acc = get_account(current_users[i]);
                        if (acc->status == 0) {
                            send(fds[i].fd, "LOCKED", 6, 0);
                        } else {
                            if (strcmp(acc->password, arg) == 0) {
                                acc->wrong_attempts = 0;
                                send(fds[i].fd, "LOGIN_OK", 8, 0);
                            } else {
                                acc->wrong_attempts++;
                                if (acc->wrong_attempts > 5) {
                                    acc->status = 0;
                                    save_accounts();
                                    send(fds[i].fd, "LOCKED", 6, 0);
                                } else {
                                    send(fds[i].fd, "ERR", 3, 0);
                                }
                            }
                        }
                    } else if (strcmp(cmd, "LOGOUT") == 0) {
                         send(fds[i].fd, "BYE", 3, 0);
                         close(fds[i].fd);
                         fds[i].fd = -1;
                         current_users[i][0] = '\0';
                    }
                }
            }
        }
    }
    close(server_sock);
    return 0;
}
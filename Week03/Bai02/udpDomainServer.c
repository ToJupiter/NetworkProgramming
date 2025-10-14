#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAXLINE 2048

void resolve_domain(const char *input, char *output) {
    struct addrinfo hints, *res, *p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    if ((status = getaddrinfo(input, NULL, &hints, &res)) != 0) {
        strcpy(output, "Not found information");
        return;
    }
    
    struct in_addr dummy_ipv4;
    struct in6_addr dummy_ipv6;
    int is_ip = (inet_pton(AF_INET, input, &dummy_ipv4) == 1) || (inet_pton(AF_INET6, input, &dummy_ipv6) == 1);
    
    if (is_ip) {
        if (getnameinfo(res->ai_addr, res->ai_addrlen, output, MAXLINE, NULL, 0, 0) != 0) {
            strcpy(output, "IP address is invalid.");
        }
    } else {
        int first = 1;
        output[0] = '\0';
        
        for (p = res; p != NULL; p = p->ai_next) {
            void *addr;
            char ipver[32];
            
            if (p->ai_family == AF_INET) {
                struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
                addr = &(ipv4->sin_addr);
                strcpy(ipver, "IPv4");
            } else {
                struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
                addr = &(ipv6->sin6_addr);
                strcpy(ipver, "IPv6");
            }
            
            inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
            
            if (first) {
                sprintf(output, "Official IP: %s", ipstr);
                first = 0;
            } else {
                sprintf(output + strlen(output), "\nAlias IP: %s", ipstr);
            }
        }
    }
    
    freeaddrinfo(res);
}

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[MAXLINE];
    char response[MAXLINE];
    
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <PortNumber>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    int port = atoi(argv[1]);
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);
    
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Server running on port %d\n", port);
    
    while (1) {
        socklen_t len = sizeof(cliaddr);
        int n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';
        
        resolve_domain(buffer, response);
        
        sendto(sockfd, (const char *)response, strlen(response), 0, (const struct sockaddr *)&cliaddr, len);
    }
    
    close(sockfd);
    return 0;
}
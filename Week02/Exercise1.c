#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>

int is_valid_ip(const char *ip){
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
    return result == 1;
}

int is_special_address(const char *ip) {
    struct in_addr addr;
    if (inet_aton(ip, &addr) == 0) {
        return 0;
    }
    
    unsigned long ip_addr = ntohl(addr.s_addr);
    
    // Loopback (127.0.0.0/8)
    if ((ip_addr & 0xFF000000) == 0x7F000000) {
        return 1;
    }
    
    //  Private networks (10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16)
    if ((ip_addr & 0xFF000000) == 0x0A000000 || 
        (ip_addr & 0xFFF00000) == 0xAC100000 || 
        (ip_addr & 0xFFFF0000) == 0xC0A80000) {
        return 1;
    }
    
    // link-local (169.254.0.0/16)
    if ((ip_addr & 0xFFFF0000) == 0xA9FE0000) {
        return 1;
    }
    
    // multicast (224.0.0.0/4)
    if ((ip_addr & 0xF0000000) == 0xE0000000) {
        return 1;
    }
    
    return 0;
}

void resolve_hostname(const char *hostname){
    struct hostent *host_info;
    
    host_info = gethostbyname(hostname);
    if (host_info == NULL){
        printf("Not found information");
        return;
    }

    if (host_info->h_addr_list[0] != NULL){
        char *ip = inet_ntoa(*(struct in_addr*)host_info->h_addr_list[0]);
        printf("Official IP: %s", ip);
        
        int i = 1;
        while (host_info->h_addr_list[i] != NULL){
            ip = inet_ntoa(*(struct in_addr*)host_info->h_addr_list[i]);
            printf(" Alias IP: %s", ip);
            i++;
        }
        printf("\n");
    } else printf("Not found information.\n");

}

void resolve_ip(const char *ip){
    struct in_addr addr;
    struct hostent *host_info;

    if (inet_aton(ip, &addr) == 0){
        printf("Invalid address\n");
        return;
    }

    if (is_special_address(ip)) {
        printf("Special address: %s\n", ip);
        return;
    }

    host_info = gethostbyaddr((const void*)&addr, sizeof(addr), AF_INET);
    if(host_info == NULL){
        printf("Not found information\n");
        return;
    }

    if (host_info->h_name != NULL){
        printf("Official name: %s", host_info->h_name);

        int i = 0;
        while(host_info->h_aliases[i] != NULL){
            printf(" Alias name: %s", host_info->h_aliases[i]);
            i++;
        }
        printf("\n");
    } else printf("Not found information\n");
}

int main(int argc, char **argv){
    if (argc != 2){
        printf("Usage: %s [hostname|IP address]\n", argv[0]);
        return 1;    
    }

    if (is_valid_ip(argv[1])){
        resolve_ip(argv[1]);
    }
    else resolve_hostname(argv[1]);

    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#define NI_MAXHOST 1025

int is_valid_ip(const char *ip, int *is_ipv6);
int is_special_ipv4(const char *ip);
int is_special_ipv6(const char *ip);
void resolve_hostname(const char *hostname, FILE *log_file);
void resolve_ip(const char *ip, int is_ipv6, FILE *log_file);
void process_input(const char *input, FILE *log_file);
void process_batch_file(const char *filename, FILE *log_file);
double get_time_diff(struct timeval start, struct timeval end);

int main(int argc, char *argv[]) {
    FILE *log_file = fopen("resolver.log", "a");
    if (log_file == NULL) {
        printf("Error opening log file\n");
        return 1;
    }
    
    time_t now;
    time(&now);
    fprintf(log_file, "\n=== New session started at %s ===\n", ctime(&now));
    
    if (argc == 2) {
        printf("Batch mode: processing file %s\n", argv[1]);
        process_batch_file(argv[1], log_file);
    } else if (argc == 1) {
        printf("Enter IP addresses or domain names (empty line to exit):\n");
        
        char input[1024];
        while (1) {
            printf("> ");
            if (fgets(input, sizeof(input), stdin) == NULL) {
                break;
            }
            
            input[strcspn(input, "\n")] = 0;
            
            if (strlen(input) == 0) {
                break;
            }
            
            process_input(input, log_file);
            printf("\n");
        }
    } else {
        printf("Usage: %s [filename]\n", argv[0]);
        fclose(log_file);
        return 1;
    }
    
    fclose(log_file);
    return 0;
}

int is_valid_ip(const char *ip, int *is_ipv6) {
    struct sockaddr_in sa;
    struct sockaddr_in6 sa6;
    
    // Try IPv4 first
    if (inet_pton(AF_INET, ip, &(sa.sin_addr)) == 1) {
        *is_ipv6 = 0;
        return 1;
    }
    
    // Then try IPv6
    if (inet_pton(AF_INET6, ip, &(sa6.sin6_addr)) == 1) {
        *is_ipv6 = 1;
        return 1;
    }
    
    return 0;
}

int is_special_ipv4(const char *ip) {
    struct in_addr addr;
    if (inet_aton(ip, &addr) == 0) {
        return 0;
    }
    
    unsigned long ip_addr = ntohl(addr.s_addr);
    
    // loopback (127.0.0.0/8)
    if ((ip_addr & 0xFF000000) == 0x7F000000) {
        return 1;
    }
    
    // private networks (10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16)
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

int is_special_ipv6(const char *ip) {
    struct in6_addr addr;
    if (inet_pton(AF_INET6, ip, &addr) != 1) {
        return 0;
    }
    
    // loopback (::1)
    if (addr.s6_addr[0] == 0 && addr.s6_addr[1] == 0 && 
        addr.s6_addr[2] == 0 && addr.s6_addr[3] == 0 && 
        addr.s6_addr[4] == 0 && addr.s6_addr[5] == 0 && 
        addr.s6_addr[6] == 0 && addr.s6_addr[7] == 0 && 
        addr.s6_addr[8] == 0 && addr.s6_addr[9] == 0 && 
        addr.s6_addr[10] == 0 && addr.s6_addr[11] == 0 && 
        addr.s6_addr[12] == 0 && addr.s6_addr[13] == 0 && 
        addr.s6_addr[14] == 0 && addr.s6_addr[15] == 1) {
        return 1;
    }
    
    // link-local (fe80::/10)
    if (addr.s6_addr[0] == 0xfe && (addr.s6_addr[1] & 0xc0) == 0x80) {
        return 1;
    }
    
    // unique local (fc00::/7)
    if ((addr.s6_addr[0] & 0xfe) == 0xfc) {
        return 1;
    }
    
    // multicast (ff00::/8)
    if (addr.s6_addr[0] == 0xff) {
        return 1;
    }
    
    return 0;
}

double get_time_diff(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
}

void resolve_hostname(const char *hostname, FILE *log_file) {
    struct addrinfo hints, *result, *rp;
    struct timeval start, end;
    int status;
    char ipstr[INET6_ADDRSTRLEN];
    int first_ip = 1;
    
    gettimeofday(&start, NULL);
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; 
    hints.ai_socktype = SOCK_STREAM;
    
    status = getaddrinfo(hostname, NULL, &hints, &result);
    gettimeofday(&end, NULL);
    
    if (status != 0) {
        printf("Not found information\n");
        fprintf(log_file, "Query: %s, Result: Not found information, Time: %.6f seconds\n", 
                hostname, get_time_diff(start, end));
        return;
    }
    
    printf("Query time: %.6f seconds\n", get_time_diff(start, end));
    
    if (result->ai_canonname && strcmp(result->ai_canonname, hostname) != 0) {
        printf("Canonical name: %s\n", result->ai_canonname);
    }
    
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        void *addr;
        const char *ipver;
        
        if (rp->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)rp->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else { 
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)rp->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }
        
        inet_ntop(rp->ai_family, addr, ipstr, sizeof(ipstr));
        
        if (first_ip) {
            printf("Official %s IP: %s\n", ipver, ipstr);
            first_ip = 0;
        } else {
            printf("Alias %s IP: %s\n", ipver, ipstr);
        }
    }
    
    fprintf(log_file, "Query: %s, Result: Found, Time: %.6f seconds\n", 
            hostname, get_time_diff(start, end));
    
    freeaddrinfo(result);
}

void resolve_ip(const char *ip, int is_ipv6, FILE *log_file) {
    struct sockaddr_in sa;
    struct sockaddr_in6 sa6;
    struct timeval start, end;
    void *addr;
    socklen_t addr_len;
    char host[NI_MAXHOST];
    int status;
    
    gettimeofday(&start, NULL);
    
    if (is_ipv6) {
        memset(&sa6, 0, sizeof(sa6));
        sa6.sin6_family = AF_INET6;
        inet_pton(AF_INET6, ip, &sa6.sin6_addr);
        addr = &sa6;
        addr_len = sizeof(sa6);
        
        if (is_special_ipv6(ip)) {
            printf("special IP address — may not have DNS record\n");
        }
    } else {
        memset(&sa, 0, sizeof(sa));
        sa.sin_family = AF_INET;
        inet_pton(AF_INET, ip, &sa.sin_addr);
        addr = &sa;
        addr_len = sizeof(sa);
        
        if (is_special_ipv4(ip)) {
            printf("special IP address — may not have DNS record\n");
        }
    }
    
    status = getnameinfo(addr, addr_len, host, sizeof(host), NULL, 0, 0);
    gettimeofday(&end, NULL);
    
    printf("Query time: %.6f seconds\n", get_time_diff(start, end));
    
    if (status != 0) {
        printf("Not found information\n");
        fprintf(log_file, "Query: %s, Result: Not found information, Time: %.6f seconds\n", 
                ip, get_time_diff(start, end));
        return;
    }
    
    printf("Official name: %s\n", host);
    
    fprintf(log_file, "Query: %s, Result: %s, Time: %.6f seconds\n", 
            ip, host, get_time_diff(start, end));
}

void process_input(const char *input, FILE *log_file) {
    if (strlen(input) == 0) {
        return;
    }
    
    char *input_copy = strdup(input);
    char *token = strtok(input_copy, " \t\n");
    
    while (token != NULL) {
        int is_ipv6 = 0;
        
        if (is_valid_ip(token, &is_ipv6)) {
            resolve_ip(token, is_ipv6, log_file);
        } else {
            resolve_hostname(token, log_file);
        }
        
        token = strtok(NULL, " \t\n");
    }
    
    free(input_copy);
}

void process_batch_file(const char *filename, FILE *log_file) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return;
    }
    
    char line[1024];
    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = 0;
        
        if (strlen(line) > 0) {
            printf("Processing: %s\n", line);
            process_input(line, log_file);
            printf("\n");
        }
    }
    
    fclose(file);
}
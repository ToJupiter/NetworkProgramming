// Modified tcpClient.txt for scenario 2.3
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXLINE 4096
#define SERV_PORT 9000

int main(int argc, char **argv) 
{
 int sockfd;
 struct sockaddr_in servaddr;
 char sendline[MAXLINE], recvline[MAXLINE];
    
 if (argc !=2) {
  perror("Usage: TCPClient <IP address of the server"); 
  exit(1);
 }
    
 if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  perror("Problem in creating the socket");
  exit(2);
 }
    
 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr= inet_addr(argv[1]);
 servaddr.sin_port =  htons(SERV_PORT);
    
 if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
  perror("Problem in connecting to the server");
  exit(3);
 }
    
 send(sockfd, "bull", 4, 0);
 send(sockfd, "dog", 3, 0);
    
 int n = recv(sockfd, recvline, MAXLINE, 0);
 if (n <= 0) {
  perror("The server terminated prematurely"); 
  exit(4);
 }
 
 printf("String received from the server: ");
 for(int i = 0; i < n; i++) {
  printf("%c", recvline[i]);
 }
 printf("\n");
 printf("Total bytes received: %d\n", n);

 close(sockfd);
 exit(0);
}
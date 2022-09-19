#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 5050       // the port client will be connecting to 
#define MAXDATASIZE 256 // max number of bytes we can get at once 


int main(int argc, char *argv[])
{
    int sockfd, numbytes;  
    char buffer[MAXDATASIZE];
    struct hostent *server;
    struct sockaddr_in server_addr; // connector's address information 

    if (argc != 2) {
        printf("usage: client hostname\n"); exit(1);  }

    if ((server=gethostbyname(argv[1])) == NULL) {  // get the host info 
        printf("ERROR, no such host\n");  exit(0);  }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("ERROR opening socket");  exit(0);  }

    bzero((char *) &server_addr, sizeof(server_addr));  // zero the rest of the struct 
    server_addr.sin_family = AF_INET;    // host byte order 
    server_addr.sin_port = htons(PORT);  // short, network byte order 
    server_addr.sin_addr = *((struct in_addr *)server->h_addr);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0) {
        perror("ERROR connecting");  exit(0);  }

    bzero(buffer,MAXDATASIZE);
    if ((numbytes=read(sockfd, buffer, MAXDATASIZE-1)) < 0) {
        perror("ERROR reading from socket");  exit(0);  }
    buffer[numbytes] = '\0';
    printf("%s",buffer);
    
    while(1){
        bzero(buffer,MAXDATASIZE);
        fgets(buffer,MAXDATASIZE-1,stdin);

        if (write(sockfd,buffer,strlen(buffer)) < 0) {
            perror("ERROR writing to socket"); exit(0);  }

        if (strncmp("bye", buffer, 3) == 0)
            break;
    }

    close(sockfd);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MYPORT 5050
#define MAXDATASIZE 256
#define BACKLOG 20

void error(const char *msg){
    perror(msg);
    exit(1);
}

int main(void){
    int sockfd, new_sockfd;        // listen on sockfd, new connection on new_sockfd
    struct sockaddr_in server_addr;    // server's address information
    struct sockaddr_in client_addr; // connector's address information
    char buffer[MAXDATASIZE];
    int sin_size;
    pid_t child_pid;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
        error("Error opening socket");
    
    bzero((char *) &server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;         // host byte order
    server_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    server_addr.sin_port = htons(MYPORT);     // short, network byte order

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
        error("Binding failed!");
    

    if (listen(sockfd, BACKLOG) < 0)
        error("ERROR on listen"); 

    sin_size = sizeof(struct sockaddr_in);

    while(1){
        if ((new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size)) < 0)
            error("ERROR on accept");
        printf("server: got connection from %s\n",inet_ntoa(client_addr.sin_addr));

        if((child_pid = fork()) == 0){
            close(sockfd);

            if (write(new_sockfd, "Start messaging \t<<write bye to quit>>\n", 
            strlen("Start messaging \t<<write bye to quit>>\n")) < 0)
                error("ERROR writing to socket");
            while(1){
                bzero(buffer,MAXDATASIZE);
                if (read(new_sockfd,buffer,MAXDATASIZE-1) < 0) 
                    error("ERROR reading from socket");
                printf("Client: %s",buffer);

                if (strncmp("bye", buffer, 3) == 0)
                    break;
                
                bzero(buffer,MAXDATASIZE);
                fgets(buffer,MAXDATASIZE-1,stdin);
                if (write(new_sockfd, buffer, strlen(buffer)) < 0)
                    error("ERROR writing to socket");
                
                if (strncmp("bye", buffer, 3) == 0)
                    break;
            }
        }
    }
   
    close(new_sockfd);  
    //close(sockfd);      
    return 0;
}

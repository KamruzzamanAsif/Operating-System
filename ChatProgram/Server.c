#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 5050       // the server port number
#define MAXCLIENT 1024
#define MAX_SERVER_BUFFER_SIZE 256

char server_buffer[MAX_SERVER_BUFFER_SIZE]; // server buffer 

int clientCount = 0;
pthread_t thread[MAXCLIENT]; // server threads for handling clients 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // thread mutex lock

struct client_info{
    int number;
    int sockID;
    struct sockaddr_in clientAddr;
    int len;
};
struct client_info Client[MAXCLIENT];







void * clientHandler(void * client_details){
    struct client_info* clientDetail = (struct client_info*) client_details;
	int number = clientDetail -> number;
	int clientSocket = clientDetail -> sockID;

	printf("Client %d connected.\n", number+1);

    // mutex lock for synchronization
    pthread_mutex_lock(&mutex);

    bzero(server_buffer, MAX_SERVER_BUFFER_SIZE);
    int len = read(clientSocket, server_buffer, MAX_SERVER_BUFFER_SIZE-1);
    if (len < 0) 
        perror("ERROR reading from socket");
    server_buffer[len] = '\0';
    
    for(int i = 0; i < clientCount; i++){
        if (write(Client[i].sockID, server_buffer, strlen(server_buffer)) < 0)
            perror("ERROR writing to socket");
    }

    pthread_mutex_unlock(&mutex);
    
}





int main(void){
    // server socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // server socket information
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);     // short, network byte order
    serverAddr.sin_addr.s_addr = htons(INADDR_ANY); 

    // bind the server socket
    if(bind(serverSocket,(struct sockaddr *) &serverAddr , sizeof(serverAddr)) == -1){
        perror("Failed to bind server socket");  
    } 
    
    // listen on the server socket
    if (listen(serverSocket, MAXCLIENT) < 0){
        perror("ERROR on listen"); 
    }
        
    printf("Server started listening on Port %d ......\n", PORT);

    int sin_size = sizeof(struct sockaddr_in); // length of client socket address
    while(1){
        Client[clientCount].sockID = accept(serverSocket, (struct sockaddr*) &Client[clientCount].clientAddr, &sin_size);
		Client[clientCount].number = clientCount;

		pthread_create(&thread[clientCount], NULL, clientHandler, (void *) &Client[clientCount]);

		clientCount ++;
    }

    return 0;
}
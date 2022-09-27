#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 5050       // the server port where client will connect 
#define MAXDATASIZE 256 // max number of bytes we can get at once
struct hostent *server;


void * doRecieving(void * sockID){

	int clientSocket = *((int *) sockID);
	char outputBuffer[MAXDATASIZE];

	while(1){
		bzero(outputBuffer, MAXDATASIZE);

		int numberOfBytes;
		if ((numberOfBytes = read(clientSocket, outputBuffer, MAXDATASIZE-1)) < 0) {
            perror("ERROR reading from socket");  
			exit(0);  
		}

        if(strncmp(outputBuffer, "exit", 4) == 0){
            exit(0);
        }

        outputBuffer[numberOfBytes] = '\0';
        printf("%s\n",outputBuffer);
	}

}

int errmsg(int rval, char *msg)
{
    if (rval < 0)
    {
        perror(msg);
        exit(1);
    }
    return rval;
}

int main(int argc, char *argv[]){

	if (argc != 3) {
        printf("usage: client client_name host_ip\n"); exit(1);  }

	if ((server=gethostbyname(argv[2])) == NULL) {  // get the host info 
        printf("ERROR, no such host\n");  exit(0);  }

	// client socket
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	// server socket address information
	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT); // short network byte order	
	
	// convert ip address to binary
    errmsg(inet_pton(AF_INET, argv[2], &serverAddr.sin_addr), "inet_pton error");

	// connect to server
	if(connect(clientSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0){
		printf("Failed to connect"); 
		exit(0);  
	} 

	printf("Connection established ............\n");

	// the thread will be running to receive messages from server
	pthread_t thread;
	pthread_create(&thread, NULL, doRecieving, (void *) &clientSocket );

	// send the client name to server
	if (write(clientSocket, argv[1], strlen(argv[1])) < 0) {
            perror("ERROR writing to socket"); 
			exit(0); 
		}

	// and we are in our main thread to send messages to server 
	char inputBuffer[MAXDATASIZE];
	while(1){
		bzero(inputBuffer, MAXDATASIZE);
        fgets(inputBuffer, MAXDATASIZE-1, stdin);

		if (write(clientSocket, inputBuffer, strlen(inputBuffer)) < 0) {
            perror("ERROR writing to socket"); 
			exit(0); 
		}

	}

    return 0;
}


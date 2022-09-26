#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>

#define BUFF_MAX 80
#define PORT 5050
#define IP INADDR_ANY
#define MAX_CLIENTS_COUNT 100
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

struct client
{
   int index;
   int socketFd;
   char name[BUFF_MAX];
   struct sockaddr_in clientSocket;
};


int clientsCount = 0;
struct client clients[MAX_CLIENTS_COUNT];
pthread_t thread[MAX_CLIENTS_COUNT];


void* startChat(void*);
void sendMessageToAll(int exceptSocketIndex, char buffer[BUFF_MAX]);
void send_target_message(char buffer[BUFF_MAX], char target[32], int exceptSocketIndex);


int main()
{
    int socketFd;
    int connectionFd;
    struct sockaddr_in serverSocket;
    struct sockaddr_in clientSocket;

    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    serverSocket.sin_family = AF_INET;
    serverSocket.sin_addr.s_addr = htons(INADDR_ANY);
    serverSocket.sin_port = htons(PORT);  


    if ((bind(socketFd, (struct sockaddr *)&serverSocket, sizeof(serverSocket))) != 0)
    {
        perror("socket bind failed...\n");
        exit(1);
    }
    else
    {
        printf("Socket successfully binded..\n");
    }

    if ((listen(socketFd, MAX_CLIENTS_COUNT)) != 0)
    {
        perror("Listen failed...\n");
        exit(1);
    }
    else
    {
        printf("Server listening..\n");
    }
    
    int i;
    for (i = 0; i < MAX_CLIENTS_COUNT; i++)
    {
        int clientSocketLength = sizeof(clientSocket);
        connectionFd = accept(socketFd, (struct sockaddr *)&clientSocket, &clientSocketLength);
        if (connectionFd < 0)
        {
            perror("server acccept failed...\n");
            exit(1);
        }
        else
        {
            printf("server acccepted the %dth client...\n", i + 1);
    	}
	
        clients[i].index = i;
        clients[i].socketFd = connectionFd;

        pthread_create(&thread[i], NULL, startChat, (void *) &clients[i]);

        clientsCount++;
    }

    for (i = 0; i < MAX_CLIENTS_COUNT; i++)
    {
	    pthread_join(thread[i], NULL);
    }

    printf("server has closed\n");
}

void* startChat(void* clientAsVoid)
{
    struct client* curClient = (struct client*) clientAsVoid;
    char buffer[BUFF_MAX];
    int socketFd = curClient->socketFd;
    char name[BUFF_MAX];
    int index = curClient->index;

    read(socketFd, name, BUFF_MAX);
    strcpy(curClient->name, name);

    bzero(buffer, BUFF_MAX);
    strcat(buffer, name);
    printf("%s has join the chat", buffer);
    strcat(buffer, " has joined the chat\n");
    sendMessageToAll(index, buffer);

    for (;;)
    {
        bzero(buffer, BUFF_MAX);
        int readCode = read(socketFd, buffer, sizeof(buffer));

        if (strncmp("exit", buffer, 4) == 0 || readCode == 0)
        {
            bzero(buffer, BUFF_MAX);
            strcat(buffer, name);
            strcat(buffer, " has left the chat\n");

            sendMessageToAll(index, buffer);
            break;
        }

        char toName[32], actualMsg[BUFF_MAX];
        bzero(toName, 32);
        bzero(actualMsg, BUFF_MAX);
        int i=1, j=0;
        while(buffer[i] != ' '){
            toName[j++] = buffer[i++];
        }
        i++;
        int k=0;
        while(i<strlen(buffer)){
            actualMsg[k++] = buffer[i++];
        }


        bzero(buffer, BUFF_MAX);
        strcat(buffer, name);
        strcat(buffer, " - ");
        strcat(buffer, actualMsg);
        
        if(strcmp(toName , "all")==0)
            sendMessageToAll(index, buffer);
        else
            send_target_message(buffer, toName, index);
        
    }

    close(socketFd);
    return NULL;
}

void sendMessageToAll(int exceptSocketIndex, char buffer[BUFF_MAX])
{   
    pthread_mutex_lock(&clients_mutex);
	int i;
	for (i = 0; i < clientsCount; i++)
	{
	    if (i != exceptSocketIndex)
	    {
            write(clients[i].socketFd, buffer, BUFF_MAX);
	    }
	}
    pthread_mutex_unlock(&clients_mutex);
}

void send_target_message(char buffer[BUFF_MAX], char target[32], int exceptSocketIndex){
    pthread_mutex_lock(&clients_mutex);
    
    for(int i = 0; i < MAX_CLIENTS_COUNT; i++){
        if(strcmp(target, clients[i].name) == 0){
            write(clients[i].socketFd, buffer, BUFF_MAX);
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}
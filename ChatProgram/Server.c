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
    char name[50];
    char ip[50]; 
    struct sockaddr_in clientAddr;
    int len;
};
struct client_info Client[MAXCLIENT];

// message thead
pthread_t mthread;


////////////////////////// Queue Part /////////////////////////////////
#define MESSAGE_SIZE 64
#define QUEUE_SIZE 3

typedef struct {
char data[MESSAGE_SIZE];
} MESSAGE;

typedef struct {
    MESSAGE messages[QUEUE_SIZE];
    int begin;
    int end;
    int current_load;
} QUEUE;

QUEUE queue;

void init_queue(QUEUE *queue) {
    queue->begin = 0;
    queue->end = 0;
    queue->current_load = 0;
    memset(&queue->messages[0], 0, QUEUE_SIZE * sizeof(MESSAGE_SIZE));
}

int enque(QUEUE *queue, MESSAGE *message) {
    if (queue->current_load < QUEUE_SIZE) {
        if (queue->end == QUEUE_SIZE) {
            queue->end = 0;
        }
        queue->messages[queue->end] = *message;
        queue->end++;
        queue->current_load++;
        return 1;
    } else {
        return 0;
    }
}

int deque(QUEUE *queue, MESSAGE *message) {
    if (queue->current_load > 0) {
        *message = queue->messages[queue->begin];
        memset(&queue->messages[queue->begin], 0, sizeof(MESSAGE));
        queue->begin = (queue->begin + 1) % QUEUE_SIZE;
        queue->current_load--;
        return 1;
    } else {
        return 0;
    }
}

////////////////////////////// queue part ends ///////////////////////////////


void *messageHandler(void * arg){
    MESSAGE temp_buffer;

    while(1){
        bzero(temp_buffer.data, sizeof(MESSAGE));
        if(deque(&queue, &temp_buffer)){
            if(temp_buffer.data[0] =='@'){
                char name[20], msg[50];
                int i = 1;
                while(temp_buffer.data[i] != ' '){
                    name[i] = temp_buffer.data[i];
                    i++;
                }

                i++;
                while(i < strlen(temp_buffer.data)){
                    msg[i] = temp_buffer.data[i];
                    i++;
                }

                if(name == "all"){
                    for(int i = 0; i < clientCount; i++){
                        if (write(Client[i].sockID, msg, strlen(msg)) < 0)
                        perror("ERROR writing to socket");
                    }
                }
                else{
                    for(int i = 0; i <clientCount; i++){
                        printf("%s %s", name, Client[i].name);
                        if(name == Client[i].name){
                            if (write(Client[i].sockID, msg, strlen(msg)) < 0)
                            perror("ERROR writing to socket");
                        }
                    }
                }
            }
        }
    }
}



void * clientHandler(void * client_details){
    struct client_info* clientDetail = (struct client_info*) client_details;
	int number = clientDetail -> number;
	int clientSocket = clientDetail -> sockID;
    MESSAGE temp_buffer;

    // get client name
    bzero(temp_buffer.data, sizeof(temp_buffer.data));
    int len = read(clientSocket, temp_buffer.data, sizeof(temp_buffer.data)-1);
    if (len < 0) 
        perror("ERROR reading from socket");
    temp_buffer.data[len] = '\0';
    *clientDetail->name = *temp_buffer.data;

    printf("%d %s\n", clientDetail->number, clientDetail->name);

	printf("Client %s connected.\n", temp_buffer.data);

    while(1){
        bzero(temp_buffer.data, sizeof(temp_buffer.data));
        int len = read(clientSocket, temp_buffer.data, sizeof(temp_buffer.data)-1);
        if (len < 0) 
            perror("ERROR reading from socket");
        temp_buffer.data[len] = '\0';

        /*
        // add client socket id to the message
        char clientSocID[sizeof(int) * 4 + 1];
        sprintf(clientSocID, "%d", clientSocket);
        char *t = strdup(temp_buffer.data);
        strcpy(temp_buffer.data, clientSocID);
        strcat(temp_buffer.data, "/"); 
        strcat(temp_buffer.data, t);
        free(t);
        */

        // enque the message  
        pthread_mutex_lock(&mutex);
        enque(&queue, &temp_buffer);
        pthread_mutex_unlock(&mutex);
    }
}



int main(void){
    init_queue(&queue);  
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

    // add message handler thread //
    pthread_create(&mthread, NULL, messageHandler, NULL);

    int sin_size = sizeof(struct sockaddr_in); // length of client socket address
    while(1){
        Client[clientCount].sockID = accept(serverSocket, (struct sockaddr*) &Client[clientCount].clientAddr, &sin_size);
		Client[clientCount].number = clientCount;
        struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&Client[clientCount].clientAddr;
        struct in_addr ipAddr = pV4Addr->sin_addr;
        char ip_address[INET_ADDRSTRLEN];
        inet_ntop( AF_INET, &ipAddr, ip_address, INET_ADDRSTRLEN );
        strcpy(Client[clientCount].ip, ip_address);

		pthread_create(&thread[clientCount], NULL, clientHandler, (void *) &Client[clientCount]);

		clientCount ++;
    }

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>  
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

bool enque(QUEUE *queue, MESSAGE *message) {
    if (queue->current_load < QUEUE_SIZE) {
        if (queue->end == QUEUE_SIZE) {
            queue->end = 0;
        }
        queue->messages[queue->end] = *message;
        queue->end++;
        queue->current_load++;
        return true;
    } else {
        return false;
    }
}

bool deque(QUEUE *queue, MESSAGE *message) {
    if (queue->current_load > 0) {
        *message = queue->messages[queue->begin];
        memset(&queue->messages[queue->begin], 0, sizeof(MESSAGE));
        queue->begin = (queue->begin + 1) % QUEUE_SIZE;
        queue->current_load--;
        return true;
    } else {
        return false;
    }
}

////////////////////////////// queue part ends ///////////////////////////////


void *messageHandler(void * arg){
    MESSAGE temp_buffer;

    while(1){
        bzero(temp_buffer.data, sizeof(MESSAGE));
        if(deque(&queue, &temp_buffer)){
            if(temp_buffer.data[0] =='@'){
                // name add kora baki...and nam er sathe sockId map koar baki 
                // then intended user er kace message dewa jabe
                char name[20];
                int i = 0;
                while(temp_buffer.data[i] != '/'){
                    name[i++] = temp_buffer.data[i];
                }
            }
            else{
                // broadcast the message to the clients 
                char id[20], actual_message[1024];
                int i = 0;
                while(temp_buffer.data[i] != '/'){
                    id[i] = temp_buffer.data[i];
                    i++;
                }
    
                int j = 0;
                i++;
                while(i<strlen(temp_buffer.data)){
                    actual_message[j++] = temp_buffer.data[i++];
                }

                for(int i = 0; i < clientCount; i++){
                    if(Client[i].sockID != atoi(id)){
                        if (write(Client[i].sockID, actual_message, strlen(actual_message)) < 0)
                        perror("ERROR writing to socket");
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

	printf("Client %d connected.\n", number+1);


    MESSAGE temp_buffer;
    while(1){
        bzero(temp_buffer.data, sizeof(temp_buffer.data));
        int len = read(clientSocket, temp_buffer.data, sizeof(temp_buffer.data)-1);
        if (len < 0) 
            perror("ERROR reading from socket");
        temp_buffer.data[len] = '\0';

        // add client socket id to the message
        char clientSocID[sizeof(int) * 4 + 1];
        sprintf(clientSocID, "%d", clientSocket);
        char *t = strdup(temp_buffer.data);
        strcpy(temp_buffer.data, clientSocID);
        strcat(temp_buffer.data, "/"); 
        strcat(temp_buffer.data, t);
        free(t);

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

		pthread_create(&thread[clientCount], NULL, clientHandler, (void *) &Client[clientCount]);

		clientCount ++;
    }

    return 0;
}
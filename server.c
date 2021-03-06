/*This program is a multithreaded server program that accepts connections from clients and receives the file uploaded by the client.
The program has two functions:
	-clientHandler: function that handles multiple clients
	-acceptClients: function that accepts connection from clients
Author: Pamela Sabio
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#include "authenticate.h"
#include "sendMessages.h"
#include "uploadFile.h"

// Function prototypes
static void *clientHandler(void *ptr);
static void acceptClients();

int sockid;
int connectionSize;
struct sockaddr_in server;

pthread_mutex_t logsLock;
pthread_mutex_t authenticationLock;
pthread_mutex_t fileLock;

int main(int argc, char *argv[]){
	if(argc < 2){
		printf("Please run the server in the following format\n");
		printf("./server PORT eg. ./server 1234\n");
	}else{
	int PORT = atoi(argv[1]);
	sockid = socket(AF_INET, SOCK_STREAM, 0);
	if(sockid == -1){
		printf("Socket have failed.\n");
		exit(EXIT_FAILURE);
	}

	printf("**********Initialising server**********\n");
	pthread_mutex_init(&logsLock,NULL);
	pthread_mutex_init(&authenticationLock,NULL);
	pthread_mutex_init(&fileLock,NULL);

	// Set the socket
	server.sin_port = htons(PORT);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	// Bind the socket
	if(bind(sockid, (struct sockaddr*) &server, sizeof(server)) < 0){
		perror("**********Server error: Could not bind socket**********");
		exit(EXIT_FAILURE);
	}else{
		printf("**********Server is up! Waiting for clients to connect**********\n");
	}
	while(1 == 1){
		acceptClients();
	}
	}
	return 0;
}

// Function for accepting and listening to clients.
void acceptClients(){
	connectionSize = sizeof(struct sockaddr_in);
	listen(sockid,0);
	struct sockaddr_in client;
	int clientSID = accept(sockid, (struct sockaddr *)&client, (socklen_t*)&connectionSize);
	if(clientSID < 0){
		printf("**********Server error: Cannot establish connection**********\n");
	}else{
		// Connection have been established
		pthread_t clientThread;
		printf("\n**********Client %d is connected.**********\n", clientSID);
		int *i = malloc(sizeof(*i));
		*i = clientSID;
		int threadReturn = pthread_create(&clientThread, NULL, clientHandler, (void*) i);
		if(threadReturn){
			printf("**********Server error: Failed to create a client thread. Code: %d**********\n", threadReturn);
		}
	}
}

// Function to handle the client thread.
void *clientHandler(void *csid){
	char message[512];
	int bufferSize = 0;
	memset(message,0,512);
	int clientSID = *((int*) csid);
	int authenticated = 0;
	printf("Client %d: ", clientSID);
	while(strcmp(message,"EXIT") != 0){
		bufferSize = recv(clientSID, message, 512,0);
		if(bufferSize == 0){
			printf("Client %d: Disconnected\n", clientSID);
			memset(message,0,512);
			strcpy(message,"EXIT");
		}else{
			// Check client message
			int response = serverAuthenticate(message,&authenticationLock, &logsLock);
			char auth[2];
			if(response == 0){
				// Valid cridentials
				strcpy(auth, "0");
				serverToClientMessage(clientSID,auth);
				authenticated = 1;
				memset(message,0,512);
				strcpy(message,"EXIT");
			}else if(response == -1){
				// Authentication failed
				strcpy(auth,"1");
				serverToClientMessage(clientSID,auth);
				printf("\nClient %d: Authentication Failed, disconnecting.\n", clientSID);
				memset(message,0,512);
				strcpy(message,"EXIT");
			}
		}
	}
	// Check if user was authenticated
	if(authenticated == 1){
		// If so receive the file.
		serverReceiveFile(clientSID,&fileLock,&logsLock);
	}
}

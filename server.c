#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#include "authenicate.h"
#include "sendMessages.h"
#include "uploadFile.h"

// Fxn prototype
static void *clientHandler(void *ptr);
static void listenConnections();
char* date_type_string(char *buff);
void logInfo(char *n, char* m);

// Variables
int sock, connectionSize;
struct sockaddr_in server;

pthread_mutex_t logsLock;
pthread_mutex_t authenticationLock;
pthread_mutex_t fileLock;

int main(int argc, char *argv[] ){
	int PORT = atoi(argv[1]);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1){
		printf("Socket have failed.\n");
		exit(EXIT_FAILURE);
	}
	printf("Server:> Setting up the server...\n");
	pthread_mutex_init(&logsLock,NULL);
	pthread_mutex_init(&authenticationLock,NULL);
	pthread_mutex_init(&fileLock,NULL);

	// Set the socket
	server.sin_port = htons(PORT);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	// Bind the socket
	if(bind(sock, (struct sockaddr*) &server, sizeof(server)) < 0){
		perror("Server:> Could not bind socket");
		exit(EXIT_FAILURE);
	}
	while(1 == 1){
		listenConnections();
	}
	return 0;
}

// Function that listens for connections from the clients.
void listenConnections(){
	connectionSize = sizeof(struct sockaddr_in);
	listen(sock,3);
	printf("Server:> Waiting for client...\n");
	struct sockaddr_in client;
	int clientSID = accept(sock, (struct sockaddr *)&client, (socklen_t*)&connectionSize);
	if(clientSID < 0){
		printf("Server:> Cannot establish connection\n");
	}else{
		// Connection have been established
		pthread_t clientThread;
		printf("\nServer:> Client: %d connected.\n", clientSID);
		int *i = malloc(sizeof(*i));
		*i = clientSID;
		int threadReturn = pthread_create(&clientThread, NULL, clientHandler, (void*) i);
		if(threadReturn){
			printf("Server:> Failed to create a client thread. Code: %d\n", threadReturn);
		}
	}
}

// Function to handle the client thread.
void *clientHandler(void *cs){
	char message[512];
	int READSIZE = 0;
	memset(message,0,512);
	int clientSID = *((int*) cs);
	int authenticated = 0;
	printf("Client %d:> ", clientSID);
	while(strcmp(message,"EXIT") != 0){
		READSIZE = recv(clientSID, message, 512,0);
		if(READSIZE == 0){
			printf("Client %d:> Disconnected\n", clientSID);
			memset(message,0,512);
			strcpy(message,"EXIT");
		}else{
			// Check client message
			int response = serverAuthenticate(message);
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
				printf("Client %d:> Failed to authenticate, disconnecting.\n", clientSID);
				memset(message,0,512);
				strcpy(message,"EXIT");
			}
		}
	}
	// Check if user was authenticated
	if(authenticated == 1){
		// If so receive the file.
		serverReceiveFile(clientSID);
	}
}

// Function to generate a timestamp
char *date_type_string(char *buff){
	time_t the_time;
	struct tm *timeInfo;
	time(&the_time);
	timeInfo = localtime(&the_time);
	strftime(buff, 80, "%d_%m_%Y", timeInfo);
	return buff;
}

void logInfo(char *user, char *message){
	// Variables
	pthread_mutex_lock(&logsLock); // Lock the log file 
	FILE *fp = fopen("logs/logFile.txt", "a"); // open the file
	char buffer[256];
	char date[50];
	char *timestamp = date_type_string(date);
	if(fp == NULL){
		printf("Failed to open the log file.\n");
		pthread_mutex_unlock(&logsLock);
		return;
	}
	fseek(fp, 0, SEEK_END); // Move to the end of the file
	fprintf(fp,"\nUser: %s | ",user);
	fprintf(fp,"%s | ", message);
	fprintf(fp,"%s",timestamp);
	pthread_mutex_unlock(&logsLock); // Unlock the file
	fclose(fp);
}

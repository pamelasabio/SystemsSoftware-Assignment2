/*
 This program transfers files from the client to the server
 This program has two functions:
 -clientUploadFile
 This function sends the path of the source and destination of the file from the client to the server.
 -serverReceiveFile
 This function receives the path of the source and destination of the uploaded file from the client and saves the file to the destination
 Author: Pamela Sabio
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include "sendMessages.h"
#include "authenticate.h"
#include "logMessage.h"

void clientUploadFile(int SID, char *file_source, char *file_dest, char *clientMessage, char *serverMessage){
	printf("Uploading %s\n", file_source);
	FILE *file = fopen(file_source,"r+");
	bzero(clientMessage, 512);
	int blockSize, i = 0;
	strcpy(clientMessage,file_source); // send the path of the source file
	clientToServerMessage(SID, clientMessage, serverMessage);
	bzero(clientMessage,512);
	strcpy(clientMessage,file_dest); // Send the path of the destination
	clientToServerMessage(SID, clientMessage, serverMessage);
	bzero(clientMessage,512);

	if(!file){
		// Error
		printf("The file the you uploaded doesn't exist.\n");

		close(SID);
		exit(EXIT_FAILURE);
	}

	while((blockSize = fread(clientMessage, sizeof(char), 512, file)) > 0){
		if(clientToServerMessage(SID, clientMessage, serverMessage) == 0){
			close(SID);
			exit(EXIT_FAILURE);
		}
		bzero(clientMessage,512);
		i++;
	}
	bzero(clientMessage,512);
	strcpy(clientMessage,"EXIT");
	clientToServerMessage(SID, clientMessage, serverMessage); // Inform the server that upload is finished
	printf("\nFile transferred successfully\n");
	close(SID);
	return;
}

void serverReceiveFile(int clientSID, pthread_mutex_t *fileLock, pthread_mutex_t *logsLock){
    char fileBuffer[512];
    char file_source[64];
    char dest_path[256];
    char file_dest[512];

    // Receive file name
    recv(clientSID,file_source,64,0);
    printf("\n\nClient %d: Receiving file: %s\n", clientSID, file_source);
    serverToClientMessage(clientSID, "OK");
    recv(clientSID,file_dest,512,0);
    serverToClientMessage(clientSID, "OK");
    strcpy(dest_path, "webserver/");
    if(strcmp(file_dest,"root") != 0){
    	strcat(file_dest,"/");
    	strcat(file_dest, file_source);
	strcat(dest_path, file_dest);
	printf("Client %d: File Destination path: %s\n", clientSID, dest_path);
    }else{
	strcat(dest_path, file_source);
	printf("Client %d: File Destination path: %s\n", clientSID, dest_path);
    }
    pthread_mutex_lock(fileLock);
    FILE *fp = fopen(dest_path, "w");
    if(fp == NULL){
        // Error
        char info[128];
        strcpy(info," Tried to modify: ");
        strcat(info,file_source);
        strcat(info," at location: ");
        strcat(info,dest_path);
        logMessage(getUserName(),info,logsLock);
    }else{
        bzero(fileBuffer,512);
        int blockSize = 0;
        int i =0;

        while((blockSize = recv(clientSID,fileBuffer,512,0)) > 0){
            printf("Client %d: File data received\n",clientSID,i,blockSize);
            if(strcmp(fileBuffer,"EXIT") == 0){
		break;
            }else{
		printf("\nDATA: %s\n",fileBuffer);
                int writeSz = fwrite(fileBuffer, sizeof(char), blockSize, fp);
                bzero(fileBuffer,512);
                i++;
            }
            serverToClientMessage(clientSID,"OK");
	}
        printf("Client %d: File transferred successfully\n", clientSID);
	char info[128];
        strcpy(info, " Modified file: ");
        strcat(info, file_source);
        strcat(info, " at directory: ");
        strcat(info, file_dest);
        logMessage(getUserName(),info,logsLock);
        serverToClientMessage(clientSID, "OK");
    }

    pthread_mutex_unlock(fileLock);
    fclose(fp);
}

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

#include "sendMessages.h"
#include "authenticate.h"

void clientUploadFile(int SID, char *file_source, char *file_dest, char *clientMessage){
	printf("Uploading %s\n", file_path);
	FILE *file = fopen(file_path,"r+");
	bzero(clientMessage, 512);
	int blockSize, i = 0;
	strcpy(clientMessage,file_path); // send the path of the source file
	clientToServerMessage();
	bzero(clientMessage,512);
	strcpy(clientMessage,file_dest); // Send the path of the destination
	clientToServerMessage();
	bzero(clientMessage,512);

	if(!file){
		// Error
		printf("Failed to locate the file..\n");

		close(SID);
		exit(EXIT_FAILURE);
	}

	while((blockSize = fread(clientMessage, sizeof(char), 512, file)) > 0){
		printf("Data sent: %d = %d\n", i, blockSize);
		if(clientToServerMessage() == 0){
			close(SID);
			exit(EXIT_FAILURE);
		}
		bzero(clientMessage,512);
		i++;
	}
	bzero(clientMessage,512);
	strcpy(clientMessage,"EXIT");
	clientToServerMessage(); // Inform the server that upload is finished
	printf("\nFile transferred successfully\n");
	close(SID);
	return;
}

void serverReceiveFile(int clientSID){
    char fileBuffer[512];
    char file_source[64];
    char file_dest[128];
    char *user = getUserName()
    // Receive file name
    recv(clientSID,file_source,64,0);
    printf("\n\nClient %d:> Receiving file: %s\n", clientSID, file_name);
    serverToClientMessage(clientSID, "OK");
    recv(clientSID,file_dest,128,0);
    serverToClientMessage(clientSID, "OK");
    printf("Client %d:> File department: %s\n", clientSID, file_dest);
    strcat(file_dest,"/");
    strcat(file_dest, file_source);
    pthread_mutex_lock(&fileLock);
    FILE *fp = fopen(file_dest, "w");
    if(fp == NULL){
        // Error
        char info[128];
        strcpy(info,"Tried to modify: ");
        strcat(info,file_source);
        strcat(info," at location: ");
        strcat(info,file_dest);
        logInfo(user,file_dest);
    }else{
        bzero(fileBuffer,512);
        int blockSize = 0;
        int i =0;
        
        while((blockSize = recv(clientSID,fileBuffer,512,0)) > 0){
            printf("Client %d:> Data received: %d = %d\n",clientSID,i,blockSize);
            if(strcmp(fileBuffer,"EXIT") == 0){
                break;
            }else{
                int writeSz = fwrite(fileBuffer, sizeof(char), blockSize, fp);
                bzero(fileBuffer,512);
                i++;
            }
            serverToClientMessage(clientSID,"OK");
        }
        printf("Client %d:> File transferred successfully\n", clientSID);
        char info[128];
        strcpy(info, "Modified file: ");
        strcat(info, file_name);
        strcat(info, " at department: ");
        strcat(info, file_dest);
        logInfo(user,info);
        serverToClientMessage(clientSID, "OK");
    }
    pthread_mutex_unlock(&fileLock);
    fclose(fp);
}

/*
 This program sends messages to from server/client to client/server
 This program has two functions:
 -clientToServerMessage
 This function sends messages to the server and receives response from the server
 -serverToClientMessage
 This function sends messages to the client.
 Author: Pamela Sabio
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int clientToServerMessage(int SID, char *clientMessage, char *serverMessage){
	if(send(SID, clientMessage, strlen(clientMessage) + 1, 0) < 0){
		// Send failed.
		printf("Connection to server failed..\n");
		return 0;
	}
	bzero(serverMessage, 512);
	if(recv(SID, serverMessage, 512, 0) < 0 ){
		// IO error
		printf("Connection to server failed..\n");
		return 0;
	}
	return 1;
}

void serverToClientMessage(int clientSID, char *response){
    if(write(clientSID, response, sizeof(response)) == -1){
        // Error
        printf("Client %d:> Writing to client failed.\n",clientSID);
    }
}

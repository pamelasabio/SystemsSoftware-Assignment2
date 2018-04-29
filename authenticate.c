/*
 This program authenticates the user who tries to connect to the server to transfer files	
 This program has three functions:
    -clientAuthenticate
    This function takes the user login credentials from the client: username and password and sends it to the server. It then takes the server response indicating whether the user is authenticated or not
    -serverAuthenticate
    This function receives the message from the client and extracts the username and password from the message.
    -authenticateUser
    This function receives the username and password and checks whether the user credentials exist in the users.txt file.
 Author: Pamela Sabio
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "sendMessages.h"
#include "logMessage.h"

#define USER_DATA "users.txt"

int authenticateUser(char *username, char *password, pthread_mutex_t *authenticationLock, pthread_mutex_t *logsLock);
char *user;

//Function for getting username and password from client to send to server
void clientAuthenticate(int SID, char *clientMessage, char *serverMessage){
	char username[20];
	char *password;
	printf("Please login for authentication\n");
	printf("Please enter your username:\n");
	printf("Username :");
	fgets(username,20,stdin);
	username[strcspn(username, "\n")] = 0;
	printf("Please enter your password\n");
	password = getpass("Password: ");
	if(password == NULL){
		perror("Your password is not valid.\n");
		exit(0);
	}

	// Send authentication message to the server with the format: USER:login:password
	memset(clientMessage, 0, strlen(clientMessage));
	strcpy(clientMessage, "USER:");
	strcat(clientMessage, username);
	strcat(clientMessage, ":");
	strcat(clientMessage, password);
	strcat(clientMessage, ":a");
	if(clientToServerMessage(SID,clientMessage,serverMessage) == 0){
		// Failed
		perror("Authentication failed");
		exit(0);

	}
    
	// Get server response and check if 0 or -1. 0 means authenticated and -1 means authentication failed
	if(strcmp(serverMessage, "0") != 0){
		// failed to authenticate
		printf("Authentication failed.\n");
		exit(0);
	}else{
		printf("User successfully authenticated.\n");
	}
}

//Function for receiving client authentication message
int serverAuthenticate(char *clientMessage, pthread_mutex_t *authenticationLock, pthread_mutex_t *logsLock){
    char *token;
    char username[64];
    char password[64];
    char message[512];
    strcpy(message, clientMessage);

    token = strtok(message, ":");
    if(strcmp(token, "USER") == 0){
        token = strtok(NULL, ":");
        strcpy(username, token);
        token = strtok(NULL, ":");
        strcpy(password,token);
        //check whether the user credentials are valid
        int validUser = authenticateUser(username, password, authenticationLock, logsLock);
        if(validUser == 0){
            // Valid cridentials
            printf("*****User Authenticated*****.");
            return 0;
        }else{
            // Authentication failed
            printf("*****Cridentials invalid*****.");
            return -1;
        }
    }

    printf("Done\n");
    return 1;

}

//Function for authenticating the user by checking if the username and password exists in the users.txt file
int authenticateUser(char *username, char *password, pthread_mutex_t *authenticationLock, pthread_mutex_t *logsLock){
    printf("Authenticating user: %s\n", username);
    user = username;
    pthread_mutex_lock(authenticationLock);
    FILE *file;
    file = fopen(USER_DATA,"r");
    char line[512];
    char ch = getc(file);
    int i =0;
    // Iterate through the file
    while( ch != EOF){
        if(ch != '\n'){
            line[i++] = ch;
        }else{
            line[i] = ':';
            line[i+1] = '\0';

            i = 0;
            char *token;
            char fileLogin[64];
            char filePassword[64];
            token = strtok(line, ":");
            strcpy(fileLogin, token);
            token = strtok(NULL, ":");
            strcpy(filePassword, token);
            
            // Check if username exists
            if(strcmp(username, fileLogin) == 0){
                // User found, check password
                if(strcmp(password,filePassword) == 0){
                    // Password is valid
                    logMessage(username,"Authenticated", logsLock);
                    fclose(file);
                    pthread_mutex_unlock(authenticationLock);
                    return 0;
                }else{
                    // Invalid password
                    logMessage(username,"Invalid Password", logsLock);
                    fclose(file);
                    pthread_mutex_unlock(authenticationLock);
                    return -1;
                }
            }
        }
        ch = getc(file);
    }
    logMessage(username,"Invalid Login", logsLock);
    fclose(file);
    pthread_mutex_unlock(authenticationLock);
    return -1;

}

char *getUserName(){
    return user;
}



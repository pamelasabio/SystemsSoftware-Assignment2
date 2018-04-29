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

#define USER_DATA "users.txt"

char *user;

//Function for getting username and password from client to send to server
void clientAuthenticate(char *clientMessage, *serverMessage){
	char username[20];
	char *password;
	printf("Authenticating user:\n");
	printf("Username :");
	fgets(username,20,stdin);
	username[strcspn(username, "\n")] = 0;
	password = getpass("Password: ");
	if(password == NULL){
		perror("Invalid Password.");
		exit(0);
	}

	// Send authentication message to the server Format: AUTH:login:password
	memset(clientMessage, 0, strlen(clientMessage));
	strcpy(clientMessage, "AUTH:");
	strcat(clientMessage, username);
	strcat(clientMessage, ":");
	strcat(clientMessage, password);
	strcat(clientMessage, ":testing:");
	if(clientToServerMessage() == 0){
		// Failed
		perror("Failed to authenticate");
		exit(0);

	}
    
	// Get server response and check if 0 or -1. 0 means authenticated and -1 means authentication failed
	if(strcmp(serverMessage, "0") != 0){
		// failed to authenticate
		printf("Authentication failed.\n");
		exit(0);
	}else{
		printf("Authenticated.\n");
	}
}

//Function for receiving client authentication message
int serverAuthenticate(char *clientMessage){
    char *token;
    char username[64];
    char password[64];
    char message[512];
    strcpy(message, clientMessage);
    
    token = strtok(message, ":");
    if(strcmp(token, "AUTH") == 0){
        token = strtok(NULL, ":");
        strcpy(username, token);
        token = strtok(NULL, ":");
        strcpy(password,token);
        //check whether the user credentials are valid
        int validUser = authenticateUser(username, password);
        if(validUser == 0){
            // Valid cridentials
            printf("Credentials valid.");
            return 0;
        }else{
            // Authentication failed
            printf("Cridentials invalid.");
            return -1;
        }
    }
    
    printf("Done\n");
    return 1;

}

//Function for authenticating the user by checking if the username and password exists in the users.txt file
int authenticateUser(char *username, char *password){
    printf("Authenticating user: %s\n", username);
    user = username;
    pthread_mutex_lock(&authenticationLock);
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
            if(strcmp(login, fileLogin) == 0){
                // User found, check password
                if(strcmp(password,filePassword) == 0){
                    // Password is valid
                    logInfo(login,"Authenticated");
                    fclose(file);
                    pthread_mutex_unlock(&authenticationLock);
                    return 0;
                }else{
                    // Invalid password
                    logInfo(login,"Invalid Password");
                    fclose(file);
                    pthread_mutex_unlock(&authenticationLock);
                    return -1;
                }
            }
        }
        ch = getc(file);
    }
    logInfo(login,"Invalid Login");
    fclose(file);
    pthread_mutex_unlock(&authenticationLock);
    return -1;

}

char *getUserName(){
    return *user;
}



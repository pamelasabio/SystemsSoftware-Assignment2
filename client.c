#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "authenticate.h"
#include "uploadFile.h"

// Function prototypes
static void makeConnection();
static int connection();
static void transferFile();

int SID;
struct sockaddr_in server;
char clientMessage[512];
char serverMessage[512];
char file_dest[50];
char file_path[150];

int PORT;
char IP[16];

// Arg 1 = IP Arg 2 = PORT 
// Arg 3 = File Destination Arg 4 = file
int main(int argc, char *argv[] ){
	if(argc < 5){
		printf("Not enough Arguments - Please follow the format:\n./client IP, PORT, File's Destination, File \nE.g. ./client 127.0.0.1 1234 root index.html\n");
		exit(EXIT_FAILURE);
	}

	PORT = atoi(argv[2]); // Set port
	if(PORT > 9999 || PORT < 100){
		printf("Invalid Port.\n");
		exit(EXIT_FAILURE);
	}
	strcpy(IP, argv[1]);  // Set IP
	strcpy(file_dest, argv[3]); // Set file Dest
	int isValidDest = 0;
	if((strcmp(file_dest, "root") == 0) || (strcmp(file_dest, "sales") == 0) || (strcmp(file_dest, "promotions") == 0) || (strcmp(file_dest, "offers") == 0) || (strcmp(file_dest, "marketing") == 0)){
		// Okay to continue 
		isValidDest = 1;
	}
	// Error checking for file destination
	if(isValidDest == 0){
		printf("Invalid file destination. Valid Destinations:\nsales\npromotions\noffers\nmarketing\n");
		exit(EXIT_FAILURE);
	}
	// set file to transfer
	strcpy(file_path, argv[4]);
	// Check file format to ensure .html
	int len = strlen(file_path);
	if(len < 5){
		printf("Invalid file.\n");
		exit(EXIT_FAILURE);
	}
	const char *extenssion = &file_path[len-5];
	if(strcmp(extenssion, ".html") != 0 || strcmp(extenssion, ".css") != 0 || strcmp(extenssion, ".css") != 0){
		printf("Only html, css and php files are allowed to be transfered.\n");
		exit(EXIT_FAILURE);
	}

	makeConnection();
	clientAuthenticate(clientMessage, serverMessage);
	printf("\nTransfering file, please wait...\n");
	clientUploadFile(SID, file_path, file_dest, clientMessage);
	exit(0);
}


void makeConnection(){
	// Make socket
	SID = socket(AF_INET, SOCK_STREAM, 0);
	if(SID == -1){
		// error
		perror("Socket not created");
		exit(0);
	}else{
		// success
		printf("Connecting to a server, please wait...\n");
		// Init the server variables
		server.sin_port = htons(PORT);
		server.sin_addr.s_addr = inet_addr(IP);
		server.sin_family = AF_INET; // IPV4 
		// Connect to a server
		if(!connection()){
			// Connection failed, try again 3 times, otherwise quit the program.
			int counter = 3;
			while(counter != 0){
				if(connection() == 1){
					// Success
					return;
				}else{
					// Try again
					counter--;
					if(counter == 0){
						perror("Failed to connect to the server.");
						exit(EXIT_FAILURE);
					}
				}
			}
		}

		return;
	}
}

int connection(){
	if(connect(SID, (struct sockaddr *) &server, sizeof(server)) < 0 ){
		// Connection have failed
		return 0;
	}else{
		// Connection succesfull
		printf("Connected.\n\n");
		return 1;
	}
}

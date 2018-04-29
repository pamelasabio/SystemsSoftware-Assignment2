/*This program is responsible for making logs and saving the logs to a text file. The username of the user, the page they modified and the timestamp is recorded.
This program has two functions:
	-dateToString: Converts the date to a string format.
	-logMessage: logs the messages into the log.txt file.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

//Function Prototype
char *dateToString(char *buff);

void logMessage(char *user, char *message, pthread_mutex_t *logsLock){
	// Variables
	pthread_mutex_lock(logsLock); // Lock the log file
	FILE *fp = fopen("logs/log.txt", "a"); // open the file
	char buffer[256];
	char date[50];
	char *timestamp = dateToString(date);
	if(fp == NULL){
		printf("Failed to open the log file.\n");
		pthread_mutex_unlock(logsLock);
		return;
	}
	fseek(fp, 0, SEEK_END); // Move to the end of the file
	if(strlen(user) > 1){
		fprintf(fp,"\nUser: %s\n ",user);
	}
	fprintf(fp,"%s\n ", message);
	fprintf(fp,"Date: %s\n\n",timestamp);
	pthread_mutex_unlock(logsLock); // Unlock the file
	fclose(fp);
}

char *dateToString(char *buff){
	time_t the_time;
	struct tm *timeInfo;
	time(&the_time);
	timeInfo = localtime(&the_time);
	strftime(buff, 80, "%d_%m_%Y [%H:%M:%S]", timeInfo);
	return buff;
}

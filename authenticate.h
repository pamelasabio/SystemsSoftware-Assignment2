#ifndef AUTHENTICATE_H
#define AUTHENTICATE_H

void clientAuthenticate(char *clientMessage, *serverMessage);
int serverAuthenticate(char *clientMessage);
int authenticateUser(char *username, char *password);
char *getUserName();

#endif

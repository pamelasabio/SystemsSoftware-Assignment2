#ifndef AUTHENTICATE_H
#define AUTHENTICATE_H

void clientAuthenticate(int SID, char *clientMessage, char *serverMessage);
int serverAuthenticate(char *clientMessage, pthread_mutex_t *authenticationLock, pthread_mutex_t *logsLock);
int authenticateUser(char *username, char *password, pthread_mutex_t *authenticationLock, pthread_mutex_t *logsLock);
char *getUserName();

#endif

#ifndef LOGMESSAGE_H
#define LOGMESSAGE_H

void logMessage(char *user, char *message, pthread_mutex_t *logsLock);
char *dateToString(char *buff);

#endif

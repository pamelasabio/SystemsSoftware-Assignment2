#ifndef UPLOADFILE_H
#define UPLOADFILE_H

void clientUploadFile(int SID, char *file_source, char *file_dest, char *clientMessage, char *serverMessage);
void serverReceiveFile(int clientSID, pthread_mutex_t *fileLock, pthread_mutex_t *logsLock);

#endif

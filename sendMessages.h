#ifndef SENDMESSAGES_H
#define SENDMESSAGES_H

int clientToServerMessage(int SID, char *clientMessage, char *serverMessage);
void serverToClientMessage(int clientSID, char *response);

#endif

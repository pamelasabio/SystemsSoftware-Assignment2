#ifndef SENDMESSAGES_H
#define SENDMESSAGES_H

int clientToServerMessage(int SID, *clientMessage, *serverMessage);
void serverToClientMessage(int clientSID, char *response);

#endif

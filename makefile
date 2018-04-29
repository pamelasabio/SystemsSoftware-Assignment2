CC = gcc
objects = obj/client.o obj/authenticate.o obj/sendMessages.o obj/uploadFile.o obj/logMessage.o 
objects2 = obj/server.o obj/authenticate.o obj/sendMessages.o obj/uploadFile.o obj/logMessage.o  
cfiles = client.c authenticate.c sendMessages.c uploadFile.c logMessage.c
cfiles2 = server.c authenticate.c sendMessages.c uploadFile.c logMessage.c
headers = authenticate.h sendMessages.h uploadFile.h logMessage.h

name = client
OBJDIR = obj

client: $(objects)
	$(CC) -o $(name) $(objects) -lm

$(OBJDIR)/client.o: client.c $(headers)
	$(CC) -c client.c -o $(OBJDIR)/client.o

server: $(objects2)
	$(CC) -pthread -o server $(objects2) -lm

$(OBJDIR)/server.o: server.c $(headers)
	$(CC) -c server.c -o $(OBJDIR)/server.o

$(OBJDIR)/authenticate.o: authenticate.c $(headers)
	$(CC) -pthread -c authenticate.c -o $(OBJDIR)/authenticate.o

$(OBJDIR)/sendMessages.o: sendMessages.c $(headers)
	$(CC) -pthread -c sendMessages.c -o $(OBJDIR)/sendMessages.o

$(OBJDIR)/uploadFile.o: uploadFile.c $(headers)
	$(CC) -pthread -c uploadFile.c -o $(OBJDIR)/uploadFile.o

$(OBJDIR)/logMessage.o: logMessage.c $(headers)
	$(CC) -pthread -c logMessage.c -o $(OBJDIR)/logMessage.o

clean:
	rm $(name) $(objects)
	rm server obj/server.o

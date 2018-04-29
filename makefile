CC = gcc
objects = obj/client.o obj/authenticate.o obj/sendMessages.o obj/uploadFile.o 
cfiles = client.c authenticate.c sendMessages.c uploadFile.c
headers = authenticate.h sendMessages.h uploadFile.h 

name = client
OBJDIR = obj

myProg: $(objects)
	$(CC) -o $(name) $(objects) -lm

$(OBJDIR)/server.o: server.c $(headers)
	$(CC) -c server.c -o $(OBJDIR)/server.o

$(OBJDIR)/authenticate.o: authenticate.c $(headers)
	$(CC) -c authenticate.c -o $(OBJDIR)/authenticate.o

$(OBJDIR)/sendMessages.o: sendMessages.c $(headers)
	$(CC) -c sendMessages.c -o $(OBJDIR)/sendMessages.o

$(OBJDIR)/uploadFile.o: uploadFile.c $(headers)
	$(CC) -c uploadFile.c -o $(OBJDIR)/uploadFile.o

clean:
	rm $(name) $(objects)


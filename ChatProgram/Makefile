#
# Makefile for src
#

BINFILES = Client Server server newServer

all: $(BINFILES)

Client: Client.c
	gcc -o Client Client.c

Server: Server.c
	gcc -o Server Server.c -lpthread

server: server.c
	gcc -o server server.c -lpthread

newServer: newServer.c
	gcc -o newServer newServer.c -lpthread


clean:
	-rm  $(BINFILES)

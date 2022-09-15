#
# Makefile for src
#

BINFILES = Client Server

all: $(BINFILES)

client: Client.c
	gcc -o Client Client.c

server: server.c
	gcc -o Server Server.c


clean:
	-rm  $(BINFILES)

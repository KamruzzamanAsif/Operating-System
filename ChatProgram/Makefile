#
# Makefile for src
#

BINFILES = Client Server

all: $(BINFILES)

client: Client.c
	gcc -o Client Client.c

server: Server.c
	gcc -o Server Server.c -lpthread


clean:
	-rm  $(BINFILES)

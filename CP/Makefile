#
# Makefile for src
#

BINFILES = server client

all: $(BINFILES)

server: server.c
	gcc -o server server.c -lpthread

client: client.c
	gcc -o client client.c -lpthread

clean:
	-rm  $(BINFILES)

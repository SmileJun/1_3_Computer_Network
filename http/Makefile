CC		= gcc -pthread
CFLAGS	= -W -Wall -O2
LDFLAGS	= -pthread

all : httpServer

server :  httpServer.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


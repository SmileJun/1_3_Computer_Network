CC		= gcc -pthread
CFLAGS	= -W -Wall -O2
LDFLAGS	= -pthread

all : server

server : common.c myChatting_server.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


all : client

client : common.c myChatting_client.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFALGS)

clean :
	$ rm -rf *.o like

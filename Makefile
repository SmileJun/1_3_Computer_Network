CC		= gcc -pthread
CFLAGS	= -W -Wall -O2
LDFLAGS	= -pthread

all : server

server : common.c myChatting_server.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean :
	$ rm -rf *.o like

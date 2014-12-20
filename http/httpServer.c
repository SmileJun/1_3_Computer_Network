#include "httpServer.h"

int main(int argc, char * argv[])
{
	if(!isValidMainArg(argc, argv))
	{
		perrorAndExit("Invalid arg input occured");
		return -1;
	}

	int serverSocket, clientSocket;
	struct sockaddr_in serverAddress, clientAddress;
	int clientAddressSize;
	char buf[BUF_SIZE];

	pthread_t requestHandlerId;

	return 0;
}

int isValidMainArg(int argc, char * argv[])
{
	if(argv == NULL)
		return FALSE;

	if(argv[0] == NULL || argv[1] == NULL)
		return FALSE;

	if(argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		return FALSE;
	}

	if(!isValidPort(argv[1]))
	{
		printf("Port should be number, and valid Range is (%d - %d)\n", MIN_PORT_NUM, MAX_PORT_NUM);
		return FALSE;
	}

	return TRUE;
}

int isValidPort(const char * str)
{
	if(str == NULL)
		return FALSE;

	if(!isNum(str) && !isValidPortRange(str))
		return FALSE;

	return TRUE;
}

int isNum(const char * str)
{
	if(str == NULL)
		return FALSE;

	int len = strlen(str);
	int i;

	for(i=0; i < len; i++)
	{
		if(!isdigit(str[i]))
			return FALSE;
	}

	return FALSE;
}

int isValidPortRange(const char * str)
{
	if(str == NULL)
		return FALSE;

	int portNum = atoi(str);
	
	if(portNum < MIN_PORT_NUM || portNum > MAX_PORT_NUM)
		return FALSE;

	return TRUE;
}

void perrorAndExit(const char * str)
{
	perror(str);
	exit(1);
}

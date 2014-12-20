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
	socklen_t clientAddressSize;
	char buf[BUF_SIZE];

	pthread_t requestHandlerId;

	serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	if(serverSocket == -1)
		perrorAndExit("serverSocket() error");
	
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(atoi(argv[1]));
	if(bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
		perrorAndExit("bind() error");

	if(listen(serverSocket, LISTEN_QUEUE_SIZE) == -1)
		perrorAndExit("listen() error");
	
	while(!isEndOfServer())
	{
		clientAddressSize = sizeof(clientAddress);
		clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);
		printf("Connection Request : %s:%d\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
		
		if(pthread_create(&requestHandlerId, NULL, requestHandler, &clientSocket) != 0)
			perror("pthread_create() error");
		if(pthread_detach(requestHandlerId) != 0)
			perror("pthread_detach() error");
	}

	close(serverSocket);
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

int isEndOfServer(void)
{
	return FALSE;
}

void * requestHandler(void * arg)
{
	int clientSocket = *((int*)arg);
	char requestLine[SMALL_BUF];
	FILE * clientRead;
	FILE * clientWrite;

	char method[10];
	char contentType[15];
	char fileName[30];

	clientRead = fdopen(clientSocket, "r");
	clientWrite = fdopen(clientSocket, "w");
	fgets(requestLine, SMALL_BUF, clientRead);

	if(strstr(requestLine, "HTTP/") == NULL)
	{
		sendError(clientWrite);
		fclose(clientRead);
		fclose(clientWrite);
		return;
	}

	strcpy(method, strtok(requestLine, " /"));
	if(strcmp(method, "GET") != 0)
	{
		sendError(clientWrite);
		fclose(clientRead);
		fclose(clientWrite);
		return;
	}
	strcpy(fileName, strtok(NULL, " /"));
	strcpy(contentType, getContentType(fileName));

	fclose(clientRead);
	sendData(clientWrite, contentType, fileName);
}

void sendData(FILE * fp, char * fileContentType, char * fileName)
{
	return (char*)NULL;
}

char * getContentType(char * file)
{
	return (char*)NULL;
}

void sendError(FILE * fp)
{
}


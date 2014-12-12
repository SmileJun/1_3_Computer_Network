#include "myChatting_client.h"

int main(int argc, char *argv[])
{
	if(!isValidMainArg(argc, argv))
	{
		perrorAndExit("inValid arg input occured");
		return -1;
	}

	int clientSocket;
	struct sockaddr_in serverAddress;
	pthread_t sendMessageId, receiveMessageId;
	void * threadReturn;

	sprintf(chatName, "%s", argv[3]);
	clientSocket = socket(PF_INET, SOCK_STREAM, 0);
	if(clientSocket == -1)
		perrorAndExit("socket() error");

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(argv[1]);
	serverAddress.sin_port = htons(atoi(argv[2]));

	if(connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress))==-1)
		perrorAndExit("connect() error");

	if(pthread_create(&sendMessageId, NULL, sendMessage, (void*)&clientSocket) != 0)
		perror("receiveMessage pthread_create() error");
	if(pthread_create(&receiveMessageId, NULL, receiveMessage, (void*)&clientSocket) != 0)
		perror("sendMessage pthread_create() error");

	if(pthread_join(sendMessageId, &threadReturn) != 0)
		perrorAndExit("sendMessage pthread_join() error");
	if(pthread_join(receiveMessageId, &threadReturn) != 0)
		perrorAndExit("receiveMessage pthread_join() error");
	
	close(clientSocket);
	return 0;
}

int isValidMainArg(int argc, char *argv[])
{
	if(argv == NULL)
		return FALSE;

	if(argv[0] == NULL || argv[1] == NULL || argv[2] == NULL || argv[3] == NULL)
		return FALSE;

	if(argc != 4)
	{
		printf("Usage : %s <IP> <port> <chatName> \n", argv[0]);
		return FALSE;
	}

	if(!isValidPort(argv[2]))
	{
		printf("Port should be number, and valid Range is (%d - %d)\n", MIN_PORT_NUM, MAX_PORT_NUM);
		return FALSE;
	}

	if(!isValidChatName(argv[3]))
	{
		printf("chatName length shoule be less than %d\n", MAX_CHATNAME_LENGTH);
		return FALSE;
	}

	return TRUE;
}


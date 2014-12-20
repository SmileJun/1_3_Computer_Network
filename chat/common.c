#include "common.h"

int isValidPort(const char *str)
{
	if(str == NULL)
		return FALSE;

	if(!isNum(str) && !isValidPortRange(str))
		return FALSE;

	return TRUE;
}

int isNum(const char *str)
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

int isValidPortRange(const char *str)
{
	if(str == NULL)
		return FALSE;

	int portNum = atoi(str);
	
	if(portNum < MIN_PORT_NUM || portNum > MAX_PORT_NUM)
		return FALSE;

	return TRUE;
}

int isValidChatName(const char *str)
{
	if(str == NULL)
		return FALSE;

	int len = strlen(str);
	
	if(len < 0 || len > MAX_CHATNAME_LENGTH)
		return FALSE;

	return TRUE;
}

void perrorAndExit(const char *str)
{
	perror(str);
	exit(EXIT_FAILURE);
}

void * sendMessage(void *arg)
{
	if(arg == NULL)
		return (void*)NULL;

	int socket = *((int*)arg);
	char inputMessage[MAX_MESSAGE_LENGTH];
	char message[TOTAL_MESSAGE_LENGTH];

	while(1)
	{
		if(fgets(inputMessage, MAX_MESSAGE_LENGTH, stdin) == NULL)
			perror("fgets() error");

		if(!strcasecmp(inputMessage, "q\n"))
		{
			if(shutdown(socket, SHUT_WR))
				perror("shutdown() error");

			break;
		}

		//sprintf() 2번째 인자인 출력 양식에 따라 common.h에 있는 TOTAL_MESSAGE_LENGTH값에
		//추가적인 공간을 제공해야 message의 index를 초과하지 않도록 할 수 있다
		//별로 좋지 못한 모양으로 보임
		//출력을 receiveMessage에서 담당하도록 하는 방식으로 바꾸는 것이 좋을 것 같다.
		//'데이터'만 전달하는 방식으로 바꾸면, 양식 변경에 따라
		//TOTAL_MESSAGE_LENGTH를 계속 바꿔줘야 하는 상황을 막을 수 있다
		if(sprintf(message, "[%s] : %s", chatName, inputMessage) < 0)
			perror("message sprintf() error");

		//'\0'을 포함해 메시지를 전달한다
		if(write(socket, message, strlen(message)+1) == -1)
			perror("message write() error");
	}
	close(socket);
	return (void*)NULL;
}

void * receiveMessage(void *arg)
{
	if(arg == NULL)
		return (void*)NULL;

	int socket = *((int*)arg);
	char message[TOTAL_MESSAGE_LENGTH];
	int readMessageLength;

	while(1)
	{
		readMessageLength = read(socket, message, TOTAL_MESSAGE_LENGTH);

		if(readMessageLength == -1)
			return (void*)-1;

		if(fputs(message, stdout) == EOF)
			perror("fputs() error");
	}

	return (void*)NULL;
}

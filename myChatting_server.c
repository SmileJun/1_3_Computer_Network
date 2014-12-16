#include "myChatting_server.h"

int epfd;
struct epoll_event * epollEvents;

int main(int argc, char *argv[])
{
	if(!isValidMainArg(argc, argv))
	{
		perrorAndExit("inValid arg input occured");
		return -1;
	}

	int serverSocket, clientSocketForServer;
	struct sockaddr_in serverAddress, clientAddressForServer;

	int reuseAddrOption = TRUE;
	int optionSize = sizeof(reuseAddrOption);

	struct epoll_event event;

	pthread_t epollServerId, sendMessageId, receiveMessageId;
	void * threadReturn;
		
	sprintf(chatName, "%s", argv[2]);

	/*
	 * STEP 1
	 * 서버 소켓 준비 및 옵션 지정
	 */
	epfd = epoll_create(1);
	if(epfd == -1)
		perrorAndExit("epoll_create() error");
	epollEvents = malloc(sizeof(struct epoll_event)*EPOLL_SIZE);
	if(epollEvents == NULL)
		perrorAndExit("epollEvents malloc() error");

	serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	if(serverSocket == -1)
		perrorAndExit("socket() error");
	
	if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (void*)&reuseAddrOption, optionSize) == -1)
		perror("setsockopt() error");	

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(atoi(argv[1]));
	if(bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
		perrorAndExit("bind() error");

	if(listen(serverSocket, LISTEN_QUEUE_SIZE) == -1)
		perrorAndExit("listen() error");

	setNonblockingMode(serverSocket);
	/*
	 * STEP 2
	 * serverSocket을 epoll 인스턴스에 등록하고
	 * serverSocket을 각 스레드 생성
	 */
	event.events = EPOLLIN;
	event.data.fd = serverSocket;
	if(epoll_ctl(epfd, EPOLL_CTL_ADD, serverSocket, &event) == -1)
		perrorAndExit("serverSocket epoll_ctl() error");

	if(pthread_create(&epollServerId, NULL, epollServer, (void *)&serverSocket) != 0)
		perrorAndExit("epollServerHandler pthread_create() error");

	/*
	 * STEP 3  
	 * server측도 다른 사람들과 함께 채팅이 가능해야 하므로
	 * client로 연결을 해서 메시지 전달을 받을 수 있도록 준비한다
	 */
	clientSocketForServer = socket(PF_INET, SOCK_STREAM, 0);
	if(clientSocketForServer == -1)
		perror("clientSocketForServer socket() error");
	
	clientAddressForServer.sin_family = AF_INET;
	clientAddressForServer.sin_addr.s_addr = htonl(INADDR_ANY);
	clientAddressForServer.sin_port = htons(atoi(argv[1]));

	if(connect(clientSocketForServer, (struct sockaddr*)&clientAddressForServer, sizeof(clientAddressForServer)) == -1)
		perror("clientSocketForServer connect() error");
	
	/*
	 * STEP 4
	 * 메시지 송수신을 위한 함수를 스레드로 등록하고
	 * 스레드의 종료를 대기한다
	 */
	if(pthread_create(&sendMessageId, NULL, sendMessage, (void*)&clientSocketForServer) != 0)
		perror("receiveMessage pthread_create() error");
	if(pthread_create(&receiveMessageId, NULL, receiveMessage, (void*)&clientSocketForServer) != 0)
		perror("sendMessage pthread_create() error");
	
	if(pthread_join(sendMessageId, &threadReturn) != 0)
		perrorAndExit("sendMessage pthread_join() error");
	if(pthread_join(receiveMessageId, &threadReturn) != 0)
		perrorAndExit("receiveMessage pthread_join() error");
	if(pthread_join(epollServerId, &threadReturn) != 0)
		perrorAndExit("epollServer pthread_join() error");
	
	/*
	 * STEP 5
	 * 서버 종료 조건시 자원을 반환하고 종료한다
	 */
	close(clientSocketForServer);
	close(serverSocket);
	close(epfd);

	return 0;
}

int isValidMainArg(int argc, char *argv[])
{
	if(argv == NULL)
		return FALSE;

	if(argv[0] == NULL || argv[1] == NULL || argv[2] == NULL)
		return FALSE;

	if(argc != 3)
	{
		printf("Usage : %s <port> <chatName> \n", argv[0]);
		return FALSE;
	}

	if(!isValidPort(argv[1]))
	{
		printf("Port should be number, and valid Range is (%d - %d)\n", MIN_PORT_NUM, MAX_PORT_NUM);
		return FALSE;
	}

	if(!isValidChatName(argv[2]))
	{
		printf("chatName length shoule be less than %d\n", MAX_CHATNAME_LENGTH);
	}

	return TRUE;
}

void * epollServer(void *arg)
{
	if(arg == NULL)
		perrorAndExit("arg is NULL");

	int serverSocket = *((int*)arg);
	int clientSocket;
	struct sockaddr_in clientAddress;
	socklen_t clientAddressSize;

	struct epoll_event * epollEvents
		= malloc(sizeof(struct epoll_event)*EPOLL_SIZE);
	if(epollEvents == NULL)
		perrorAndExit("epollEvents malloc() error");
	struct epoll_event event;
	int eventNum = 0;
	
	int clientSocketNum = 0;
	int maxClientSocketNum = 0;
	int readMessageLength = 0;
	int i, j;

	char message[TOTAL_MESSAGE_LENGTH];

	while(!isEndOfServer())
	{
		eventNum = epoll_wait(epfd, epollEvents, EPOLL_SIZE, -1);
		if(eventNum == -1)
		{
			perror("epoll_wait() error");
			break;
		}

		for(i=0; i < eventNum; ++i)
		{
			int eventSocket = epollEvents[i].data.fd;

			if(eventSocket == serverSocket)
			{
				clientAddressSize = sizeof(clientAddress);

				// 3 : epfd, 4 : serverSocket
				// ..??
				// clientSocketForServer가 5번 fd가 아니라 6번에 들어가버린다.
				// 어쩔 수 없이 mychatting_server.h 에 CLIENT_FD_START_INDEX는 일단 6으로 등록하였다
				// 원인 불명 ...ㅜㅜ
				clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);
				if(clientSocket == -1)
				{
					perror("accept() error");
					continue;
				}
				
				setNonblockingMode(clientSocket);
				event.events = EPOLLIN | EPOLLET;
				event.data.fd = clientSocket;
				if(epoll_ctl(epfd, EPOLL_CTL_ADD, clientSocket, &event) == -1)
					perrorAndExit("clientSocket epoll_ctl() error");

				printf("connected client: %d \n", clientSocket);
				increaseSocketNum(&clientSocketNum, &maxClientSocketNum);
			}
			else
			{
				while(1)
				{
					clientSocket = eventSocket;
					readMessageLength = read(clientSocket, message, TOTAL_MESSAGE_LENGTH);

					if(readMessageLength == 0)
					{
						epoll_ctl(epfd, EPOLL_CTL_DEL, clientSocket, NULL);
						close(clientSocket);	
						printf("closed client: %d \n", clientSocket);
						clientSocketNum--;
						break;
					}
					else if(readMessageLength < 0)
					{
						if(errno == EAGAIN || errno == EWOULDBLOCK)
							break;
						
						perror("message read() error");
					}
					else
					{
						// 현재 client의 최소 ~ 최대 범위를 기반으로
						// 모든 clientSocket에게 메시지를 보내고 있는데
						// close된 소켓에게도 write()를 수행하는 문제가 있다.
						// skip하는 별도의 방법을 추후 생각해볼 것
						for(j=CLIENT_FD_START_INDEX; j < CLIENT_FD_START_INDEX + maxClientSocketNum; j++)
						{
							if(clientSocket == j)
								continue;
							
							writeMessageToClient(&j, message, readMessageLength);
						}
					}
				}
			}
		}
	}

	return (void*)NULL;
}

int isEndOfServer(void)
{
	return FALSE;
}

void increaseSocketNum(int *clientSocketNum, int *maxClientSocketNum)
{
	if(clientSocketNum == NULL || maxClientSocketNum == NULL)
		return;

	(*clientSocketNum)++;
	if(*clientSocketNum > *maxClientSocketNum)
		(*maxClientSocketNum)++;

	if(*clientSocketNum > EPOLL_SIZE)
		perror("EPOLL_SIZE over");
}


void writeMessageToClient(int *clientSocket, const char *message, int messageLength)
{
	if(clientSocket == NULL || message == NULL)
		return;

	if(write(*clientSocket, message, messageLength) == -1)
	{
		// close된 소켓에 의한 경우는 일단 별도의 에러 메시지를 출력하지 않는
		if(errno == EBADF)
			return;

		perror("message write() error");
	}
}

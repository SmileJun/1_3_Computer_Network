#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <ctype.h>

#define TRUE 1
#define FALSE 0

#define LISTEN_QUEUE_SIZE 10

#define MIN_PORT_NUM 1024
#define MAX_PORT_NUM 65535
#define MAX_CHATNAME_LENGTH 256
#define MAX_MESSAGE_LENGTH 1024

// 전송될 message의 최종적인 길이
// common.c의 sendMessage 함수와 관련된 부분
// +5는 [ ], :, '공백'에 의한 부분, -1은 '\0'문자 갯수 감소로 인한 부분 
#define TOTAL_MESSAGE_LENGTH (MAX_CHATNAME_LENGTH) + (MAX_MESSAGE_LENGTH) + 5 - 1

#define EPOLL_SIZE 1 + 30 // serverSocketNum + clientSocketNum

#define setNonblockingMode(fd)						\
	do {											\
		int flag = fcntl((fd), F_GETFL, 0);			\
		fcntl((fd), F_SETFL, flag | O_NONBLOCK);	\
	} while (0);

int isValidPort(const char *str);
int isNum(const char *str);
int isValidPortRange(const char *str);
int isValidChatName(const char *str);
void perrorAndExit(const char *str);

void * sendMessage(void *arg);
void * receiveMessage(void *arg);

char chatName[MAX_CHATNAME_LENGTH];

#endif

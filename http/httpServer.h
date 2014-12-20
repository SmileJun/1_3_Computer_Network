#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <ctype.h>

#define TRUE 1
#define FALSE 0
#define MIN_PORT_NUM 1024
#define MAX_PORT_NUM 65535

#define LISTEN_QUEUE_SIZE 10
#define BUF_SIZE 1024
#define SMALL_BUF 100

int isValidMainArg(int argc, char * argv[]);
int isValidPort(const char * str);
int isNum(const char * str);
int isValidPortRange(const char * str);
void perrorAndExit(const char * str);

void * requestHandler(void * arg);
void sendData(FILE * fp, char * ct, char * fileName);
char * contentType(char * file);
void sendError(FILE * fp);

#endif

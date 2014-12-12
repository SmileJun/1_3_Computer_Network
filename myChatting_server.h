#ifndef __MY_CHAT_SERVER_H__
#define __MY_CHAT_SERVER_H__

#include "common.h"

#define CLIENT_FD_START_INDEX 6

int isValidMainArg(int argc, char *argv[]);
void * epollServer(void *arg);
int isEndOfServer(void);

#endif

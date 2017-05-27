
#include <stdio.h>          /* printf */
#include <stdlib.h>         /* exit */
#include <string.h>         /* bzero */
#include <sys/socket.h>     /* struct sockaddr, socket, listen, bind, accept, recv, send */
#include <sys/wait.h>       /* waitpid */
#include <arpa/inet.h>      /* struct sockaddr */
#include <unistd.h>         /* exit, fork */
#include <signal.h>         /* signal */
#include <time.h>           /* TIME, time_t */
#include <pthread.h>        /* pthread_t, pthread_create */
#include <sys/stat.h>       /* lstat() */
#include <sys/types.h>      /* mode_t */
#include <netdb.h> 			/* hostent */

#include "decoder.h"

#define Sockaddr struct sockaddr
#define Sockaddr_in struct sockaddr_in

int *openSocket(int port);

void in_thread(int *sockfd);
void *start_thread(void *arg);

void start(int connfd);

char *request(char* buffer,char* host);
int dnsResolve(char* host,char* ip);


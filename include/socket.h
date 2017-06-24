#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdio.h>          /* printf */
#include <stdlib.h>         /* exit */
#include <string.h>         /* bzero */
#include <sys/socket.h>     /* struct sockaddr, socket, listen, bind, accept, recv, send */
#include <sys/wait.h>       /* waitpid */
#include <sys/time.h>
#include <arpa/inet.h>      /* struct sockaddr */
#include <unistd.h>         /* exit, fork */
#include <signal.h>         /* signal */
#include <time.h>           /* TIME, time_t */
#include <pthread.h>        /* pthread_t, pthread_create */
#include <sys/stat.h>       /* lstat() */
#include <sys/types.h>      /* mode_t */
#include <netdb.h> 			/* hostent */
#include <unistd.h>    //usleep
#include <fcntl.h> //fcntl
#include <poll.h>          // For poll()
#include <sys/types.h>


#define CHUNK_SIZE 512
#define Sockaddr struct sockaddr
#define Sockaddr_in struct sockaddr_in

struct sockthread{
	int iptr;
	char client[16];
};

int *openSocket(int port);

void in_thread(int *sockfd);
void *start_thread(void *arg);

//Rotina de tratamento de uma requisição
void start(int connfd,char* client);

//requisição de uma pagina
int request(char* buffer,char* host, char* response);
//transforma uma url em um op
int dnsResolve(char* host,char* ip);

int establishConnection(struct addrinfo *info);
struct addrinfo * getHostInfo(char *host);


int recv_timeout(int sockfd, int timeout, char *response);
void logMessage(char const *client,char const *host,char const *path, char const  *method, int cod);

char* time_system();
char* time2string(const struct tm *timeptr);

#endif

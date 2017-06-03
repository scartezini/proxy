#include <stdlib.h>         /* exit */
#include <stdio.h>          /* printf */
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

#define HUE 1
#define DEBUG 0
#define BUFFSIZE 1000000
#define HOSTSIZE 800
#define TYPE 16               


int decodeHTTP(char *buffer,char *path,char *type,char *method, char *version);
int filterHost(char *host);
int filterTerms(char *buffer);
int makeHTTP(char *response, int cod);

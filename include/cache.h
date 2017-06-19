#include <stdlib.h>         /* exit */
#include <stdio.h>          /* printf */
#include <string.h>         /* bzero */
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pthread.h>

#include "list.h"
#include "decoder.h"

#define CACHE_FILE "resources/index_cache.txt"
#define CACHE_SIZE 5
pthread_mutex_t lock;

List *cache_first; //most recent

void fileName(char* file,char* host,char* path,char* method, char* version);
int inCache(char* file);
int writeCache(char* file,char* buffer);
int readCache(char* file, char* response);

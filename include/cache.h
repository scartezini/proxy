#ifndef CACHE_H_
#define CACHE_H_

#include <stdlib.h>         /* exit */
#include <stdio.h>          /* printf */
#include <string.h>         /* bzero */
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pthread.h>
#include <bits/stdc++.h>

//#include "list.h"
//#include "decoder.h"

#define CACHE_FILE "resources/index_cache.txt"
#define CACHE_SIZE 5
static pthread_mutex_t lock;
static std::list<std::string> cache_first; //most recent

void fileName(char* file,char* host,char* path,char* method, char* version);
int inCache(char* file);
int writeCache(char* file,char* buffer);
int readCache(char* file, char* response);

#endif

#ifndef CACHE_H_
#define CACHE_H_

#include <stdlib.h>         /* exit */
#include <stdio.h>          /* printf */
#include <string.h>         /* bzero */
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pthread.h>
#include <iostream>
#include <list>

#define CACHE_FILE "resources/index_cache.txt"
#define CACHE_SIZE 20
static pthread_mutex_t lock;
static std::list<std::string> cache_first; //most recent

//Cria o nome no padrao para indentifica a pagina em cache
void fileName(char* file,char* host,char* path,char* method, char* version);
//Verifica se o arquivo esta em cache
int inCache(char* file);
//Escreve uma pagina em cache
int writeCache(char* file,char* buffer);
//Ler um arquivo da cache
int readCache(char* file, char* response);

#endif

#include <stdlib.h>         /* exit */
#include <stdio.h>          /* printf */
#include <string.h>         /* bzero */
#include <sys/stat.h>
#include <fcntl.h>
 
/*  Not technically required, but needed on some UNIX distributions */
#include <sys/types.h>

#include "decoder.h"



void fileName(char* file,char* host,char* path,char* method, char* version);
int inCache(char* file);
int writeCache(char* file,char* buffer);
int readCache(char* file, char* response);

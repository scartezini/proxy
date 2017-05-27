#include "../include/decoder.h"

int decodeHTTP(char *buffer,char *path,char *method, char *version, char *host){

	int i, k, l;

	for(i = 0; buffer[i] != ' ' && buffer[i] != '\0' && i < 9; i++)
		method[i] = buffer[i];
	method[i] = '\0';

	for(i = i+1, k = 0; buffer[i] != ' ' && buffer[i] != '\0'; i++,k++)
		path[k] = buffer[i];
	path[k] = '\0';

	for(i = i+1, k = 0; buffer[i] != '\r' && buffer[i] != '\0'; i++,k++)
		version[k] = buffer[i];
	version[k] = '\0';

	for(i = i+8, k = 0; buffer[i] != '\r' && buffer[i] != '\0'; i++,k++)
		host[k] = buffer[i];
	host[k] = '\0';

	int length;
	length = strlen(method)+strlen(path)+strlen(version)+2;
	
	if(path[0] == 'h'){
	  	for(k = 7+strlen(host), l = 0; path[k] != '\0' ; k++, l++)
			path[l] = path[k];
	  	path[l] = '\0';
	}else //this is not request for proxy
	  return -1;

	for(k = length, l = 0; buffer[k] != '\0'; k++,l++)
		buffer[l] = buffer[k];
	buffer[l] = '\0';

	char newBuffer[BUFFSIZE];
	strcat(newBuffer,method);
	strcat(newBuffer," ");
	strcat(newBuffer,path);
	strcat(newBuffer," ");
	strcat(newBuffer,version);
	
	for(k = 0, l = strlen(newBuffer); buffer[k] != '\0'; k++,l++)
	  newBuffer[l] = buffer[k];
	newBuffer[l] = '\0';

	buffer = newBuffer;

#if DEBUG == 1
	printf("method: %s\n",method);
	printf("path: %s\n",path);
	printf("version: %s\n",version);
	printf("host: %s\n",host);
	printf("header: \n%s\n",newBuffer);
	printf("buffer: \n%s\n",buffer);
#endif 
 	
	if(!strcmp(method,"GET")){
	  return 1;
	}else if(!strcmp(method,"POST")){
	  return 2;
	}else 
	  return 3;


	return 0;
}




int filterProxy(char *buffer,char *path,char *method, char *version, char *host){
	return 0;
}

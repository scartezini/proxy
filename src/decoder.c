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

	//remove Accept-Encoding
	char *pch, pch_c[BUFFSIZE];
	int leng_line;

	pch = strstr(buffer,"Accept-Encoding");
	if(pch != NULL){
		strcpy(pch_c,pch);
		for(leng_line = 0; pch_c[leng_line] != '\n'; ++leng_line){}
	
		for(k = leng_line+1, l = 0; pch[k] != '\0' ; k++, l++ )
			pch[l] = pch[k];
		pch[l] = '\0';
	}

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

	strcpy(buffer,newBuffer);

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



int filterHost(char *host){
	 
    // Procurar na Whitelist pelo host
    // Se achar, return 1

	FILE *fp, *fq;
	char line[100];
	char* result;
	fp = fopen(fp_WHITELIST , "r");
	fq = fopen(fp_BLACKLIST , "r");
	

	if (fp == NULL)
		return -1;

	while (fgets(line, 100, fp) != NULL){
		result = strstr(line, host);

		if (result != NULL)
			return 1;
		
		if (feof (fp))
			break;
	}
	fclose(fp);
	
	if (fq == NULL)
		return -1;
	
	while (fgets(line, 100, fq) != NULL){
		result = strstr(line, host);

		if (result != NULL)
			return 2;
		
		if (feof (fq))
			break;
	}
	fclose(fq);

	return 0;


    // Procura no deny terms termos dentro do buffer proibidos
    // Se achar, return 2
    // Se não achar, return 1
}


int filterTerms(char *buffer){
	FILE *fp;
	char line[100];
	char* result;
	fp = fopen( fp_DENYTERMS, "r");
	

	if (fp == NULL)
		return -1;
	
	while (fgets(line, 100, fp) != NULL){
		line[strcspn(line, "\n")] = 0;
		result = strstr(buffer,line);

		if (result != NULL){
			return 1;
		}
		
		if (feof (fp))
			break;
	}

	fclose(fp);
	return 0;
}


void makeHTTP(char *response, int cod){
	
	memset(response,0,BUFFSIZE);

//#if HUE == 1
	strcat(response,"HTTP/1.1 302 Found\r\n");
	switch(cod){
		case 401:
			strcat(response,"Location: https://http.cat/401\r\n");
			break;
		case 403: 
			strcat(response,"Location: https://http.cat/403\r\n");
			break;
		case 500:
			strcat(response,"Location: https://http.cat/500\r\n");
			break;
		default:
			strcat(response,"Location: https://http.cat/418\r\n");

	}
	strcat(response,"\r\n");
//#else

//#endif

}


void makeReqModified(char* request, char* cache){
	char *modified, *pch;
	char date[40];



	pch = strstr(cache,"Date: ");
	if(pch == NULL)
		return;
	
	int i,j;
	for(i=0,j=7; pch[j] != 0 && pch[j] != '\r';i++,j++)
		date[i] = pch[j];
	date[i] = 0;
	

	modified = strstr(request,"\r\n\r\n"); //final do cabeçalho

	if(modified == NULL)
		return;

	modified = modified + 2;

	
	strcpy(modified,"If-Modified-Since: ");
	strcat(modified,date);
	strcat(modified,"\r\n\r\n");
}	

int grepHttpCode(char *http){
	char aux[BUFFSIZE];

	strcpy(aux,http+9); //remove "HTTP/1.1 "
	aux[3] = 0;

	return atoi(aux);
}







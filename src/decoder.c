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


int searchList(char *path,char *buffer)
{

    // Procurar na Whitelist pelo host
    // Se achar, return 1

	FILE *fp, *fq, *fr;
	char line[100];
	char* result;
	fp = fopen(WHITE_LIST, "r");
	fq = fopen(BLACK_LIST, "r");
	fr = fopen(DENY_TERMS, "r");
	

	if (fp == NULL)
		return -1;

	while (fgets(line, 100, fp) != NULL)
	{
		result = strstr(line, path);

		if (result != NULL)
			return 1;
		
		if (feof (fp))
			break;
	}
	fclose(fp);
	
	if (fq == NULL)
		return -1;
	
	while (fgets(line, 100, fq) != NULL)
	{
		result = strstr(line, path);

		if (result != NULL)
			return 2;
		
		if (feof (fq))
			break;
	}
	fclose(fq);

if (fr == NULL)
		return -1;
	
	while (fgets(line, 100, fq) != NULL)
	{
		result = strstr(line, path);

		if (result != NULL)
			return 1;
		
		if (feof (fr))
			return 2;
			break;
	}
	fclose(fq);

    // Procura no deny terms termos dentro do buffer proibidos
    // Se achar, return 2
    // Se não achar, return 1

}

int filterProxy(char *buffer,char *path,char *method, char *version, char *host){
	 
	 switch (host, searchList(path, buffer))
    {
        case 1:
       // msgDest();
       // logAutorizando();
       // msgRem();
            break;
        case 2:
       // descarteReq();
       // msgDestNegando();
       // logDescartando();
            break;
        default:
        break;
    }

	return 0;
}

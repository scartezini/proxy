#include "../include/cache.h"


void fileName(char* file,char* host,char* path,char* method, char* version){
	char pch[800];

	strcpy(pch,host);
	strcat(pch,path);
	strcat(pch,method);
	strcat(pch,version);
	strcat(pch,".txt\0");

	for(int i = 0; pch[i] != 0; i++){
		if(pch[i] == '/')
			pch[i] = '_';
	}

	strcpy(file,"resources/cache/");
	strcat(file,pch);

}


int inCache(char* file){
	FILE* f;

	f = fopen(file,"r");

	if(f == NULL)
		return 0;
	return 1;

}

int writeCache(char* file,char* buffer){
	FILE *f;
	f = fopen(file,"w");
 
	if(f == NULL)
		return -1;

	fputs(buffer,f);	
	fclose(f);

	pthread_mutex_lock(&lock);
	cache_first.remove(std::string(file));
	cache_first.push_front(std::string(file));

	if(cache_first.size() > CACHE_SIZE){
		remove(cache_first.back().c_str());
		cache_first.pop_back();

	}
	pthread_mutex_unlock(&lock);
	return 0;
}


int readCache(char* file, char* response){

	FILE *f;
	unsigned int lSize;

	f = fopen(file,"r");
	if(f == NULL)
		return -1;

	fseek(f,0,SEEK_END);
	lSize = ftell(f);
	rewind(f);
	

	if(fread(response,1,lSize,f) != lSize)
		return -3;

	fclose(f);
	
	return 0;
	
} 



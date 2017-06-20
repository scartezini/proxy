#include "../include/socket.h"
#include "../include/decoder.h"
#include "../include/cache.h"  


#define MAXPENDING 20             

int main(int argc, char *argv[]){

  	int port;
	int cacheSize;
	int *sockfd;

	if(argc != 2){
	  printf("You must run like a ./proxy <port_number>");
	  exit(-1);
	}

	port = atoi(argv[1]);

	
	//Ler o index que ja estava em cache
	//Mais informação olhar na cache.c e cache.h
	FILE *cacheF;

	cacheF = fopen(CACHE_FILE,"r");
	if(cacheF != NULL){
		int i;
		char line[HOSTSIZE];


		for(i = 0; i < CACHE_SIZE && !feof(cacheF); i++){
			fgets(line, HOSTSIZE, cacheF); 	
			cache_first.push_back(std::string(line));
		}

		fclose(cacheF);
	}



	sockfd = openSocket(port);
	
	if(listen(*sockfd, MAXPENDING) < 0){
    	printf("Failed in try execute socket in server");
		exit(-1);
	}

	in_thread(sockfd);
	
	//escrevendo em arquivo o que esta em cache
	cacheF = fopen(CACHE_FILE,"w");
	if(cacheF != NULL){
		for(std::string s : cache_first)
			fprintf(cacheF,"%s\n",s.c_str());	
		
		fclose(cacheF);
	}

	free(sockfd);
	return 0;
}




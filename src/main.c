#include "../include/socket.h"
#include "../include/decoder.h"
#include "../include/cache.h"  
#include "../include/ui.h"


#define MAXPENDING 10

int main(int argc, char *argv[]){
	


  	int port;
	int *sockfd;

	if(argc < 2 || argc > 3){
	  printf("You must run like a ./proxy <port_number> -inspec\n");
	  exit(-1);
	}
	
	if(argc == 3)
		if(!strcmp(argv[2],"-inspec"))
			inspec = true;
	

	if(pthread_mutex_init(&lock, NULL) &&
	   pthread_mutex_init(&recv_lock, NULL) && 
	   pthread_mutex_init(&resp_lock, NULL)){ 
		printf("\n mutex init failed\n");
		exit(-5);
	}
	
	if(inspec){
		init_gtk_lock();
		in_thread_gtk();
	}

	port = atoi(argv[1]);
	

//	signal(SIGINT, intHandler);
	
	//inicia a lista de controle de cache com o que ja ta em cache	
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

	//socket para receber requisiçoes
	sockfd = openSocket(port);
	if(listen(*sockfd, MAXPENDING) < 0){
    	printf("Failed in try execute socket in server");
		exit(-1);
	}

	in_thread(sockfd,inspec);

	//escrevendo em arquivo a lista de controle de cache
	cacheF = fopen(CACHE_FILE,"w");
	if(cacheF != NULL){
		for(std::string s : cache_first)
			fprintf(cacheF,"%s\n",s.c_str());	
		
		fclose(cacheF);
	}

	free(sockfd);
	return 0;
}




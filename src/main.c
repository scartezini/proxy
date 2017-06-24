#include "../include/socket.h"
#include "../include/decoder.h"
#include "../include/cache.h"  


#define MAXPENDING 20             

int main(int argc, char *argv[]){

  	int port;
	int *sockfd;

	if(argc != 2){
	  printf("You must run like a ./proxy <port_number>");
	  exit(-1);
	}

	port = atoi(argv[1]);
	//socket para receber requisiçoes
	sockfd = openSocket(port);
	
	if(listen(*sockfd, MAXPENDING) < 0){
    	printf("Failed in try execute socket in server");
		exit(-1);
	}

	in_thread(sockfd);
	return 0;
}




#include "../include/decoder.h"
#include "../include/socket.h"


#define MAXPENDING 5             

int main(int argc, char *argv[]){

  	int port;
	int *sockfd;

	if(argc != 2)
	  printf("You must insert server port number");
	
	port = atoi(argv[1]);
	sockfd = openSocket(port);
	
	
	if(listen(*sockfd, MAXPENDING) < 0){
    	printf("Failed in try execute socket in server");
		exit(-1);
	}

	in_thread(sockfd);

	free(sockfd);
	return 0;
}




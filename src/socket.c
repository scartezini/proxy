#include "../include/socket.h"

int *openSocket(int port){
 
	int *sockfd;
	struct sockaddr_in servaddr; /* Define um socket para o servidor */
  
	sockfd = (int *) malloc(sizeof(int)); 
  
	/* chama a funcao socket para especificar o tipo do protocolo */
	if((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("Failed to make socket");
		exit(-3);
	}
  
  	/*populando os dados do servidor*/
	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET;
	/* Aceita qualquer faixa de IP que a maquina possa responder. */
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(port); /* define a porta */

	/* vincula um socket a um endereco */
	if(bind(*sockfd, (Sockaddr *) &servaddr, sizeof(servaddr)) < 0){
    	printf("Failed to bind socket in server");
		exit(-4);
  	}
  
	return sockfd;
}


void in_thread(int *sockfd){
	struct sockaddr_in client; 
	socklen_t clientlen;
	int *iptr;
	pthread_t tid;
  
  	while(1){
    	iptr = (int *) malloc(sizeof(int)); 
		/* iptr aceita a escuta do cliente */
		*iptr = accept(*sockfd, (Sockaddr *) &client, &clientlen); 
		// log inet_ntoa(client.sin_addr);
		printf("\t\t----- Pedido de: %s -----\n",inet_ntoa(client.sin_addr));
		/* cria uma thread */
    	pthread_create(&tid, NULL, &start_thread, iptr);
  	}
	return;
}


void *start_thread(void *arg){
	int connfd;
  
	connfd = *((int *) arg);
	pthread_detach(pthread_self());
	start(connfd);
	close(connfd);
  
	return NULL;
}



void start(int connfd){
	char buffer[BUFFSIZE], path[BUFFSIZE],  host[HOSTSIZE], response[BUFFSIZE];
	char method[9], version[10], ip[16];
	int status, filter, n;

	if((n = recv(connfd,buffer,BUFFSIZE,0)) < 0){
		printf("Failed in receiveri\n");
		exit(-2);
	}

	buffer[n] = '\0';
  
  	//printf("%s\n", buffer);
	//TODO log
	status = decodeHTTP(buffer,path,method,version,host);
	filter = filterProxy(buffer,path,method,version,host);
	


	switch(filter){
		case 1: // whitelist
			break;
		case 2: // blacklist
			break;
		case 3: // deny_terms
			break;
		default: // pass
			request(buffer,host,response,connfd);	
		 	break;		
	}
	
	
}


int request(char* buffer,char* host, char * response,int clientfd){
	int servfd;
	char buf[100];
	
	if((servfd = establishConnection(getHostInfo(host))) == -1){
		printf("Error\n");
		return -1;
	}
	
	//char req[1000];
	//strcpy(req,"GET / HTTP/1.1\r\nHost: www.unb.br\r\n\r\n");
	if(send(servfd, buffer, strlen(buffer),0) < 0){
		printf("Error\n");
		return -1;
	}
	
	memset(response,0,BUFFSIZE);
	
	recv_timeout(servfd,4,response, clientfd);

	close(servfd);
}


int establishConnection(struct addrinfo *info){
	if(info == NULL){
		return -1; 
	}

  	int clientfd;

	for(;info != NULL;info = info->ai_next){
		if((clientfd = socket(info ->ai_family,
							info->ai_socktype,
							info->ai_protocol)) < 0){
			continue;
		}
		
		if(connect(clientfd, info->ai_addr, info->ai_addrlen) < 0){
			continue;
		}

		freeaddrinfo(info);
		return clientfd;
	}


	freeaddrinfo(info);
	return -1;
}


int dnsResolve(char* host,char* ip){
  
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
         
    if ((he = gethostbyname(host)) == NULL) {
        printf("Filed in get host name\n");
        return -1;
	}
    
 
    addr_list = (struct in_addr **) he->h_addr_list;
     
    for(i = 0; addr_list[i] != NULL; i++) {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]));
        return 0;
    }
     
    return 0;
}


struct addrinfo *getHostInfo(char *host){
	int r;
	struct addrinfo hints, *host_addrinfo;


	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if((r = getaddrinfo(host,"80",&hints,&host_addrinfo))){
		return NULL;
	}
	
	return host_addrinfo;
}


int recv_timeout(int sockfd, int timeout, char *response,int clientfd){
	int size_recv, total_size = 0;
	struct timeval begin, now;
	char chunk[CHUNK_SIZE];
	double timediff;

	//make socket non bloking 
	fcntl(sockfd, F_SETFL, O_NONBLOCK);

	gettimeofday(&begin, NULL);
	memset(response,'\0',BUFFSIZE);
	
	while(1){
		gettimeofday(&now, NULL);

		timediff = (now.tv_sec - begin.tv_sec) + 1e-6 * (now.tv_usec - begin.tv_usec);
		
		if(total_size > 0 && timediff > timeout)
			break;
		else if(timediff > timeout*2)
		   break;	

		memset(chunk, 0, CHUNK_SIZE);
		if((size_recv = recv(sockfd, chunk, CHUNK_SIZE,0)) < 0)
			usleep(100000);
		else{
			total_size += size_recv;
	//		printf("%s",chunk);
			//strcat(response,chunk);
			//printf("%s",chunk);
			if(send(clientfd,chunk,strlen(chunk),0) < 0)
				break;
			gettimeofday(&begin, NULL);
		}
	}

	return total_size;


}




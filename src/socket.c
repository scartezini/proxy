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
	char buffer[BUFFSIZE], path[BUFFSIZE],  host[HOSTSIZE];
	char method[9], version[10], ip[16];
	int status, filter, n;

	if((n = recv(connfd,buffer,BUFFSIZE,0)) < 0){
		printf("Failed in receiveri\n");
		exit(-2);
	}

	buffer[n] = '\0';
  
	//TODO lo
  	printf("%s\n", buffer);
	status = decodeHTTP(buffer,path,method,version,host);
	filter = filterProxy(buffer,path,method,version,host);
	
	if(dnsResolve(host,ip))
		return;

#if DEBUG == 1
	printf("host: %s\nip: %s\n",host,ip);
#endif


	switch(filter){
		case 1: // whitelist
			break;
		case 2: // blacklist
			break;
		case 3: // deny_terms
			break;
		default: // pass
			request(buffer,ip);	
		 	break;		
	}
	
	
}


char *request(char* buffer,char* ip){

	

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




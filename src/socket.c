#include "../include/socket.h"
#include "../include/decoder.h"
#include "../include/cache.h"  

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
	struct sockthread *sockthread;
	pthread_t tid;
  
	if(pthread_mutex_init(&lock, NULL) != 0){
		printf("\n mutex init failed\n");
		exit(-5);
	}

	while(1){
    	sockthread = (struct sockthread *) malloc(sizeof(struct sockthread)); 
		/* iptr aceita a escuta do cliente */
		sockthread->iptr = accept(*sockfd, (Sockaddr *) &client, &clientlen); 
		// log inet_ntoa(client.sin_addr);
		strcpy(sockthread->client,inet_ntoa(client.sin_addr));
		/* cria uma thread */
    	pthread_create(&tid, NULL, &start_thread, sockthread);
  	}
	return;
}


void *start_thread(void *arg){
	int connfd;
	char client[16];
	struct sockthread sockthread;
	
	sockthread = *((struct sockthread*) arg);
	connfd = sockthread.iptr;
	strcpy(client,sockthread.client);
	pthread_detach(pthread_self());
	start(connfd,client);
	close(connfd);
  
	return NULL;
}



void start(int connfd,char *client){
	char buffer[BUFFSIZE], path[BUFFSIZE],  host[HOSTSIZE];
   	char response[BUFFSIZE], file[HOSTSIZE],  cache[BUFFSIZE];
	char method[9], version[10]; 
	int status, filter, n;

	memset(buffer,'\0',BUFFSIZE);
	if((n = recv(connfd,buffer,BUFFSIZE,0)) <= 0){
		printf("Failed in receiver\n");
		return;
	}

	status = decodeHTTP(buffer,path,method,version,host);
	logMessage(client,host,path,method, 0);

	filter = filterHost(host);

	//erro na requisiçaõ retorna avisando um erro
	if(filter < 0 || status < 0){ //error
		makeHTTP(response,1);
		logMessage(client,"web-proxy","",NULL, 500);
		send(connfd,response,strlen(response),0);
		return;
	}

	//host esta na blacklist responde uma mensagem informando isso
	if(filter == 2){ // blacklist
		makeHTTP(response,2);
		logMessage(client,host,path,"blacklist", 1);
		send(connfd,response,strlen(response),0);
		return;
	}
	

	//gera o nome da requisição para indentificar em cache
	fileName(file,host,path,method,version);

	//se estiver e cache ira tratar a pag em cache
	if(!readCache(file,cache)){
		makeReqModified(buffer,cache);   //make http req
		logMessage("web-proxy",host,path,"If-Modified-Since",2);

		//faz uma requisição para verificar em a pag em cache ainda é valida
		request(buffer,host,response);	
	
		int code;
		code = grepHttpCode(response);
		logMessage("web-proxy",host,path,NULL,code);
		//se for valida usar a pag em cache
		if(code == 304) //  304 Not Modified
			strcpy(response,cache);
	}else{
		//se a pag nao estiver em cache fazer a requisição
		request(buffer,host,response);	
	}
	
	//escrever a pag em cache
	writeCache(file,response);

	//passar no filtro de termos se nao estiver na whitelist
	if(filter != 1){ // no whitelist
		//se nao passar no filtro retornar um erro
		if(filterTerms(response)){
			makeHTTP(response,3);
			logMessage(client,host,path,"whitelist", 1);
		}
	}
	

	logMessage(client,host,path,NULL,grepHttpCode(response));
	//responde para quem fez a requisição
	send(connfd,response,strlen(response),0);

	
}


int request(char* buffer,char* host, char * response){
	int servfd;
	
	if((servfd = establishConnection(getHostInfo(host))) == -1){
		printf("Failed in establishConnection\n");
		return -1;
	}
	
	if(send(servfd, buffer, strlen(buffer),0) < 0){
		printf("Error\n");
		return -1;
	}
	
	memset(response,0,BUFFSIZE);
	recv_timeout(servfd,4,response);
	close(servfd);

	return 0;
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


int recv_timeout(int sockfd, int timeout, char *response){
	int size_recv, total_size = 0;
	struct timeval begin, now;
	char chunk[CHUNK_SIZE];
	double timediff;

	//make socket non bloking 
	fcntl(sockfd, F_SETFL, O_NONBLOCK);
//	fcntl(clientfd, F_SETFL, O_NONBLOCK);

	gettimeofday(&begin, NULL);
	memset(response,'\0',BUFFSIZE);
	
	while(keepRunning){
		gettimeofday(&now, NULL);

		timediff = (now.tv_sec - begin.tv_sec) + 1e-6 * (now.tv_usec - begin.tv_usec);
		
		if(total_size > 0 && timediff > timeout)
			break;
		else if(timediff > timeout*2)
		   break;	

		memset(chunk, '\0', CHUNK_SIZE);
		if((size_recv = recv(sockfd, chunk, CHUNK_SIZE,0)) <= 0)
			usleep(100000);
		else{
			total_size += size_recv;
			strcat(response,chunk);
			gettimeofday(&begin, NULL);
		}
	}
	
	return total_size;


}

void logMessage(char const *client,char const *host,char const *path, char const  *method, int cod){
	FILE *fp;
	fp = fopen("resources/log.txt","a");
	
	switch(cod){
		case 0:
			printf("r[%s]\t%s\t-\t%s\t%s%s\n", time_system(),client,method,host,path);
			fprintf(fp, "r[%s]\t%s\t-\t%s\t%s%s\n", time_system(),client,method,host,path);
			break;
		case 1:
			printf("b[%s]\t%s\t-\t%s%s\t-\t%s\n", time_system(),client,host,path,method);
			fprintf(fp, "b[%s]\t%s\t-\t%s%s\t-\t%s\n", time_system(),client,host,path,method);
			break;
		case 2:
			printf("c[%s]\t%s\t-\t%s%s\t-\t%s\n", time_system(),client,host,path,method);
			fprintf(fp, "c[%s]\t%s\t-\t%s%s\t-\t%s\n", time_system(),client,host,path,method);
			break;
		default:
			printf("h[%s]\t%s%s\t%d\t-\t%s\n",time_system(),host,path,cod,client);
			fprintf(fp, "h[%s]\t%s%s\t%d\t-\t%s\n",time_system(),host,path,cod,client);
	}

	fclose(fp);
}

char* time2string(const struct tm *timeptr){
	static const char wday_name[][4] = {
		"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

	static const char mon_name[][4] = {
	    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
	    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	
	static char result[26];
	    sprintf(result, "%.3s %.3s%3d %.2d:%.2d:%.2d %d",
		wday_name[timeptr->tm_wday],
		mon_name[timeptr->tm_mon],
		timeptr->tm_mday, timeptr->tm_hour,
		timeptr->tm_min, timeptr->tm_sec,
		1900 + timeptr->tm_year);
	
	return result;
}


char* time_system(){
	time_t rawtime;
	struct tm * timeinfo;
	
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	return time2string(timeinfo);
}


void intHandler(int dummy) {
	    keepRunning = 0;
}


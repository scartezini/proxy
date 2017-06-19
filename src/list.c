#include "../include/list.h"


void insert(List **list, char *url, int max_len){
	
	char *pch;

	int i, flag = 0;
	List *ptr;
	List* newNode;
	newNode = (List*) malloc(sizeof(List));
	newNode->url = (char*) malloc(900*sizeof(char));
	strcpy(newNode->url,url);
	newNode->next = NULL;
	newNode->prev = NULL;


	for(ptr = *list, i=1; ptr->next != NULL; ptr = ptr->next, i++){
		printf("url: ");
		printf("%s\n",ptr->url);
		if(!strcmp(ptr->url,url)){
			pch = strstr(ptr->url,".txt");
			pch[4] = 0;
			remove(ptr->url);
			removeNode(&ptr);
			flag = 1;
			break;
		}
	}
	

	newNode->next = *list;
	(*list)->prev = newNode;
	(*list) =  newNode;

	if(i >= max_len && !flag){
		pch = strstr(ptr->url,".txt");
		pch[4] = 0;
		remove(ptr->url);
		removeNode(&ptr);
	}

}


void removeNode(List **list){
	
	if(*list == NULL)
		return;

	if((*list)->prev != NULL)
		(*list)->prev->next = (*list)->next;

	if((*list)->next != NULL)
		(*list)->next->prev = (*list)->prev;

//	free((*list)->url);
	free(*list);
}


List* find(List *list,char *url){

	List *ptr;
	ptr = list;
	
	while(ptr != NULL){
		if(!strcmp(ptr->url,url)){
			return ptr;
		}		
		ptr = ptr->next;
	}

	return NULL;

}


List* insert_last(List *list,char *url){

	List* newNode;
	newNode = (List*) calloc(1,sizeof(List));
	newNode->url = (char*) malloc(strlen(url)*sizeof(char));
	strcpy(newNode->url,url);

	if(list == NULL)
		return newNode;
	else{
		newNode->prev = list;
		list->next = newNode;

		return list;
	}

}

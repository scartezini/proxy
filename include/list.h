#include <string.h>     
#include <stdio.h>
#include <stdlib.h>

typedef struct list {
	char *url;

	struct list *next;
	struct list *prev;
} List ;


void insert(List **list, char *url, int max_len);
List* insert_last(List *list,char *url);
void removeNode(List **list);
List* find(List *list,char *url);




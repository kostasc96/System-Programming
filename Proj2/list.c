#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"



void initialize(list_pool *mylist)
{
	mylist->head = NULL;
	mylist->tail = NULL;
	mylist->size_of_list = 0;
}

void insert_pool(list_pool *mylist,int a,int b,int c,int d,char e[100],char f[100],int g,int h)
{
	node* new_node = (node*)malloc(sizeof(node));
	new_node->numofpool = a;
	new_node->pid = b;
	new_node->in_fd = c;
	new_node->out_fd = d;
	strcpy(new_node->name_in,e);
	strcpy(new_node->name_out,f);
	new_node->numofjobs = g;
	new_node->active = h;
	new_node->next = NULL;
	mylist->size_of_list++;

	if(mylist->head == NULL)
	{
		mylist->head = new_node;
		mylist->head->next = NULL;
		mylist->tail = mylist->head;
	}
	else
	{
		mylist->tail->next = new_node;
		mylist->tail = new_node;
	}
}


void print(list_pool *mylist)
{
	node* temp;
	temp = mylist->head;
	while(temp!=NULL)
	{
		printf("pid of pool %d is %d\n",temp->numofpool,temp->pid);
		temp = temp->next;
	}
	
}


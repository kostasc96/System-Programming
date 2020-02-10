#ifndef __list__
#define __list__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*typedef struct pool
{
    int numofpool;
    int pid;
    int in_fd;
    int out_fd;
    char name_in[100];
    char name_out[100];
    int numofjobs;
	int active;
}pool;
*/

typedef struct node
{
	int numofpool;
    int pid;
    int in_fd;
    int out_fd;
    char name_in[100];
    char name_out[100];
    int numofjobs;
	int active;
    struct node* next;
}node;

typedef struct list_pool
{
    node* head;
    node* tail;
    int size_of_list;
}list_pool;


void initialize(list_pool *mylist);
void insert_pool(list_pool *mylist,int a,int b,int c,int d,char e[100],char f[100],int g,int h);
void print(list_pool *mylist);



#endif

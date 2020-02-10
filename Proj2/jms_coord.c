#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include "list.h"


int main(int argc, char *argv[])
{
	int j;
	int counter_in = 0;
	int counter_out = 0;
	int counter4 = 0;
	int counter6 = 0;
	int nread;
	int out_fd;
	int num_pool = 0;
	char buffer[500];
	char command[500];
	char* token;
	char *jms_in = "jms_in.fifo";
	char *jms_out = "jms_out.fifo";
	int num_jobs;
	char pool_in[60];
	char pool_out[60];
	char in[60];
    char out[60];
	int pool_in_fd;
	int pool_out_fd;
	char path[100];
	char tempbuffer[500];
	for(j=0; j<argc; j++)
    {
		char* a=argv[j];
		if(!strcmp(a,"-l"))
		{
			strcpy(path,argv[j+1]);
		}
		if(!strcmp(a,"-n"))
		{
			num_jobs = atoi(argv[j+1]);
		}
    }
	mkdir(path, 0777);
	mkfifo(jms_in, 0777);
	mkfifo(jms_out, 0777);
	int in_fd = open(jms_in, O_RDONLY|O_NONBLOCK);
	out_fd = open(jms_out, O_WRONLY);
	
	list_pool mylist;
    initialize(&mylist);

	while(1)
	{
		memset(buffer,0,sizeof(buffer));
		if(read(in_fd,buffer,500) > 0)
		{
				char buffer2[500];
				strcpy(buffer2,buffer);
	    		token = strtok(buffer2," ");
	    		if(!strcmp(token,"submit"))
				{
					if((mylist.head == NULL)||(mylist.tail->numofjobs == num_jobs))
					{
						int pid = fork();
						sprintf(pool_in,"in_%d.fifo",num_pool);
						sprintf(pool_out,"out_%d.fifo",num_pool);
						if(pid > 0)
						{
							int n_jobs = 0;
							mkfifo(pool_in, 0777);
							mkfifo(pool_out, 0777);
							pool_in_fd = -1;
							pool_out_fd = open(pool_out, O_RDONLY|O_NONBLOCK);
							char buf[500];
						
							pool_in_fd = open(pool_in, O_WRONLY);
						
							insert_pool(&mylist,num_pool,pid,pool_in_fd,pool_out_fd,pool_in,pool_out,n_jobs,1);
							num_pool++;
						}
						else
						{
							//printf("before pool -> %s\n",pool_in);
							//printf("before pool -> %s\n",pool_out);
							char numjobs[20];
							sprintf(numjobs,"%d",num_jobs);
							char numpool1[20];
							sprintf(numpool1,"%d",num_pool);
							execl("pool","-i",pool_in,"-o",pool_out,"-n",numjobs,"-m",numpool1,"-l",path,NULL);
						}
					}
				
				//printf("list values -> %d\n",mylist.tail->in_fd);
				//printf("list values -> %d\n",mylist.tail->out_fd);
				//printf("Num of jobs of tail: %d\n",mylist.tail->info.numofjobs);
				write(mylist.tail->in_fd,buffer,500);		 		
				}
				else if(!strcmp(token,"status"))
				{
					char tempbuffer2[500];
					strcpy(tempbuffer2,buffer);
					token = strtok(NULL,"\0");
					int pos = atoi(token);
					node* current;
					current = mylist.head;
					while(current!=NULL)
					{
						if((pos>=(current->numofpool)*num_jobs)&&(pos<(current->numofpool)*num_jobs + num_jobs)&&(current->active == 1))   //meta 8a tsekarw an pool active edw
						{
							write(current->in_fd,tempbuffer2,500);
						}
						current = current->next;
					}
				}
				else if(!strcmp(token,"status-all"))
				{
					char tempbuffer3[500];
					strcpy(tempbuffer3,buffer);
					node* current2;
					current2 = mylist.head;
					int nj = num_jobs*(num_pool-1) + mylist.tail->numofjobs;
					char tmp3[500];
					sprintf(tmp3,"all,%d",nj);
					write(out_fd,tmp3,500);
					while(current2!=NULL)
					{
						write(current2->in_fd,tempbuffer3,500);
						current2 = current2->next;
					}
				}
				else if(!strcmp(token,"show-active"))
				{
					char tempbuffer4[500];
					strcpy(tempbuffer4,buffer);
					node* current3;
					current3 = mylist.head;
					write(out_fd,"show-active",500);
					while(current3!=NULL)
					{
						write(current3->in_fd,tempbuffer4,500);
						current3 = current3->next;
					}
				}
				else if(!strcmp(token,"show-finished"))
				{
					char tempbuffer44[500];
					strcpy(tempbuffer44,buffer);
					node* current4;
					current4 = mylist.head;
					write(out_fd,"show-finished",500);
					while(current4!=NULL)
					{
						write(current4->in_fd,tempbuffer44,500);
						current4 = current4->next;
					}
				}
				else if(!strcmp(token,"show-pools"))
				{
					char tempbuffer5[500];
					strcpy(tempbuffer5,buffer);
					char tmpbf6[500];
					memset(tmpbf6,0,500);
					sprintf(tmpbf6,"show-pools,%d",mylist.size_of_list);
					write(out_fd,tmpbf6,500);
					node* current4;
					current4 = mylist.head;
					while(current4!=NULL)
					{
						write(current4->in_fd,tempbuffer5,500);
						current4 = current4->next;
					}
				}
				else if(!strcmp(token,"suspend"))
				{
					char tempbuffer7[500];
					strcpy(tempbuffer7,buffer);
					token = strtok(NULL,"\0");
					int suspend = atoi(token);
					node* current7;
					current7 = mylist.head;
					while(current7!=NULL)
					{
						if((suspend>=(current7->numofpool)*num_jobs)&&(suspend<(current7->numofpool)*num_jobs + num_jobs)&&(current7->active == 1))   //meta 8a tsekarw an pool active edw
						{
							write(current7->in_fd,tempbuffer7,500);
						}
						current7 = current7->next;
					}
				}
				else if(!strcmp(token,"resume"))
				{
					char tempbuffer8[500];
					strcpy(tempbuffer8,buffer);
					token = strtok(NULL,"\0");
					int resume = atoi(token);
					node* current8;
					current8 = mylist.head;
					while(current8!=NULL)
					{
						if((resume>=(current8->numofpool)*num_jobs)&&(resume<(current8->numofpool)*num_jobs + num_jobs)&&(current8->active == 1))   //meta 8a tsekarw an pool active edw
						{
							write(current8->in_fd,tempbuffer8,500);
						}
						current8 = current8->next;
					}
				}
				
				node* cur;
				cur = mylist.head;
				while(cur!=NULL)
				{
					write(cur->in_fd,"allow",6);	
					cur = cur->next;
				}
				
		}
		else
		{
				node* temp;
				temp = mylist.head;
				while(temp!=NULL)
				{
					char buf2[500];
					memset(buf2,0,sizeof(buf2));
					if((read(temp->out_fd,buf2,500)) > 0)
					{
						char *tempbuf = strdup(buf2);
						tempbuf = strtok(tempbuf, ",");
						char *tempbuf2;
						tempbuf2 = strtok(NULL, "\0");
						char tempbuf3[500];
						memset(tempbuf3,0,500);
						if(tempbuf2 !=NULL)
						{
							strcpy(tempbuf3,tempbuf2);
						}
						if(!strcmp(tempbuf,"forconsole"))
						{
							//out_fd = open(jms_out, O_WRONLY);
							temp->numofjobs++;
							//printf("Num of jobs in pool: %d\n",temp->info.numofjobs);
							write(out_fd,tempbuf3,500);
							//close(out_fd);
						}
						else if(!strcmp(tempbuf,"forconsole2"))
						{
							write(out_fd,tempbuf3,500);
						}
						else if(!strcmp(tempbuf,"forconsole3"))
						{
							write(out_fd,tempbuf3,500);
						}
						else if(!strcmp(tempbuf,"forconsole4"))
						{
							counter4++;
							char tmpb3[500];
							if(tempbuf2 != NULL)
							{
								strcpy(tmpb3,tempbuf2);
								char* tmpb4;
								tmpb4 = strtok(tmpb3,",");
								while(tmpb4!=NULL)
								{
									int nact = atoi(tmpb4);
									char tmpb5[500];
									sprintf(tmpb5,"JobID %d\n",nact);
									write(out_fd,tmpb5,500);
									tmpb4 = strtok(NULL, ",");
								}
							}
							if(counter4 == mylist.size_of_list)
							{
								write(out_fd,"end4",500);
							}
						}
						else if(!strcmp(tempbuf,"forconsole5"))
						{
							write(out_fd,tempbuf3,500);
						}
						else if(!strcmp(tempbuf,"forconsole6"))
						{
							counter6++;
							char tmpb33[500];
							if(tempbuf2 != NULL)
							{
								strcpy(tmpb33,tempbuf2);
								char* tmpb44;
								tmpb44 = strtok(tmpb33,",");
								while(tmpb44!=NULL)
								{
									int nact2 = atoi(tmpb44);
									char tmpb55[500];
									sprintf(tmpb55,"JobID %d",nact2);
									printf("%s\n",tmpb55);
									write(out_fd,tmpb55,500);
									tmpb44 = strtok(NULL, ",");
								}
							}
							if(counter6 == mylist.size_of_list)
							{
								write(out_fd,"end6",500);
							}
						}
						else if(!strcmp(tempbuf,"forconsole7"))
						{
							write(out_fd,tempbuf3,500);
						}
						else if(!strcmp(tempbuf,"forconsole8"))
						{
							write(out_fd,tempbuf3,500);
						}
						else if(!strcmp(buf2,"Done"))
						{
							//printf("Pool completed\n");
							temp->active = 0;
						}
					}
					temp = temp->next;
				}
		}

	}
	
	return 0;
}

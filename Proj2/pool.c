#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>



int main(int argc, char *argv[])
{
    char in[50];
    char out[50];
	char buffer[500];
    int j;
	int i=0;
	int n=0;
	int finished = 0;
    int in_fd,out_fd;
	int njobs;
	int* active;
	int* pidjobs;
	int status,endID;
	int k;
	int poolid;
	char path[50];
	

    for(j=0; j<argc; j++)
    {
		char* a=argv[j];
		if(!strcmp(a,"-i"))
		{
			strcpy(in,argv[j+1]);
		}
		if(!strcmp(a,"-o"))
		{
			strcpy(out,argv[j+1]);
		}
		if(!strcmp(a,"-n"))
		{
			njobs = atoi(argv[j+1]);
		}
		if(!strcmp(a,"-m"))
		{
			poolid = atoi(argv[j+1]);
		}
		if(!strcmp(a,"-l"))
		{
			strcpy(path,argv[j+1]);
		}
    }

	active = malloc(njobs*sizeof(int));
	pidjobs = malloc(njobs*sizeof(int));

	
	for(n=0;n<njobs;n++)
	{
		active[n] = 50;
	}
	//print

   // printf("in pool -> %s\n",in);
   // printf("in pool -> %s\n",out);
   //printf("max num jobs: %d\n",njobs);


    in_fd = open(in, O_RDONLY|O_NONBLOCK);
    out_fd = open(out, O_WRONLY);
	
	
	//write(out_fd,"connection",11);
	
	
	while(1)
	{
		memset(buffer,0,sizeof(buffer));
		if((read(in_fd,buffer,500) > 0))
		{
			char *buffer2 = strdup(buffer);
			buffer2 = strtok(buffer2, " ");
			char *buffer3;
			buffer3 = strtok(NULL, "\0");
			int size_array = 0;
			
			if(!strcmp(buffer2,"submit"))
			{
				char** command;
				command = (char**)malloc(sizeof(char*));
				char* token = strtok(buffer3," ");
				while(token != NULL)
				{
					command = (char**)realloc(command,(size_array+1)*sizeof(char*));
					command[size_array] = strdup(token);
					size_array++;
					token = strtok(NULL, " ");
				}
			
			
			int pid = fork();
			char buf[500];
			
			if(pid > 0)
			{
				active[i] = 1;
				pidjobs[i] = pid;
				sprintf(buf,"forconsole,JobID:%d,PID:%d",i,pid);
				i++;
			}
			else
			{
				char res_out[500];
				sprintf(res_out,"%ssdi1400229_%d_%d/",path,i,getpid());
				mkdir(res_out,0777);
				sprintf(res_out,"%ssdi1400229_%d_%d/out_%d",path,i,getpid(),i);
				char err[500];
				sprintf(err, "%ssdi1400229_%d_%d/err_%d",path,i,getpid(),i);
				int fd_path_out = open(res_out, O_RDWR|O_CREAT, 0666);
				int fd_err = open(err, O_RDWR|O_CREAT, 0666);
				dup2(fd_path_out, 1);
				dup2(fd_err, 2);
				close(fd_path_out);
				close(fd_err);
				execvp(command[0],command);
			}
			
			write(out_fd,buf,29);
			
			int l;
			for(l=0;l<size_array;l++)
			{
				free(command[l]);
			}
			free(command);
		}
		else if(!strcmp(buffer2,"status"))
		{
			char tempb[500];
			int logicid2;
			logicid2 = atoi(buffer3);
			int logicid1; 
			if(poolid!=0)
			{
				logicid1 = (logicid2%poolid)*njobs;
			}
			else
			{
				logicid1 = logicid2;
			}
			if(active[logicid1] == 1)                                               //tsekarw an einai active h oxi to sugkekrimeno job mesa ap ton pinaka pou exw ftia3ei kai meta ftiaxnw to munhma opws to zhtaei h ekfwnish
			{
				sprintf(tempb,"forconsole2,JobID %d Status: Active",logicid2);
				write(out_fd,tempb,500);
			}
			else if(active[logicid1] == 2)
			{
				sprintf(tempb,"forconsole2,JobID %d Status: Suspended",logicid2);
				write(out_fd,tempb,500);
			}
			else 
			{
				sprintf(tempb,"forconsole2,JobID %d Status: Finished",logicid2);
				write(out_fd,tempb,500);
			}
		}
		else if(!strcmp(buffer2,"suspend"))
		{
			char tempb7[500];
			int logicid22;
			logicid22 = atoi(buffer3);
			int logicid11; 
			if(poolid!=0)
			{
				logicid11 = (logicid22%poolid)*njobs;
			}
			else
			{
				logicid11 = logicid22;
			}
			kill(pidjobs[logicid11], SIGSTOP);
			active[logicid11] = 2;
			sprintf(tempb7,"forconsole7,Sent suspend signal to JobID %d",logicid22);
			write(out_fd,tempb7,500);
		}
		else if(!strcmp(buffer2,"resume"))
		{
			char tempb8[500];
			int logicid222;
			logicid222 = atoi(buffer3);
			int logicid111; 
			if(poolid!=0)
			{
				logicid111 = (logicid222%poolid)*njobs;
			}
			else
			{
				logicid111 = logicid222;
			}
			kill(pidjobs[logicid111], SIGCONT);
			active[logicid111] = 0;
			sprintf(tempb8,"forconsole8,Sent resume signal to JobID %d",logicid222);
			write(out_fd,tempb8,500);
		}
		else if(!strcmp(buffer2,"status-all"))
		{
			char tempb2[500];
			int k;
			for(k=0;k<i;k++)
			{
				int idjob = poolid*njobs + k;
				if(active[k] == 1)
				{
					sprintf(tempb2,"forconsole3,JobID %d Status: Active",idjob);
				}
				else if(active[k] == 2)
				{
					sprintf(tempb2,"forconsole3,JobID %d Status: Suspended",idjob);
				}
				else
				{
					sprintf(tempb2,"forconsole3,JobID %d Status: Finished",idjob);
				}
				write(out_fd,tempb2,500);
			}
		}
		else if(!strcmp(buffer2,"show-active"))
		{
			char act1[500];
			memset(act1,0,500);
			strcat(act1,"forconsole4,");
			int w;
			for(w=0;w<i;w++)
			{
				if(active[w] == 1)
				{
					char act3[500];
					sprintf(act3,"%d,",w);
					strcat(act1,act3);
				}
			}
			//printf("act: %s\n",act1);
			write(out_fd,act1,500);
		}
		else if(!strcmp(buffer2,"show-finished"))
		{
			char act11[500];
			memset(act11,0,500);
			strcat(act11,"forconsole6,");
			int ww;
			for(ww=0;ww<i;ww++)
			{
				if(active[ww] == 0)
				{
					char act33[500];
					sprintf(act33,"%d,",ww);
					strcat(act11,act33);
				}
			}
			write(out_fd,act11,500);
		}
		else if(!strcmp(buffer2,"show-pools"))
		{
			int cnt = 0;
			char pools1[500];
			memset(pools1,0,500);
			strcat(pools1,"forconsole5,");
			int ii;
			for(ii=0;ii<i;ii++)
			{
				if(active[ii] == 1)
				{
					cnt++;
				}
			}
			char pools2[500];
			memset(pools2,0,500);
			sprintf(pools2,"%d %d",getpid(),cnt);
			strcat(pools1,pools2);
			write(out_fd,pools1,500);
		}
		else if((finished == njobs)&&(!strcmp(buffer2,"allow")))
		{
			write(out_fd,"Done",5);
		}
		}
		else
		{
			for(k=0;k<i;k++)
			{
				endID = waitpid(pidjobs[k], &status, WNOHANG|WUNTRACED);
				if (endID == pidjobs[k])
				{
					finished++;
					active[k] = 0;
				}
			}
		}

	}

    return 0;
}

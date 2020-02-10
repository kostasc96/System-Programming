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
	int j;
	char jms_in[50];
	char jms_out[50];
	char op_file[50];
	for(j=0; j<argc; j++)
    {
		char* a=argv[j];
		if(!strcmp(a,"-w"))
		{
			strcpy(jms_in,argv[j+1]);
		}
		if(!strcmp(a,"-r"))
		{
			strcpy(jms_out,argv[j+1]);
		}
		if(!strcmp(a,"-o"))
		{
			strcpy(op_file,argv[j+1]);
		}
    }
	
	char buf1[500];
	char buf2[500];
	char buf[500];
	char buf4[500];
	char buf5[500];
	char buf6[500];
	int in_fd = open(jms_in, O_WRONLY);
	int out_fd = open(jms_out, O_RDONLY|O_NONBLOCK);
	
	
		
	while(fgets(buf1, sizeof(buf1), stdin) != NULL)
	{
		buf1[strlen(buf1)-1] = 0;
		write(in_fd,buf1,500); 
		memset(buf2,0,sizeof(buf2));
		//while(read(out_fd,buf2,500) == 0);
		while(1)
		{
			if(read(out_fd,buf2,500) > 0)
			{
				char temp[500];
				strcpy(temp,buf2);
				char* token;
				token = strtok(temp,",");
				if(!strcmp(token, "all"))
				{
					token = strtok(NULL,"\0");
					int num = atoi(token);
					int i;
					for(i=0;i<num;i++)
					{
						while(1)
						{
							memset(buf, 0, 500);
							if(read(out_fd,buf,500) > 0)
							{
								printf("%d. %s\n", i+1, buf);
								break;
							}
						}
					}
					break;
				}
				else if(!strcmp(temp, "show-active"))
				{
					while(1)
					{
						while(1)
						{
							memset(buf4, 0, 500);
							if(read(out_fd,buf4,500) > 0)
							{
								if(!strcmp(buf4,"end4"))
								{
									printf("%s\n",buf4);
								}
								break;
							}
						}
						if(!strcmp(buf4,"end4"))
						{
							break;
						}
					}
					break;
				}
				else if(!strcmp(token, "show-pools"))
				{
					token = strtok(NULL,"\0");
					int num2 = atoi(token);
					int ii;
					for(ii=0;ii<num2;ii++)
					{
						while(1)
						{
							memset(buf, 0, 500);
							if(read(out_fd,buf5,500) > 0)
							{
								printf("%d. %s\n", ii+1, buf5);
								break;
							}
						}
					}
						break;
				}
				else if(!strcmp(token, "show-finished"))
				{
					while(1)
					{
						while(1)
						{
							memset(buf6, 0, 500);
							if(read(out_fd,buf6,500) > 0)
							{
								if(!strcmp(buf6,"end6"))
								{
									printf("%s\n",buf6);
								}
								break;
							}
						}
						if(!strcmp(buf4,"end6"))
						{
							break;
						}
					}
					break;
				}
			
				
				printf("%s\n",buf2);
				break;
			}
		}
	}
	
	return 0;
}
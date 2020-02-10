# include <stdio.h>
# include <sys/wait.h> /* sockets */
# include <sys/types.h> /* sockets */
# include <sys/socket.h> /* sockets */
# include <netinet/in.h> /* internet sockets */
# include <netdb.h> /* ge th os tb ya dd r */
# include <unistd.h> /* fork */
# include <stdlib.h> /* exit */
# include <ctype.h> /* toupper */
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>
#define perror2(s , e) fprintf(stderr,"%s: %s\n",s,strerror(e))


char* arraydir[500];
int counterd = 0;

void perror_exit(char* message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

void discover_files(char* file)
{
	struct stat statbuf;
	DIR *dir_ptr;
	struct dirent *direntp;
	
	if(stat(file, &statbuf) ==  -1)
	{
		perror ("Failed to get file status");
		exit(2);
	}
	
	printf("%s\n",file);
	arraydir[counterd] = malloc(sizeof(char)*(strlen(file)+1));
	strcpy(arraydir[counterd],file);
	counterd++;
	
	
	if((statbuf.st_mode & S_IFMT) ==  S_IFREG)
	{
		printf("Regular file\n");
	}
	else
		printf("Directory\n");
		
	
	if(S_ISDIR(statbuf.st_mode))
	{	
		dir_ptr = opendir(file);	
		while((direntp=readdir(dir_ptr)) != NULL)
		{
			if(strcmp(direntp->d_name, ".") && strcmp(direntp->d_name, ".."))
			{
				char buf[128];
				strcpy(buf, file);
				strcat(buf, "/");
				strcat(buf, direntp->d_name);
				discover_files(buf);
			}
		}
		closedir(dir_ptr);
	}
}


void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
  
    //Free the socket pointer
	
	int sizestr;
	char buffer[500];
  
	if(read(sock,buffer,2)< 0)
	{
			printf("char error read\n");
			perror_exit("error read\n");
	}
	
	
	if(read(sock,(void*)&sizestr,sizeof(int)) < 0)
	{
		printf("int error read\n");
		perror_exit("error read\n");
	}
   
    int i;
	
	for(i=0;i<sizestr;i++)
	{
		if(read(sock,buffer + i,sizeof(char)) < 0)
		{
			printf("char error read\n");
			perror_exit("error read\n");
		}
	}
	
	int id,delay;
	char dirfname[500];
	char *end_str;
	//printf("buffer content server %s\n",buffer);
	char *token = strtok_r(buffer," ", &end_str);
	if(!strcmp(token,"LIST"))
	{
		token = strtok_r(NULL," ", &end_str);
		id = atoi(token);
		token = strtok_r(NULL," ", &end_str);
		delay = atoi(token);
		//printf("id %d delay %d\n",id,delay);		
		
		char buffer[500];
		if(read(sock,buffer,2)< 0)
		{
			printf("char error read\n");
			perror_exit("error read\n");
		}  
		
		int numoffiles = counterd;
		
		if(write(sock,(void*)&numoffiles,sizeof(int)) < 0)
		{
			printf("int write error\n");
			perror_exit("error write\n");
		}
	
		int j;
		for(i=0;i<counterd;i++)
		{
			int size = strlen(arraydir[i]);
			if(write(sock,(void*)&size,sizeof(int)) < 0)
			{
				printf("int write error\n");
				perror_exit("error write\n");
			}
			for(j=0;arraydir[i][j]!=0;j++)
			{
				if(write(sock,arraydir[i] + j,sizeof(char)) < 0)
				{
					printf("char write error\n");
					perror_exit("error write\n");
				}
			}
			write(sock,"b",2);
			if(read(sock,buffer,2)< 0)
			{
				printf("char error read\n");
				perror_exit("error read\n");
			}  
		}
		
		
	}
	else if(!strcmp(token,"FETCH"))
	{
		token = strtok_r(NULL," ", &end_str);
		strcpy(dirfname,token);
		printf("dirfname %s\n",dirfname);
	}
	
	
	
    free(socket_desc);
     
    return 0;
}


void join_threads(pthread_t* tids,int n)
{
	int err;
	int i;
	for(i=0;i<n;i++) 
	{
		if(err = pthread_join(*(tids + i), NULL )) 
		{
			perror2("pthread_join", err);
			exit(1);
		}
	}
}



int main(int argc , char *argv[])
{
	int port;
	int j;
	char* dirfname;
    for(j=0; j<argc; j++)
    {
		char* a=argv[j];
		if(!strcmp(a,"-p"))
		{
			port = atoi(argv[j+1]);
		}
		if(!strcmp(a,"-d"))
		{
			dirfname = argv[j+1];
		}
    }
	
	
	int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
	
	//Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
	
	server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);        
	
	if(bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
	
	listen(socket_desc , 3);
     
    
    puts("Waiting for incoming connections...");
   // c = sizeof(struct sockaddr_in);
   
   socklen_t clientc = sizeof(server);
	
	pthread_t* threads = malloc(sizeof(pthread_t));
	int tcounter = 0;
	
	
	discover_files(dirfname);
	
	int k;
	printf("array files:\n");
	for(k=0;k<counterd;k++)
	{
		printf("%s\n",arraydir[k]);
	}
	
	while((client_sock = accept(socket_desc, (struct sockaddr *)&client,&clientc)))
    {
        puts("Connection accepted");
         
		 
		threads = realloc(threads,sizeof(pthread_t)*(tcounter+1));
		
        new_sock = malloc(1);
        *new_sock = client_sock;
         
        if(pthread_create(&threads[tcounter] , NULL ,connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
		tcounter++; 
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }
     
    
	
	
	if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
	
	join_threads(threads,tcounter+1);
     
     
	
	return 0;
}





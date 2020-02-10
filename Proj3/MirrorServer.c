#include <stdio.h>
#include <sys/wait.h> /* sockets */
#include <sys/types.h> /* sockets */
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <netdb.h> /* ge th os tb ya dd r */
#include <unistd.h> /* fork */
#include <stdlib.h> /* exit */
#include <ctype.h> /* toupper */
#include <string.h>
#include <pthread.h>
#define MAX_SLEEP 10
#define perror2(s , e) fprintf(stderr,"%s: %s\n",s,strerror(e))
#define POOL_SIZE 6

char ***inf;
int numinfo = 0;


typedef struct info{
	char* filename;
	int port;
	char* address;
	int directory;   //poio filename einai apo to sugkekrimeno directory 
	int id;    //se poio thread to diaxeiristike
	int counter; //posa filename prepei na perastoun apo thn sugkekrimenh sundesh  
}infot;

typedef struct{
	infot data[POOL_SIZE];
	int start;
	int end;
	int count;
}pool_t;

pthread_mutex_t mtx;
pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;
pool_t pool;
int numdevicesdone = 0;

void initialize(pool_t* pool)
{
	pool->start = 0;
	pool->end = -1;
	pool->count = 0;
}

void place(pool_t* pool,infot data) 
{
	pthread_mutex_lock(&mtx);
	while(pool->count >= POOL_SIZE) 
	{
		printf(">>Found Buffer Full\n");
		pthread_cond_wait(&cond_nonfull ,&mtx);
	}
	pool->end = (pool->end + 1) % POOL_SIZE;
	pool->data[pool->end] = data;
	pool->count++;
	pthread_mutex_unlock(&mtx);
}

infot obtain(pool_t* pool) 
{
	infot data;
	pthread_mutex_lock(&mtx);
	while(pool->count <= 0) 
	{
		printf(" >> Found Buffer Empty \n");
		pthread_cond_wait(&cond_nonempty,&mtx);
	}
	data = pool->data[pool->start];
	if(data.directory == data.counter - 1)
	{
		numdevicesdone++;
	}
	pool->start = (pool->start + 1) % POOL_SIZE;
	pool->count--;
	pthread_mutex_unlock(&mtx);
	return data;
}

void perror_exit(char* message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

void* sleeping(void* arg) 
{
	long int sl =(long)arg;
	printf("thread % ld sleeping % ld seconds ...\n",pthread_self(),sl);
	sleep(sl); /* Sleep a number of seconds */
	printf("thread % ld waking up \n",pthread_self());
	pthread_exit(NULL);
}


void* worker(void* ptr)
{
	int id = (int)ptr;
	while(numdevicesdone < numinfo) 
	{
		infot info = obtain(&pool);
		printf("%s\n",info.filename);
		pthread_cond_signal(&cond_nonfull);
		usleep(500000);
	}
	pthread_exit(0) ;
}



void* threadmanager(void* arg)                      //mirrormanager
{
	int sock;
	struct sockaddr_in server ;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct hostent *rem ;
	
	int id = (int)arg;
	int port = atoi(inf[id][1]);
    /* Create socket */
    if((sock = socket(AF_INET , SOCK_STREAM,0)) < 0)
    perror_exit("socket");
    /* Find server address */
    if((rem = gethostbyname(inf[id][0])) == NULL)
    {
        herror("gethostbyname");
        exit(1);
    }

    server.sin_family = AF_INET ; /* Internet domain */
    memcpy(&server.sin_addr, rem->h_addr,rem->h_length);
    server.sin_port = htons(port); /* Server port */
    /* Initiate connection */
    if(connect(sock , serverptr , sizeof(server)) < 0)
        perror_exit(" connect ");
    printf("Connecting to %s port %d\n " , inf[id][0] , port);
	
	char buf[500];
	sprintf(buf,"LIST %d %s",id,inf[id][3]);
	
	int sizebuf = strlen(buf);
	
	write(sock,"a",2);
    
    if(write(sock,(void*)&sizebuf,sizeof(int)) < 0)
    {
        printf("int write error\n");
		perror_exit("error write\n");
    }
	
	int i;
	
	for(i=0;buf[i]!=0;i++)
    {
		if(write(sock,buf + i,sizeof(char)) < 0)
    	{
            printf("char write error\n");
			perror_exit("error write\n");
    	}
    }

	write(sock,"b",2);
	
	
	int numoffiles;
	if(read(sock,(void*)&numoffiles,sizeof(int)) < 0)
	{
		printf("int write error\n");
		perror_exit("error write\n");
	}
	
	char buffer[500];
	char **filesarray;
	filesarray = malloc(sizeof(char*)*numoffiles);
	int size;
	int j,k;
	for(j=0;j<numoffiles;j++)
	{
		if(read(sock,(void*)&size,sizeof(int)) < 0)
		{
			printf("int write error\n");
			perror_exit("error write\n");
		}
		filesarray[j] = malloc(sizeof(char)*(size+1));
		for(k=0;k<size;k++)
		{
			if(read(sock,filesarray[j] + k,sizeof(char)) < 0)
			{
				printf("char write error\n");
				perror_exit("error write\n");
			}
		}
		filesarray[size] = "\0";
		if(read(sock,buffer,2)< 0)
		{
			printf("char error read\n");
			perror_exit("error read\n");
		} 
		write(sock,"b",2);
	}
	
	for(j=0;j<numoffiles;j++)
	{
		printf("%s\n",filesarray[j]);
	}
	
	infot data;
	
	for(j=0;j<numoffiles;j++)
	{
		data.filename = strdup(filesarray[j]); 
		data.address = strdup(inf[id][0]);
		data.port = port;
		data.directory = j;
		data.id = id;
		data.counter = numoffiles;
	}
	
	place(&pool,data);
	pthread_cond_signal(&cond_nonempty);
	usleep(0) ;


	close(sock);
	
	pthread_exit(NULL);
}

void create_threads(pthread_t* tids,int n)
{
	int err;
	long int sl;
	int i;
	for(i=0;i<n;i++) 
	{
		//sl = random() % MAX_SLEEP + 1; /* Sleeping time 1.. MAX_SLEEP */
		if(err = pthread_create(tids +i, NULL, threadmanager, (void *)i)) 
		{
			perror2("pthread_create", err);
			exit(1);
		}
	}
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


void create_worker_threads(pthread_t* tids,int n)
{
	int err;
	long int sl;
	int i;
	for(i=0;i<n;i++) 
	{
		//sl = random() % MAX_SLEEP + 1; /* Sleeping time 1.. MAX_SLEEP */
		if(err = pthread_create(tids +i, NULL, worker, (void *)i)) 
		{
			perror2("pthread_create", err);
			exit(1);
		}
	}
}


void join_worker_threads(pthread_t* tids,int n)
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



int main(int argc,char *argv[])
{
	int workers;
    int port, sock, newsock, i;
    //char ***inf;
    char *dirfname;
    int j;
    for(j=0; j<argc; j++)
    {
		char* a=argv[j];
		if(!strcmp(a,"-p"))
		{
			port = atoi(argv[j+1]);
		}
		if(!strcmp(a,"-m"))
		{
			dirfname = argv[j+1];
		}
		if(!strcmp(a,"-w"))
		{
			workers = atoi(argv[j+1]);
		}
    }
    struct sockaddr_in server,client ;
    socklen_t clientlen;
    struct sockaddr *serverptr =(struct sockaddr *)&server ;
    struct sockaddr *clientptr =(struct sockaddr *)&client ;
    struct hostent *rem ;

    /* Create socket */
    if (( sock = socket( AF_INET , SOCK_STREAM , 0) ) < 0)
        perror_exit(" socket ");
    server.sin_family = AF_INET; /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY) ;
    server.sin_port = htons(port); /* The given port */
    /* Bind socket to address */
    
    if(bind(sock , serverptr , sizeof(server )) < 0)
        perror_exit(" bind " );
    /* Listen for connections */
    if(listen(sock , 5) < 0)
        perror_exit( " listen ") ;
    printf(" Listening for connections to port %d\n" , port );


    while(1)
    {
		/* accept connection */
		if((newsock = accept(sock,clientptr,&clientlen)) < 0)
			perror_exit("accept");
		/* Find client s address */
		//   if((rem = gethostbyaddr((char *)&client.sin_addr.s_addr,sizeof(client.sin_addr.s_addr),client.sin_family)) == NULL )
		//  {
		//      herror("gethostbyaddr");
		//      exit(1);
		//  }
		//  printf("Accepted connection from %s\n",rem->h_name);
		printf (" Accepted connection \n ");
    
		int sizestr;
		char buffer[500];
  
		if(read(newsock,(void*)&sizestr,sizeof(int)) < 0)
		{
			printf("int error read\n");
			perror_exit("error read\n");
		}
    
		printf("size of str: %d\n",sizestr);
   
		for(i=0;i<sizestr;i++)
		{
			if(read(newsock,buffer + i,sizeof(char)) < 0)
			{
				printf("char error read\n");
				perror_exit("error read\n");
			}
		}

		//printf("buffer %s\n",buffer);
		//printf("len of str is: %d\n",sizestr);
		//int numinfo = 0;
		i = 0;
		char *end_str;
		char temp[500];
		strcpy(temp,buffer);
		inf = malloc(sizeof(char**));
		char *token = strtok_r(temp, ",", &end_str);
		while(token != NULL)
		{
			inf = realloc(inf, sizeof(char**)*(i+1));
            inf[i] = malloc(4*sizeof(char*));
			int k=0;
			char *end_token;
            char *token2 = strtok_r(token, ":", &end_token);
			while(token2 != NULL)
            {
                inf[i][k] = strdup(token2);
                k++;

                token2 = strtok_r(NULL, ":", &end_token);
            }
			numinfo++;
			i++;
			token = strtok_r(NULL, ",", &end_str);
		} 

	/*	printf("info\n");
		int l,ll;
		for(l=0;l<numinfo;l++)
		{
			for(ll=0;ll<4;ll++)
			{
				printf("%s\n",inf[l][ll]);
			}
		}
	*/
		

		int n,ii,err;
		long sl;
		pthread_t* tids;
		pthread_t* tids2;
		
		n = numinfo;
		//printf("numinfo %d\n",numinfo);
		
		if((tids = malloc(n * sizeof(pthread_t))) == NULL) 
		{
			perror("malloc\n");
			exit(1);
		}
		
		if((tids2 = malloc(workers * sizeof(pthread_t))) == NULL) 
		{
			perror("malloc\n");
			exit(1);
		}
		
		srandom((unsigned int)time(NULL));
		
		pthread_t cons,prod;
		initialize(&pool);
		pthread_mutex_init(&mtx,0);
		pthread_cond_init(&cond_nonempty,0);
		pthread_cond_init(&cond_nonfull,0);
		
		create_threads(tids,n);
		create_worker_threads(tids2,workers);
		
		join_threads(tids,n);
		join_worker_threads(tids2,workers);
		
		pthread_cond_destroy(&cond_nonempty);
		pthread_cond_destroy(&cond_nonfull);
		pthread_mutex_destroy(&mtx);


    
		int times,times2;
		for(times=0;times<numinfo;times++)
		{
			for(times2=0;times2<times;times2++)
			{
				free(inf[times][times2]);
			}
			free(inf[times]);
		}
		free(inf);
		
		close(newsock);
    }

    close(sock);
    return 0;
}

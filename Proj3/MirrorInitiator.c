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

void perror_exit(char* message)
{
    perror(message);
    exit(EXIT_FAILURE);
}


int main(int argc,char *argv [])
{
    int port,sock,i;
    char *address;
    char *str;
    int j;
    for(j=0; j<argc; j++)
    {
		char* a=argv[j];
		if(!strcmp(a,"-p"))
		{
			port = atoi(argv[j+1]);
		}
		if(!strcmp(a,"-s"))
		{
			str = argv[j+1];
		}
		if(!strcmp(a,"-n"))
		{
			address = argv[j+1];
		}
    }
    char buf[256];
    struct sockaddr_in server ;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct hostent *rem ;


    /* Create socket */
    if((sock = socket(AF_INET , SOCK_STREAM,0)) < 0)
    perror_exit("socket");
    /* Find server address */
    if((rem = gethostbyname(address)) == NULL)
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
    printf("Connecting to %s port %d\n " , address , port);

    int sizestr = strlen(str) + 1;
    
    if(write(sock,(void*)&sizestr,sizeof(int)) < 0)
    {
        printf("int write error\n");
		perror_exit("error write\n");
    }

    //printf("size str: %d\n",sizestr);

    for(i=0;str[i]!=0;i++)
    {
	if(write(sock,str + i,sizeof(char)) < 0)
    	{
            printf("char write error\n");
			perror_exit("error write\n");
    	}
    }

    close(sock); /* Close socket and exit */




    return 0;
}

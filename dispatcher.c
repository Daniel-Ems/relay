#define _XOPEN_SOURCE
#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h> 
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>

#define SOCK_PATH "echo_socket"

//Needs to be volatile because it is affected outside the program. 

volatile int what;
volatile bool exit_run;
volatile pthread_t fgets_thread;
char str[4096];
char buf[4096];
int file_array[100];
int i = 0;
int s;

//Allows for proper exit.
void signal_handler(int signal_name)
{
        close(what);
        printf("closed appropriately\n");
        exit(1); 
}


void *fgets_function(void *arg)
{
    int *value;
    value = (int*)1;
    for(;;)
    {
        if(fgets(buf, sizeof(buf), stdin)==NULL)
        {
           shutdown(s, SHUT_RDWR);
           //close(s);
           pthread_exit(NULL);
        }
        else
        {
            for(int a = 0; a <= i; a++)
            { 
                send(file_array[a], buf, strlen(buf) +1, 0);
            }
        }
    }
    close(what);
    close(s);
    exit(1);
}

    
int main(void)
{

//sigaction struct  to handle sigint. 
struct sigaction sigs;

sigs.sa_handler = signal_handler;
sigs.sa_flags = 0;
sigemptyset(&sigs.sa_mask);

int d_ip;
int t;
int rc;
int sc;
int fg;
int *value;


struct sockaddr_in sa, remote;

sa.sin_family = AF_INET;
inet_pton(AF_INET, "127.0.0.1", &d_ip);
sa.sin_addr.s_addr = d_ip;
sa.sin_port  = htons(51236);
remote.sin_family = AF_INET;

    if(sigaction(SIGINT, &sigs, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    
    if((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    if((bind(s, (struct sockaddr *)&sa, sizeof(sa))) == -1)
    {
        perror("bind");
        exit(1);
    }

    if(listen(s, 100) == -1)
    {
        perror("listen");
        exit(1);
    }

    printf("Waiting for a connection ..\n");
    fg = pthread_create((pthread_t *)&fgets_thread, NULL, fgets_function,(void *)&s);
    for(;;)
    { 
        t = sizeof(remote);
        
        if ((what = accept(s, (struct sockaddr *)&remote, &t)) == -1)
        {
            pthread_join(fgets_thread, NULL);
            break;
        }

        file_array[i] = what;

        i++;

        printf("Connected. \n");
    }
    //close(s);
    close(s);
    return 0;

}


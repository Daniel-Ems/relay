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

#define SOCK_PATH "echo_socket"

//Needs to be volatile because it is affected outside the program. 
volatile int what;
volatile pthread_t recieve;
volatile pthread_t sending;
char str[4096];
char buf[4096];

//Allows for proper exit.
void signal_handler(int signal_name)
{
        //closes socket and exits the program
        //pthread joins here 
        pthread_join(recieve, NULL);
        pthread_join(sending, NULL);
        close(what);
        printf("closed appropriately\n");
        exit(1); 
}

void *recieve_function(void *args)
{
    int n;
    int rec_fd = *((int *)args);
    while(recv(rec_fd, str, 100, 0))
    {
    /*
        n = recv(rec_fd, str, 100, 0);
        if(n <= 0){
            if(n<0) perror("recv");
                
        }
    */
        printf("message: %s", str);
    }
    
}


void *send_function(void *args)
{
    int send_fd = *((int *)args);
    for(;;)
    {
            fgets(buf, sizeof(buf), stdin);
            send(send_fd, buf, strlen(buf) + 1, 0);
    }
}

int main(void)
{
int pfds[2];

//sigaction struct  to handle sigint. 
struct sigaction sigs;

sigs.sa_handler = signal_handler;
sigs.sa_flags = 0;
sigemptyset(&sigs.sa_mask);


int d_ip;
int s_ip;
int s;
int len;
int t;
int rc;
int sc;


struct sockaddr_in sa, remote;

sa.sin_family = AF_INET;
//inet_pton(AF_INET, "127.0.0.1", &d_ip);
sa.sin_addr.s_addr = htons(INADDR_ANY);
sa.sin_port = htons(51236);

remote.sin_family = AF_INET;

    if(s, sigaction(SIGINT, &sigs, NULL) == -1) {
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

    if(listen(s, 0) == -1)
    {
        perror("listen");
        exit(1);
    }

    
        
        printf("Waiting for a connection ..\n");
        //pipe(pfds);

        for(;;)
        {

        t = sizeof(remote);
        if ((what = accept(s, (struct sockaddr *)&remote, &t)) == -1)
        {
            perror("accept");
            exit(1);
        }
        
        printf("Connected. \n");
        
        rc = pthread_create((pthread_t *)&recieve, NULL, recieve_function,(void *) &what);
        sc = pthread_create((pthread_t *)&sending, NULL, send_function, (void *) &what);
        }
        /*
        for(;;){
            n = recv(what, str, 100, 0);
            if(n <= 0){
                if(n<0) perror("recv");
                
            }

            printf("message: %s", str);
            
            printf("> ");
            fgets(str, sizeof(str), stdin);
            if(send(what, str, strlen(str) + 1, 0)) {
                perror("send");
                    
                }
            
        }
        */ 
    pthread_join(recieve, NULL);
    close(what);
    printf("Exited Successfully\n");
    return 0;
}


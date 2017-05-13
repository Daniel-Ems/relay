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
volatile int remote_fd;
volatile bool exit_run;
volatile pthread_t fgets_thread;
char str[4096]; // not currently using
char buf[4096];
int fd_array[100];
int i = 0;//array counter
int socket_fd;

//Allows for proper exit.
void signal_handler(int signal_name)
{
        shutdown(socket_fd, SHUT_RDWR);
        close(socket_fd);
        exit(1);
}


void *fgets_function(void *arg)
{

    for(;;)
    {
        if(fgets(buf, sizeof(buf), stdin)==NULL)
        {
           shutdown(socket_fd, SHUT_RDWR);
           pthread_exit(NULL);
        }
        else
        {
            for(int a = 0; a <= i; a++)
            { 
                send(fd_array[a], buf, strlen(buf) +1, 0);
            }
        }
    }
    close(remote_fd);
    close(socket_fd);
    exit(1);
}

    
int main(void)
{

//sigaction struct  to handle sigint. 
struct sigaction sigs;

sigs.sa_handler = signal_handler;
sigs.sa_flags = 0;
sigemptyset(&sigs.sa_mask);





struct sockaddr_in sock_struct, rem_struct;

sock_struct.sin_family = AF_INET;

int d_ip;
inet_pton(AF_INET, "127.0.0.1", &d_ip);
sock_struct.sin_addr.s_addr = d_ip;
sock_struct.sin_port  = htons(51236);
rem_struct.sin_family = AF_INET;

    if(sigaction(SIGINT, &sigs, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    
    if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    if((bind(socket_fd, (struct sockaddr *)&sock_struct, sizeof(sock_struct))) == -1)
    {
        perror("bind");
        exit(1);
    }

    if(listen(socket_fd, 100) == -1)
    {
        perror("listen");
        exit(1);
    }

    printf("Waiting for a connection ..\n");

    int t_retval;
    t_retval = pthread_create((pthread_t *)&fgets_thread, NULL, fgets_function,(void *)&socket_fd);

    int rs_size;
    for(;;)
    { 
        rs_size = sizeof(rem_struct);
        
        if ((remote_fd = accept(socket_fd, (struct sockaddr *)&rem_struct, &rs_size)) == -1)
        {
            pthread_join(fgets_thread, NULL);
            printf("I joined\n");
            break;
        }

        fd_array[i] = remote_fd;

        i++;

        printf("Connected. \n");
    }

    close(socket_fd);
    return 0;

}


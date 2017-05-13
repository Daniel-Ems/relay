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

/*
   ports have a TIME_WAIT attached which makes ports wait upon closing for 
   any packets still in the network. This will cause a slight delay between 
   runs of the program. We appreciate your patieints.
*/

enum port{ 
           //The port being used. Changes must be reflected in listener.c
           PORT = 51236,
           //MAX buffer sie for fgets
           MAX_BUF = 100,
           //Maximum number of connections allowed. 
           MAX_CONNECT = 100,
         };

//Needs to be volatile. Could be affected from outside the program. 
volatile int server_fd;
volatile pthread_t fgets_thread;
volatile int client_fd;

//Stores input from fgets
char buf[MAX_BUF];

//holds file descriptors from accept()
int fd_array[MAX_CONNECT];
int i = 0; //array counter

//client file descriptors
//int client_fd;

//Allows for proper handling of Sigint(str-c).
void signal_handler(int signal_name)
{
        if(signal_name == SIGINT)
        {
            shutdown(server_fd, SHUT_RDWR);
            close(server_fd);
            exit(1);
        }
}

//Threaded to handle user input.
void *fgets_function(void *args)
{
    args = NULL;
    for(;;)
    {
        //If recieves EOT: send shutdown signals and exit thread.
        if(fgets(buf, sizeof(buf), stdin)==NULL)
        {
           shutdown(server_fd, SHUT_RDWR);
           pthread_exit(args);
        }
        else
        {
            //Send message to all file_descriptors in fd_array
            for(int a = 0; a <= i; a++)
            { 
                send(fd_array[a], buf, strlen(buf) +1, 0);
            }
        }
    }
}

    
int main(void)
{

    //sigaction struct to handle sigint. 
    struct sigaction sigs;

    sigs.sa_handler = signal_handler; //function to handle signals
    sigs.sa_flags = 0;
    sigemptyset(&sigs.sa_mask);


    if(sigaction(SIGINT, &sigs, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    
    //Declare and assign values to structs necessary for sockets. This is were
    //communication protocols(AF_INET), addresses, and ports are assigned. 
    struct sockaddr_in server_struct;
    server_struct.sin_family = AF_INET; //IPV4
    int d_ip;
    inet_pton(AF_INET, "127.0.0.1", &d_ip); //loopback
    server_struct.sin_addr.s_addr = d_ip;
    server_struct.sin_port  = htons(PORT); //port listening on

    struct sockaddr_in client_struct; //client socket
    client_struct.sin_family = AF_INET;

    //Creates socket file descriptor. AF_INET(IPv4), and sock stream are used 
    //to allow for reliable tcp connections.
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    //Assigns address to socket. Otherwise the socket previously created
    //would live in name space.
    if((bind(server_fd, (struct sockaddr *)&server_struct, 
                         sizeof(server_struct))) == -1)
    {
        perror("bind");
        exit(1);
    }

    //Makes the socket passive(listens for connections using accept();
    if(listen(server_fd, 100) == -1)
    {
        perror("listen");
        exit(1);
    }

    printf("Waiting for a connection ..\n");

    int t_retval;
    //Thread fgets in order to allow a fgets to constantly be able to revieve
    //input and send to file descriptors in fd_array.
    t_retval = pthread_create((pthread_t *)&fgets_thread, NULL, fgets_function, 
                                                                         NULL);
                                                                         
    if(t_retval)
    {
        perror("thread");
        exit(1);
    }
    //used for readability in accept.
    unsigned int rs_size;

    //loop is necessary to continue entering into accept blocks.
    for(;;)
    { 
        //used for multiple fds
        rs_size = sizeof(client_struct);
        
        //In condidtional to allow for breaks in accept blocks.
        if ((client_fd = accept(server_fd, (struct sockaddr *)&client_struct, 
                                                            &rs_size)) == -1)
        {
            //If the accept block is broke, clean up and exit. 
            pthread_join(fgets_thread, NULL);
            break;
        }

        fd_array[i] = client_fd;

        i++;

        printf("Connected. \n");
    }
    //properly close the socket. 
    close(server_fd);
    return 0;

}


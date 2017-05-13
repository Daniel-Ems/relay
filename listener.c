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

//port to be used
enum port{ 
           PORT = 51236,
           MAX_MESS = 100,
         };


int main(void)
{
    //declare client struct and assign values for ipv4 and port number
    struct sockaddr_in client_struct;
    client_struct.sin_family = AF_INET;
    client_struct.sin_port = htons(PORT);

    //initialize the address and recieve the file pointer
    int client_fd;
    if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    printf("trying to connect...\n");

    //used for clarity in connet
    int client_sz;
    client_sz = sizeof(client_struct);

    //asks server socket for a connection. 
    if(connect(client_fd, (struct sockaddr *)&client_struct, client_sz) == -1)
    {
        perror("connect");
        exit(1);
    }

    printf("connected.\n");

    //placed in for loop to continously allow for continuou transmission 
    char str[MAX_MESS];
    while(recv(client_fd, str, 100, 0))
    {
        printf("%s", str);
    }

    //properly shut down the port. 
    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);
    return 0;
}

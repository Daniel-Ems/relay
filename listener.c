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
    int client_fd;
    int client_sz;

    struct sockaddr_in client_struct;

    char str[MAX_MESS];

    if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    printf("trying to connect...\n");

    client_struct.sin_family = AF_INET;
    client_struct.sin_port = htons(PORT);

    client_sz = sizeof(client_struct);

    if(connect(client_fd, (struct sockaddr *)&client_struct, client_sz) == -1)
    {
        perror("connect");
        exit(1);
    }

    printf("connected.\n");

    while(recv(client_fd, str, 100, 0))
    {
        printf("%s", str);
    }
    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);
    return 0;
}

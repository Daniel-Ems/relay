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

int main(void)
{

    int var1, var2, var3;
    struct sockaddr_in remote;
    char str[100];

    if((var1 = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    printf("trying to connect...\n");

    remote.sin_family = AF_INET;
    //remote.sin_addr.s_addr = htons("127.0.0.1");
    remote.sin_port = htons(51236);
    var2 = sizeof(remote);

    if(connect(var1, (struct sockaddr *)&remote, var2) == -1)
    {
        perror("connect");
        exit(1);
    }

    printf("connected.\n");

    while(recv(var1, str, 100, 0))
    {
        printf("%s", str);
    }
    close(var1);
    return 0;
}

#define _XOPEN_SOURCE
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h> 
#include <errno.h>
#include <string.h>
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

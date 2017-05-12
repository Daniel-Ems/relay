#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/futex.h>
#include <sys/time.h>

int main(void)
{
    char buf[4096];
    int file_descriptor;
    char *data;
    int futex_var = 1;
    file_descriptor = shm_open("/danny_distributor", O_RDONLY,0400);
    int pagesize = getpagesize();
    if(file_descriptor == -1)
    {
        perror("file_descriptor");
        exit(1);
    }

    posix_fallocate(file_descriptor, 0, pagesize);
    data = mmap(NULL, 4096, PROT_READ, MAP_SHARED, file_descriptor, 0);
    if(data == (caddr_t)(-1))
    {
        perror("mmap");
        exit(1);
    }

    for(;;)
    {
        futex(data, futex_var, FUTEX_WAIT, NULL);
        printf("%s", data);
    }
    return 0;
}

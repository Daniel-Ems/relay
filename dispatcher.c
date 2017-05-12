#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <linux/futex.h>


int main(void)
{

    struct stat sbuf;
    int file_descriptor;
    int futex_var = 0;;
    file_descriptor = shm_open("/danny_distributor", O_RDWR | O_CREAT | O_TRUNC, 0644);
    int pagesize = getpagesize();
    if(file_descriptor == -1)
    {
        perror("shm_open");
        exit(1);
    }

    posix_fallocate(file_descriptor, 0, pagesize);

    char *data = mmap(NULL, 4096, PROT_WRITE, MAP_SHARED, file_descriptor, 0);


    if(data == (caddr_t)(-1))
    {
        perror("mmap");
        exit(1);
    }
    

    for(;;)
    {
        fgets(data, 4096, stdin);
        futex(data, futex_var, FUTEX_WAKE, NULL);
    }
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/socket.h>
struct Enroll{
    char cid[5];
    char loginId[11];
};
int main()
{
    struct Enroll e;
    int fd=open("enrolled_database.txt",O_RDONLY);
    
    while(read(fd,&e,sizeof(e)) > 0)
    {
        printf("Cid  : %s Loginid %s\n",e.cid,e.loginId);
    }
    return 0;
}
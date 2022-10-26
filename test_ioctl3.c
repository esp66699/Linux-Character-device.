#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include "yw_chr_locked.h"
int main(int argc,char* argv[])
{
    int n,retval=0;
    int fd;
    fd=open("/dev/mychardev",O_RDWR);
    switch(argv[1][0])
    {
        case'0'://复位
            ioctl(fd,MY_IOC_RESET);
            printf("reset my\n");
            break;
        case '1': //通过指针  获取长度
            ioctl(fd,MY_IOCP_GET_LENS,&n);
            printf("get lens pointer，%d\n",n);
            break;
        case '2': //通过值  获取长度
            n=ioctl(fd,MY_IOCV_GET_LENS,n);
            printf("get lens value %d\n",n);
            break;
        case '3': //通过指针  设置长度
            n=argv[2][0]-'0';
            retval= ioctl(fd,MY_IOCP_SET_LENS,&n);
            printf("set lens value,%d  ，%s\n",n,strerror(errno));
            break;
        case '4': //值的方法   设置长度
            n=argv[2][0]-'0';
            retval= ioctl(fd,MY_IOCV_SET_LENS,n);
            printf("set lens value,%d\n",n);
            break;
}
close(fd);
return 0;
}





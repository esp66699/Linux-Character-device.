#include <stdio.h>
#include <errno.h>
#include <string.h>
// File handle header file
#include <fcntl.h>    //for open
#include <unistd.h>   // for close


#define DRIVER_DEVICE_FILE  "/dev/mychardev"

int main()
{
	int fd;
	int result;
	char buffer[50] = {0};

	fd = open(DRIVER_DEVICE_FILE,O_RDWR);
	if(fd == -1)
		perror("open");

       //result = read(fd, buffer, sizeof(buffer));
	//printf("read result:%d, buffer:[%s]\r\n", result, buffer);
       //if(result == -1)
		//perror("read");

	memcpy(buffer,"Lewis well done",strlen("Lewis well done"));
        //snprintf(buffer, sizeof(buffer), "Hello Driver");
        result = write(fd,buffer,strlen(buffer));
        printf("write result:%d, len:%ld, data:%s\r\n", result, strlen(buffer), buffer);	
	if(result == -1)
		perror("write");

	memset(buffer,0,strlen(buffer));
	result = read(fd,buffer,sizeof(buffer));
        printf("read result:%d, buffer:[%s]\r\n", result, buffer);
	
            if(result == -1)
		perror("read");

	if(close(fd) != 0)
		perror("close");

	//printf("%s\n",buffer);
	return 0;
}

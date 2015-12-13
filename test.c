#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE "/dev/cipher"

int main(){
	
	int fd;
	char write_buff[100],read_buff[100];
	
	fd = open(DEVICE, O_RDWR);
	
	if(fd == -1){
		printf("file %s error opening",DEVICE);
		exit(-1);
	}
	printf("Writing data: ");
	scanf(" %[^\n]",write_buff);
	write(fd,write_buff,sizeof(write_buff));
	
	printf("\nReading Data:\n");
	read(fd,read_buff,sizeof(read_buff));
	printf("device says: %s\n",read_buff);
	
	close(fd);
	return 0;
}

/*
 *  ioctl.c - the process to use ioctl's to control the kernel module
 *
 *  Until now we could have used cat for input and output.  But now
 *  we need to do ioctl's, which require writing our own process.
 */

/* 
 * device specifics, such as ioctl numbers and the
 * major device file. 
 */
#include "cipherdev.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>		/* open */
#include <unistd.h>		/* exit */
#include <sys/ioctl.h>		/* ioctl */

#define DEVICE "/dev/cipher"

/* 
 * Functions for the ioctl calls 
 */

void ioctl_set_method(int file_desc, int method)
{
	int ret_val;

	ret_val = ioctl(file_desc, IOCTL_SET_METHOD, method);

	if (ret_val < 0) {
		printf("ioctl_set_method failed:%d\n", ret_val);
		exit(-1);
	}
}

void ioctl_get_method(int file_desc)
{
	int ret_val;

	ret_val = ioctl(file_desc, IOCTL_GET_METHOD, 0);

	if (ret_val < 0) {
		printf("ioctl_get_method failed:%d\n", ret_val);
		exit(-1);
	}
	else{
		printf("ioctl_get_method returned:%d\n", ret_val);
	}
}

void ioctl_set_mode(int file_desc, int mode)
{
	int ret_val;
	ret_val = ioctl(file_desc, IOCTL_SET_MODE, mode);

	if (ret_val < 0) {
		printf("ioctl_set_mode failed:%d\n", ret_val);
		exit(-1);
	}
}

void ioctl_get_mode(int file_desc)
{
	int ret_val;

	ret_val = ioctl(file_desc, IOCTL_GET_MODE, 0);

	if (ret_val < 0) {
		printf("ioctl_get_mode failed:%d\n", ret_val);
		exit(-1);
	}
	else{
		printf("ioctl_get_mode returned:%d\n", ret_val);
	}
}

void ioctl_set_key(int file_desc, const char* key)
{
	int ret_val;
	ret_val = ioctl(file_desc, IOCTL_SET_KEY, key);

	if (ret_val < 0) {
		printf("ioctl_set_key failed:%d\n", ret_val);
		exit(-1);
	}
}

void ioctl_get_key(int file_desc, char* key)
{
	int ret_val;

	ret_val = ioctl(file_desc, IOCTL_GET_KEY, key);

	if (ret_val < 0) {
		printf("ioctl_get_key failed:%d\n", ret_val);
		exit(-1);
	}
	else{
		printf("ioctl_get_key returned:%s\n", key);
	}
}

/* 
 * Main - Call the ioctl functions 
 */
int main()
{
	int file_desc;
	char in[100],out[100];

	file_desc = open(DEVICE, O_RDWR);
	
	if(file_desc == -1){
		printf("file %s error opening",DEVICE);
		exit(-1);
	}

	//Set Method
	ioctl_set_method(file_desc,VIGENERE);
	ioctl_get_method(file_desc);
	//Set Mode
	ioctl_set_mode(file_desc,ENCIPHER);
	ioctl_get_mode(file_desc);
	//Set and get key
	printf("Enter key: ");
	scanf("%s",in);
	ioctl_set_key(file_desc,in);
	ioctl_get_key(file_desc,out);
	
	
	close(file_desc);
	return 0;
}

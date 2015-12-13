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

/* 
 * Functions for the ioctl calls 
 */

ioctl_set_method(int file_desc, int method)
{
	int ret_val;

	ret_val = ioctl(file_desc, IOCTL_SET_METHOD, method);

	if (ret_val < 0) {
		printf("ioctl_set_method failed:%d\n", ret_val);
		exit(-1);
	}
}

ioctl_get_method(int file_desc)
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

/* 
 * Main - Call the ioctl functions 
 */
int main()
{
	int file_desc;

	file_desc = open(DEVICE_FILE_NAME, O_RDWR);
	if (file_desc < 0) {
		printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
		exit(-1);
	}

	ioctl_get_method(file_desc);
	ioctl_set_method(file_desc,VIGENERE);
	ioctl_get_method(file_desc);

	close(file_desc);
	return 0;
}

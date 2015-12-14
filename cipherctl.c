#include "cipherdev.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>	
#include <sys/ioctl.h>

#define DEVICE "/dev/cipher"
//Commands
#define METHOD "method"
#define METHOD_VINEGERE "vigenere"
#define METHOD_CAESAR "caesar"
#define KEY "key"
#define MODE "mode"
#define MODE_ENCIPHER "encipher"
#define MODE_DECIPHER "decipher"
#define CLEAR "clear"
#define WRITE "write"
#define READ "read"
#define WRITE_MSG "write_msg"
#define READ_MSG "read_msg"
/***************************************************************************
 *  Helper function
 ***************************************************************************/
void ioctl_set_method(int fp, int method)
{
	int ret_val;
	ret_val = ioctl(fp, IOCTL_SET_METHOD, method);
	if (ret_val < 0) {
		printf("Set Method failed\n");
		exit(-1);
	}
	printf("Set Method success\n");
}

void ioctl_get_method(int fp)
{
	int ret_val;
	ret_val = ioctl(fp, IOCTL_GET_METHOD, 0);
	if (ret_val < 0) {
		printf("Get Method failed\n");
		exit(-1);
	}
	switch(ret_val){
		case VIGENERE:
			printf("Get Method : %s\n",METHOD_VINEGERE);
			break;
		case CAESAR:
			printf("Get Method : %s\n",METHOD_CAESAR);
			break;
	}
}
void ioctl_set_mode(int file_desc, int mode)
{
	int ret_val;
	ret_val = ioctl(file_desc, IOCTL_SET_MODE, mode);
	if (ret_val < 0) {
		printf("Set Mode failed\n");
		exit(-1);
	}
	printf("Set Mode success\n");
}

void ioctl_get_mode(int fp)
{
	int ret_val;
	ret_val = ioctl(fp, IOCTL_GET_MODE, 0);
	if (ret_val < 0) {
		printf("Get Mode failed\n");
		exit(-1);
	}
	switch(ret_val){
		case ENCIPHER:
			printf("Get Mode : %s\n",MODE_ENCIPHER);
			break;
		case DECIPHER:
			printf("Get Mode : %s\n",MODE_DECIPHER);
			break;
	}
}
void ioctl_set_key(int fp, const char* key)
{
	int ret_val;
	ret_val = ioctl(fp, IOCTL_SET_KEY, key);
	if (ret_val < 0) {
		printf("Set Key failed\n");
		exit(-1);
	}
	printf("Set Key success\n");
}
void ioctl_get_key(int fp)
{
	char key[BUF_LEN];
	int ret_val;
	ret_val = ioctl(fp, IOCTL_GET_KEY, key);
	if (ret_val < 0) {
		printf("Get Key failed\n");
		exit(-1);
	}
	printf("Get Key: %s\n",key);
}
void ioctl_write_msg(int fp, const char* mesg,int mode){ // Mode = 0 -> device read/write 1->IOCTL read/write
	int ret_val;
	switch(mode){
		case 0:
			ret_val = write(fp, mesg, strlen(mesg));
			break;
		case 1:
			ret_val = ioctl(fp, IOCTL_SET_MESG, mesg);
			break;
	}
	if (ret_val < 0) {
		printf("Write failed\n");
		exit(-1);
	}
	printf("Write successfull\n");
}

void ioctl_read_msg(int fp,int mode){ // Mode = 0 -> device read/write 1->IOCTL read/write
	int ret_val;
	char mesg[BUF_LEN];
	
	switch(mode){
		case 0:
			ret_val = read(fp, mesg, BUF_LEN);
			break;
		case 1:
			ret_val = ioctl(fp, IOCTL_GET_MESG, mesg);
			break;
	}
	if (ret_val < 0) {
		printf("Read failed\n");
		exit(-1);
	}
	printf("Message: %s\n",mesg);
}
/***************************************************************************
 *  Main Controller
 ***************************************************************************/
int main(int argc, char **argv) {
	int fp;
	fp = open(DEVICE, O_RDWR);
	if (argc < 2) {
		//TODO: Print usage
		printf("Invalid usage for %s\n", argv[0]);
		exit(-1);
	}
	//Set/Get method
	if (strcmp(argv[1], METHOD) == 0) {
		if (argc > 3) {
			//TODO: Print usage
			printf("Usage: %s method [vigenere | caesar]\n", argv[0]);
			exit(-1);
		}
		if (argc == 2) {
			//print method
			ioctl_get_method(fp);
		}else{
			//Set method
			if(strcmp(argv[2], METHOD_VINEGERE) == 0){
				ioctl_set_method(fp,VIGENERE);
			} else if(strcmp(argv[2], METHOD_CAESAR) == 0){
				ioctl_set_method(fp,CAESAR);
			} else {
				printf("Invalid Method: %s method [vigenere | caesar]\n", argv[0]);
				exit(-1);
			}
		}
	}
	//Set/Get Mode
	if (strcmp(argv[1], MODE) == 0) {
		if (argc > 3) {
			//TODO: Print usage
			printf("Usage: %s mode [encipher | decipher]\n", argv[0]);
			exit(-1);
		}
		if (argc == 2) {
			//print mode
			ioctl_get_mode(fp);
		}else{
			//Set mode
			if(strcmp(argv[2], MODE_ENCIPHER) == 0){
				ioctl_set_mode(fp,ENCIPHER);
			} else if(strcmp(argv[2], MODE_DECIPHER) == 0){
				ioctl_set_mode(fp,DECIPHER);
			} else {
				printf("Invalid Mode: %s mode [encipher | decipher]\n", argv[0]);
				exit(-1);
			}
		}
	}
	// Set/Get Key
	if (strcmp(argv[1], KEY) == 0) {
		if (argc > 3) {
			//TODO: Print usage
			printf("Usage: %s key [key]\n", argv[0]);
			exit(-1);
		}
		if (argc == 2) {
			//print key
			ioctl_get_key(fp);
		}else{
			//Set key
			ioctl_set_key(fp,argv[2]);			
		}
	}

	if (strcmp(argv[1], CLEAR) == 0) {

	}
	if (strcmp(argv[1], WRITE) == 0) {
		if (argc != 3) {
			//TODO: Print usage
			printf("Usage: %s write [message]\n", argv[0]);
			exit(-1);
		}else{
			ioctl_write_msg(fp,argv[2],1);
		}
	}
	if (strcmp(argv[1], WRITE_MSG) == 0) {
		if (argc != 3) {
			//TODO: Print usage
			printf("Usage: %s write_msg [message]\n", argv[0]);
			exit(-1);
		}else{
			ioctl_write_msg(fp,argv[2],0);
		}
	}
	if (strcmp(argv[1], READ) == 0) {
		if (argc != 2) {
			//TODO: Print usage
			printf("Usage: %s read\n", argv[0]);
			exit(-1);
		}else{
			ioctl_read_msg(fp,1);
		}

	}
	if (strcmp(argv[1], READ_MSG) == 0) {
		if (argc != 2) {
			//TODO: Print usage
			printf("Usage: %s read_msg\n", argv[0]);
			exit(-1);
		}else{
			ioctl_read_msg(fp,0);
		}
	}

	return 0;
}

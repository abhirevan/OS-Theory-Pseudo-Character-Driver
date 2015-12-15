/* cipherdev header */
/* cipherdev.h */
/* by William Katsak <wkatsak@cs.rutgers.edu> */
/* for CS 519, Spring 2015, Rutgers University */
#ifndef CIPHERDEV_H
#define CIPHERDEV_H
#include <linux/ioctl.h>

#define CIPHERDEV_MAGIC_NUMBER 'k'
#define DEVICE_NAME "cipher"
#define DEFAULT -1
// Cipher Methods
#define VIGENERE 0
#define CAESAR 1
// Cipher MODES
#define ENCIPHER 0
#define DECIPHER 1

// Cipher Methods
#define IOCTL_SET_METHOD _IOR(CIPHERDEV_MAGIC_NUMBER, 0, int)
#define IOCTL_GET_METHOD _IOW(CIPHERDEV_MAGIC_NUMBER, 1, int)
// Cipher MODES
#define IOCTL_SET_MODE _IOR(CIPHERDEV_MAGIC_NUMBER, 2, int)
#define IOCTL_GET_MODE _IOW(CIPHERDEV_MAGIC_NUMBER, 3, int)
// Cipher Key
#define IOCTL_SET_KEY _IOR(CIPHERDEV_MAGIC_NUMBER, 4, char*)
#define IOCTL_GET_KEY _IOR(CIPHERDEV_MAGIC_NUMBER, 5, char*)
// Clear cipher
#define IOCTL_CLEAR_CIPHER _IOWR(CIPHERDEV_MAGIC_NUMBER, 6, int)

#define BUF_LEN 100
#define SUCCESS 0
#define ERROR -1

#endif

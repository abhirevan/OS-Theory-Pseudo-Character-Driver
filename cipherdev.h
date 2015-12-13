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

#define IOCTL_SET_METHOD _IOR(CIPHERDEV_MAGIC_NUMBER, 0, int)
#define IOCTL_GET_METHOD _IOW(CIPHERDEV_MAGIC_NUMBER, 1, int)

#endif

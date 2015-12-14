/***************************************************************************
 * 
 ***************************************************************************/
#define pr_fmt(fmt) "["KBUILD_MODNAME "]: " fmt
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mutex.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include "cipherdev.h"
#include <linux/semaphore.h>
#include <linux/ctype.h>
//#include <linux/string.h>

#define BUF_LEN 100
#define SUCCESS 0
#define ERROR -1
/***************************************************************************
 * Globals
 ***************************************************************************/
 static struct class *cipherdev_class = NULL;
static struct device *cipherdev_device = NULL;
static struct cdev cipherdev_cdev;
static int cipherdev_major;
static struct file_operations cipherdev_fops;
int ret;
char *tempStr;

struct cipher_device_t{
	char data[BUF_LEN];
	struct semaphore sem;
	int cipher_method;
	int cipher_mode;
	char cipher_key[BUF_LEN];
} cipher_device;
/***************************************************************************
 * Helper functions
 ***************************************************************************/
void convertToUpperCase(char *sPtr)
{
      while(*sPtr != '\0')
      {
         *sPtr = toupper((unsigned char)*sPtr);
         sPtr++;
       }
}
/***************************************************************************
 * Module functions
 ***************************************************************************/
static int __init cipherdev_init(void)
{
	dev_t dev = 0;

	pr_info("module loaded\n");

	// Go through the stuff required to register the character device.

	// Create a class for the device (this is class like "classification", not
	// the object oriented construct).
	cipherdev_class = class_create(THIS_MODULE, "cipherdev");
	if (IS_ERR(cipherdev_class)) {
		pr_err("error in class_create(), cannot load module.\n");
		ret = PTR_ERR(cipherdev_class);
		goto err_class_create;
	}

	// Allocate a single minor for the device
	ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
	if (ret) {
		pr_err("error in alloc_chrdev_region(), cannot load module.\n");
		goto err_alloc_chrdev_region;
	}
	// Extract the major number
	cipherdev_major = MAJOR(dev);
	pr_info("cipher Major number is %d",cipherdev_major);
	pr_info("Use: mknod /dev/%s c %d 0",DEVICE_NAME,cipherdev_major);
	
	// Set up and add the cdev
	cdev_init(&cipherdev_cdev, &cipherdev_fops);
	cipherdev_cdev.owner = THIS_MODULE;
	ret = cdev_add(&cipherdev_cdev, MKDEV(cipherdev_major, 0), 1);
	if (ret) {
		pr_err("error in cdev_add(), cannot load module.\n");
		goto err_cdev_add;
	}

	// Create a device structure
	// This is what lets the system automatically create the /dev entry.
	cipherdev_device = device_create(cipherdev_class, NULL, dev, NULL, "cipher");
	if (IS_ERR(cipherdev_device)) {
		pr_err("error in device_create(), cannot load module.\n");
		ret = PTR_ERR(cipherdev_device);
		goto err_device_create;
	}
	//Init data and key
	memset(cipher_device.data, '\0', BUF_LEN);
	memset(cipher_device.cipher_key, '\0', BUF_LEN);
	
	//Init semaphore
	sema_init(&cipher_device.sem,1);
	
	//Init Cipher_device
	cipher_device.cipher_method = DEFAULT;
	cipher_device.cipher_mode = DEFAULT;
	
	// If no errors have occured, return 0.
	return 0;

	// Otherwise, we jump to one of these labels and unwind the setup procedure.
err_device_create:
	// Remove cdev
	cdev_del(&cipherdev_cdev);

err_cdev_add:
	// Unregister the chrdev region
	unregister_chrdev_region(MKDEV(cipherdev_major, 0), 1);

err_alloc_chrdev_region:
	// Clean up class
	if (cipherdev_class)
		class_destroy(cipherdev_class);

err_class_create:
	// Nothing to do.
	return 0;
}

static void __exit cipherdev_exit(void)
{
	dev_t dev = MKDEV(cipherdev_major, 0);

	pr_info("module unloaded\n");

	// Destroy device
	device_destroy(cipherdev_class, dev);
	// Remove cdev
	cdev_del(&cipherdev_cdev);
	// Unregister the chrdev region
	unregister_chrdev_region(MKDEV(cipherdev_major, 0), 1);
	// Clean up class
	if (cipherdev_class)
		class_destroy(cipherdev_class);
}

static int cipherdev_open(struct inode *inode, struct file *filp){
	pr_info("cipherdev_open(%p,%p)\n", inode, filp);
	//allow only 1 process
	if(down_interruptible(&cipher_device.sem) !=0){
		pr_err("cipher: could not lock device during open\n");
		return -1;
	}
	pr_info("cipher: opened device");
	return 0;
}

static int cipherdev_release(struct inode *inode, struct file *filp)
{
	pr_info("cipherdev_release(%p,%p)\n", inode, filp);
	//Release the process
	up(&cipher_device.sem);
	pr_info("cipher: released device");
	return 0;
}

static ssize_t cipherdev_read(struct file* filp,char* buffer,size_t length,loff_t* offset){
	pr_info("cipher: reading from device");
	ret= copy_to_user(buffer,cipher_device.data,length);
	return ret;
	
	/*
	int bytes_read = 0;
	//If end return 0
	if (*(cipher_device.ptr) == 0)
		return 0;
	while (length && *(cipher_device.ptr)) {
		put_user(*(cipher_device.ptr++), buffer++);
		length--;
		bytes_read++;
	}
	pr_info("cipher: Read %d bytes, %d left\n",bytes_read, length);
	return bytes_read;
	* */
}

static ssize_t cipherdev_write(struct file *filp,const char* buffer, size_t length, loff_t * offset){
	pr_info("cipher: writing to device");
	if(length > BUF_LEN)
	{
		pr_err("cipher device: Cannot write more than %d bytes",BUF_LEN);
		return -1;
	}
	ret =  copy_from_user(cipher_device.data,buffer,length);
	return ret;
	/*
	int i;
	for (i = 0; i < length && i < BUF_LEN; i++)
		get_user(cipher_device.data[i], buffer + i);
	cipher_device.ptr = cipher_device.data;
	return i;
	*/
}

int cipherdev_ioctl(struct file *file,unsigned int ioctl_num,unsigned long ioctl_param){
	pr_info("cipher ioctl: File:%p IOCTL:%d",file,ioctl_num);
	switch (ioctl_num) {
		case IOCTL_SET_METHOD:
			cipher_device.cipher_method = ioctl_param;
			pr_info("cipher ioctl: Method set as: %d",cipher_device.cipher_method);
			break;
		case IOCTL_GET_METHOD:
			pr_info("cipher ioctl: Method get as: %d",cipher_device.cipher_method);
			return cipher_device.cipher_method;
			break;
		case IOCTL_SET_MODE:
			cipher_device.cipher_mode = ioctl_param;
			pr_info("cipher ioctl: Mode set as: %d",cipher_device.cipher_mode);
			break;
		case IOCTL_GET_MODE:
			pr_info("cipher ioctl: Mode get as: %d",cipher_device.cipher_mode);
			return cipher_device.cipher_mode;
			break;
		case IOCTL_SET_KEY:
			pr_info("cipher ioctl: Set Key: %s of lenghth: %d",(char *)ioctl_param,strlen((char *)ioctl_param));
			tempStr=(char *)ioctl_param;
			convertToUpperCase(tempStr);
			strcpy(cipher_device.cipher_key,tempStr);
			return SUCCESS;
			break;
		case IOCTL_GET_KEY:
			strcpy(ioctl_param,cipher_device.cipher_key);
			pr_info("cipher ioctl: Set Key: %s of lenghth: %d",(char *)ioctl_param,strlen((char *)ioctl_param));
			return SUCCESS;
			break;
		case IOCTL_CLEAR_CIPHER:
			break;
		case IOCTL_SET_MESG:
			break;
		case IOCTL_GET_MESG:
			break;
		default :
			pr_err("cipher device: Incorrect IOCTL_NUMBER: %d",ioctl_num);
	}

	return SUCCESS;
}

static struct file_operations cipherdev_fops = {
	.owner = THIS_MODULE,
	.open = cipherdev_open,
	.release = cipherdev_release,
	.write = cipherdev_write,
	.unlocked_ioctl = cipherdev_ioctl,
	.read = cipherdev_read
};

module_init(cipherdev_init);
module_exit(cipherdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abhijit Shanbhag <abhijit.shanbhag@rutgers.edu>");
MODULE_DESCRIPTION("CS519-Fall-2015 - cipherdev");

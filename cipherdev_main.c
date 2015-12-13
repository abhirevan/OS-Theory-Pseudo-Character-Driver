/* cipherdev skeleton */
/* cipherdev_main.c */
/* by William Katsak <wkatsak@cs.rutgers.edu> */
/* for CS 519, Fall 2015, Rutgers University */

/* This is not guaranteed to be complete/correct with regards to setting up
	the character device, just a reasonable starting point */

/* *************************************************************************/

/* This sets up some functions for printing output, and tagging the output */
/* with the name of the module */
/* These functions are pr_info(), pr_warn(), pr_err(), etc. and behave */
/* just like printf(). You can search LXR for information on these functions */
/* and other pr_ functions. */
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

static struct class *cipherdev_class = NULL;
static struct device *cipherdev_device = NULL;
static struct cdev cipherdev_cdev;
static int cipherdev_major;
static struct file_operations cipherdev_fops;
int ret;

struct cipher_device_t{
	char data[100];
	struct semaphore sem;
} cipher_device;

/* Module initization. Happens every time the module is loaded. */
/* At a minimum, you need to initialize the character device structures. */
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
	//Init semaphore
	sema_init(&cipher_device.sem,1);

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

/* Module initization. Happens every time the module is loaded. */
/* You need to clean up the character device structures as well as */
/* ANYTHING else that you set up in init(). */
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
}

static ssize_t cipherdev_write(struct file *filp,const char* buffer, size_t length, loff_t * offset){
	pr_info("cipher: writing to device");
	ret =  copy_from_user(cipher_device.data,buffer,length);
	return ret;
}

/* File operations: put the pointers to your operation handlers here. */
static struct file_operations cipherdev_fops = {
	.owner = THIS_MODULE,
	.open = cipherdev_open,
	.release = cipherdev_release,
	.write = cipherdev_write,
	.read = cipherdev_read
};

module_init(cipherdev_init);
module_exit(cipherdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abhijit Shanbhag <abhijit.shanbhag@rutgers.edu>");
MODULE_DESCRIPTION("CS519-Fall-2015 - cipherdev");

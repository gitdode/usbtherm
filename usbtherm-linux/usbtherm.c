/*
 * usbtherm.c
 *
 *  Created on: 27.05.2016
 *      Author: dode
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/usb.h>
#include <linux/cdev.h>

#define DRV_NAME	"usbtherm"
#define SUCCESS		0

MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");

static const struct usb_device_id usbtherm_usb_tbl[] = {
	/* Custom USBTherm device */
	{USB_DEVICE(0x0df7, 0x0700)},
	{}
};

MODULE_DEVICE_TABLE(usb, usbtherm_usb_tbl);

/* static char *some_data __initdata = "Some data"; */

static struct class *class = NULL;
static struct cdev cdev;
static dev_t dev = 0;

/*
 * Called when a process tries to open the device file, like
 * "cat /dev/usbtherm0".
 */
static int device_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "usbtherm: Device was opened!\n");

	try_module_get(THIS_MODULE);

	return SUCCESS;
}

/*
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "usbtherm: Device was released!\n");

	module_put(THIS_MODULE);

	return SUCCESS;
}

/**
 * Called when a process reads from the device file.
 */
static ssize_t device_read(struct file *filp,
		char *buffer,
		size_t length,
		loff_t * offset)
{
	printk(KERN_INFO "usbtherm: Thank you for reading from device!\n");

	return SUCCESS;
}

/*
 * Called when a process writes to the device file: echo "hi" > /dev/usbtherm0
 */
static ssize_t device_write(struct file *filp,
		const char *buff,
		size_t len,
		loff_t *offset)
{
	printk(KERN_WARNING "usbtherm: Writing to device is not yet supported!\n");

	return -EINVAL;
}

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

/**
 * Destroys the device and class if the class was created, and unregisters
 * the device.
 */
static void cleanup(void) {
	if (class)
	{
		device_destroy(class, dev);
		cdev_del(&cdev);
		class_destroy(class);
	}

	unregister_chrdev_region(dev, 1);
}

/**
 * Allocates a char device, creates a class and adds and creates the device.
 */
static int __init usbtherm_init(void)
{
	int err = 0;
	int major = 0;
	struct device *device = NULL;

	err = alloc_chrdev_region(&dev, 0, 1, DRV_NAME);
	if (err < 0)
	{
		printk(KERN_WARNING "usbtherm: Allocating chrdev failed");

		return err;
	}
	major = MAJOR(dev);
	printk(KERN_INFO "usbtherm: Got chrdev major: %d\n", major);

	class = class_create(THIS_MODULE, DRV_NAME);
	if (IS_ERR(class))
	{
		err = PTR_ERR(class);
		printk(KERN_WARNING "usbtherm: Creating class failed");
		unregister_chrdev_region(dev, 1);

		return err;
	}

	cdev_init(&cdev, &fops);
	err = cdev_add(&cdev, dev, 1);
	if (err < 0)
	{
		printk(KERN_WARNING "usbtherm: Adding device %s%d failed",
				DRV_NAME, 0);
		cleanup();

		return err;
	}

	device = device_create(class, NULL, dev, NULL, DRV_NAME "%d", 0);
	if (IS_ERR(device))
	{
		err = PTR_ERR(device);
		printk(KERN_WARNING "usbtherm: Creating device %s%d failed",
				DRV_NAME, 0);
		cleanup();

		return err;
	}

	return SUCCESS;
}

/**
 * Cleans up.
 */
static void __exit usbtherm_exit(void)
{
	cleanup();

	printk(KERN_INFO "usbtherm: Goodbye!\n");
}

module_init(usbtherm_init);
module_exit(usbtherm_exit);

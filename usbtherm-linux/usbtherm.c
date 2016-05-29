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
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/usb.h>
#include <linux/cdev.h>

#define DRV_NAME	"usbtherm"
#define SUCCESS		0

enum usbtherm_type {
	DODES_USB_THERMOMETER
};

static const struct usb_device_id usbtherm_usb_tbl[] = {
	/* Custom USBTherm device */
	{USB_DEVICE(0x0df7, 0x0700), .driver_info = DODES_USB_THERMOMETER},
	{}
};

MODULE_DEVICE_TABLE(usb, usbtherm_usb_tbl);

struct usbtherm {
	struct usb_device *usbdev;
	enum usbtherm_type type;
};

/* static char *some_data __initdata = "Some data"; */

static struct class *class = NULL;
static struct cdev cdev;
static dev_t dev = 0;

/*
 * Called when a process tries to open the device file, like
 * "cat /dev/usbtherm0".
 * TODO handle multiple devices or limit to just one (-EBUSY)?
 */
static int device_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "usbtherm: Device was opened\n");

	try_module_get(THIS_MODULE);

	return SUCCESS;
}

/*
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "usbtherm: Device was released\n");

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
	.owner =	THIS_MODULE,
	.read = 	device_read,
	.write = 	device_write,
	.open = 	device_open,
	.release = 	device_release
};

/*
 * Could be used instead of alloc_chrdev_region, cdev_init, cdev_add and
 * device_create?
static struct usb_class_driver usbtherm_class = {
	.name = "usbtherm%d",
	.fops = &fops,
	.minor_base = 0,
};
*/

static int usbtherm_probe(struct usb_interface *interface,
		const struct usb_device_id *id)
{
	int err = 0;
	struct usb_device *usbdev = interface_to_usbdev(interface);
	struct usbtherm *dev = NULL;

	dev = kzalloc(sizeof(struct usbtherm), GFP_KERNEL);
	if (dev == NULL) {
		err = -ENOMEM;
		goto nomem;
	}

	dev->usbdev = usb_get_dev(usbdev);
	dev->type = id->driver_info;

	usb_set_intfdata(interface, dev);

	printk(KERN_INFO "usbtherm: USB device now connected\n");

	return SUCCESS;

nomem:
	return err;
}

static void usbtherm_disconnect(struct usb_interface *interface)
{
	struct usbtherm *dev;

	dev = usb_get_intfdata(interface);

	usb_set_intfdata(interface, NULL);
	usb_put_dev(dev->usbdev);

	kfree(dev);

	printk(KERN_INFO "usbtherm: USB device was disconnected\n");
}

static struct usb_driver usbtherm_driver = {
    .name = 		DRV_NAME,
    .id_table =		usbtherm_usb_tbl,
    .probe = 		usbtherm_probe,
    .disconnect = 	usbtherm_disconnect,
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

	usb_deregister(&usbtherm_driver);
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
		goto error;
	}

	device = device_create(class, NULL, dev, NULL, DRV_NAME "%d", 0);
	if (IS_ERR(device))
	{
		err = PTR_ERR(device);
		printk(KERN_WARNING "usbtherm: Creating device %s%d failed",
				DRV_NAME, 0);
		goto error;
	}

	err = usb_register(&usbtherm_driver);
	if (err < 0)
	{
		printk(KERN_WARNING "usbtherm: Registering USB driver failed");
		goto error;
	}

	return SUCCESS;

error:
	cleanup();
	return err;
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

MODULE_AUTHOR("Torsten Römer <dode@luniks.net>");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");

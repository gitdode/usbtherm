/*
 * usbtherm.c
 *
 *  Created on: 27.05.2016
 *      Author: Torsten Römer, dode@luniks.net
 *
 *	Learned from, inspired by and thanks to:
 *		- http://tldp.org/LDP/lkmpg/2.6/html/lkmpg.html
 *		- drivers/usb/misc/usbled.c
 *		- https://github.com/mavam/ml-driver/blob/master/ml_driver.c
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

static struct usb_driver usbtherm_driver;

/*
 * Called when a process tries to open the device file, like
 * "cat /dev/usbtherm0".
 */
static int device_open(struct inode *inode, struct file *filp)
{
	int err = 0;
	int minor = 0;
	struct usb_interface *interface = NULL;
	struct usbtherm *dev = NULL;

	minor = iminor(inode);

	interface = usb_find_interface(&usbtherm_driver, minor);
	if (! interface)
	{
		err = -ENODEV;
		printk(KERN_WARNING "usbtherm: Could not find USB interface!\n");
		goto error;
	}

	dev = usb_get_intfdata(interface);
	if (! dev) {
		err = -ENODEV;
		printk(KERN_WARNING "usbtherm: Could not get USB device!\n");
		goto error;
	}

	filp->private_data = dev;

	printk(KERN_INFO "usbtherm: Device was opened!\n");

	try_module_get(THIS_MODULE);

	return SUCCESS;

error:
 	 return err;
}

/*
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *filp)
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
	struct usbtherm *dev = NULL;

	dev = filp->private_data;

	printk(KERN_INFO "usbtherm: Reading from USB device %04x\n",
			dev->usbdev->descriptor.idVendor);

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

static struct usb_class_driver usbtherm_class = {
	.name = "usbtherm%d",
	.fops = &fops,
	.minor_base = 0,
};

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

	err = usb_register_dev(interface, &usbtherm_class);

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

	usb_deregister_dev(interface, &usbtherm_class);

	kfree(dev);

	printk(KERN_INFO "usbtherm: USB device was disconnected\n");
}

static struct usb_driver usbtherm_driver = {
    .name = 		DRV_NAME,
    .id_table =		usbtherm_usb_tbl,
    .probe = 		usbtherm_probe,
    .disconnect = 	usbtherm_disconnect,
};

module_usb_driver(usbtherm_driver);

MODULE_AUTHOR("Torsten Römer <dode@luniks.net>");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");

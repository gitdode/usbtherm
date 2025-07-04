/*
 * usbtherm.c
 *
 * Created on: 27.05.2016
 *     Author: Torsten Römer, dode@luniks.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 *
 * Linux USB driver for USBTherm, a simple AVR micro controller based USB
 * thermometer.
 *
 * Learned from and thanks to:
 *       - http://tldp.org/LDP/lkmpg/2.6/html/lkmpg.html
 *       - drivers/usb/misc/usbled.c
 *       - https://github.com/mavam/ml-driver/blob/master/ml_driver.c
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/usb.h>
#include <linux/uaccess.h>

#define DRV_NAME            "usbtherm"
#define SUCCESS             0
#define MSG_LEN             64
#define CUSTOM_REQ_TEMP     0

enum usbtherm_type {
    DODES_USB_THERMOMETER
};

/**
 * List of USB device vendor and product id's that this driver feels
 * responsible for. The module is loaded when one of the devices listed
 * is connected.
 */
static const struct usb_device_id usbtherm_usb_tbl[] = {
    /* Custom USBTherm device */
    {USB_DEVICE(0xd0de, 0x0001), .driver_info = DODES_USB_THERMOMETER},
    {} /* terminator */
};

MODULE_DEVICE_TABLE(usb, usbtherm_usb_tbl);

struct usbtherm {
    struct usb_device *usbdev;
    enum usbtherm_type type;
};

static struct usb_driver usbtherm_driver;
static char message[MSG_LEN];

/*
 * Called when a process tries to open the device file, like
 * "cat /dev/usbtherm0".
 */
static int device_open(struct inode *inode, struct file *filp) {
    int err = 0;
    int minor = 0;
    struct usb_interface *interface = NULL;
    struct usbtherm *dev = NULL;
    char data[16];
    char *urb_transfer_buffer;

    minor = iminor(inode);

    interface = usb_find_interface(&usbtherm_driver, minor);
    if (!interface) {
        err = -ENODEV;
        printk(KERN_WARNING "usbtherm: Could not find USB interface!\n");
        goto error;
    }

    dev = usb_get_intfdata(interface);
    if (!dev) {
        err = -ENODEV;
        printk(KERN_WARNING "usbtherm: Could not get USB device!\n");
        goto error;
    }

    /*
     * If dev is needed in for example device_read.
     */
    /* filp->private_data = dev; */

    /*
     * Send a custom "vendor" type status request to read
     * the temperature value from the device.
     */
    urb_transfer_buffer = kzalloc(sizeof (data), GFP_KERNEL);

    err = usb_control_msg(dev->usbdev,
            usb_rcvctrlpipe(dev->usbdev, USB_DIR_OUT),
            CUSTOM_REQ_TEMP, USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
            0, 0, urb_transfer_buffer, sizeof (data), 1000);

    strscpy(data, urb_transfer_buffer, sizeof (data));

    kfree(urb_transfer_buffer);

    if (err < 0) {
        err = -EIO;
        goto error;
    }

    snprintf(message, MSG_LEN, "%s\n", data);

    /* printk(KERN_DEBUG "usbtherm: Device was opened"); */

    return SUCCESS;

error:
    return err;
}

/**
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *filp) {
    /* printk(KERN_DEBUG "usbtherm: Device was released\n"); */

    return SUCCESS;
}

/**
 * Called when a process reads from the device file, i.e. "cat /dev/usbtherm0".
 */
static ssize_t device_read(struct file *filp, char *buffer, size_t length,
        loff_t *offset) {
    int err = 0;
    size_t msg_len = 0;
    size_t len_read = 0;

    /* printk(KERN_DEBUG "usbtherm: Reading from device\n"); */

    msg_len = strlen(message);

    if (*offset >= msg_len) {
        return 0;
    }

    len_read = msg_len - *offset;
    if (len_read > length) {
        len_read = length;
    }

    err = copy_to_user(buffer, message + *offset, len_read);
    if (err) {
        err = -EFAULT;
        goto error;
    }

    *offset += len_read;

    return len_read;

error:
    return err;
}

/**
 * Called when a process writes to the device file, i.e.
 * echo "hello" > /dev/usbtherm0
 */
static ssize_t device_write(struct file *filp, const char *buff, size_t len,
        loff_t *offset) {
    printk(KERN_WARNING "usbtherm: Writing to USBTherm is not supported!\n");

    return -EINVAL;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

/**
 * Have the device file created with read and write permission for everyone.
 */
static char *usbtherm_devnode(const struct device *dev, umode_t *mode) {
    if (!mode) {
        return NULL;
    }
    *mode = 0666;

    return NULL;
}

static struct usb_class_driver usbtherm_class = {
    .name = DRV_NAME "%d",
    .devnode = usbtherm_devnode,
    .fops = &fops,
    .minor_base = 0
};

/**
 * Called when the USB device was connected.
 */
static int usbtherm_probe(struct usb_interface *interface,
        const struct usb_device_id *id) {
    int err = 0;
    struct usb_device *usbdev = interface_to_usbdev(interface);
    struct usbtherm *dev = NULL;

    dev = kzalloc(sizeof (struct usbtherm), GFP_KERNEL);
    if (dev == NULL) {
        err = -ENOMEM;
        goto error;
    }

    dev->usbdev = usb_get_dev(usbdev);
    dev->type = id->driver_info;

    usb_set_intfdata(interface, dev);

    /*
     * Creates the device file in /dev and the class in /sys/class/usbmisc
     */
    err = usb_register_dev(interface, &usbtherm_class);
    if (err != 0) {
        printk(KERN_WARNING "usbtherm: Could not register USB device!\n");
    }

    printk(KERN_INFO "usbtherm: USB device was connected\n");

    return SUCCESS;

error:
    return err;
}

/**
 * Called when the USB device is disconnected.
 */
static void usbtherm_disconnect(struct usb_interface *interface) {
    struct usbtherm *dev;

    dev = usb_get_intfdata(interface);

    usb_set_intfdata(interface, NULL);
    usb_put_dev(dev->usbdev);

    /*
     * Cleans up the device file in /dev and the class in /sys/class/usbmisc
     */
    usb_deregister_dev(interface, &usbtherm_class);

    kfree(dev);

    printk(KERN_INFO "usbtherm: USB device was disconnected\n");
}

/**
 * Initialize the usb_driver structure.
 */
static struct usb_driver usbtherm_driver = {
    .name = DRV_NAME,
    .id_table = usbtherm_usb_tbl,
    .probe = usbtherm_probe,
    .disconnect = usbtherm_disconnect,
};

/**
 * Replaces module_init() and module_exit().
 */
module_usb_driver(usbtherm_driver);

MODULE_AUTHOR("Torsten Römer <dode@luniks.net>");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");
MODULE_DESCRIPTION("Simple driver for USBTherm, an AVR MCU based USB thermometer");

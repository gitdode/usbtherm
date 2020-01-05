#!/bin/sh

# supported methods to access the device: 
# - DEVICEFILE (Linux kernel module driver)
# - LIBUSB (libusb interface "usbtherm")
METHOD=LIBUSB
# only relevant for method DEVICEFILE
DEVICE=/dev/usbtherm0
BASEDIR=`dirname $0`

cd $BASEDIR
java -jar usbtherm-ui.jar $METHOD $DEVICE > /tmp/usbtherm.log 2>&1

# Simple makefile to build and install the "usbtherm" module.

obj-m += usbtherm.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
		
# See http://unix.stackexchange.com/q/288540/169470
install:
	cp $(shell pwd)/usbtherm.ko /lib/modules/$(shell uname -r)/kernel/drivers/usb/misc
	depmod -a

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

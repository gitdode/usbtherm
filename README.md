# USBTherm
Simple micro controller based USB thermometer with Linux kernel driver module and simple Java desktop app

This repository contains four projects:

* usbtherm-avr - USB thermometer based on an AVR micro controller and the https://github.com/obdev/v-usb firmware-only USB implementation
* usbtherm-kicad - KiCad project including schematic and PCB layout and a board at https://oshpark.com/shared_projects/ln73noyo
* usbtherm-linux - Basic but complete Linux kernel USB character device driver module
* usbtherm-libusb - Minimal interface to the device using libusb (much simpler and portable alternative to the Linux driver)
* usbtherm-ui - Simple Java desktop application displaying the temperature

A detailed description of the project can be found http://luniks.net/usbtherm.jsp

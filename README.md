# USBTherm
Simple micro controller based USB thermometer with Linux kernel driver module and simple Java desktop app

<img src="http://luniks.net/usbtherm/screens/USBTherm.png"/>

This repository contains three projects:

* usbtherm-avr - USB thermometer based on an AVR micro controller and the <a href="https://github.com/obdev/v-usb">V-USB</a> firmware-only USB implementation
* usbtherm-linux - Basic but complete Linux kernel USB character device driver module
* usbtherm-ui - Simple Java desktop application displaying the temperature

The project usbtherm-avr contains a KiCad project including schematic and PCB layout.

3 PCBs are currently being manufactured by <a href="https://oshpark.com/shared_projects/NU2Iejeb">OSH Park</a>, curious to see if they work at all :-)

A detailed description of the project follows.

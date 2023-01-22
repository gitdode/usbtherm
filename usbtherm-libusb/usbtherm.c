/*
 * usbtherm.c
 *
 * Created on: 02.01.2020
 *     Author: Torsten Römer, dode@luniks.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 *
 * libusb interface for USBTherm, a simple AVR micro controller based USB
 * thermometer.
 */

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>

#define ID_VENDOR           0xd0de
#define ID_PRODUCT          0x0001
#define CUSTOM_REQ_TEMP     0

int main(int argc, char *argv[]) {
    libusb_device_handle *device;
    int status;
    unsigned char data[16];

    status = libusb_init(NULL);
    if (status < 0) {
        printf("Initialization failed: %s\n", libusb_error_name(status));

        return status;
    }

    device = libusb_open_device_with_vid_pid(NULL, ID_VENDOR, ID_PRODUCT);
    if (device == 0) {
        printf("Failed to open USBTherm device\n");
        goto error;
    }

    status = libusb_control_transfer(device,
            LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE,
            CUSTOM_REQ_TEMP, 0, 0, data, sizeof(data), 1000);
    // should have transferred at least 1 byte
    if (status < 1) {
        libusb_close(device);
        printf("Data transfer from USBTherm device failed\n");
        goto error;
    }

    printf("%s\n", data);

    libusb_close(device);
    libusb_exit(NULL);

    return 0;

error:
    libusb_exit(NULL);
    return -1;
}

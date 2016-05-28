/*
 * usbtherm.c
 *
 *  Created on: 26.05.2016
 *      Author: dode@luniks.net
 */

#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "usbdrv/usbdrv.h"
#include "usbdrv/oddebug.h"

#define SUCCESS 0

usbMsgLen_t usbFunctionSetup(uchar data[8]) {
	usbRequest_t *request = (void *)data;

    return SUCCESS;
}

int main(void) {

	while(true) {

	}

	return SUCCESS;
}

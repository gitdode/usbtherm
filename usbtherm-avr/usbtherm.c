/*
 * usbtherm.c
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 *
 * Firmware for USBTherm, a simple AVR micro controller based USB thermometer.
 *
 * Created on: 26.05.2016
 *     Author: dode@luniks.net
 *
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "usbdrv/usbdrv.h"
/* #include "usbdrv/oddebug.h" */

/*
 * Simple module providing functions to print and read a string via USART.
 * Maybe not working for all AVR controllers.
 * The V-USB module "usbdrv/oddebug.h" provides more advanced debug logging.
 */
#include "usart.h"

/* The pins for V-USB are set up in usbdrv/usbconfig.h */
#define DDR_LED			DDRB
#define	PORT_LED		PORTB
#define PIN_LED			PB0
#define PIN_TEMP 		PC0
#define AREF_MV			5000

/* Weight of the exponential weighted moving average as bit shift */
#define EWMA_BS			2

/* Output of the TMP36 is 750 mV @ 25°C, 10 mV per °C */
#define TMP36_MV_0C		500
#define TMP36_MV_20C	700

/* Timer0 interrupts per second */
#define INTS_SEC 		F_CPU / (1024UL * 255)

/* Request from the kernel driver */
#define CUSTOM_REQ_TEMP	0

static volatile uint8_t ints = 0;
static int32_t mVAvg = TMP36_MV_20C << EWMA_BS;

ISR(TIMER0_COMPA_vect) {
	ints++;
}

EMPTY_INTERRUPT(ADC_vect);

/**
 * Sets up the pins.
 */
static void initPins(void) {
	// set LED pin as output pin
	DDR_LED |= (1 << PIN_LED);
}

/**
 * Sets up the timer.
 */
static void initTimer(void) {
	// timer0 clear timer on compare match mode, TOP OCR0A
	TCCR0A |= (1 << WGM01);
	// timer0 clock prescaler/1024/255 ~ 46 Hz @ 12 MHz ~ 61 Hz @ 16 MHz
	TCCR0B |= (1 << CS02) | (1 << CS00);
	OCR0A = 255;

	// enable timer0 compare match A interrupt
	TIMSK0 |= (1 << OCIE0A);
}

/**
 * Sets up the ADC.
 */
static void initADC(void) {
	set_sleep_mode(SLEEP_MODE_IDLE);

	// use AVCC as reference voltage
	ADMUX |= (1 << REFS0);
	// disable digital input on the ADC inputs to reduce digital noise
	DIDR0 = 0b00111111;
	// ADC clock prescaler/64 ~ 187.5 kHz @ 12 MHz ~ 250 kHz @ 16 MHz
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1);
	// enable ADC interrupt
	ADCSRA |= (1 << ADIE);
	// enable ADC
	ADCSRA |= (1 << ADEN);
	// measure at pin PIN_TEMP
	ADMUX = (0b11110000 & ADMUX) | PIN_TEMP;
}

/**
 * Sets up V-USB.
 */
static void initUSB(void) {
	wdt_disable();
	usbInit();
	usbDeviceDisconnect();
	_delay_ms(255);
	usbDeviceConnect();
}

/**
 * Measures the voltage of the temperature sensor with 16x oversampling during
 * idle sleep mode to reduce digital noise, and calculates an exponential
 * weighted moving average.
 */
static void measureTemp(void) {
	uint32_t overValue = 0;
	for (uint8_t i = 0; i < 16; i++) {
		ADCSRA |= (1 << ADSC);
		sleep_mode();
		overValue += ADC;
	}

	int16_t mV = (((overValue >> 2) * AREF_MV) >> 12);

	// calculate EWMA
	mVAvg = mV + mVAvg - ((mVAvg - EWMA_BS) >> EWMA_BS);
}

/**
 * Translates the averaged voltage to degrees Celsius and prints the result.
 */
static void printTemp(void) {
	div_t temp = div((mVAvg >> EWMA_BS) - TMP36_MV_0C, 10);
	char buf[16];
	snprintf(buf, sizeof(buf), "%d.%d °C\n", temp.quot, abs(temp.rem));
	printString(buf);
}

/**
 * Called by the driver to read the temperature value in °C x10.
 */
uchar usbFunctionRead(uchar *data, uchar len) {

	/**
	 * The temperature value is short enough to be read at once.
	 * TODO okay like that?
	 */
	int16_t tempx10 = (mVAvg >> EWMA_BS) - TMP36_MV_0C;
	snprintf((char *)data, len, "%d", tempx10);

    return len;
}

/**
 * Sets up the implemented requests.
 */
usbMsgLen_t usbFunctionSetup(uchar data[8]) {
	usbRequest_t *req = (void *)data;

	/*
	 * The only implemented request - tells the driver to read data
	 * (the temperature value) with usbFunctionRead().
	 * Also, USB_CFG_IMPLEMENT_FN_READ must be set to 1 in usbdrv/usbconfig.h.
	 */
	if (req->bRequest == CUSTOM_REQ_TEMP) {
		return USB_NO_MSG;
	}

	/**
	 * Return no data for unimplemented requests.
	 */
	return 0;
}

int main(void) {

	// enable USART RX complete interrupt 0
	// UCSR0B |= (1 << RXCIE0);
	initUSART();
	initPins();
	initTimer();
	initADC();
	initUSB();

	// enable global interrupts
	sei();

	while (true) {

		/**
		 * About every second, turn on the LED, measure the temperature,
		 * print the temperature via USART, turn off the LED.
		 */
		if (ints >= INTS_SEC) {
			ints = 0;
			// set LED pin high
			PORT_LED |= (1 << PIN_LED);
			measureTemp();
			printTemp();
			// set LED pin low
			PORT_LED &= ~(1 << PIN_LED);

		}
		usbPoll();
	}

	return 0;
}

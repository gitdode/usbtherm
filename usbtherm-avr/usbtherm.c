/*
 * usbtherm.c
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 *
 * Created on: 26.05.2016
 *     Author: dode@luniks.net
 *
 */

#include <stdbool.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>

/* #include "usbdrv/usbdrv.h" */
/* #include "usbdrv/oddebug.h" */

#include "usart.h"

#define SUCCESS 		0
#define PIN_TEMP 		PC0
#define PIN_LED			PB1
#define AREF_MV			5000
#define TMP36_MV_0C		500
#define TMP36_MV_20C	700

/*
usbMsgLen_t usbFunctionSetup(uchar data[8]) {
	usbRequest_t *request = (void *)data;

    return SUCCESS;
}
*/

static volatile uint8_t ints = 0;
static int32_t mVAvg = TMP36_MV_20C << 2; // 20°C x4

ISR(TIMER0_COMPA_vect) {
	ints++;
}

EMPTY_INTERRUPT(ADC_vect);

/**
 * Sets up the pins.
 */
static void initPins(void) {
	// set pin PB1 as output pin
	DDRB |= (1 << PIN_LED);
}

/**
 * Sets up the timer.
 */
static void initTimer(void) {
	// timer0 clear timer on compare match mode, TOP OCR0A
	TCCR0A |= (1 << WGM01);
	// timer0 clock prescaler/?/255 = ? KHz @ 1 MHz
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
	// ADC clock prescaler/8 = 125 kHz @ 1 MHz
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0);
	// enable ADC interrupt
	ADCSRA |= (1 << ADIE);
	// enable ADC
	ADCSRA |= (1 << ADEN);
	// measure at pin PIN_TEMP
	ADMUX = (0b11110000 & ADMUX) | PIN_TEMP;
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

	// calculate EWMA x4
	mVAvg = mV + mVAvg - ((mVAvg - 2) >> 2);
}

/**
 * Translates the averaged voltage to degrees celsius and prints the result.
 */
static void printTemp(void) {
	div_t temp = div((mVAvg >> 2) - TMP36_MV_0C, 10);
	char buf[20];
	snprintf(buf, sizeof(buf), "Temp is %d.%d °C\n", temp.quot, abs(temp.rem));
	printString(buf);
}

int main(void) {

	// enable USART RX complete interrupt 0
	// UCSR0B |= (1 << RXCIE0);
	initUSART();
	initPins();
	initTimer();
	initADC();

	// enable global interrupts
	sei();

	while (true) {
		if (ints >= 4) {
			// set LED pin high
			PORTB |= (1 << PIN_LED);
			ints = 0;
			measureTemp();
			printTemp();
			// set LED pin low
			PORTB &= ~(1 << PIN_LED);
		}
	}

	return SUCCESS;
}

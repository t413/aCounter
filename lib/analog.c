// Simple analog to digitial conversion

#include <avr/io.h>
#include <avr/interrupt.h>
#include "analog.h"
#include "binary.h"


static uint8_t aref = 1; // default to AREF = Vcc


void analogReference(uint8_t mode) {
	aref = mode & 0x03;
}



int16_t analogRead(uint8_t pin) {
	uint8_t low = 0, high = 0;

	ADCSRA = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //setup prescaler
	ADCSRA |= (1 << ADEN); //enable adc

	// set the analog reference (high two bits of ADMUX) and select the
	// channel (low 4 bits).  this also sets ADLAR (left-adjust result)
	// to 0 (the default).
	ADMUX = ((aref & 0x03) << REFS0) | (pin & 0x07);

	// start the conversion
	ADCSRA |= (1 << ADSC);

	// ADSC is cleared when the conversion finishes
	while (ADCSRA & (1 << ADSC));

	// we have to read ADCL first; doing so locks both ADCL
	// and ADCH until ADCH is read.  reading ADCL second would
	// cause the results of each conversion to be discarded,
	// as ADCL and ADCH would be locked when it completed.
	low  = ADCL;
	high = ADCH;

	// combine the two bytes
	return (high << 8) | low;
}


void startRunADC() {
	// clear ADLAR in ADMUX (0x7C) to right-adjust the result
	// ADCL will contain lower 8 bits, ADCH upper 2 (in last two bits)
	ADMUX &= B11011111;

	// Set REFS1..0 in ADMUX (0x7C) to change reference voltage to the
	// proper source (01)
	ADMUX |= B01000000;

	// Clear MUX3..0 in ADMUX (0x7C) in preparation for setting the analog
	// input
	ADMUX &= B11110000;

	// Set MUX3..0 in ADMUX (0x7C) to read from AD8 (Internal temp)
	// Do not set above 15! You will overrun other parts of ADMUX. A full
	// list of possible inputs is available in Table 24-4 of the ATMega328
	// datasheet
	ADMUX |= 8;
	// ADMUX |= B00001000; // Binary equivalent

	// Set ADEN in ADCSRA (0x7A) to enable the ADC.
	// Note, this instruction takes 12 ADC clocks to execute
	ADCSRA |= B10000000;

	// Set ADATE in ADCSRA (0x7A) to enable auto-triggering.
	ADCSRA |= B00100000;

	// Clear ADTS2..0 in ADCSRB (0x7B) to set trigger mode to free running.
	// This means that as soon as an ADC has finished, the next will be
	// immediately started.
	ADCSRB &= B11111000;

	// Set the Prescaler to 128 (16000KHz/128 = 125KHz)
	// Above 200KHz 10-bit results are not reliable.
	ADCSRA |= B00000111;

	// Set ADIE in ADCSRA (0x7A) to enable the ADC interrupt.
	// Without this, the internal interrupt will not trigger.
	ADCSRA |= B00001000;

	// Enable global interrupts
	// AVR macro included in <avr/interrupts.h>, which the Arduino IDE
	// supplies by default.
	sei();

	// Set ADSC in ADCSRA (0x7A) to start the ADC conversion
	ADCSRA |=B01000000;
}


/**
 *  \file analog.c
 *
 *	\date: 14.12.2012
 *  \author: Christian Brommer & Dennis Schuldt
 */

#include <avr/io.h>
#include "avr/interrupt.h"
#include "global.h"
#include "analog.h"

//
// ADC is connected to an 8-channel Analog Multiplexer which allows 8 single-ended voltage inputs
// constructed from the pins of Port A. The single-ended voltage inputs refer to 0V (GND).
//
// Code supports 4 channels:
//		player 1 X and Y direction: PORTA-0 (Y, pin 40), PORTA-1 (X, pin 39)
//		player 2 X and Y direction:	PORTA-2 (Y, pin 38), PORTA-3 (X, pin 37)
//
//	Joy 1 switch: PORTA-4 (pin 36)
//	Joy 2 switch: PORTA=5 (pin 35)
//
void analog_init(void)
{
	cli();
	volatile uint16_t result;
	
	ADC_result[0] = 600;									// Initialize event variables
	ADC_result[1] = 600;
	ADC_result[2] = 600;
	ADC_result[3] = 600;

	ADC_ch=0;											 	// Start first measurement with channel 0
// Set Voltage Reference Selection to 01
// Set ADLAR == 0: use all 10 bits
// Set MUX to 0
// Set Vcc is connected to AVcc
	sbi(ADMUX, REFS0);										// AVCC with external capacitor at AREF pin
// ADPS2:0: ADC Prescaler Select Bits. Select 128 Division Factor
	// Select 128 Division Factor => 125 KHz if F_CPU==16000000
	// Select 128 Division Factor => 156.25 KHz if F_CPU==20000000
	// Frequency prescaler should be between 50 - 200khz -> CLK durch 50 und 200khz -> Tabelle
	// Also disable ADATE: ADC Auto Trigger
	ADCSRA |= (_BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0));
// ADTS2:0: ADC Auto Trigger Source. Unused. Run ADC in free running mode
	ADCSRB = 0x00;
// ADEN: ADC Enable
	sbi(ADCSRA, ADEN);                  					// activate ADC
// ADSC: ADC Start Conversion
	sbi(ADCSRA, ADSC);			        					// start ADC conversion
// ADSC will read as one as long as a conversion is in progress. When the conversion is complete, it returns to zero. 
	while (bit_is_set(ADCSRA,ADSC)) ;        				// wait until conversion is complete
	result = ADC;											// first dummy measurement initializes the ADC
// ADIE: ADC Interrupt Enable
	sbi(ADCSRA, ADIE);										// activate ADC Interrupt
	sei();
}
//
// Start a new conversion
// After ADC_read() has been called, ADC_result[] reflects previous conversions.
// ADC_read() sets channel arg in ADMUX but it stays that way until the next ADC_read()
// channel (ADC_ch) is updated in ISR so ADC_read needs to be called 4 times to do
// all channels. joystick_calibration() is called 4 times in main.c
// channel argument is always ADC_ch.
//
void ADC_read(uint8_t channel)
{
	if (bit_is_set(ADCSRA,ADSC)) return;					// return if a measurement is active
	ADMUX = (ADMUX & ~(0x1F)) | (channel & 0x1F);			// Choose channel
	ADCSRA |= _BV(ADSC);            						// start ADC conversion
}
//
// Start a new conversion and wait for completion
//
void ADC_BlockingRead(uint8_t channel)
{
	while (bit_is_set(ADCSRA,ADSC)) ;						// wait if a measurement is active
	ADMUX = (ADMUX & ~(0x1F)) | (channel & 0x1F);			// Choose channel
	sbi(ADCSRA, ADSC);	            						// start ADC conversion
	while (bit_is_set(ADCSRA,ADSC)) ;						// wait until measurement is completed
}
//
// Conversion completed. Store result in current (ADC_ch) channel
// and advance to the next channel, provided ADMUX is updated
// Conversion complete flag may be needed if code that started
// the conversion needs the result immediately.
// Free running mode so a new conversion start immediately
//
ISR(ADC_vect)
{
	ADC_result[ADC_ch]=ADC;									// write ADC result to the corresponding variable
	ADC_ch++;												// next channel in ADC_result but not in ADMUX???
	if (ADC_ch>=ADC_max) ADC_ch=0;							// rotate through channels
	ADMUX = (ADMUX & ~(0x1F)) | (ADC_ch & 0x1F);			// Update channel. Added JdR
	// May be needed for newer AVRs
	sbi(ADCSRA, ADIF);										//clear interrupt flag
}

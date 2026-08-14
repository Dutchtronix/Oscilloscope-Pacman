/**
 *	\file timer.c
 *
 *	\date 14.12.2012
 * 	\author Christian Brommer & Dennis Schuldt
 *
 * Update by JdR (Dutchtronix) to run the graphics processor
 * on the Dutchtronix AVR Oscilloscope Clock. This clock
 * has a D to A converter needed for rendering the graphics
 * on an analog oscilloscope.
 *
 */


#include "avr/io.h"
#include <avr/interrupt.h>
#include "stdint.h"
#include "stdio.h"

#include "global.h"
#include <util/delay.h>

#include "timerG.h"
#include "graphics.h"


void timer_init(void)
{
#if F_CPU==16000000

	// Timer 0 - 8bit
	//Prescaler 64, 16000000/64/256=976,5625 -> 1/976.5625=0.001024s => 1ms
	TCCR0B |= ((1<<CS00) | (1<<CS01)); 
	TIMSK0 |= (1<<TOIE0);

#elif F_CPU==20000000
//
// timer0 set for  1.024 mSec.
//
	TCCR0A = _BV(WGM01);						//CTC mode. 
	TCCR0B = _BV(CS02);							//prescaler fClk/256.  tPeriod = 12.8 uS at 20Mhz
	TCNT0 = 0;
	OCR0A = Timer0CountUp;						//load counter register for 1024 uSec interrupt
	sbi(TIFR0, OCF0A);							//clear pending match
	sbi(TIMSK0, OCIE0A);						//Enable TC0.matchA
#else
	error
#endif
//	sei();
}

void timer_data_allocate(void)
{
	pacman_move.value = 0;
	pacman_move.timer_counter = 0;

	pacman_flash.value = 0;
	pacman_flash.timer_counter = 0;

	pacman_destroy.value = 24;
	pacman_destroy.timer_counter = 0;

	ms_pacman_destroy.value =34;
	ms_pacman_destroy.timer_counter = 0;
}

//8-bit Timer
#if F_CPU==16000000
ISR(TIMER0_OVF_vect)
#elif F_CPU==20000000
ISR(TIMER0_COMPA_vect)
#else
	error
#endif
{
	TIFR0 = _BV(OCF0A);							//clear pending match. For future AVRs

	//PacMan mouth flash
	pacman_flash.timer_counter++;

	if (pacman_flash.timer_counter ==  250) {
		if(pacman_flash.value == 1)				//toggle pacman_flash.value
			pacman_flash.value = 0;
		else
			pacman_flash.value = 1;

		pacman_flash.timer_counter = 0;
	}

	//PacMan move timer
	pacman_move.timer_counter++;

	if (pacman_move.timer_counter ==  50) {
		pacman_move.timer_counter = 0;

		pacman_move.value++;
	}

	//PacMan destroy

	if (graphics_control.pacman.dead==1)	{		//Only compute the destruction counter when needed
		pacman_destroy.timer_counter++;

		if (pacman_destroy.timer_counter ==  50)	{
			pacman_destroy.timer_counter = 0;

			if (!pacman_destroy.value == 0)
				pacman_destroy.value--;
		}
	}

	//Ms. PacMan destroy

	if (graphics_control.ms_pacman.dead==1) {		//Only compute the destruction counter when needed
		ms_pacman_destroy.timer_counter++;

		if (ms_pacman_destroy.timer_counter ==  50) {
			ms_pacman_destroy.timer_counter = 0;

			if (!ms_pacman_destroy.value == 0)
				ms_pacman_destroy.value--;
		}
	}
}

/**
 *  \file timer.c
 *
 *  \date: 14.12.2012
 *  \author: Christian Brommer & Dennis Schuldt
 */

#include "avr/io.h"
#include <avr/interrupt.h>
#include "stdint.h"
#include "stdio.h"

#include "timer.h"
#include "global.h"
#include "character_logic.h"
#include "ghost_ai.h"

#include <util/delay.h>

void timer_init(void)
{
#if F_CPU==16000000
	// Timer 0 - 8bit
	TCCR0B |= ((1<<CS00) | (1<<CS01));  //Prescaler 64, 16000000/64/256=976.5625 -> 1/976.5625=0.001024s => 1ms
	TIMSK0 |= (1<<TOIE0);
#elif F_CPU==20000000
//
// timer0 set for  1.024 mSec.
//
#define Timer0Clock (F_CPU/256)
#define Timer0CountUp (1024UL * Timer0Clock / 1000000UL)
#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )
//
// the prescaler is set so that timer0 ticks every 256 clock cycles, and
// the match handler is called every Timer0CountUp ticks.
//
#define MICROSECONDS_PER_TIMER0_MATCH (clockCyclesToMicroseconds(256 * Timer0CountUp))
	TCCR0A = _BV(WGM01);						//CTC mode. 
	TCCR0B = _BV(CS02);							//prescaler fClk/256.  tPeriod = 12.8 uS at 20Mhz
	TCNT0 = 0;
	OCR0A = Timer0CountUp;						//load counter register for 1024 uSec interrupt
	TIFR0 = _BV(OCF0A);							//clear pending match
	TIMSK0 = _BV(OCIE0A);						//Enable TC0.matchA
#else
	error
#endif
	sei();
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
	// Zähler für Pacman und Geister hochzählen
	logic_control.update_pacman_position_counter++;
	logic_control.ghost_1.update_position_counter++;
	logic_control.ghost_2.update_position_counter++;
	logic_control.ghost_3.update_position_counter++;
	logic_control.ghost_4.update_position_counter++;

	if (logic_control.hold==0) {		// Wenn Logik nicht angehalten, Logikzähler hochzählen
		logic_control.fruits.timer++;
		logic_control.ghost_counter++;
		if ((logic_control.ghost_1.graphic_state==2) || (logic_control.ghost_2.graphic_state==2) ||
		    (logic_control.ghost_3.graphic_state==2) || (logic_control.ghost_4.graphic_state==2))
			logic_control.pacman_counter++;
	}

	// Vergleich für alle Positionsupdates der Charaktere abfragen
	if (logic_control.update_pacman_position_counter == logic_control.update_pacman_time) logic_control.update_pacman_position_flag=1;
	if (logic_control.ghost_1.update_position_counter == logic_control.ghost_1.update_time) logic_control.ghost_1.update_position_flag=1;
	if (logic_control.ghost_2.update_position_counter == logic_control.ghost_2.update_time) logic_control.ghost_2.update_position_flag=1;
	if (logic_control.ghost_3.update_position_counter == logic_control.ghost_3.update_time) logic_control.ghost_3.update_position_flag=1;
	if (logic_control.ghost_4.update_position_counter == logic_control.ghost_4.update_time) logic_control.ghost_4.update_position_flag=1;


	// Timer for fruit
	if ((logic_control.fruits.timer == logic_control.fruits.timer_limit)) {
		// Früchte nicht mehr darstellen, da Zeit abgelaufen
		// Don't render the fruits anymore since time has expired.
		logic_control.fruits.apple.show_in_map=0;
		logic_control.fruits.strawberry.show_in_map=0;
		logic_control.fruits.pineapple.show_in_map=0;
		logic_control.fruits.cherry.show_in_map=0;
		logic_control.fruits.banana.show_in_map=0;

		logic_control.fruits.fruit_display=0;
	}

	// toggle zwischen Geister verfolgen Pacman/Ms.Pacman und Ghost in seine Home Ecke
	if (logic_control.ghost_counter == logic_control.ghost_chase_time) {
		logic_control.ghost_logic_mode_toggle=1;
	}
	else if (logic_control.ghost_counter == logic_control.ghost_chase_time+logic_control.ghost_home_time)	{
		logic_control.ghost_logic_mode_toggle=0;
		logic_control.ghost_counter=0;
	}

	// Timer, wie lange Pacman Geister fressen kann, wenn er einen großen Punkt gegessen hat
	if (logic_control.pacman_counter == logic_control.pacman_dinner_time) {
		logic_control.pacman_dinner_toggle=1;
		logic_control.pacman_counter=0;
	}
	TIFR0 = _BV(OCF0A);							//clear pending match. For future AVRs
}

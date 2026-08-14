/**
 *  \file global.h
 *  \brief Stellt globale Daten und Macros bereit, so das Funktionen übergreifend darauf zugreifen können
 *
 *  \date: 14.12.2012
 *  \author: Christian Brommer & Dennis Schuldt
 */

#define F_CPU 20000000

#include <stdint.h>
#include "avr/eeprom.h"

#ifndef GLOBAL_DATA_H_
#define GLOBAL_DATA_H_

#ifndef cbi
#define cbi(reg,bit)	reg &= ~(_BV(bit))
#endif
#ifndef sbi
#define sbi(reg,bit)	reg |= (_BV(bit))
#endif
//
// Only support One Player Mode for now.
// Only affects hardware level
//
#define TWOPLAYERMODE 0
//
// Serial port currently only used for sending data
//
#define SERIALRECEIVE 0

/**Defines**/
#if defined __AVR_ATmega328__ || defined __AVR_ATmega328P__
#define joystick_PORT					PORTC
#define joystick_DDR					DDRC
#define joystick1_pin					PC4
#define joystick1_S1_pressed			!(PINC &(1<<PINC4))
#define joystick1_S1_not_pressed		PINC &(1<<PINC4)
#if TWOPLAYERMODE
UNSUPPORTED
#else
#define joystick2_pin					PC5
#define joystick2_S1_pressed			0
#define joystick2_S1_not_pressed		1
#endif

#else							// ATmega1284(p)
#define joystick_PORT					PORTA
#define joystick_DDR					DDRA

#define joystick1_pin					PA4
#define joystick1_S1_pressed			!(PINA &(1<<PINA4))
#define joystick1_S1_not_pressed		PINA &(1<<PINA4)

#define joystick2_pin					PA5
#endif

#if TWOPLAYERMODE
#define joystick2_S1_pressed			!(PINA &(1<<PINA5))
#define joystick2_S1_not_pressed		PINA &(1<<PINA5)
#else
#define joystick2_S1_pressed			0
#define joystick2_S1_not_pressed		1
#endif

//Port Macros
#define PORT_0_OFF						PORTB &= ~(1<<PB0)
#define eeprom_rst_jumper_set			!(PINB &(1<<PINB0))

// adc
volatile uint16_t ADC_result[8];
volatile uint8_t ADC_ch;
volatile uint8_t ADC_max;		//2 for 1 Player, 4 for 2 Players

// global enums
// TODO should be enum direction {neutral_y,right,down,left,up, neutral_x};
enum direction {neutral,right,down,left,up};
enum character_input_mode {joystick,random_direction,target_pacman,target_pacman_4,eyes_home,home,target_ms_pacman};
enum ghost_graphic_state {dont_show, filled, not_filled, eyes, filled_flash, not_filled_flash};
enum gamestate {startscreen, gameplay, gameover, scores, calibration, no_serial, the_end};

// map defines
#define grid_dim 7
#define grid_center_offset 3

// serial enum
enum numerated_serial_data {
	game_state,
	player_mode,
	highscore,
	pacman_score,
	lives,
	level,

	//PacMan
	pacman_x_position,
	pacman_y_position,
	pacman_direction,
	pacman_dead,
	pacman_mouth_lock,

	//Ms. PacMan
	ms_pacman_x_position,
	ms_pacman_y_position,
	ms_pacman_direction,
	ms_pacman_dead,
	ms_pacman_mouth_lock,

	//Ghosts
	ghost_1_x_position,
	ghost_1_y_position,
	ghost_1_state,

	ghost_2_x_position,
	ghost_2_y_position,
	ghost_2_state,

	ghost_3_x_position,
	ghost_3_y_position,
	ghost_3_state,

	ghost_4_x_position,
	ghost_4_y_position,
	ghost_4_state,

	//Fruits
	cherry_show_in_map,
	strawberry_show_in_map,
	banana_show_in_map,
	apple_show_in_map,
	pineapple_show_in_map,

	//dots
	dots,
	seperate_dots, 			//placeholder, not in use

	ms_pacman_score,
	result,
	new_highscore_state
};

#endif /* GLOBAL_DATA_H_ */

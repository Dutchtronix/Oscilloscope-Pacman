/**
 *	\file serial.c
 *
 *	\date 14.12.2012
 * 	\author Christian Brommer & Dennis Schuldt
 *
 * Update by JdR (Dutchtronix) to run the graphics processor
 * on the Dutchtronix AVR Oscilloscope Clock. This clock
 * has a D to A converter needed for rendering the graphics
 * on an analog oscilloscope.
 *
 * Issues to deal with:
 *	- new clock speed is 20 MHz
 *	- lots of tables need to be move to code space
 *	- German comments
 *	- option to replace Pacman char generator with Dutchtronix char generator
 *	- must replace Pacman gbham() with Dutchtronix line drawing code
 *	- make coordinates uint8_t type
 *	- Use Dutchtronix Serial driver but Pacman uses 9 bits data 
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

#include "global.h"
#include "serialG.h"
#include "graphics.h"

void serial_init(void)
{
	cli();
	//mode select

	//Asynchronous_Normal_mode
	UCSR0C &=~(_BV(UMSEL01)|_BV(UMSEL00));
	
	/*Parity Mode*/
	//Disabled
	UCSR0C &=~(_BV(UPM01)|_BV(UPM00));

	/*Stop Bit(s)*/
	//1-Bit
	UCSR0C &=~_BV(USBS0);

	/*Character Size*/
	//9-bit
	UCSR0B |=_BV(UCSZ02);
	UCSR0C |=(_BV(UCSZ00)|_BV(UCSZ01));

	//Asynchronous double Speed (U2Xn = 1)
	UCSR0A  |=(U2X0 << 1);

#if F_CPU==16000000
	//set baudrate to 57600 baud 
	UBRR0L = 34;
#elif F_CPU==20000000
	//set baudrate to 57600 baud: 42,1
	UBRR0L = 42;
	//set baudrate to 115200 baud: 21,1
	// UBRR0L = 21;
#else
	error
#endif

#if 1
	// Enable receiver
	sbi(UCSR0B, RXEN0);
	sbi(UCSR0B, RXCIE0);
#else
	/* Enable receiver and transmitter */
	// TODO are we sending anything?
	UCSR0B |= _BV(RXEN0)|_BV(TXEN0);

	UCSR0B |=(_BV(TXCIE0)|_BV(RXCIE0));
	sei();
#endif
}

void serial_data_allocate(void)
{
	serial_data_address = 0;

	serial_data_pos = 0;

	serial_got_dot_num = 0;

	serial_com_data[0].pointer = &graphics_control.game_state;
	serial_com_data[0].length = 1;

	serial_com_data[1].pointer = &graphics_control.player_mode;
	serial_com_data[1].length = 1;

	serial_com_data[2].pointer = &graphics_control.highscore_string[0];
	serial_com_data[2].length = 8;

	serial_com_data[3].pointer = &graphics_control.score_string_1[0];
	serial_com_data[3].length = 8;

	serial_com_data[4].pointer = &graphics_control.lives;
	serial_com_data[4].length = 1;

	serial_com_data[5].pointer = &graphics_control.level;
	serial_com_data[5].length = 1;

	//PacMan

	serial_com_data[6].pointer = &graphics_control.pacman.x_position;
	serial_com_data[6].length = 1;

	serial_com_data[7].pointer = &graphics_control.pacman.y_position;
	serial_com_data[7].length = 1;

	serial_com_data[8].pointer = &graphics_control.pacman.direction;
	serial_com_data[8].length = 1;

	serial_com_data[9].pointer = &graphics_control.pacman.dead;
	serial_com_data[9].length = 1;

	serial_com_data[10].pointer = &graphics_control.pacman.mouth_lock;
	serial_com_data[10].length = 1;

	//ms_pacman

	serial_com_data[11].pointer = &graphics_control.ms_pacman.x_position;
	serial_com_data[11].length = 1;

	serial_com_data[12].pointer = &graphics_control.ms_pacman.y_position;
	serial_com_data[12].length = 1;

	serial_com_data[13].pointer = &graphics_control.ms_pacman.direction;
	serial_com_data[13].length = 1;

	serial_com_data[14].pointer = &graphics_control.ms_pacman.dead;
	serial_com_data[14].length = 1;

	serial_com_data[15].pointer = &graphics_control.ms_pacman.mouth_lock;
	serial_com_data[15].length = 1;

	/* Ghosts */

	//ghost_1
	serial_com_data[16].pointer = &graphics_control.ghosts.ghost_1.x_position;
	serial_com_data[16].length = 1;

	serial_com_data[17].pointer = &graphics_control.ghosts.ghost_1.y_position;
	serial_com_data[17].length = 1;

	serial_com_data[18].pointer = &graphics_control.ghosts.ghost_1.state;
	serial_com_data[18].length = 1;

	//ghost_2
	serial_com_data[19].pointer = &graphics_control.ghosts.ghost_2.x_position;
	serial_com_data[19].length = 1;

	serial_com_data[20].pointer = &graphics_control.ghosts.ghost_2.y_position;
	serial_com_data[20].length = 1;

	serial_com_data[21].pointer = &graphics_control.ghosts.ghost_2.state;
	serial_com_data[21].length = 1;

	//ghost_3
	serial_com_data[22].pointer = &graphics_control.ghosts.ghost_3.x_position;
	serial_com_data[22].length = 1;

	serial_com_data[23].pointer = &graphics_control.ghosts.ghost_3.y_position;
	serial_com_data[23].length = 1;

	serial_com_data[24].pointer = &graphics_control.ghosts.ghost_3.state;
	serial_com_data[24].length = 1;

	//ghost_4
	serial_com_data[25].pointer = &graphics_control.ghosts.ghost_4.x_position;
	serial_com_data[25].length = 1;

	serial_com_data[26].pointer = &graphics_control.ghosts.ghost_4.y_position;
	serial_com_data[26].length = 1;

	serial_com_data[27].pointer = &graphics_control.ghosts.ghost_4.state;
	serial_com_data[27].length = 1;

	/* fruits */

	//cherry
	serial_com_data[28].pointer = &graphics_control.fruits.cherry.show_in_map;
	serial_com_data[28].length = 1;

	//strawberry
	serial_com_data[29].pointer = &graphics_control.fruits.strawberry.show_in_map;
	serial_com_data[29].length = 1;

	//banana
	serial_com_data[30].pointer = &graphics_control.fruits.banana.show_in_map;
	serial_com_data[30].length = 1;

	//apple
	serial_com_data[31].pointer = &graphics_control.fruits.apple.show_in_map;
	serial_com_data[31].length = 1;

	//pineapple
	serial_com_data[32].pointer = &graphics_control.fruits.pineapple.show_in_map;
	serial_com_data[32].length = 1;


	//dots
	serial_com_data[33].pointer = &graphics_control.dots[0];
	serial_com_data[33].length = 243;

	/**attention**/
	//Datenadresse 34 wird für separate dots in "send_dot_segment()" benutzt!


	serial_com_data[35].pointer = &graphics_control.score_string_2[0];
	serial_com_data[35].length = 8;

	serial_com_data[36].pointer = &graphics_control.result;
	serial_com_data[36].length = 1;

	serial_com_data[37].pointer = &graphics_control.new_highscore_state;
	serial_com_data[37].length = 1;
}

extern void TurnBeamOff(void);
extern void RestoreBeam(void);

//RX InterruptServiceRoutine, USART0
#ifdef USART_RX_vect
ISR(USART_RX_vect)
#else
ISR(USART0_RX_vect)
#endif
{
	TurnBeamOff();
	// Reading data from buffer, clears the buffer

	if(UCSR0B & _BV(RXB80))	{			// 9. Daten-Bit überprüfen
		// Receive Data address
		serial_data_address =  UDR0;
		serial_data_pos = 0;			//zurücksetzen bei neuer Daten-Adresse
	}
	else {
		// Receive Data
		if(serial_data_address == 34) {	//special dot routine für Daten-Addresse "34"
			if(!serial_got_dot_num) {
				serial_data_pos = UDR0;	//Nummer des Punktes schreiben, da sie noch nicht empfangen wurde
				serial_got_dot_num = 1;
			}
			else {
				//Dot number received; write the dot information to the corresponding previously received position.
				//dot Nummer wurde empfangen, schreiben der dot Information in die entsprechend zuvor empfangene Position
				(serial_com_data[33].pointer)[serial_data_pos] = UDR0;
				serial_got_dot_num = 0;
			}
		}
		else {
			if(serial_com_data[serial_data_address].length == 1) {	//Prüfen ob in ein Array geschrieben werden soll
				//serial_data_address does not belong to an Array
				*(serial_com_data[serial_data_address].pointer) = UDR0;
			}
			else {
				//serial_data_address gehört zu einem Array
				//array nur beschreiben solange es im gültigen bereich ist(also nicht über die Länge das Arrays hinaus)
				if(serial_data_pos <= serial_com_data[serial_data_address].length) {
					(serial_com_data[serial_data_address].pointer)[serial_data_pos] = UDR0;	//Empfangene Daten in die Array-Positionen schreiben
					serial_data_pos++;	//increment position in Array
				}
				else {
					volatile uint8_t dump = UDR0;	// buffer leeren wenn es auserhalb des Array-bereichs ist
				}
			}
		}
	}
	RestoreBeam();
}


/***Für die Logik***/

// enum, zum vereinfachten auswählen der zu verschickenden Daten im Logik-Controller

//enum numerated_serial_data{
//	game_state,
//	player_mode,
//	highscore,
//	score,
//	lives,
//	level,
//
//	//PacMan
//	pacman_x_position,
//	pacman_y_position,
//	pacman_direction,
//	pacman_dead,
//	pacman_mouth_lock,
//
//	//Ms. PacMan
//	ms_pacman_x_position,
//	ms_pacman_y_position,
//	ms_pacman_direction,
//	ms_pacman_dead,
//	ms_pacman_mouth_lock,
//
//	//Ghosts
//	ghost_1_x_position,
//	ghost_1_y_position,
//	ghost_1_state,
//
//	ghost_2_x_position,
//	ghost_2_y_position,
//	ghost_2_state,
//
//	ghost_3_x_position,
//	ghost_3_y_position,
//	ghost_3_state,
//
//	ghost_4_x_position,
//	ghost_4_y_position,
//	ghost_4_state,
//
//	//Fruits
//	cherry_show_in_map,
//	strawberry_show_in_map,
//	banana_show_in_map,
//	apple_show_in_map,
//	pineapple_show_in_map,
//
//	//dots
//	dots
//};

/**
 *	\file main.c
 *	\brief Hauptprogramm des Grafik-Controllers
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
 *	- new clock speed is 20 MHz. Timer constants.
 *	- lots of tables need to be move to code space
 *	- German comments
 *	- option to replace Pacman char generator with Dutchtronix char generator
 *	- must replace Pacman gbham() with Dutchtronix CDrawVector()
 *	- make coordinates uint8_t type
 *	- Use Dutchtronix Serial driver but Pacman uses 9 bits data so use Pacman driver for now.
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <inttypes.h>

#include "global.h"
#include "timerG.h"
#include "serialG.h"

//include graphic library's
#include "map_functions.h"
#include "text_generator.h"
#include "graphics.h"

//#define draft	///< define welches, wenn es definiert ist, nur die trysection darstellt

void DACinit(void);
extern uint8_t XOffset, YOffset, GXOffset, GYOffset;
extern uint8_t LastChanA, LastChanB;

/**
 * \brief initialisieren der IO Ports des Controllers
 *
 * bei allen nicht verwendeten IO´s die Pull-Up Widerstände aktivieren
*/
void io_init(void)
{
	DACinit();
}

void InitSwitch(void)
{
	SWDDR &= ~(_BV(SW1Bit)|_BV(SW2Bit));	//set as input
	SWPort |= (_BV(SW1Bit)|_BV(SW2Bit));	//enable pull-up resistor
}

/**
 * \brief Hauptfunktion die im Controller verarbeitet wird
 *
 * \callgraph
*/
int main(void)
{
	io_init();						//initialize controller IO Ports

	graphics_control_allocate();	//initialisiert die Daten für die Grafikschnittstelle

	timer_data_allocate();			// initialize timer Data

	timer_init();					// initialize timer

	serial_data_allocate();

	serial_init();

//	InitSwitch();

	sei();

	while(1) {


#ifndef draft	//ist draft nicht definiert, wird das eigentliche Spiel angezeigt


		//write controls !!only for check their functionality!!

//		graphics_control.game_state = 6;
//		graphics_control.result = 2;
//
//		graphics_control.new_highscore_state = 1;
//		graphics_control.level = 5;
//
//		graphics_control.highscore = 132456;
//		graphics_control.score = 1334;
//
//		graphics_control.player_mode = 2;
//
//		graphics_control.ms_pacman.direction = 1;
//		graphics_control.pacman.direction = 1;
//
//		graphics_control.pacman.mouth_lock = 0;
//		graphics_control.ms_pacman.mouth_lock = 0;
//

//		graphics_control.ghosts.ghost_1.state = 1;

//		graphics_control.ghosts.ghost_2.state = 1;

//		graphics_control.ghosts.ghost_3.state = 1;

//		graphics_control.ghosts.ghost_4.state = 1;

//
//
//		graphics_control.fruits.cherry.show_in_map = 1;
//		graphics_control.fruits.strawberry.show_in_map = 0;
//		graphics_control.fruits.banana.show_in_map = 0;
//		graphics_control.fruits.apple.show_in_map = 0;
//		graphics_control.fruits.pineapple.show_in_map = 0;

		//(ende) write controls !!only for checking their functionality!!


		// Each case contains a while loop so that it is not necessary to constantly jump in and out,
		// As long as serial communication runs via interrupts, this poses no problem.
		// The while loop is exited when the "game_state" changes.
		switch (graphics_control.game_state) {

			case 0:	//Startbildschirm

				while(graphics_control.game_state == 0) {
					draw_startup_screen();
				}

				break;

			case 1: //Gameplay
			case 2: //Game Over wird in "draw_gameplay()" berücksichtigt

				while(graphics_control.game_state == 1 || graphics_control.game_state == 2) {
					draw_gameplay();
				}

				break;

			case 3:	//Scores

				while(graphics_control.game_state == 3) {
					//dummy´s
					draw_scores();
				}

				break;

			case 4: //Calibration

				while(graphics_control.game_state == 4)	{
					draw_calibration();
				}

				break;

			case 5: //show "no serial connection"
				while(graphics_control.game_state == 5) {
					if(pacman_flash.value) {
						print_gbham_F(PSTR("!"),127-88,127); print_gbham_F(PSTR("!"),127+88,127);
					}
					print_gbham_F(PSTR("no serial connection"),47,127);
				}
				break;

			case 6: //Level 254 reached

				while(graphics_control.game_state == 6) {
					print_gbham_F(PSTR("The End !"),97,130);
					print_gbham_F(PSTR("Level 254"),67,100);
					if(pacman_flash.value) {
						print_gbham_F(PSTR("Done"),160,100);
					}
				}
				break;

			default:
				graphics_control.game_state = 1;
				break;

		}

#else //ist draft definiert, wird die trysection dargestellt

	//trysection


//	place_ghost(38,24, 0);
//	place_ghost(58,24, 1);
//	place_pacman(75,24,3,0);
//	place_pacman(38,40,4,1);
//
//	place_pacman(38,23,3,1);
//
//	place_pacman(15,40,1,1);
//
//	place_pacman(15,23,2,1);
//	draw_map();

//	place_fruit(90,24,1);
//	place_fruit(105,24,2);
//	place_fruit(118,24,3);
//	place_fruit(130,24,4);
//	place_fruit(145,24,5);
//	place_eyes(160,24);
//
//	place_ghost(255-pacman_move.value - 20,128,0);

		//Zeichensatz
//		print_gbham_F(PSTR("0 1 2 3 4 5 6 7 8 9 : !"),20,150);
//		print_gbham_F(PSTR("A B C D E F G H i j k l"),20,130);
//		print_gbham_F(PSTR("M N O P Q R S T U V W"),20,110);
//		print_gbham_F(PSTR("X Y Z ("),20,90);
#endif
	}


}

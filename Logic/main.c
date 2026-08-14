/**
 *  \file main.c
 *  \brief Hauptprogramm des Logik-Controllers
 *
 *  Info: ADC Ports = PORTA
 * 		 - PA0 = y-Axis Joystick 1
 * 		 - PA1 = x-Axis Joystick 1
 * 		 - PA2 = y-Axis Joystick 2
 * 		 - PA3 = x-Axis Joystick 2
 * 		 - PA4 = Trigger Joystick 1
 * 		 - PA5 = Trigger Joystick 2
 *
 * 		 Serial
 * 		 - PD0 = RX0 (Spieler 1)
 * 		 - PD1 = TX0 (Spieler 1)
 * 		 - PD2 = RX1 (Spieler 2)
 * 		 - PD3 = TX1 (Spieler 2)
 *
 *  \date: 14.12.2012
 *  \author: Christian Brommer & Dennis Schuldt
 */

//
// Plan:
//	 run this on an Arduino type Atmega328p (eventually)
//	but start logic portion on ATmega1284(p) in single player mode
//
// Limitations:
//	Need to move fixed data to code based since only 2K RAM
//	Atmega328p has only 1 serial port so only single player mode (AVR32EA28 has 3 UARTS)
//	Moved constant data to code space to bring RAM use under 2K
//
// Assumption:
//	9 bit serial mode supported
//	20 MHz with xtal supported
//
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include "string.h"
#include "stdlib.h"
#include "avr/eeprom.h"

#include "global.h"

#include "util/delay.h"

#include "timer.h"
#include "analog.h"
#include "character_logic.h"
#include "ghost_ai.h"
#include "game_logic.h"
#include "serial_send.h"
#include "serial.h"

uint32_t highscore_eeprom EEMEM = 0;	// EEPROM initialize

/**
 * \brief Funktion zum Initialisieren der notwendigen I/O Pins des Mikrocontrollers
*/
void io_init(void)
{
	// Joysticks Trigger S1 - high (pull-up resistor)
	sbi(joystick_PORT, joystick1_pin);
#if TWOPLAYERMODE
	sbi(joystick_PORT, joystick2_pin);
#endif
	//joystick_PORT |= ((_BV(joystick1_pin)) | (_BV(joystick2_pin)));

	// set as input
	cbi(joystick_DDR, joystick1_pin);
#if TWOPLAYERMODE
	cbi(joystick_DDR, joystick2_pin);
#endif
	//joystick_DDR &= ~((_BV(joystick1_pin)) | (_BV(joystick2_pin)));

	// debug, LEDs, Ports etc...
	cbi(PORTB, PB1);
	sbi(DDRB, PB1);

	sbi(PORTB, PB0);
	cbi(DDRB, PB0);
}

/**
 * \brief Funktion zum Senden der Logik Daten über die serielle Schnittstelle (1 Spieler)
*/
void display_1player_game()
{
	if (logic_control.ghost_got_pacman == 1) {					// Wenn der Ghost Pacman gefangen hat
		send_to_graphics1();									// Wenn Pacman gefangen, Pacman auflösen und Sachen zurücksetzen
		logic_control.pacman.dead=0;
		logic_control.lives-=1;

		pacman_init();
		ghost_data_init();
		_delay_ms(1800);										// 1,8s Zeitgeben um Pacman aufzulösen

		send_logic_data();										// um Lebensanzeige upzudaten
		logic_control.ghost_got_pacman=0;						// Hilfsvariable zurücksetzen
	} else {
		send_to_graphics1();									// Daten an Grafikeinheit 1 schicken
	}
}

#if TWOPLAYERMODE
/**
 * \brief Funktion zum Senden der Logik Daten über die serielle Schnittstelle (2 Spieler)
*/
void display_2player_game()
{
	if (logic_control.ghost_got_pacman == 1) {					// Wenn der Ghost Pacman gefangen hat
		send_to_graphics1();
		send_to_graphics2();
		logic_control.pacman.dead=0;
		logic_control.lives-=1;

		pacman_init();
		ms_pacman_init();
		ghost_data_init();
		_delay_ms(1800);

		send_logic_data();
		logic_control.ghost_got_pacman=0;
	} else {
		if (logic_control.ghost_got_pacman == 2) {				// Wenn der Ghost Ms.Pacman gefangen hat
			send_to_graphics1();
			send_to_graphics2();
			logic_control.ms_pacman.dead=0;
			logic_control.lives-=1;

			pacman_init();
			ms_pacman_init();
			ghost_data_init();
			_delay_ms(1800);

			send_logic_data();
			logic_control.ghost_got_pacman=0;
		} else {
			send_to_graphics1();
			send_to_graphics2();
		}
	}
}
#endif

/**
 * \brief Funktion zum auslesen bzw. rücksetzen des EEPROM
*/
void eeprom_readout(void)
{
	if (eeprom_rst_jumper_set) {										// Wenn EEPROM Reset Jumper gesetzt, dann EEPROM auf 0 schreiben
		eeprom_write_dword(&highscore_eeprom,0);
	}
	logic_control.highscore=eeprom_read_dword(&highscore_eeprom);		 // read Highscore from EEPROM
}

/**
 * \callgraph
 */
int main(void)
{
	io_init();
	timer_init();
	analog_init();
	serial_init_logic();
	serial_data_allocate();
	logic_init();
	pacman_init();
	ghost_data_init();

	eeprom_readout();
	send_graphics_init(); // Als letztes Grafik Init schicken, sodass schon alles initialisiert ist

	while(1) {
		switch (logic_control.game_state)							// Switch für Game State Machine
		{
			case startscreen: //start screen

				if (joystick1_S1_pressed) {
					logic_control.player1_mode=0;
					ADC_max=2;										// ADC Kanäle Anzahl für 1 Spieler
					logic_control.game_state=gameplay;
					send_logic_data();
				}

				if (joystick2_S1_pressed) {
					logic_control.player1_mode=1;
					logic_control.player2_mode=2;
					ms_pacman_init();
					ADC_max=4;										// ADC Kanäle Anzahl für 2 Spieler
					logic_control.game_state=gameplay;
					send_logic_data();
				}
				break;

			case gameplay: //Gameplay
				if (logic_control.player1_mode==0) {							// 1 player mode
					update_pacman();											// Pacman Position updaten
					update_ghosts();											// Geister Positionen updaten

					check_fruits(&logic_control.fruits, &logic_control.pacman); // Überprüfen auf Früchte
					check_dots(&logic_control.pacman);							// Überprüfen auf gesammelte Punkte
					logic_check();												// Logik überprüfen

					display_1player_game();										// Daten rausschicken

					// Solange Pacman oder Ms.Pacman sich nicht bewegt, ist die Logik angehalten
					if (logic_control.pacman.direction == neutral && logic_control.ms_pacman.direction == neutral)
						logic_control.hold=1;
					else
						logic_control.hold=0;
				}
#if TWOPLAYERMODE
				if (logic_control.player2_mode==2) {
					update_pacman_mspacman();
					update_ghosts();

					check_fruits(&logic_control.fruits, &logic_control.pacman);
					check_fruits(&logic_control.fruits, &logic_control.ms_pacman);
					check_dots(&logic_control.pacman);
					check_dots(&logic_control.ms_pacman);
					logic_check();

					display_2player_game();

					// Solange Pacman oder Ms.Pacman sich nicht bewegt, ist die Logik angehalten
					if (logic_control.pacman.direction == neutral && logic_control.ms_pacman.direction == neutral)
						logic_control.hold=1;
					else
						logic_control.hold=0;
				} else {
					if (joystick2_S1_pressed) {					// "else" Bedingung damit der 2te Spieler dem Einzelspiel jederzeit beitreten kann
						logic_control.player1_mode=1;			// auch bei Player 1 - Ms. Pacman anzeigen
						logic_control.player2_mode=2;
						ms_pacman_init();

						ADC_max=4;								// ADC Kanaäle für 2 Spieler anpassen
						send_all_dots();
						send_to_graphics1();					// für sofortige Ms.Pacman Position für Grafikseite 1
						send_to_graphics2();
						send_logic_data();
					}
				}
#endif
				break;


			case gameover: //Game Over
				if (joystick1_S1_pressed || joystick2_S1_pressed) {	// wenn Gameover, Joystick Trigger drücken für Scorescreen
					while(joystick1_S1_pressed || joystick2_S1_pressed);	//debounce
					logic_control.game_state=scores;
					_delay_ms(100);											//more debounce

					send_graphics_init();
				}
				break;


			case scores:	//Scores
				// Score Werte in String schreiben und rausschicken
				ltoa(logic_control.pacman.score, (char *)string_pacman_score, 10);
				strcat((char *)string_pacman_score, "\n");
				serial_send(pacman_score, 0);
				serial_send(pacman_score, 1);

				ltoa(logic_control.ms_pacman.score, (char *)string_ms_pacman_score, 10);
				strcat((char *)string_ms_pacman_score, "\n");
				serial_send(ms_pacman_score, 0);
				serial_send(ms_pacman_score, 1);

				// Abfragen für Gewinner/Verlierer und entsprechenden Zustand senden
				if (logic_control.pacman.score > logic_control.ms_pacman.score)	{
					logic_control.result=1;
					serial_send(result, 0);
					logic_control.result=2;
					serial_send(result, 1);
				} else {
					if (logic_control.pacman.score < logic_control.ms_pacman.score) {
						logic_control.result=2;
						serial_send(result, 0);
						logic_control.result=1;
						serial_send(result, 1);
					} else
						if (logic_control.pacman.score == logic_control.ms_pacman.score) {
							logic_control.result=1;
							serial_send(result, 0);
							serial_send(result, 1);
						}
				}
				// Check if there was a new High Score
				if ((logic_control.pacman.score > logic_control.highscore) || (logic_control.ms_pacman.score > logic_control.highscore)) {
					if (logic_control.pacman.score > logic_control.highscore)
						logic_control.highscore=logic_control.pacman.score;
					if (logic_control.ms_pacman.score > logic_control.highscore)
						logic_control.highscore=logic_control.ms_pacman.score;

					// Write High Score to String and send
					ltoa(logic_control.highscore, (char *)string_highscore, 10);
					strcat((char *)string_highscore, "\n");
					serial_send(highscore, 0);
					serial_send(highscore, 1);

					eeprom_update_dword(&highscore_eeprom, logic_control.highscore);		// write new High Score to EEPROM 

					// Send out a notification about the new high score
					logic_control.new_highscore_state=1;
					serial_send(new_highscore_state, 0);
					serial_send(new_highscore_state, 1);
				}


				if (joystick1_S1_pressed || joystick2_S1_pressed) {				// Confirm by pressing the joystick trigger
					while(joystick1_S1_pressed || joystick2_S1_pressed); 		// "debounce"

					logic_init();
					pacman_init();
					ghost_data_init();
					logic_control.game_state=startscreen;

					send_graphics_init();
				}

				break;


			case calibration: //Calibration
				if (joystick1_S1_pressed || joystick2_S1_pressed) {				// Mit einem Joystick Trigger bestätigen
					while (joystick1_S1_pressed || joystick2_S1_pressed);		//debounce

					for (uint8_t a=0; a<5 ;a++)	{		// Loop to reliably read all four ADC channels. ADC_max
						joystick_calibration();
					}

					logic_control.game_state=startscreen;
					serial_send(game_state, 0);
					serial_send(game_state, 1);
				}
				break;

			case the_end: //The End
				if ((joystick1_S1_pressed || joystick2_S1_pressed)) {	//wenn Level 254 geschafft, Joystick drücken für Scorescreen
					while (joystick1_S1_pressed || joystick2_S1_pressed);
					logic_control.game_state=scores;

					send_graphics_init();
				}
				break;

			default:
				break;
		}
	}
}

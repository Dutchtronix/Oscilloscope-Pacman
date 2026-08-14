/**
 *  \file serial.c
 *
 *  \date: 14.12.2012
 *  \author: Christian Brommer & Dennis Schuldt
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

#include "serial.h"
#include "character_logic.h"
#include "global.h"

#if 0
//
// serial_init() for the graphics controller
// Same file was used for both components
//
void serial_init(void)
{
	cli();
	//mode select
		//Asynchronous double Speed (U2Xn = 1)
		UCSR0A  |= (U2X0 <<1);

		//Asynchronous_Normal_mode
		UCSR0C &=~((1<<UMSEL01)|(1<<UMSEL00));

	/*Parity Mode*/
		//Disabled
		UCSR0C &=~((1<<UPM01)|(1<<UPM00));

	/*Stop Bit(s)*/
		//1-Bit
		UCSR0C &=~(1<<USBS0);

	/*Character Size*/
		//8-bit
//		UCSR0B &=~(1<<UCSZ02);
//		UCSR0C |=((1<<UCSZ00)|(1<<UCSZ01));

		//9-bit
		UCSR0B |=(1<<UCSZ02);
		UCSR0C |=((1<<UCSZ00)|(1<<UCSZ01));

		//set baudrate to 250000.
		UBRR0L = 4;

		/* Enable receiver and transmitter */
		UCSR0B |= (1<<RXEN0)|(1<<TXEN0);

		UCSR0B |=((1<<TXCIE0)|(1<<RXCIE0));

		sei();
}
#endif
//
// if 2 serial ports available, could use the second port
// to send debug data in 1 player mode
//
void serial_init_logic(void)	//initialize both Ports
{

	//mode select
	//Asynchronous double Speed (U2Xn = 1)
	UCSR0A |=(U2X0 <<1);
	
	//Asynchronous_Normal_mode
	UCSR0C &=~((1<<UMSEL01)|(1<<UMSEL00));

	/*Parity Mode*/
	//Disabled
	UCSR0C &=~((1<<UPM01)|(1<<UPM00));

	/*Stop Bit(s)*/
	//1-Bit
	UCSR0C &=~(1<<USBS0);

	/*Character Size*/

	//9-bit
	UCSR0B |=(1<<UCSZ02);
	UCSR0C |=((1<<UCSZ00)|(1<<UCSZ01));

#if F_CPU==16000000
	//set baudrate to 250000 ?? 1,34 is 57.6k baud rate
	UBRR0L = 34;
#elif F_CPU==20000000
	//set baudrate to 57600 baud: 42,1
	UBRR0L = 42;
	//set baudrate to 115200 baud: 21,1
	// UBRR0L = 21;
#else
	error
#endif

#if SERIALRECEIVE
	/* Enable receiver and transmitter */
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0);
	/* Enable receiver and transmitter interrupts */
	UCSR0B |=(1<<TXCIE0);	TODO
	/* Enable receiver and transmitter interrupts */
	UCSR0B |=((1<<TXCIE0)|(1<<RXCIE0));
#else
	/* Enable transmitter only */
	UCSR0B |= (1<<TXEN0);
#endif

#if TWOPLAYERMODE
	//mode select
	//Asynchronous double Speed (U2Xn = 1)
	UCSR1A |=(U2X1 <<1);

	//Asynchronous_Normal_mode
	UCSR1C &=~((1<<UMSEL11)|(1<<UMSEL10));
	
	/*Parity Mode*/
	//Disabled
	UCSR1C &=~((1<<UPM11)|(1<<UPM10));

	/*Stop Bit(s)*/
	//1-Bit
	UCSR1C &=~(1<<USBS1);

	/*Character Size*/

	//9-bit
	UCSR1B |=(1<<UCSZ12);
	UCSR1C |=((1<<UCSZ10)|(1<<UCSZ11));

#if F_CPU==16000000
	//set baudrate to 250000 ?? 1,34 is 57.6k baud rate
	UBRR1L = 34;
#elif F_CPU==20000000
	//set baudrate to 57600 baud: 42,1
	UBRR1L = 42;
	//set baudrate to 500000 baud: 4,1
	//UBRR1L = 4;
#else
	error
#endif

#if SERIALRECEIVE
	/* Enable receiver and transmitter */
	UCSR1B |= (1<<RXEN1)|(1<<TXEN1);
	/* Enable receiver and transmitter interrupts */
	UCSR1B |=(1<<TXCIE1);	TODO
	/* Enable receiver and transmitter interrupts */
	UCSR1B |=((1<<TXCIE1)|(1<<RXCIE1));
#else
	/* Enable transmitter only */
	UCSR1B |= (1<<TXEN1);
#endif

#endif

}

void serial_data_allocate(void)
{
	serial_data_address = 0;

	serial_data_pos = 0;

	serial_got_dot_num = 0;

	serial_com_data[0].pointer = &logic_control.game_state;
	serial_com_data[0].length = 1;

	serial_com_data[1].pointer = &logic_control.player_mode;
	serial_com_data[1].length = 1;

	serial_com_data[2].pointer = &string_highscore[0];
	serial_com_data[2].length = 8;

	serial_com_data[3].pointer = &string_pacman_score[0];
	serial_com_data[3].length = 8;

	serial_com_data[4].pointer = &logic_control.lives;
	serial_com_data[4].length = 1;

	serial_com_data[5].pointer = &logic_control.level;
	serial_com_data[5].length = 1;

	//PacMan

	serial_com_data[6].pointer = &logic_control.pacman.x_pixel;
	serial_com_data[6].length = 1;

	serial_com_data[7].pointer = &logic_control.pacman.y_pixel;
	serial_com_data[7].length = 1;

	serial_com_data[8].pointer = &logic_control.pacman.direction;
	serial_com_data[8].length = 1;

	serial_com_data[9].pointer = &logic_control.pacman.dead;
	serial_com_data[9].length = 1;

	serial_com_data[10].pointer = &logic_control.pacman.mouth_lock;
	serial_com_data[10].length = 1;

	//ms_pacman

	serial_com_data[11].pointer = &logic_control.ms_pacman.x_pixel;
	serial_com_data[11].length = 1;

	serial_com_data[12].pointer = &logic_control.ms_pacman.y_pixel;
	serial_com_data[12].length = 1;

	serial_com_data[13].pointer = &logic_control.ms_pacman.direction;
	serial_com_data[13].length = 1;

	serial_com_data[14].pointer = &logic_control.ms_pacman.dead;
	serial_com_data[14].length = 1;

	serial_com_data[15].pointer = &logic_control.ms_pacman.mouth_lock;
	serial_com_data[15].length = 1;

	/* Ghosts */

	//ghost_1
	serial_com_data[16].pointer = &logic_control.ghost_1.x_pixel;
	serial_com_data[16].length = 1;

	serial_com_data[17].pointer = &logic_control.ghost_1.y_pixel;
	serial_com_data[17].length = 1;

	serial_com_data[18].pointer = &logic_control.ghost_1.graphic_state;
	serial_com_data[18].length = 1;

	//ghost_2
	serial_com_data[19].pointer = &logic_control.ghost_2.x_pixel;
	serial_com_data[19].length = 1;

	serial_com_data[20].pointer = &logic_control.ghost_2.y_pixel;
	serial_com_data[20].length = 1;

	serial_com_data[21].pointer = &logic_control.ghost_2.graphic_state;
	serial_com_data[21].length = 1;

	//ghost_3
	serial_com_data[22].pointer = &logic_control.ghost_3.x_pixel;
	serial_com_data[22].length = 1;

	serial_com_data[23].pointer = &logic_control.ghost_3.y_pixel;
	serial_com_data[23].length = 1;

	serial_com_data[24].pointer = &logic_control.ghost_3.graphic_state;
	serial_com_data[24].length = 1;

	//ghost_4
	serial_com_data[25].pointer = &logic_control.ghost_4.x_pixel;
	serial_com_data[25].length = 1;

	serial_com_data[26].pointer = &logic_control.ghost_4.y_pixel;
	serial_com_data[26].length = 1;

	serial_com_data[27].pointer = &logic_control.ghost_4.graphic_state;
	serial_com_data[27].length = 1;

	/* fruits */

	//cherry
	serial_com_data[28].pointer = &logic_control.fruits.cherry.show_in_map;
	serial_com_data[28].length = 1;

	//strawberry
	serial_com_data[29].pointer = &logic_control.fruits.strawberry.show_in_map;
	serial_com_data[29].length = 1;

	//banana
	serial_com_data[30].pointer = &logic_control.fruits.banana.show_in_map;
	serial_com_data[30].length = 1;

	//apple
	serial_com_data[31].pointer = &logic_control.fruits.apple.show_in_map;
	serial_com_data[31].length = 1;

	//pineapple
	serial_com_data[32].pointer = &logic_control.fruits.pineapple.show_in_map;
	serial_com_data[32].length = 1;


	//dots
	serial_com_data[33].pointer = &logic_control.dots[0];
	serial_com_data[33].length = 244;

	/**attention**/
	//Datenadresse 34 wird für separate dots in "send_dot_segment()" benutzt!

	serial_com_data[35].pointer = &string_ms_pacman_score[0];
	serial_com_data[35].length = 8;

	serial_com_data[36].pointer = &logic_control.result;
	serial_com_data[36].length = 1;

	serial_com_data[37].pointer = &logic_control.new_highscore_state;
	serial_com_data[37].length = 1;
}


void serial_send(uint8_t data_address, uint8_t serial_port)
{
	uint8_t ready = 0;			//ist 0 so lange nicht alle Daten verschickt wurden
	serial_data_pos = 0;		//zurücksetzen bei neuer Daten-Adresse

	switch (serial_port) {
		case 0:	//von Port 0 senden
			while(!ready) {		//so lange durchlaufen bis der Sende-Vorgang abgeschlossen wurde
				//jeweils warten, bis der Buffer wieder leer ist

				//Adresse Senden
				while ( !( UCSR0A & (1<<UDRE0)) ) ;
				UCSR0B |=(1<<TXB80);	//9. Bit setzen
				UDR0 = data_address;	//Daten Addresse schicken

				//Daten senden
				while ( !( UCSR0A & (1<<UDRE0)) ) ;
				UCSR0B &=~(1<<TXB80);	//9. Bit zurücksetzen
				if(serial_com_data[data_address].length == 1) {	//Prüfen ob ein Array gesendet werden soll
					//serial_data_address gehört nicht zu einem Array
					UDR0 = *(serial_com_data[data_address].pointer);	//Wert senden
					ready = 1;	//Sende-Vorgang abgeschlossen
				}
				else {
					//serial_data_address gehört zu einem Array
					//Array nur beschreiben so lange es im gültigen Bereich ist(also nicht über die Länge das Arrays hinaus)
					for(serial_data_pos = 0; serial_data_pos < serial_com_data[data_address].length; serial_data_pos++) {
						while ( !( UCSR0A & (1<<UDRE0)) ) ;
						UDR0 = (serial_com_data[data_address].pointer)[serial_data_pos];	//Wert senden
					}
					ready = 1;	//Sende-Vorgang abgeschlossen
				}
			}
			break;

#if TWOPLAYERMODE
		case 1:	//von Port 1 senden
			while(!ready) {	//so lange durchlaufen bis der Sende-Vorgang abgeschlossen wurde
				//jeweils warten, bis der Buffer wieder leer ist

				//Adresse Senden
				while ( !( UCSR1A & (1<<UDRE1)) ) ;
				UCSR1B |=(1<<TXB81);	//9. Bit setzen
				UDR1 = data_address;	//Daten Addresse schicken

				//Daten senden
				while ( !( UCSR1A & (1<<UDRE1)) ) ;
				UCSR1B &=~(1<<TXB81);	//9. Bit zurücksetzen
				if(serial_com_data[data_address].length == 1) {	//Prüfen ob ein Array gesendet werden soll
					//serial_data_address gehört nicht zu einem Array
					UDR1 = *(serial_com_data[data_address].pointer);	//Wert senden
					ready = 1;	//Sende-Vorgang abgeschlossen
				}
				else {
					//serial_data_address gehört zu einem Array
					//Array nur beschreiben so lange es im gültigen Bereich ist(also nicht über die Länge das Arrays hinaus)
					for(serial_data_pos = 0; serial_data_pos < serial_com_data[data_address].length; serial_data_pos++) {
						while ( !( UCSR1A & (1<<UDRE1)) ) ;
						UDR1 = (serial_com_data[data_address].pointer)[serial_data_pos];	//Wert senden
					}
					ready = 1;	//Sende-Vorgang abgeschlossen
				}
			}
			break;
#endif

		default:	//nichts senden
			break;
	}
}

void send_dot_segment(uint8_t dot_num, uint8_t state, uint8_t serial_port)
{
	//jeweils warten, bis der Buffer wieder leer ist

	switch (serial_port) {
		case 0:
			//Adresse Senden
			while ( !( UCSR0A & (1<<UDRE0)) ) ;
			UCSR0B |=(1<<TXB80);	//9. Bit setzen
			UDR0 = 34;	//Daten Addresse schicken

			//Daten senden
			while ( !( UCSR0A & (1<<UDRE0)) ) ;
			UCSR0B &=~(1<<TXB80);	//9. Bit zurücksetzen
			UDR0 = dot_num;	//Nummer des dot senden
			while ( !( UCSR0A & (1<<UDRE0)) ) ;
			UDR0 = state;	//Status des dot senden
			break;

#if TWOPLAYERMODE
		case 1:
			//Adresse Senden
			while ( !( UCSR1A & (1<<UDRE1)) ) ;
			UCSR1B |=(1<<TXB81);	//9. Bit setzen
			UDR1 = 34;	//Daten Addresse schicken

			//Daten senden
			while ( !( UCSR1A & (1<<UDRE1)) ) ;
			UCSR1B &=~(1<<TXB81);	//9. Bit zurücksetzen
			UDR1 = dot_num;	//Nummer des dot senden
			while ( !( UCSR1A & (1<<UDRE1)) ) ;
			UDR1 = state;	//Status des dot senden
			break;
#endif

		default:	//nichts senden
			break;
	}
}

#if SERIALRECEIVE
//RX InterruptServiceRoutine, USART0
// This code seems to be for the Graphics controller which receives the data. TODO
ISR(USART0_RX_vect)
{
	// Reading data from buffer, clears the buffer

	if(UCSR0B & (1<<RXB80))		// 9. Daten-Bit überprüfen
	{
		//Daten Addresse empfangen

		serial_data_address =  UDR0;
		serial_data_pos = 0;	//zurücksetzen bei neuer Daten-Adresse
	}
	else
	{
		//Daten empfangen

		if(serial_data_address == 34)	//special dot routine für Daten-Addresse "34"
		{
			if(!serial_got_dot_num)
			{
				serial_data_pos = UDR0;	//Nummer des Punktes schreiben, da sie noch nicht empfangen wurde
				serial_got_dot_num = 1;
			}
			else
			{
				//dot Nummer wurde empfangen, schreiben der dot Information in die entsprechend zuvor empfangene Position
				(serial_com_data[33].pointer)[serial_data_pos] = UDR0;
				serial_got_dot_num = 0;
			}
		}
		else
		{
			if(serial_com_data[serial_data_address].length == 1)	//Prüfen ob in ein Array geschrieben werden soll
			{
				//serial_data_address gehört nicht zu einem Array
				*(serial_com_data[serial_data_address].pointer) = UDR0;
			}
			else
			{
				//serial_data_address gehört zu einem Array

				if(serial_data_pos <= serial_com_data[serial_data_address].length)	//array nur beschreiben solange es im gültigen bereich ist(also nicht über die Länge das Arrays hinaus)
				{
					(serial_com_data[serial_data_address].pointer)[serial_data_pos] = UDR0;	//Empfangene Daten in die Array-Positionen schreiben

					serial_data_pos++;	//position im Array incrementieren
				}
				else
				{
					volatile uint8_t dump = UDR0;	// buffer leeren wenn es auserhalb des Array-bereichs ist
				}

			}
		}
	}
}
#endif


/***Für die Logik***/

// enum, for the simplified selection of the data to be sent in the logic controller

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

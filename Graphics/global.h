/**
 *	\file global.h
 *	\brief Stellt globale Daten und Macros bereit, so das Funktionen Source übergreifend darauf zugreifen können
 *
 *	\date 14.12.2012
 * 	\author Christian Brommer & Dennis Schuldt
 */


#ifndef GLOBAL_DATA_H_
#define GLOBAL_DATA_H_

#include "config.h"

/// Das Struct stellt alle Variablen für eine komplette Timer-Variable bereit
typedef volatile struct
{
	/// Benutzerdefinierte Variable. Kann für die jeweilige Anwendung beschrieben werden
	uint8_t value;

	/// Counter-Variable, zählt die Timer Ereignisse
	uint8_t timer_counter;

}timer_data;


timer_data pacman_move; ///< Daten für die PacMan bewegungs Animationen

timer_data pacman_flash;	///< Daten für den blinkenden PacMan-Mund und verschiedene Textelemente

timer_data pacman_destroy;	///< Daten für den sich auflösenden PacMan

timer_data ms_pacman_destroy;	///< Daten für die sich auflösende Ms. PacMan

//Taster Abfrage Macros
#define button_0_pressed			!(PINA &(1<<PINA0))
#define button_0_not_pressed		PINA &(1<<PINA0)
#define button_1_pressed			!(PINA &(1<<PINA1))
#define button_1_not_pressed		PINA &(1<<PINA1)
#define button_2_pressed			!(PINA &(1<<PINA2))
#define button_2_not_pressed		PINA &(1<<PINA2)
#define button_3_pressed			!(PINA &(1<<PINA3))
#define button_3_not_pressed		PINA &(1<<PINA3)
#define button_4_pressed			!(PINA &(1<<PINA4))
#define button_4_not_pressed		PINA &(1<<PINA4)
#define button_5_pressed			!(PINA &(1<<PINA5))
#define button_5_not_pressed		PINA &(1<<PINA5)
#define button_6_pressed			!(PINA &(1<<PINA6))
#define button_6_not_pressed		PINA &(1<<PINA6)
#define button_7_pressed			!(PINA &(1<<PINA7))
#define button_7_not_pressed		PINA &(1<<PINA7)

//LED Macros
#define LED_0_ON					PORTA &= ~(1<<PA0)
#define LED_0_OFF					PORTA |= (1<<PA0)

#define LED_1_ON					PORTA &= ~(1<<PA1)
#define LED_1_OFF					PORTA |= (1<<PA1)




#endif /* GLOBAL_DATA_H_ */

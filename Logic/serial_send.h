/**
 *  \file serial_send.h
 *  \brief Library die alle notwendigen Sendefunktionen zur Grafik bereitstellt
 *
 *  \date: 14.12.2012
 *  \author: Christian Brommer & Dennis Schuldt
 */

#ifndef SERIAL_SEND_H_
#define SERIAL_SEND_H_

/**
 * \brief Funktion zum Initialisieren der Grafik (via seriell)
*/
void send_graphics_init(void);

/**
 * \brief Funktion zum senden der Logikdaten (via seriell)
*/
void send_logic_data(void);

/**
 * \brief Funktion zum senden der Spieldaten pro Logik Durchlauf an Spieler 1 (via seriell)
*/
void send_to_graphics1(void);

/**
 * \brief Funktion zum senden der Spieldaten pro Logik Durchlauf an Spieler 2 (via seriell)
*/
void send_to_graphics2(void);

/**
 * \brief Funktion zum senden der aller Punkte (via seriell)
*/
void send_all_dots(void);

#endif /* SERIAL_SEND_H_ */

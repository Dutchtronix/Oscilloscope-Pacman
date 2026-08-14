/**
 *	\file serial.h
 *	\brief Stellt Funktionen zur seriellen Kommunikation bereit
 *
 *	\date 14.12.2012
 * 	\author Christian Brommer & Dennis Schuldt
 */

#ifndef SERIAL_H_
#define SERIAL_H_


//globale Variablen, der Seriellen Funktionen

volatile uint8_t serial_data_address;	///< Adresse der Variable, in der die empfangenen Daten gespeichert werden sollen

volatile uint8_t serial_data_pos;	///< Position im Array, wenn die zu beschreibende Variable ein Array ist

volatile uint8_t serial_got_dot_num;	///< gibt an ob die Position des dot´s übertragen wurde

typedef volatile struct
{
	uint8_t volatile *pointer;	///< Pointer der auf die Datei zeigt
	uint8_t length;	///< Länge der Datei (8 Byte = Länge 1)
}serial_data;

serial_data serial_com_data[38];	///< Daten Array, mit Informationen zu den entsprechenden Daten für die serielle Kommunikation



//Funktions-Deklarationen

/**
 * \brief zum initialisieren des UART0 Registers, des Grafik-Controllers
 *
*/
void serial_init(void);


/**
 * \brief zum initialisieren beider UART Register (0 und 1), des Logik-Controllers
 *
*/
void serial_init_logic(void);


/**
 * \brief zum initialisieren / zuordnen der seriell zu verschickenden Daten und den seriellen Funktions-Variablen
 *
*/
void serial_data_allocate(void);


/**
 * \brief zum senden einer bestimmten Variablen, aus der Seriellen Kommunikation, an einen wählbaren seriellen Port (0 oder 1)
 *
*/
void serial_send(uint8_t data_address, uint8_t serial_port);


/**
 * \brief zum senden des Status eines bestimmten dot´s
 *
*/
void send_dot_segment(uint8_t dot_num, uint8_t state, uint8_t serial_port);



#endif /* SERIAL_H_ */

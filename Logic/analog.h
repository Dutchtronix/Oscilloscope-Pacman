/**
 *  \file analog.h
 *  \brief Library für das Ansprechen des 8-bit AVR AD-Wandlers
 *
 *  \date: 14.12.2012
 *  \author: Christian Brommer & Dennis Schuldt
 */

#include <stdint.h>

#ifndef __analog_h__
#define __analog_h__

/**
 * \brief Funktion zum Initialisieren der AVR AD Wandler Hardware
*/
void analog_init();

/**
 * \brief Funktion zum Auslesen eines bestimmten AD Kanal
 *
 * \param channel Kanal des AD Wandlers
 *
*/
void ADC_read(uint8_t channel);
void ADC_BlockingRead(uint8_t channel);

#endif

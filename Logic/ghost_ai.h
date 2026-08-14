/**
 *  \file ghost_ai.h
 *  \brief Library für die künstliche Intelligenz der Geister
 *
 *  \date: 14.12.2012
 *  \author: Christian Brommer & Dennis Schuldt
 */

#include <stdint.h>
#include "global.h"
#include "character_logic.h"

#ifndef GHOST_AI_H_
#define GHOST_AI_H_

/**
 * \brief Funktion zum Initialisieren der Geister Daten
*/
void ghost_data_init(void);

/**
 * \brief Funktion zur zufälligen Richtungsvorgabe von Geistern
*/
void randomize_me(volatile struct character_data *data);

/**
 * \brief Funktion zur Richtungsvorgabe von Geistern mit dem Ziel von Pacman/Ms.Pacman
*/
void ghost_target_pacman(volatile struct character_data *ghost, uint8_t mode);	//mode=2 -> target ist pacman, mode=3 -> target ist woanders/14pixel vor pacman oder so

/**
 * \brief Funktion zur Überprüfung, zwischen Pacman/Geistern oder Geistern als Augen in Käfig Position
 * \param data Übergabe des Strukt der Logik Steuervariablen
*/
uint8_t check_ghosts(logic_data *data);

/**
 * \brief Funktion zum zeitbasierten ändern der Geister Logikmodi
*/
void ghost_logic_toggle(void);

#endif /* GHOST_AI_H_ */

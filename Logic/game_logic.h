/**
 *  \file game_logic.h
 *  \brief Library für die Spielelogik
 *
 *  \date: 14.12.2012
 *  \author: Christian Brommer & Dennis Schuldt
 */

#ifndef GAME_LOGIC_H_
#define GAME_LOGIC_H_

/**
 * \brief Funktion zum Initialisieren der Spiellogik Variablen
*/
void logic_init(void);

/**
 * \brief Funktion zum Initialisieren der Spiellogik Variablen beim Level Aufstieg
*/
void logic_init_next_level(void);

/**
 * \brief Funktion zum Abfragen aller Logik relevanten Aktivitäten (Früchte, Levelaufstieg etc.)
*/
void logic_check(void);

/**
 * \brief Funktion zum Levelabhängigen anzeigen der Früchte
*/
void bonus_fruit_display(void);

/**
 * \brief Funktion zum Wechseln ins nächste Level
*/
void next_level(void);

/**
 * \brief Funktion zum zeitbasierten ändern der nicht ausgefüllten (fressbaren) Geister
*/
void pacman_meal_toggle(void);

#endif /* GRID_LOGIC_H_ */

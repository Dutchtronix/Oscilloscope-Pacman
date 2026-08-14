/**
 *	\file timer.h
 *	\brief Definieren der Timer Funktionen
 *
 *  Der eigentliche Timer, wird durch das Interrupt in der \c timer.c bestimmt
 *
 *	\date 14.12.2012
 * 	\author Christian Brommer & Dennis Schuldt
 */


#ifndef TIMER_H_
#define TIMER_H_

/**
 *  \brief Initialisieren des Timers
 *
 *  Entsprechende Register setzen
 *  \attention und Interrupts aktivieren
 *
 */
void timer_init(void);


/**
 *  \brief Initialisieren der globalen Timer-Daten
 *
 * Daten die im Timer-Interrupt und in entsprechend anderen Funktionen verwendet werden
 *
 * \attention Die Variablen müssen als \c volatile deklariert werden, da sie im Interrupt verwendet werden und ansonsten möglicherweise vom Compiler weg optimiert werden
 *
 */
void timer_data_allocate(void);

#endif /* TIMER_H_ */

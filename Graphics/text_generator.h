/**
 *	\file text_generator.h
 *	\brief Stellt die Funktionen für die Text-Darstellung auf dem Oszilloskop bereit
 *
 *	\date 14.12.2012
 * 	\author Christian Brommer & Dennis Schuldt
 */

#ifndef TEXT_GENERATOR_H_
#define TEXT_GENERATOR_H_


/**
 * \brief Stellt ein einzelnes Zeichen dar
 *
 * (*data)[2] ist ein Pointer auf ein zweidimensionales Array. Beim übergeben an die Funktion, ist es auf Grund des Pointers unerheblich wie lang das Array ist.
 * In der Funktion wird dann mit data[][] darauf zu gegriffen.
 *
 *
 * \param data Datensatz des Zeichens, das dargestellt werden soll
 * \param array_size Größe des übergebenen Arrays
 * \attention Array-Elemente starten bei 0
 *
 * \attention Dazu muss im übergebenen Array, im ersten Array-Segment die Zeichenbreite und Zeichenhöhe, und im zweiten Segment die Arraylänge angegeben sein.
*/
void print_character_gbham(uint8_t (*data)[2], uint8_t array_size);


/**
 * \brief Stellt eine Zeichenkette dar
 *
 * gibt eine Zeichenkette, beginnend bei den Startkoordinaten aus
 *
 * \param crt Zeichenkette die Dargestellt werden soll (kann ohne Endkennung angegeben werden)
 * \param x_position, y_position Position der Zeichenkette im 2D Raum
 *
 * \callergraph
*/
void print_gbham(char crt[], uint8_t x_position, uint8_t y_position);
void print_gbham_F(const char *crt, uint8_t x_position, uint8_t y_position);

/**
 * \brief wandelt eine 32 Bit Integer-Variable in einen String um
 *
 * \param value Wert der umgewandelt werden soll
 * \param string Array in das die Zeichen geschrieben werden sollen
 * \param zeros führende Nullen anzeigen oder nicht
 *
 * \attention Die Funktion schließt die Zeichenkette mit "\n" ab
 *
*/
void int32_to_str(uint32_t value, char string[], uint8_t zeros);

#endif /* TEXT_GENERATOR_H_ */

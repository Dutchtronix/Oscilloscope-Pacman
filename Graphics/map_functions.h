/**
 *	\file map_functions.h
 *	\brief Funktionen zum darstellen der Level Grafikdaten
 *
 *	\date 14.12.2012
 * 	\author Christian Brommer & Dennis Schuldt
 */

#ifndef MAP_FUNCTIONS_H_
#define MAP_FUNCTIONS_H_


/**
 * \brief Testfunktion zum ¸berpr¸fen einzelner Elemente des outer_top_frame Arrays
 *
 * \param a Position im Array die dargestellt werden soll
 *
*/
void draw_map_komponent(uint8_t a);


/**
 * \brief darstellen eines kompletten Elements
 *
 * \param data Datensatz, der dargestellt werden soll
 * \param array_size Grˆﬂe des ¸bergebenen Arrays
 * \attention Array-Elemente starten bei 0
*/
void draw_map_elements(const uint8_t (*data)[2], uint8_t array_size);


/**
 * \brief darstellen des kompletten Spielfelds
*/
void draw_map(void);


/**
 * \brief darstellen des PacMan Logos
*/
void draw_pacman_logo(void);


/**
 * \brief darstellen eines Symbols, an einer freiw‰hlbaren relativen Position.
 *
 * \attention
 * Dazu muss im ¸bergebenen Array, in der ersten Zeile der Ausgangspunkt (Origin) des Elements stehen
 *
 * \param data ¸bergebener Datensatz, der dargestellt werden soll
 * \param array_size grˆﬂe des ¸bergebenen Arrays
 * \param x_position, y_position Position des Elements im 2D Raum
*/
void draw_relative_element(const uint8_t (*data)[2], uint8_t array_size, uint8_t x_position, uint8_t y_position);


/**
 * \brief darstellen des PacMan, an einer freiw‰hlbaren relativen Position, mit w‰hlbarer Richtung und wahlweise offenem und zuem Mund
 *
 * \param x_position, y_position Position des PacMan im 2D Raum
 *
 * \param direction bestimmt die Richtung in die der PacMan zeigt
 * \arg 0 => neutrale Position, PacMan schaut nach links und blinkt nicht <b>(unabh‰ngig von der \c mouth Variable)</b>
 * \arg 1 => nach rechts ausgerichtet
 * \arg 2 => nach unten ausgerichtet
 * \arg 3 => nach links ausgerichtet
 * \arg 4 => nach oben ausgerichtet
 *
 * \param mouth bestimmt ob der Mund geˆffnet oder geschlossen dargestellt wird
 * \arg 0 => Mund geschlossen
 * \arg 1 => Mund geˆffnet
*/
void place_pacman(uint8_t x_position, uint8_t y_position, uint8_t direction, uint8_t mouth);


/**
 * \brief darstellen von Ms. PacMan, an einer freiw‰hlbaren relativen Position, mit w‰hlbarer Richtung und wahlweise offenem und zuem Mund
 *
 * \param x_position, y_position Position von Ms. PacMan im 2D Raum
 *
 * \param direction bestimmt die Richtung in Ms. PacMan zeigt
 * \arg 0 => neutrale Position, Ms. PacMan schaut nach links und blinkt nicht <b>(unabh‰ngig von der \c mouth Variable)</b>
 * \arg 1 => nach rechts ausgerichtet
 * \arg 2 => nach unten ausgerichtet
 * \arg 3 => nach links ausgerichtet
 * \arg 4 => nach oben ausgerichtet
 *
 * \param mouth bestimmt ob der Mund geˆffnet oder geschlossen dargestellt wird
 * \arg 0 => Mund geschlossen
 * \arg 1 => Mund geˆffnet
*/
void place_ms_pacman(uint8_t x_position, uint8_t y_position, uint8_t direction, uint8_t mouth);


/**
 * \brief darstellen eines, sich auflˆsenden PacMan
 *
 *\attention
 * intern wird das Timervariablenstruct \c pacman_destroy benutzt
 *
 * \param x_position, y_position Position des PacMan im 2D Raum
 *
 * \return gibt eine 1 zur¸ck wenn der PacMan einmal komplett aufgelˆst wurde
 *
 */
uint8_t destroy_pacman(uint8_t x_position, uint8_t y_position);


/**
 * \brief darstellen einer, sich auflˆsenden Ms. PacMan
 *
 *\attention
 * intern wird das Timervariablenstruct \c ms_pacman_destroy benutzt
 *
 * \param x_position, y_position Position des PacMan im 2D Raum
 *
 * \return gibt eine 1 zur¸ck wenn Ms. PacMan einmal komplett aufgelˆst wurde
 *
 */
uint8_t destroy_ms_pacman(uint8_t x_position, uint8_t y_position);


/**
 * \brief darstellen eines Geistes, an einer freiw‰hlbaren relativen Position, wahlweise ausgef¸llt oder nicht ausgef¸llt
 *
 * \param x_position, y_position Position des Geistes im 2D Raum
 *
 * \param fill ausgef¸llt oder nicht ausgef¸llter Geist
 * \arg 0 => nicht ausgef¸llt
 * \arg 1 => ausgef¸llt
*/
void place_ghost(uint8_t x_position, uint8_t y_position, uint8_t fill);


/**
 * \brief darstellen der Augen eines gefressenen Geistes, an einer freiw‰hlbaren relativen Position
 *
 * \param x_position, y_position Position der Augen im 2D Raum
*/
void place_eyes(uint8_t x_position, uint8_t y_position);


/**
 * \brief darstellen der Fr¸chte Symbole
 *
 * \param x_position, y_position Position der Frucht im 2D Raum
 *
 * \param fruit bestimmt, welche der Fr¸chte dargestellt werden soll
 * \arg 1 => Kirsche
 * \arg 2 => Banane
 * \arg 3 => Birne
 * \arg 4 => Apfel
 * \arg 5 => Erdbeere
 *
*/
void place_fruit(uint8_t x_position, uint8_t y_position, uint8_t fruit);


/**
 * \brief darstellen der zu fressenden Punkte (klein und groﬂ)
 *
 * die Positionen der Punkte werden aus \c dot_coordinates in \c map_data.h gelesen
 *
 * \param dot_number Nummer des darzustellenden Punktes
 *
 * \param choice w‰hlen ob ein groﬂer oder kleiner Punkt dargestellt werden soll
*/
void place_dot(uint8_t dot_number, uint8_t choice);

/**
 * \brief darstellen der zu fressenden kleinen Punkte
 *
 * \param dot_number Nummer des darzustellenden Punktes
*/
void place_small_dot(uint8_t dot_number);


#endif /* MAP_FUNCTIONS_H_ */

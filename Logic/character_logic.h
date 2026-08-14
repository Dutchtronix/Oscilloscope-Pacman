/**
 *  \file character_logic.h
 *  \brief Library für die Bewegung aller Spielfiguren auf dem Spielfeld
 *
 *  \date: 14.12.2012
 *  \author: Christian Brommer & Dennis Schuldt
 */

#include <stdint.h>
#include "global.h"

#ifndef PACMAN_LOGIC_H_
#define PACMAN_LOGIC_H_

/* defines */
//ADC
#define adc_step 0.005
#define adc_joystick_treshold 0.5

/// Array pro Spieler (Joystick), der die berechneten Spannungslimits des Joysticks (Kalibrierung) enthält
/**
 * \[0] => neutral_y
 * \[1] => right
 * \[2] => down
 * \[3] => left
 * \[4] => up
 * \[5} => neutral_x
 */
uint16_t adc_limits_joy1[6];		//indexed by enum direction {neutral,right,down,left,up}
uint16_t adc_limits_joy2[6];		//indexed by enum direction {neutral,right,down,left,up}

typedef volatile struct
{
	///Bestimmt welche Animationen auf dem Bildschirm dargstellt werden
	/**
	 * \arg 0 => Startbildschirm
	 * \arg 1 => Gameplay
	 * \arg 2 => Gameover
	 * \arg 3 => Scores
	 * \arg 4 => Calibration
	 * \arg 5 => "The End"
	 */
	uint8_t game_state;

	///Bestimmt für welchen Spieler, die Dartsellung im Mehrspielermodus angezeigt wird
	/**
	 *
	 * Im laufenden Betrieb kann von 0 zu 1 oder 0 zu 2 gewechselt werden
	 *
	 * \arg 0 => Einspielermodus
	 * \arg 1 => Zweispielermodus - Spieler 1
	 * 				UP1, + Ms. PacMan
	 *
	 * \arg 2 => Zweispielermodus - Spieler 2
	 * 				Up2, + Ms. PacMan
	 */
	uint8_t player_mode;
	uint8_t player1_mode;			 ///< Logik Variable für die verschiedenen Spielmodi für Spieler 1
	uint8_t player2_mode;			 ///< Logik Variable für die verschiedenen Spielmodi für Spieler 2

	uint32_t highscore;		    	 ///< Variable für den Highscore angezeigt wird
	uint8_t new_highscore_state;	 ///< Wenn new_highscore_state gesetzt ist (=1), wird auf dem Score-Screen "New Highscore angezeigt"

	///Bestimmt welcher Text auf dem Score-Screen angezeigt wird
	/**
	 *
	 * \arg 0 => nichts anzeigen
	 * \arg 1 => gewonnen
	 * \arg 2 => verloren
	 */
	uint8_t result;

	uint8_t lives;								///< Anzahl der Leben (maximal 3)
	uint8_t level;								///< Angabe des Levels
	uint8_t dots[244];							///< Dieser Array gibt an welche der Punkte dargestellt bzw. nicht dargestellt werden
	uint8_t dot_counter; 						///< Anzahl der gesammelten Punkte
	uint8_t update_pacman_position_counter;		///< counter Variable für Pacman Positionsupdate
	uint8_t update_pacman_position_flag;    	///< toggle flag, wenn die Zeit für ein Pacman/Ms.Pacman Positionsupdate erreicht ist (=1)
	uint8_t update_pacman_time;					///< update Zeit für Pacman/Ms.Pacman in ms -> Standard = 20ms

	uint8_t ghost_got_pacman; 					///< Variable =1 wenn ein Geist Pacman/Ms.Pacman gefangen hat

	uint16_t ghost_counter;						///< counter für zeitlich abhängige Umschaltung der Geist Logik
	uint16_t ghost_home_time;					///< Variable für die Zeit in der ein Geist Pacman nicht verfolgt -> in ms (Level abhängig)
	uint16_t ghost_chase_time;					///< Variable für die Zeit in der ein Geist Pacman verfolgt -> in ms (Level abhängig)
	uint8_t ghost_logic_mode_toggle;			///< toggle flag für die unterschiedlichen Geister Logik Modi, 1 = target home. 2 = Standard Target

	uint16_t pacman_counter;					///< counter für zeitlich abhängige Umschaltung wie lange Pacman/Ms.Pacman Geister fangen kann
	uint16_t pacman_dinner_time;				///< Variable für die Zeit in der Pacman/Ms.Pacman einen Geist fangen kann -> in ms
	uint8_t pacman_dinner_toggle;				///< toggle flag für die zeitlich abhängige Umschaltung wie lange Pacman/Ms.Pacman Geister fangen kann

	uint8_t hold;								///< bestimmt den Spielstart, immer 1 wenn pacman.direction oder mspacman.direction = neutral

	struct character_data	/// fasst alle Character spezifischen Eigenschaften zusammen
	{
		// Logik Variablen für alle Charakter
		uint8_t x_pixel;						///< aktuelle x-position
		uint8_t y_pixel;						///< aktuelle y-position
		uint8_t x_grid;							///< aktuelle x-Feld Position
		uint8_t y_grid;							///< aktuelle y-Feld Position
		enum direction direction;				///< aktuelle Bewegungsrichtung
		enum direction next_direction;			///< nächste Bewegungsrichtung
		uint8_t pixel_per_step;					///< Angabe für viele Pixel pro Programmdurchlauf gelaufen werden
		uint8_t grid_valid; 					///< Angabe ob das aktuelle Feld gültig ist -> 0 = nicht gültig , 1 = gültig
		uint8_t next_grid_valid; 				///< Angabe ob das nächste Feld gültig ist -> 0 = nicht gültig , 1 = gültig

		/// sorgt in Abhängigkeit vom Wert dafür, das der PacMan aufgelöst wird
		/**
		* \arg 0 => PacMan/Ms.Pacman Joystick Modus
		* \arg 1 => Geist Modus = zufällig
		* \arg 2 => Geist Modus = Ziel Pacman
		* \arg 3 => Geist Modus = Ziel Pacman + 4 Felder vor Pacman
		* \arg 4 => Geist Modus = Augen suchen den Weg zum Käfig
		* \arg 5 => Geist Modus = Geist sucht den Weg in seine eigene Ecke
		*/
		uint8_t logic_mode;

		// Pacman - Ms.Pacman only
		/// sorgt in Abhängigkeit vom Wert dafür, das der PacMan aufgelöst wird
		/**
		* \arg 0 => PacMan wird normal angezeigt
		* \arg 1 => PacMan wird einmal aufgelöst
		* \arg 2 => PacMan wurde aufgelöst und es wird kein PacMan angezeigt solange die Variable den Wert 2 beibehält
		*/
		uint8_t dead;

		uint8_t global_dots;	///< Punkt den Pacman/Ms. Pacman gefressen hat, da sich 2 in einem Schritt ändern können, muss dies beim senden beachtet werden
		uint32_t score;			///< Wert der als Score angezeigt wird
		uint32_t score_old;		///< alter Score Wert zum Vergleichen, ob sich der Score geändert hat und ob er rausgeschickt werden soll

		/// sorgt dafür das der PacMan den Mund zu behält und nicht mehr blinkt, wenn er z.b. vor eine Wand läuft
		/**
		* \arg 0 => PacMan Mund blinkt (nicht gesperrt)
		* \arg 1 => PacMan Mund blinken ist gesperrt, PacMan wird mit zuem Mund dargestellt
		*/
		uint8_t mouth_lock;


		//Ghosts only
		uint8_t ghost_help; 			///< Hilfsvariable, wenn ein Geist vor einer Wand steht (=1)
		uint8_t ghost_reverse_help; 	///< Hilfsvarialbe, damit die Augen und nicht ausgefüllte Geister einmal in die entgegengesetzte Richtung umdrehen dürfen (=0 noch nicht umgekehrt, =1 einmal umgekehrt)

		/// Bestimmt wie der Geist gezeichnet wird
		/**
		* \arg 0 => nicht anzeigen
		* \arg 1 => ausgefüllt
		* \arg 2 => nicht ausgefüllt
		* \arg 3 => Geister-Augen darstellen
		* \arg 4 => Geister blinken ausgefüllt
		* \arg 5 => Geister blinken nicht ausgefüllt
		*/
		uint8_t graphic_state;

		uint8_t cage;					///< Hilfsvariable, wenn ein Geist im Käfig ist (=1)
		uint8_t dotcounter_ghost;		///< "persönlicher" DOT Counter für Geister
		// ghost home coordinates
		uint8_t home_corner_x;			///< x-Position der eigenen Ecke
		uint8_t home_corner_y;			///< y-Position der eigenen Ecke
		uint8_t cage_inside_x_grid;		///< x Feld Position innerhalb des Käfigs
		uint8_t cage_inside_y_grid;		///< y Feld Position innerhalb des Käfigs
		uint8_t cage_outside_x_grid;	///< x Feld Position ausserhalb des Käfigs
		uint8_t cage_outside_y_grid;	///< y Feld Position ausserhalb des Käfig

		// timer for every ghost
		uint8_t update_position_counter; // counter für Character Positions update
		uint8_t update_position_flag;	 // toggle flag, wenn Zeit für Pacman update erreicht ist
		uint8_t update_time;			 // update Zeit für Geister in ms Standard = 20ms so wie pacman
	}
	pacman,		///< bereitgestellter PacMan
	ms_pacman,	///< bereitgestellte Ms. PacMan
	ghost_1,	///< bereitgestellter Geist 1
	ghost_2,	///< bereitgestellter Geist 2
	ghost_3,	///< bereitgestellter Geist 3
	ghost_4;	///< bereitgestellter Geist 4

	struct fruits	/// fasst alle Früchte in einem Struct zusammen
	{
		uint16_t points;		 // Punkte für die Frucht abhängig vom Level
		uint8_t fruit_display;	 // 1 = irgendeine Frucht wird angezeigt
		uint8_t fruit1_done;	 // 1 = 1te Frucht für Bonus wurde angezeigt
		uint8_t fruit2_done;	 // 1 = 1te Frucht für Bonus wurde angezeigt

		volatile uint16_t timer; // Counter für die Länge der Frucht Anzeige
		volatile uint16_t timer_limit; // Counter für die Länge der Frucht Anzeige

		struct fruit_logic_data	/// fasst alle steuerbaren Früchte Eigenschaften zusammen
		{
			uint8_t show_in_map; ///< gibt an ob die Frucht im Spielfeld angezeigt wird, so das sie gesammelt werden kann
		}
		cherry,
		strawberry,
		banana,
		apple,
		pineapple;
	}fruits;	///< bereitgestellte Früchte

}logic_data;

logic_data logic_control; ///< bereigestellter Strukt der Logik Steuervariablen

// Punkte Strings
uint8_t string_pacman_score[9];			///< String über den Seriell die Pacman Punkte verschickt werden
uint8_t string_ms_pacman_score[9];		///< String über den Seriell die Ms.Pacman Punkte verschickt werden
uint8_t string_highscore[9];			///< String über den Seriell der Highscore verschickt wird

/**
 * \brief Funktion zum internen Initialisieren der darzustellenden Punkte
*/
void dot_init(void);

/**
 * \brief Funktion zum Initialisieren der Pacman Spielfigur (zusätzlich Ms.Pacman ausserhalb des Spielfeld platzieren)
*/
void pacman_init(void);

/**
 * \brief Funktion zum Initialisieren der Ms.Pacman Spielfigur
*/
void ms_pacman_init(void);

/**
 * \brief Funktion zum Positionsupdate von Pacman (1 Spieler)
*/
void update_pacman(void);

/**
 * \brief Funktion zum Positionsupdate von Pacman und Ms.Pacman (2 Spieler)
*/
void update_pacman_mspacman(void);

/**
 * \brief Funktion zum Positionsupdate von allen 4 Geistern
*/
void update_ghosts(void);

/**
 * \brief Funktion zum Kalibrieren der Joysticks
*/
void joystick_calibration(void);

/**
 * \brief Funktion zur Änderung der Bewegungsrichtung mit dem Joystick
*/
void get_joystick_direction(void);

/**
 * \brief Funktion zur Änderung und Überprüfung der aktuellen Bewegungsrichtung
 * \param data, mode Übergabe von dem gewünschtem Charakter Strukt und Bewegungsmodus
*/
void update_character_position(volatile struct character_data *data, uint8_t mode);

/**
 * \brief Funktion zur Änderung und Überprüfung der nächsten Bewegungsrichtung
 * \param data, mode Übergabe von dem gewünschtem Charakter Strukt und Bewegungsmodus
*/
void check_next_character_direction(volatile struct character_data *data, uint8_t mode);

/**
 * \brief Funktion zum Überprüfen, ob sich der Charakter in einem gültigen Feld befindet
 * \param x_pos, y_pos Übergabe von der zu überprüfenden x und y Position
*/
uint8_t check_grid_valid(uint8_t x_pos, uint8_t y_pos);

/**
 * \brief Funktion zum Überprüfen, ob ein Punkt gesammelt wurde
 * \param data Übergabe von dem gewünschtem Charakter Strukt (Pacman oder Ms.Pacman)
*/
void check_dots(volatile struct character_data *data);

/**
 * \brief Funktion zum Überprüfen, ob eine Frucht (Bonus Punkte) gesammelt wurde
 * \param fruit, data Übergabe von der Frucht und dem gewünschtem Charakter Strukt
*/
void check_fruits(volatile struct fruits *fruit, volatile struct character_data *data);

#endif /* PACMAN_LOGIC_H_ */

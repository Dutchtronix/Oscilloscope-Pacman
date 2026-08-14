/**
 *	\file graphics.h
 *	\brief Hauptfunktionen zum darstellen der Level Grafik und Text Daten
 *
 * 	Fasst alle essentiellen Grafik-Funktionen in Funktionen für generelle Szenen und Bildschirm-Darstellungen zusammen, so das nur dieser Header für die Grafiken eingebunden werden muss.
 *
 *	\date 14.12.2012
 * 	\author Christian Brommer & Dennis Schuldt
 */

#ifndef GRAPHICS_H_
#define GRAPHICS_H_


/*Global Grafik Data*/


/// Das Struct stellt alle Variablen für die Kommunikation zwischen der Grafik-/ und Logikeinheit bereit
typedef volatile struct
{
	///Bestimmt welche Animationen auf dem Bildschirm dargstellt werden
	/**
	 * \arg 0 => Startbildschirm
	 * \arg 1 => Gameplay
	 * \arg 2 => Gameover
	 * \arg 3 => Scores
	 * \arg 4 => Calibration
	 * \arg 5 => no serial connection
	 * \arg 6 => Level 254 gewonnen
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

	///Bestimmt welcher Text auf dem Score-Screen angezeigt wird
	/**
	 *
	 * \arg 0 => nichts anzeigen
	 * \arg 1 => gewonnen
	 * \arg 2 => verloren
	 *
	 */
	uint8_t result;

	///Wenn new_highscore_state gesetzt ist, wird auf dem Score-Screen "New Highscore angezeigt"
	uint8_t new_highscore_state;

	///Strings für die Scor-Werte

	uint8_t highscore_string[8];	///< String für den Highscore Wert
	uint8_t score_string_1[8];		///< String für den Score Wert des Spielers
	uint8_t score_string_2[8];		///< String für den Score Wert des anderen Spielers

	uint8_t lives;		///< Anzahl der Leben (maximal 3)

	uint8_t level;		///< Angabe des Levels, bestimmt welche Früchte angezeigt werden usw.


	struct pacman_data	/// fasst alle steuerbaren PacMan Eigenschaften zusammen
	{
		uint8_t x_position;	///< X Position im 2D-Raum
		uint8_t y_position;	///< Y-Position im 2D-Raum

		/// bestimmt die Richtung in die der PacMan zeigt
		/**
		 * \arg 0 => neutrale Position, PacMan schaut nach links und blinkt nicht <b>(unabhängig von der \c mouth Variable)</b>
		 * \arg 1 => nach rechts ausgerichtet
		 * \arg 2 => nach unten ausgerichtet
		 * \arg 3 => nach links ausgerichtet
		 * \arg 4 => nach oben ausgerichtet
		 */
		uint8_t direction;


		/// sorgt in Abhängigkeit vom Wert dafür, das der PacMan aufgelöst wird
		/**
		* \arg 0 => PacMan wird normal angezeigt
		* \arg 1 => PacMan wird einmal aufgelöst
		* \arg 2 => PacMan wurde aufgelöst und es wird kein PacMan angezeigt solange die Variable den Wert 2 beibehält
		*/
		uint8_t dead;


		/// sorgt dafür das der PacMan den Mund zu behält und nicht mehr blinkt, wenn er z.b. vor eine Wand läuft
		/**
		* \arg 0 => PacMan Mund blinkt (nicht gesperrt)
		* \arg 1 => PacMan Mund blinken ist gesperrt, PacMan wird mit zuem Mund dargestellt
		*/
		uint8_t mouth_lock;

	}
	pacman,	///< bereitgestellter PacMan
	ms_pacman;	///< bereitgestellte Ms. PacMan


	struct ghost_outline	/// fasst alle Geister in einem Struct zusammen
	{
		struct ghost_data 	/// fasst alle steuerbaren Geister Eigenschaften zusammen
		{
			uint8_t x_position; ///< X Position im 2D-Raum
			uint8_t y_position; ///< y Position im 2D-Raum

			/// Bestimmt wie der Geist gezeichnet wird
			/**
			* \arg 0 => nicht anzeigen
			* \arg 1 => ausgefüllt
			* \arg 2 => nicht ausgefüllt
			* \arg 3 => Geister-Augen darstellen
			* \arg 4 => ausgefüllte Geister im Käfig hüpfen lassen
			* \arg 5 => nicht ausgefüllte Geister im Käfig hüpfen lassen
			*/
			uint8_t state;

		}ghost_1,ghost_2,ghost_3,ghost_4;

	}ghosts;	///< bereitgestellte Geister



	struct fruit_outline	/// fasst alle Früchte in einem Struct zusammen
	{
		struct fruit_data	/// fasst alle steuerbaren Früchte Eigenschaften zusammen
		{
			uint8_t show_in_map; ///< gibt an ob die Frucht im Spielfeld angezeigt wird, so das sie gesammelt werden kann

		}cherry,strawberry,banana,apple,pineapple;

	}fruits;	///< bereitgestellte Früchte

	uint8_t dots[244];	///< Dieses Array gibt an welche der Punkte dargestellt bzw. nicht dargestellt werden
	uint8_t bigdots[244];

}graphic_data;

graphic_data graphics_control; ///< bereigestellte Steuervariablen




/**
 * \brief initialisiert die Daten für die Grafikschnittstelle
 *
 * belegt das \c grafics_control Struct mit default Werten
 */
void graphics_control_allocate(void);


/**
 * \brief zum darstellen des Startbildschirms
 *
 * Wie dem PacMan Logo, Programmierer Daten und einer kleinen Animation aus PacMan und Geistern
 *
 * \callgraph
 *
*/
void draw_startup_screen(void);

/**
 * \brief zum darstellen aller Map-Daten, dem Level, PacMan und allen anderen Symbolen
 *
 * So lange der Pacman in neutral Direction ist, wird "Ready!" angezeigt und UP1 blinkt nicht,
 * ist der PacMan nicht mehr in neutral, ist der Text weg und UP1 fängt an zu blinken
 *
 * Bei Gameover: alles außer PacMan und Geistern anzeigen. Punkte so wie sie waren und Gamover anzeigen
 * \callgraph
*/
void draw_gameplay(void);


/**
 * \brief zum darstellen der Score-Liste
 *
*/
void draw_scores(void);

/**
 * \brief zum darstellen der Kalibrierung
 *
*/
void draw_calibration(void);



#endif /* GRAPHICS_H_ */

/**
 *  \file character_logic.c
 *
 *  \date: 14.12.2012
 *  \author: Christian Brommer & Dennis Schuldt
 */

#include <avr/io.h>

#include "global.h"
#include "analog.h"
#include "character_logic.h"
#include "grid_data.h"
#include "ghost_ai.h"

void pacman_init(void)
{
	// initialize PacMan Variables
	logic_control.pacman.grid_valid=1;
	logic_control.pacman.next_grid_valid=0;
	logic_control.pacman.direction=neutral;
	logic_control.pacman.next_direction=neutral;
	logic_control.pacman.pixel_per_step=1;
	logic_control.pacman.logic_mode=joystick;

	logic_control.pacman.x_grid=18;
	logic_control.pacman.x_pixel=(logic_control.pacman.x_grid*grid_dim);
	logic_control.pacman.y_grid=9;
	logic_control.pacman.y_pixel=(logic_control.pacman.y_grid*grid_dim)+grid_center_offset;

#if TWOPLAYERMODE
	// initialize Ms.PacMan Variables
	logic_control.ms_pacman.grid_valid=1;
	logic_control.ms_pacman.next_grid_valid=0;
	logic_control.ms_pacman.direction=neutral;
	logic_control.ms_pacman.next_direction=neutral;
	logic_control.ms_pacman.pixel_per_step=1;
	logic_control.ms_pacman.logic_mode=joystick;

	logic_control.ms_pacman.x_grid=0;
	logic_control.ms_pacman.x_pixel=(logic_control.ms_pacman.x_grid*grid_dim);
	logic_control.ms_pacman.y_grid=0;
	logic_control.ms_pacman.y_pixel=(logic_control.ms_pacman.y_grid*grid_dim)+grid_center_offset;
#endif
}

#if TWOPLAYERMODE
void ms_pacman_init(void)
{
	// Ms.PacMan Variablen initialize
	logic_control.ms_pacman.grid_valid=1;
	logic_control.ms_pacman.next_grid_valid=0;
	logic_control.ms_pacman.direction=neutral;
	logic_control.ms_pacman.next_direction=neutral;
	logic_control.ms_pacman.pixel_per_step=1;
	logic_control.ms_pacman.logic_mode=joystick;

	logic_control.ms_pacman.x_grid=18;													// start coordinates
	logic_control.ms_pacman.x_pixel=(logic_control.ms_pacman.x_grid*grid_dim);
	logic_control.ms_pacman.y_grid=9;
	logic_control.ms_pacman.y_pixel=(logic_control.ms_pacman.y_grid*grid_dim)+grid_center_offset;
}
#endif

void dot_init(void)
{
	for (uint8_t i=0 ; i<=243 ; i++) {			// Punkt Status (aller Punkte) auf 1 setzen = Dots vorhanden
#if DOTSINFLASH		
		dots_pixel_present[i]=1;
#endif
	}
}

void update_pacman()
{
	if (logic_control.update_pacman_position_flag) {			// Wenn entsprechende Zeit abgelaufen ist, Pacman Position aktualisieren
		update_character_position(&logic_control.pacman, logic_control.pacman.logic_mode);
		logic_control.ghost_got_pacman=check_ghosts(&logic_control);	// Geister/Pacman/Ms.Pacman Positionen vergleichen

		logic_control.update_pacman_position_counter=0;
		logic_control.update_pacman_position_flag=0;
	}
}

#if TWOPLAYERMODE
void update_pacman_mspacman()
{
	if (logic_control.update_pacman_position_flag) {		// Wenn entsprechende Zeit abgelaufen ist, Pacman&Ms.Pacman Position aktualisieren
		update_character_position(&logic_control.pacman, logic_control.pacman.logic_mode);
		update_character_position(&logic_control.ms_pacman, logic_control.ms_pacman.logic_mode);
		logic_control.ghost_got_pacman=check_ghosts(&logic_control);	// Geister/Pacman/Ms.Pacman Positionen vergleichen

		logic_control.update_pacman_position_counter=0;
		logic_control.update_pacman_position_flag=0;
	}
}
#endif

void update_ghosts()
{
	// Geister erst starten, wenn Pacman oder Ms. Pacman sich das erste mal bewegt hat und die entsprechende Zeit abgelaufen ist
	if (logic_control.ghost_1.update_position_flag && logic_control.hold == 0) {
		update_character_position(&logic_control.ghost_1, logic_control.ghost_1.logic_mode);

		logic_control.ghost_1.update_position_counter=0;
		logic_control.ghost_1.update_position_flag=0;
	}

	if (logic_control.ghost_2.update_position_flag && logic_control.hold == 0) {
		update_character_position(&logic_control.ghost_2, logic_control.ghost_2.logic_mode);

		logic_control.ghost_2.update_position_counter=0;
		logic_control.ghost_2.update_position_flag=0;
	}

	if (logic_control.ghost_3.update_position_flag && logic_control.hold == 0) {
		update_character_position(&logic_control.ghost_3, logic_control.ghost_3.logic_mode);

		logic_control.ghost_3.update_position_counter=0;
		logic_control.ghost_3.update_position_flag=0;
	}

	if (logic_control.ghost_4.update_position_flag && logic_control.hold == 0) {
		update_character_position(&logic_control.ghost_4, logic_control.ghost_4.logic_mode);

		logic_control.ghost_4.update_position_counter=0;
		logic_control.ghost_4.update_position_flag=0;
	}

	// Wenn noch kein Ghost Pacman/Ms.Pacman gefressen hat, dann nochmal checken um jede Positionsänderung abzufragen
	if (!logic_control.ghost_got_pacman) logic_control.ghost_got_pacman=check_ghosts(&logic_control);
}
//
// Current code observations:
//	ADC_read(ch) just starts a conversion for channel ADC_ch. The conversion will not be finished upon return.
//	The ISR will update the conversion result into ADC_result[ADC_ch] and update ADC_ch.
//
void joystick_calibration(void)
{
	ADC_max=4; 							// read all 4 Joystick inputs but that is not what's happening
//	ADC_read(ADC_ch);						// start conversions for channel ADC_ch only
	ADC_BlockingRead(ADC_ch);
	// May need to wait here until all 4 channels have been read TODO
	adc_limits_joy1[0]=ADC_result[0];	// Store the result in Limit Neutral_y
	adc_limits_joy1[5]=ADC_result[1];	// Store the result in Limit Neutral_x
	adc_limits_joy2[0]=ADC_result[2];
	adc_limits_joy2[5]=ADC_result[3];

	// Calculation of Joystick 1 Limits (Direction of movement)
	adc_limits_joy1[right] = ((adc_limits_joy1[5]*adc_step)+adc_joystick_treshold)/adc_step;
	adc_limits_joy1[left] = ((adc_limits_joy1[5]*adc_step)-adc_joystick_treshold)/adc_step;
	adc_limits_joy1[down] = ((adc_limits_joy1[0]*adc_step)+adc_joystick_treshold)/adc_step;
	adc_limits_joy1[up] = ((adc_limits_joy1[0]*adc_step)-adc_joystick_treshold)/adc_step;

	// Calculation of Joystick 2 Limits	(Direction of movement)
	adc_limits_joy2[right] = ((adc_limits_joy2[5]*adc_step)+adc_joystick_treshold)/adc_step;
	adc_limits_joy2[left] = ((adc_limits_joy2[5]*adc_step)-adc_joystick_treshold)/adc_step;
	adc_limits_joy2[down] = ((adc_limits_joy2[0]*adc_step)+adc_joystick_treshold)/adc_step;
	adc_limits_joy2[up] = ((adc_limits_joy2[0]*adc_step)-adc_joystick_treshold)/adc_step;
}

void get_joystick_direction(void)
{
	uint16_t x_joy_1=0, y_joy_1=0, x_joy_2=0, y_joy_2=0;

	ADC_read(ADC_ch);												// Start reading value for channel ADC_ch

	if (logic_control.player_mode==0) {								// Direction query for single player
		y_joy_1=ADC_result[0];
		x_joy_1=ADC_result[1];

		if (x_joy_1>adc_limits_joy1[right])
			logic_control.pacman.next_direction=right;			// x right
		else if (x_joy_1<adc_limits_joy1[left])
			logic_control.pacman.next_direction=left; 		// x left

		if (y_joy_1<adc_limits_joy1[up]) logic_control.pacman.next_direction=up; 				// y up
		else if (y_joy_1>adc_limits_joy1[down]) logic_control.pacman.next_direction=down; 		// y down
	} else {
		// ASSERT(logic_control.player_mode!=0);
//		if (logic_control.player_mode!=0) {								// Direction query for 2 players
		{
		y_joy_1=ADC_result[0];
		x_joy_1=ADC_result[1];
		y_joy_2=ADC_result[2];
		x_joy_2=ADC_result[3];

		if (x_joy_1>adc_limits_joy1[right]) logic_control.pacman.next_direction=right;			// x right
		else if (x_joy_1<adc_limits_joy1[left]) logic_control.pacman.next_direction=left; 		// x left

		if (y_joy_1<adc_limits_joy1[up]) logic_control.pacman.next_direction=up; 				// y up
		else if (y_joy_1>adc_limits_joy1[down]) logic_control.pacman.next_direction=down; 		// y down

		if (x_joy_2>adc_limits_joy2[right]) logic_control.ms_pacman.next_direction=right;		// x right
		else if (x_joy_2<adc_limits_joy2[left]) logic_control.ms_pacman.next_direction=left; 	// x left

		if (y_joy_2<adc_limits_joy2[up]) logic_control.ms_pacman.next_direction=up; 			// y up
		else if (y_joy_2>adc_limits_joy2[down]) logic_control.ms_pacman.next_direction=down; 	// y down
		}
	}
}

void update_character_position(struct character_data *data, uint8_t mode)
{
	check_next_character_direction(data, mode);

	if (!data->grid_valid && mode==0)  	// Abfrage für PacMan steht vor Wand
	{
		data->mouth_lock = 1;			// PacMan Mund bleibt offen
	} else data->mouth_lock = 0;

	if ((!data->grid_valid || data->next_direction==neutral) && mode==1)  // Abfrage Ghost steht vor Wand
	{
		data->ghost_help=1;												  // Help Variable für Richtungsänderung wird gesetzt
	}

	switch(data->direction)
	{
		case up:		data->grid_valid=check_grid_valid(data->x_pixel, data->y_pixel+4);
						if (data->grid_valid) { // Abfrage ob nächstes Feld in gleicher Richtung gültig ist, wenn ja Charakter bewegen
							data->y_pixel+=data->pixel_per_step;	// Charakter Position entsprechend der Richtung ändern
						}
						break;

		case down:    	data->grid_valid=check_grid_valid(data->x_pixel, data->y_pixel-4);
						if (data->grid_valid) {
							data->y_pixel-=data->pixel_per_step;
						}
						break;

		case right:  	data->grid_valid=check_grid_valid(data->x_pixel+4, data->y_pixel);
						if (data->grid_valid) {
							data->x_pixel+=data->pixel_per_step;
							if(data->x_pixel==220 && data->y_pixel==129) data->x_pixel=31;	// Erkennung von horizontalem Gang, wenn damit er auf der anderen Seite wieder rauskommt
						}
						break;

		case left: 	  	data->grid_valid=check_grid_valid(data->x_pixel-4, data->y_pixel);
						if (data->grid_valid) {
							data->x_pixel-=data->pixel_per_step;
							if(data->x_pixel==31 && data->y_pixel==129) data->x_pixel=220; // Erkennung von horizontalem Gang, wenn damit er auf der anderen Seite wieder rauskommt
						}
						break;

		case neutral: 	break; // Wert für Start und wenn man vor einer Wand steht
	}

	// update Charakter Feld Positionen
	data->y_grid=data->y_pixel/grid_dim;
	data->x_grid=data->x_pixel/grid_dim;
}

void check_next_character_direction(struct character_data *data, uint8_t mode)
{
	// Determine the input for the direction depending on the provided mode variable.
	if (mode==joystick) get_joystick_direction();
	if (mode==random_direction) randomize_me(data);
	if (mode>1) ghost_target_pacman(data, mode); // when one of the ghost modes is called with target

	// Switch command to check if the next direction is also valid.
	switch(data->next_direction)
	{
		case up:	data->next_grid_valid = check_grid_valid(data->x_pixel, data->y_pixel+grid_dim); // Review of the next field
					// If the next field is valid and centered for the new direction, then adopt the direction.
					if (data->next_grid_valid && (data->x_pixel-(data->x_grid*grid_dim)==grid_center_offset))  
						data->direction=data->next_direction;
					break;

		case down:	data->next_grid_valid = check_grid_valid(data->x_pixel, data->y_pixel-grid_dim);
					if (data->next_grid_valid && (data->x_pixel-(data->x_grid*grid_dim)==grid_center_offset))
						data->direction=data->next_direction;

					break;

		case right: data->next_grid_valid = check_grid_valid(data->x_pixel+grid_dim, data->y_pixel);
					if (data->next_grid_valid && (data->y_pixel-(data->y_grid*grid_dim)==grid_center_offset))
						data->direction=data->next_direction;
					break;

		case left:  data->next_grid_valid = check_grid_valid(data->x_pixel-grid_dim, data->y_pixel);
					if (data->next_grid_valid && (data->y_pixel-(data->y_grid*grid_dim)==grid_center_offset))
						data->direction=data->next_direction;
					break;

		case neutral: break;
	}
}

uint8_t check_grid_valid(uint8_t x_pos, uint8_t y_pos)
{
	uint8_t grid_valid=0;

	// Aus der übergebenen Position das aktuelle Feld berechnen
	uint8_t y_grid=y_pos/grid_dim;
	uint8_t x_grid=x_pos/grid_dim;

	// Überprüfung des berechneten Kästchen auf Gültigkeit (unterteilt in 5 Bereiche)
	if ((y_grid>=22) && (x_grid<=17)) {		// top_left
			for (uint8_t box=0; box<=54; box++) {
				if ((x_grid==pgm_read_byte(&valid_grid_top_left[box][0])) && (y_grid==pgm_read_byte(&valid_grid_top_left[box][1]))) {
					grid_valid=1;   // grid gültig
				}
			}
		}

	if ((y_grid>=22) && (x_grid>=18)) { 		// top_right
			for (uint8_t box=0; box<=54; box++) {
				if ((x_grid==pgm_read_byte(&valid_grid_top_right[box][0])) && (y_grid==pgm_read_byte(&valid_grid_top_right[box][1]))) {
					grid_valid=1;   // grid gültig
				}
			}
		}

	if ((y_grid>=13) && (y_grid<=21)) {  		// middle
			for (uint8_t box=0; box<=68; box++) {
				if ((x_grid==pgm_read_byte(&valid_grid_middle[box][0])) && (y_grid==pgm_read_byte(&valid_grid_middle[box][1]))) {
					grid_valid=1;   // grid gültig
				}
			}
		}

	if ((y_grid<=12) && (x_grid<=17)) {  		// bottom_left
			for (uint8_t box=0; box<=62; box++) {
				if ((x_grid==pgm_read_byte(&valid_grid_bottom_left[box][0])) && (y_grid==pgm_read_byte(&valid_grid_bottom_left[box][1]))) {
					grid_valid=1;   // grid gültig
				}
			}
		}

	if ((y_grid<=12) && (x_grid>=18)) { 		// bottom_right
			for (uint8_t box=0; box<=62; box++) {
				if ((x_grid==pgm_read_byte(&valid_grid_bottom_right[box][0])) && (y_grid==pgm_read_byte(&valid_grid_bottom_right[box][1]))) {
					grid_valid=1;   // grid gültig
				}
			}
		}

	return grid_valid;				// Rückgabe ob das geforderte Feld gültig ist oder nicht
}

void check_dots(struct character_data *data)
{
	for (uint8_t dots=0; dots<=243; dots++) {		// Schleife zur Abfrage alle 244 Punkte
		// Wenn x und y position in Tabelle hinterlegt und der Punkt noch vorhanden
		// If x and y positions are stored in a table and the point is still present
#if DOTSINFLASH
		if ((data->x_pixel==pgm_read_byte(&dots_pixel_status[dots][0])) && (data->y_pixel==pgm_read_byte(&dots_pixel_status[dots][1])) && (dots_pixel_present[dots])) {
			dots_pixel_present[dots]=0;								// Punkt auf 0 (nicht vorhanden) setzten. Set point to 0 (not present)
			logic_control.dots[dots]=dots_pixel_present[dots];		// umschreiben auf logic_control Strukt, da Koordinaten in .h definiert sind und somit nirgendwo anders inkludiert werden kann
#else
		if ((data->x_pixel==dots_pixel_status[dots][0]) && (data->y_pixel==dots_pixel_status[dots][1]) && (dots_pixel_status[dots][2])) {
			dots_pixel_status[dots][2]=0;								// Punkt auf 0 (nicht vorhanden) setzten. Set point to 0 (not present)
			logic_control.dots[dots]=dots_pixel_status[dots][2];		// umschreiben auf logic_control Strukt, da Koordinaten in .h definiert sind und somit nirgendwo anders inkludiert werden kann
#endif
			data->global_dots=dots;										// globale Punkt Nummer (zum verschicken) schreiben
			data->score+=10;											// Score um 10 Punkte hochzählen

			logic_control.dot_counter++;			  //gegessene Punkte hochzählen
			// Punkte Counter hochzählen für jeden Charakter, nur für Geister sinnvoll (Abfrage wann sie den Käfig wieder verlassen dürfen
			logic_control.ghost_1.dotcounter_ghost++;
			logic_control.ghost_2.dotcounter_ghost++;
			logic_control.ghost_3.dotcounter_ghost++;
			logic_control.ghost_4.dotcounter_ghost++;

			if (dots==33 || dots==103 || dots==174 || dots==228) { // Abfrage auf große Punkte (bottom_left, bottom_right, top_left, top_right)
				data->score+=40;						// Score um weitere 40 Punkte hochzählen (10+40=50 Punkte für großen Punkt)
				logic_control.pacman_counter=0;			// counter für Geister fressen auf 0 setzen

				// Geister nicht ausgefüllt darstellen = Pacman kann sie fressen aber nur, wenn es keine Augen sind (falls man einen zweiten großen Punkt ist, bevor die Zeit abgelaufen ist)
				if(logic_control.ghost_1.graphic_state!=eyes) {
						logic_control.ghost_1.logic_mode=home;				// Geister Logik auf Geister nach Hause schicken
						logic_control.ghost_1.ghost_reverse_help=1;			// Variable setzen, das Geister einmal "umdrehen" können
						logic_control.ghost_1.update_time=40;				// Geister langsamer machen
						logic_control.ghost_1.update_position_counter=0;	// update Position Counter zurücksetzen
						if (logic_control.ghost_1.cage) logic_control.ghost_1.graphic_state=not_filled_flash;	// Wenn im Käfig, dann nicht ausgefüllt blinken, ansonsten ausgefüllt blinken
						else logic_control.ghost_1.graphic_state=not_filled;
					}
				if(logic_control.ghost_2.graphic_state!=eyes) {
						logic_control.ghost_2.logic_mode=home;
						logic_control.ghost_2.ghost_reverse_help=1;
						logic_control.ghost_2.update_time=40;
						logic_control.ghost_2.update_position_counter=0;
						if (logic_control.ghost_2.cage) logic_control.ghost_2.graphic_state=not_filled_flash;
						else logic_control.ghost_2.graphic_state=not_filled;
					}
				if(logic_control.ghost_3.graphic_state!=eyes) {
						logic_control.ghost_3.logic_mode=home;
						logic_control.ghost_3.ghost_reverse_help=1;
						logic_control.ghost_3.update_time=40;
						logic_control.ghost_3.update_position_counter=0;
						if (logic_control.ghost_3.cage) logic_control.ghost_3.graphic_state=not_filled_flash;
						else logic_control.ghost_3.graphic_state=not_filled;
					}
				if(logic_control.ghost_4.graphic_state!=eyes) {
						logic_control.ghost_4.logic_mode=home;
						logic_control.ghost_4.ghost_reverse_help=1;
						logic_control.ghost_4.update_time=40;
						logic_control.ghost_4.update_position_counter=0;
						if (logic_control.ghost_4.cage) logic_control.ghost_4.graphic_state=not_filled_flash;
						else logic_control.ghost_4.graphic_state=not_filled;
					}
			}
		}
	}
}

void check_fruits(struct fruits *fruit, struct character_data *data)
{
	// eine der 5 Früchte vorhanden und Pacman/Ms.Pacman (bzw. übergebenen Charakter auf deren Position
	if (logic_control.fruits.fruit_display &&
		data->x_pixel==pgm_read_byte(&fruits_pixel_postion[0][0]) && data->y_pixel==pgm_read_byte(&fruits_pixel_postion[0][1])) {
		data->score+=fruit->points;				// Score hochzählen, Punkte abhängig von der Frucht

		// alle Früchte nicht mehr anzeigen
		fruit->apple.show_in_map=0;
		fruit->strawberry.show_in_map=0;
		fruit->pineapple.show_in_map=0;
		fruit->cherry.show_in_map=0;
		fruit->banana.show_in_map=0;

		// Hilfsvariable für Fruchtanzeige zurücksetzen
		logic_control.fruits.fruit_display=0;
	}
}



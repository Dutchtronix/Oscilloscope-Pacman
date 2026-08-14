/**
 *	\file graphics.c
 *
 *	\date 14.12.2012
 * 	\author Christian Brommer & Dennis Schuldt
 *
 * Update by JdR (Dutchtronix) to run the graphics processor
 * on the Dutchtronix AVR Oscilloscope Clock. This clock
 * has a D to A converter needed for rendering the graphics
 * on an analog oscilloscope.
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <inttypes.h>

#include "global.h"
#include "graphics.h"
#include "map_functions.h"
#include "text_generator.h"
#include "draw_engine.h"

void graphics_control_allocate(void)
{
	//generals
	graphics_control.game_state = 5;				//no serial connection
//	graphics_control.game_state = 0;				//test

	graphics_control.player_mode = 0;				//Single Player mode

	graphics_control.result = 0;					//show nothing

	graphics_control.new_highscore_state = 0;		//show nothing

	//Score-Strings initialisieren (als dafault Wert wird 0 angezeigt)
	graphics_control.highscore_string[0] = '0';		//wird vor dem Start des Spiels, seriel übertragen und aktualisiert
	graphics_control.highscore_string[1] = '\n';

	graphics_control.score_string_1[0] = '0';		//ist beim starten des Spiels = 0 und wird wärend des Spiels aktualisiert
	graphics_control.score_string_1[1] = '\n';

	graphics_control.score_string_2[0] = '0';		//ist beim starten des Spiels = 0 und wird wärend des Spiels aktualisiert
	graphics_control.score_string_2[1] = '\n';

	graphics_control.lives = 3;

	graphics_control.level = 1;

	//PacMan
	graphics_control.pacman.dead = 0;
	graphics_control.pacman.direction = 0;			//neutrale Position
	graphics_control.pacman.mouth_lock = 0;
	graphics_control.pacman.x_position = 126;
	graphics_control.pacman.y_position = 66;

	//Ms. PacMan
	graphics_control.ms_pacman.dead = 0;
	graphics_control.ms_pacman.direction = 0;		//neutrale Position
	graphics_control.ms_pacman.mouth_lock = 0;
	graphics_control.ms_pacman.x_position = 140;
	graphics_control.ms_pacman.y_position = 66;

	//Ghosts
	graphics_control.ghosts.ghost_1.state = 4;
	graphics_control.ghosts.ghost_1.x_position = 0;
	graphics_control.ghosts.ghost_1.y_position = 0;

	graphics_control.ghosts.ghost_2.state = 4;
	graphics_control.ghosts.ghost_2.x_position = 0;
	graphics_control.ghosts.ghost_2.y_position = 0;

	graphics_control.ghosts.ghost_3.state = 4;
	graphics_control.ghosts.ghost_3.x_position = 0;
	graphics_control.ghosts.ghost_3.y_position = 0;

	graphics_control.ghosts.ghost_4.state = 4;
	graphics_control.ghosts.ghost_4.x_position = 0;
	graphics_control.ghosts.ghost_4.y_position = 0;

	//fruits in entsprechender Reihenfolge

	//cherry
	graphics_control.fruits.cherry.show_in_map = 0;

	//strawberry
	graphics_control.fruits.strawberry.show_in_map = 0;

	//banana
	graphics_control.fruits.banana.show_in_map = 0;

	//apple
	graphics_control.fruits.apple.show_in_map = 0;

	//pineapple
	graphics_control.fruits.pineapple.show_in_map = 0;

	//initialisieren der dots
	for (int i=0; i<= 243; i++)	{
		graphics_control.dots[i]=1;
		graphics_control.bigdots[i] = 0;
	}
	graphics_control.bigdots[33] = graphics_control.bigdots[103] = graphics_control.bigdots[174] = graphics_control.bigdots[228] = 1;
}

extern uint8_t dot_coordinates[244][2];
extern uint8_t relative_small_dot[6][2];

void draw_gameplay(void)
{
	//map:
	draw_map();

	if(!(graphics_control.game_state==2)) {		//nachfolgende Elemente werden beim Gameover nicht angezeigt
	//Pac-Man
		switch (graphics_control.player_mode) {
			case 2:	//	Zweispielermodus - Spieler 2	PacMan und Ms. PacMan darstellen
				//no break
			case 1:	//	Zweispielermodus - Spieler 1	PacMan und Ms. PacMan darstellen
				if(!graphics_control.ms_pacman.dead) {
					//Ms. PacMan ganz normal zeichnen
					place_ms_pacman(graphics_control.ms_pacman.x_position, graphics_control.ms_pacman.y_position, graphics_control.ms_pacman.direction, pacman_flash.value||graphics_control.ms_pacman.mouth_lock);	//der Ms. PacMan-Mund blinkt nur wenn die "ODER Bedingung", nicht durch mouth_lock fest auf eins geschrieben wird
				}
				else {	// ist Ms. PacMan dead = 1, dann wird direction ignorieren und Ms. PacMan an der Stelle aufgelösen und nicht weiter anzeigen, es seiden dead wird wieder = 0
					if(graphics_control.ms_pacman.dead == 1) {
						if(destroy_ms_pacman(graphics_control.ms_pacman.x_position,graphics_control.ms_pacman.y_position)) //führt die destroy Funktion aus, die Funktion gibt eine 1 zurück wenn Ms. PacMan komplett aufgelöst wurde
							graphics_control.ms_pacman.dead = 2;	//so lange die dead Variable = 2 ist wird Ms. PacMan nicht gezeichnet
					}
				}
				//no break

			case 0:	//	Einspieler
				if(!graphics_control.pacman.dead) {
					//PacMan ganz normal zeichnen
					place_pacman(graphics_control.pacman.x_position, graphics_control.pacman.y_position, graphics_control.pacman.direction, pacman_flash.value||graphics_control.pacman.mouth_lock);	//der PacMan-Mund blinkt nur wenn die "ODER Bedingung", nicht durch mouth_lock fest auf eins geschrieben wird
				}
				else {	// ist PacMan dead = 1, dann wird direction ignorieren, der PacMan an der Stelle aufgelösen und nicht weiter anzeigen, es seiden dead wird wieder = 0
					if(graphics_control.pacman.dead == 1) {
						if(destroy_pacman(graphics_control.pacman.x_position,graphics_control.pacman.y_position)) //führt die destroy Funktion aus, die Funktion gibt eine 1 zurück wenn der PacMan komplett aufgelöst wurde
							graphics_control.pacman.dead = 2;	//so lange die dead Variable = 2 ist wird kein PacMan gezeichnet
					}
				}
				break;

			default:	//nichts darstellen
				break;
		}

	//ghosts:
		//ghost_1
		switch (graphics_control.ghosts.ghost_1.state)
		{
			case 1:	//ausgefüllt
				place_ghost(graphics_control.ghosts.ghost_1.x_position, graphics_control.ghosts.ghost_1.y_position, 1);
				break;
			case 2: //nicht ausgefüllt
				place_ghost(graphics_control.ghosts.ghost_1.x_position, graphics_control.ghosts.ghost_1.y_position, 0);
				break;
			case 3:	//Augen darstellen
				place_eyes(graphics_control.ghosts.ghost_1.x_position, graphics_control.ghosts.ghost_1.y_position);
				break;
			default:	//case 0: nicht anzeigen
				break;
		}
		//ghost_2
		switch (graphics_control.ghosts.ghost_2.state)
		{
			case 1:	//ausgefüllt
				place_ghost(graphics_control.ghosts.ghost_2.x_position, graphics_control.ghosts.ghost_2.y_position, 1);
				break;
			case 2: //nicht ausgefüllt
				place_ghost(graphics_control.ghosts.ghost_2.x_position, graphics_control.ghosts.ghost_2.y_position, 0);
				break;
			case 3:	//Augen darstellen
				place_eyes(graphics_control.ghosts.ghost_2.x_position, graphics_control.ghosts.ghost_2.y_position);
				break;
			case 4: //ausgefüllte Geister im Käfig hüpfen lassen
				graphics_control.ghosts.ghost_2.x_position = 126;
				graphics_control.ghosts.ghost_2.y_position = pacman_flash.value ? 130 : 127;
				place_ghost(graphics_control.ghosts.ghost_2.x_position, graphics_control.ghosts.ghost_2.y_position, 1);
				break;
			case 5: //nicht ausgefüllte Geister im Käfig hüpfen lassen
				graphics_control.ghosts.ghost_2.x_position = 126;
				graphics_control.ghosts.ghost_2.y_position = pacman_flash.value ? 130 : 127;
				place_ghost(graphics_control.ghosts.ghost_2.x_position, graphics_control.ghosts.ghost_2.y_position, 0);
				break;
			default:	//case 0: nicht anzeigen
				break;
		}
		//ghost_3
		switch (graphics_control.ghosts.ghost_3.state)
		{
			case 1:	//ausgefüllt
				place_ghost(graphics_control.ghosts.ghost_3.x_position, graphics_control.ghosts.ghost_3.y_position, 1);
				break;
			case 2: //nicht ausgefüllt
				place_ghost(graphics_control.ghosts.ghost_3.x_position, graphics_control.ghosts.ghost_3.y_position, 0);
				break;
			case 3:	//Augen darstellen
				place_eyes(graphics_control.ghosts.ghost_3.x_position, graphics_control.ghosts.ghost_3.y_position);
				break;
			case 4:	//ausgefüllte Geister im Käfig hüpfen lassen
				graphics_control.ghosts.ghost_3.x_position = 112;
				graphics_control.ghosts.ghost_3.y_position = pacman_flash.value ? 127 : 130;
				place_ghost(graphics_control.ghosts.ghost_3.x_position, graphics_control.ghosts.ghost_3.y_position, 1);
				break;
			case 5:	//nicht ausgefüllte Geister im Käfig hüpfen lassen
				graphics_control.ghosts.ghost_3.x_position = 112;
				graphics_control.ghosts.ghost_3.y_position = pacman_flash.value ? 127 : 130;
				place_ghost(graphics_control.ghosts.ghost_3.x_position, graphics_control.ghosts.ghost_3.y_position, 0);
				break;
			default:	//case 0: nicht anzeigen
				break;
		}

		//ghost_4
		switch (graphics_control.ghosts.ghost_4.state)
		{
			case 1:	//ausgefüllt
				place_ghost(graphics_control.ghosts.ghost_4.x_position, graphics_control.ghosts.ghost_4.y_position, 1);
				break;
			case 2: //nicht ausgefüllt
				place_ghost(graphics_control.ghosts.ghost_4.x_position, graphics_control.ghosts.ghost_4.y_position, 0);
				break;
			case 3:	//Augen darstellen
				place_eyes(graphics_control.ghosts.ghost_4.x_position, graphics_control.ghosts.ghost_4.y_position);
				break;
			case 4:	//ausgefüllte Geister im Käfig hüpfen lassen
				graphics_control.ghosts.ghost_4.x_position = 140;
				graphics_control.ghosts.ghost_4.y_position = pacman_flash.value ? 127 : 130;
				place_ghost(graphics_control.ghosts.ghost_4.x_position, graphics_control.ghosts.ghost_4.y_position, 1);
				break;
			case 5:	//nicht ausgefüllte Geister im Käfig hüpfen lassen
				graphics_control.ghosts.ghost_4.x_position = 140;
				graphics_control.ghosts.ghost_4.y_position = pacman_flash.value ? 127 : 130;
				place_ghost(graphics_control.ghosts.ghost_4.x_position, graphics_control.ghosts.ghost_4.y_position, 0);
				break;
			default:	//case 0: nicht anzeigen
				break;
		}
	}

	//lives:
		switch (graphics_control.lives) {
			case 0:	//nichts anzeigen
				break;

			default:	//bei Werten die größer als Drei sind, werden trotzdem nur Drei angezeigt

			case 3:	//drei Leben anzeigen
				place_pacman(102,5,0,0);
				//no break

			case 2:	//zwei Leben anzeigen
				place_pacman(89,5,0,0);
				//no break

			case 1:	//ein Leben anzeigen
				place_pacman(76,5,0,0);
				break;
		}


	//fruits
		if(!(graphics_control.game_state==2)) {	//nachfolgende Elemente werden beim Gameover nicht angezeigt
			//Früchte die bei "show_in_map" angezeigt werden, es kann immer nur eine Frucht gleichzeitig dargestellt werden.

			//Position der Frucht
#define in_map_fruit_position_x 125
#define in_map_fruit_position_y 108

			if(graphics_control.fruits.cherry.show_in_map)
				place_fruit(in_map_fruit_position_x,in_map_fruit_position_y,1);

			else if(graphics_control.fruits.strawberry.show_in_map)
				place_fruit(in_map_fruit_position_x,in_map_fruit_position_y,5);

			else if(graphics_control.fruits.banana.show_in_map)
				place_fruit(in_map_fruit_position_x,in_map_fruit_position_y,2);

			else if(graphics_control.fruits.apple.show_in_map)
				place_fruit(in_map_fruit_position_x,in_map_fruit_position_y,4);

			else if(graphics_control.fruits.pineapple.show_in_map)
				place_fruit(in_map_fruit_position_x,in_map_fruit_position_y,3);
		}

		//level fruits, Früchte die am Spielfeldrand angezeigt werden

		switch (graphics_control.level)
		{
			default:// alles anzeigen
				//no break

			case 5: //pineapple
				place_fruit(160,5,3);
				//no break

			case 4: //apple
				place_fruit(172,5,4);
				//no break

			case 3: //banana
				place_fruit(186,5,2);
				//no break

			case 2:	//strawberry
				place_fruit(201,5,5);
				//no break

			case 1:	//nur cherry
				place_fruit(215,5,1);
				break;
		}


	//dots

//		uint8_t dot_size_choice = 1;
		uint8_t dot_size_choice;

		for(uint8_t a = 0 ;a <= 243; a++) {
			//determine whether a large or small dot needs to be displayed (the four values ​​refer to the respective array segment of "graphics_control.dots")
			//number of the big dots 33, 103, 174, 228
#if 1
			dot_size_choice = !graphics_control.bigdots[a];
			if(graphics_control.dots[a]) {
				if (dot_size_choice) {								//small dots, abfragen ob der Punkt angezeigt werden soll
//					place_dot(a, dot_size_choice);
//					draw_relative_element_s(relative_small_dot, 5, dot_coordinates[a][0], dot_coordinates[a][1]);

					//offset Position from dem ersten Arraysegment lesen:
					//absolute Position auf dem Bildschirm, differenz für die Datenpunkte
					uint8_t x_diff = dot_coordinates[a][0] - relative_small_dot[0][0];
					uint8_t y_diff = dot_coordinates[a][1] - relative_small_dot[0][1];

					//bei 1 beginnen da in 0 die Offsetposition steht
					// Start with 1 because the Offset Position is contained in location 0
					gbham((x_diff + relative_small_dot[1][0]), (y_diff + relative_small_dot[1][1]), (x_diff + relative_small_dot[2][0]), (y_diff + relative_small_dot[2][1]));
					gbham((x_diff + relative_small_dot[2][0]), (y_diff + relative_small_dot[2][1]), (x_diff + relative_small_dot[3][0]), (y_diff + relative_small_dot[3][1]));
					gbham((x_diff + relative_small_dot[3][0]), (y_diff + relative_small_dot[3][1]), (x_diff + relative_small_dot[4][0]), (y_diff + relative_small_dot[4][1]));
					gbham((x_diff + relative_small_dot[4][0]), (y_diff + relative_small_dot[4][1]), (x_diff + relative_small_dot[5][0]), (y_diff + relative_small_dot[5][1]));
				}
				else {
					//prevent all points from being plotted at neutral positions
					// <- do not flash during the "Get Ready" phase (i.e., lock the flash to a fixed state)
					// flashing big dots; check whether the dot should be displayed; if so, it is displayed flashing.
					// Additionally, the large dots are displayed continuously during the "Get Ready" phase (as long as Pac-Man or Ms. Pac-Man are in the neutral state)
					if (pacman_flash.value /*<- Blinken der großen Punkte*/ || !(graphics_control.pacman.direction || graphics_control.ms_pacman.direction))
						place_dot(a, dot_size_choice);
				}
			}
#else
			dot_size_choice = !((a == 33) || (a == 103) || (a == 174) || (a == 228));
			if(graphics_control.dots[a] && dot_size_choice)								//small dots, abfragen ob der Punkt angezeigt werden soll
				place_dot(a, dot_size_choice);
			else if(!dot_size_choice) {		//prevent all points from being plotted at neutral positions
				/*<- do not flash during the "Get Ready" phase (i.e., lock the flash to a fixed state)*/ )
				//flashing big dots; check whether the dot should be displayed; if so, it is displayed flashing.
				// Additionally, the large dots are displayed continuously during the "Get Ready" phase (as long as Pac-Man or Ms. Pac-Man are in the neutral state)
					if(graphics_control.dots[a] && (pacman_flash.value /*<- Blinken der großen Punkte*/ || !(graphics_control.pacman.direction || graphics_control.ms_pacman.direction))
						place_dot(a, dot_size_choice);
				}
#endif
		}

	//Text
		//UP, READY! & Game Over
		if((graphics_control.pacman.direction || graphics_control.ms_pacman.direction) && !(graphics_control.game_state==2)) {	//If `direction` is not equal to 0, the text should flash. Otherwise, or during Game Over, the text does not flash.
			//If Pac-Man or Ms. Pac-Man no longer has `direction = 0`, "UP 1" or "UP 2" flashes.

			if(pacman_flash.value) {
				if(graphics_control.player_mode == 0 || graphics_control.player_mode == 1)	//bei Einspieler Modus oder Zweispieler (als Spieler 1) wird UP1 anzeigen
//					print_gbham_F(PSTR("UP 1"),23,245);
					print_gbham_F(PSTR("up 1"),23,245-16);
				else if(graphics_control.player_mode == 2)	//beim Zweispielermodus, als Spieler 2 wird UP2 angezeigt
//					print_gbham_F(PSTR("UP 2"),23,245);
					print_gbham_F(PSTR("up 2"),23,245-16);
			}
		}
		else {
			//ist direction gleich 0, wird "UP 1" bzw. "UP 2" und "READY!" dauerhaft angezeigt. Bei Game Over, wird "GAME OVER" angezeigt

			if(graphics_control.player_mode == 0 || graphics_control.player_mode == 1)	//bei Einspieler Modus oder Zweispieler (als Spieler 1) wird UP1 anzeigen
//				print_gbham_F(PSTR("UP 1"),23,245);
				print_gbham_F(PSTR("up 1"),23,245-16);
			else if(graphics_control.player_mode == 2)	//beim Zweispielermodus, als Spieler 2 wird UP2 angezeigt
//				print_gbham_F(PSTR("UP 2"),23,245);
				print_gbham_F(PSTR("up 2"),23,245-16);

			if(!(graphics_control.game_state==2))	//nicht Game Over
//				print_gbham_F(PSTR("READY!"),100,101);
				print_gbham_F(PSTR("ready"),98,98);
			else
//				print_gbham_F(PSTR("GAME OVER"),87,101);	//Game Over
				print_gbham_F(PSTR("game over"),80,98);
		}

		//HIGHSCORE Text
//		print_gbham_F(PSTR("HIGHSCORE"),158,245);
		print_gbham_F(PSTR("high"),134,245-16);

		//darstellen der Score strings

		//Score
		switch (graphics_control.player_mode)
		{
			case 0:	//Spieler 1 Score anzeigen
				//no break
			case 1:	//Spieler 1 Score anzeigen
//				print_gbham((char *)graphics_control.score_string_1,23,232);
				print_gbham((char *)graphics_control.score_string_1,23+48,245-16);
				break;
			case 2:	//Spieler 2 Score anzeigen
//				print_gbham((char *)graphics_control.score_string_2,23,232);
				print_gbham((char *)graphics_control.score_string_2,23+48,245-16);
				break;
			default://nicht anzeigen
				break;
		}

		//Highscore
//		print_gbham((char *)graphics_control.highscore_string,186,232);
		print_gbham((char *)graphics_control.highscore_string,186,245-16);

//		print_gbham_F(PSTR("LIVES:"),23,-2);	//-2 ist möglich, da der Uhrsprungspunkt wegen buchstaben wie G und Q tiefer ligt.
		print_gbham_F(PSTR("lives:"),8,-2);	//-2 is possible because the baseline lies lower due to letters like g and q.
}

void draw_startup_screen(void)
{
	draw_pacman_logo();

	place_pacman(255-pacman_move.value, 125, 3, pacman_flash.value);
	place_ghost(255-pacman_move.value - 30,125,0);
	place_ghost(255-pacman_move.value - 60,125,0);

	if(pacman_flash.value) {
		print_gbham_F(PSTR("insert coin"),85,95);
		print_gbham_F(PSTR("insert coin"),85,95);	//up intensity
	}

	print_gbham_F(PSTR("Programmed by:"),25,60);

	print_gbham_F(PSTR("Christian Brommer"),40,45);
	print_gbham_F(PSTR("Dennis Schuldt"),40,30);
}


void draw_scores(void)
{
	print_gbham_F(PSTR("Scores"),50,200);

	print_gbham_F(PSTR("Player 1:"),50,140);	print_gbham((char *)graphics_control.score_string_1,160,140);
	print_gbham_F(PSTR("Player 2:"),50,100);	print_gbham((char *)graphics_control.score_string_2,160,100);

	print_gbham_F(PSTR("Highscore:"),50,60);		print_gbham((char *)graphics_control.highscore_string,160,60);

	// won oder lose
	switch (graphics_control.result) {
		default:
		case 0:	//nichts anzeigen
			break;

		case 1:
			if(pacman_flash.value)
				print_gbham_F(PSTR("Winner"),160,200);
			break;

		case 2:
			if(pacman_flash.value)
				print_gbham_F(PSTR("Loser"),160,200);
			break;
	}

	//new score
	if(graphics_control.new_highscore_state && pacman_flash.value)
	{
		print_gbham_F(PSTR("new high score"),77,20);
	}
}

void draw_calibration(void)
{
#define text_x_offset 5	///< Text-Shift in X Direction

	print_gbham_F(PSTR("J o y s t i c k"),80-text_x_offset,215);
	print_gbham_F(PSTR("     S c r e e n"),66-text_x_offset,195);
	print_gbham_F(PSTR("C a l i b r a t i o n"),52-text_x_offset,175);

	if(pacman_flash.value)
	{
		print_gbham_F(PSTR("!"),70-text_x_offset,215); print_gbham_F(PSTR("!"),189-text_x_offset,215);	//about the joystick
		print_gbham_F(PSTR("!"),57-text_x_offset,195); print_gbham_F(PSTR("!"),202-text_x_offset,195);	//about the screen
		print_gbham_F(PSTR("!"),42-text_x_offset,175); print_gbham_F(PSTR("!"),219-text_x_offset,175);	//about the calibration
	}


#define x_center	125	///< middle screen Position in X direction
#define y_center	129	///< middle screen Position in Y direction

#define x_grid_length 31	///< Length of "one" grid square in the X-direction on the oscilloscope
#define y_grid_length 33	///< Length of "one" grid square in the Y-direction on the oscilloscope

//define, so that it is computed at compile time
#define x_left	x_center-x_grid_length
#define x_right	x_center+x_grid_length
#define y_bottom	y_center-y_grid_length
#define y_top	y_center+y_grid_length

	// Draw each one twice in reverse order so that it is easier to make out.
	gbham(x_center,y_bottom,x_center,y_top);	//X - Adjust
	gbham(x_center,y_top,x_center,y_bottom);

	gbham(x_left,y_center,x_right,y_center);	//y - Adjust
	gbham(x_right,y_center,x_left,y_center);

	// Box around the cross
	gbham(x_left,y_bottom,x_left,y_top);
	gbham(x_left,y_top,x_right,y_top);
	gbham(x_right,y_top,x_right,y_bottom);
	gbham(x_right,y_bottom,x_left,y_bottom);

	print_gbham_F(PSTR("Center the cross on the"),5,75);					//"Kreuz in die mitte des Bild"
	print_gbham_F(PSTR("screen and adjust it by"),5,61);
	print_gbham_F(PSTR("width of one square in"),5,47);
	print_gbham_F(PSTR("each direction  Return"),5,32);
	print_gbham_F(PSTR("joystick to the neutral"),5,17);
	print_gbham_F(PSTR("position, press trigger"),5,3);
}

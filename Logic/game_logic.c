/**
 *  \file game_logic.c
 *
 *  \date: 14.12.2012
 *  \author: Christian Brommer & Dennis Schuldt
 */
#include "global.h"
#include "character_logic.h"
#include "serial.h"
#include "serial_send.h"
#include "game_logic.h"
#include "ghost_ai.h"
#include "util/delay.h"

void logic_init(void)
{
	// Pre-initialize all game logic variables accordingly.
	logic_control.game_state=calibration;	// Erster Zustand mit dem gestartet wird
	logic_control.hold=1;					// Spielstart mit angehaltener Logik
	logic_control.player_mode=0;			// Start in Single Player Mode
	logic_control.player1_mode=0;
	logic_control.player2_mode=0;
	logic_control.pacman.score=0;
	logic_control.pacman.score_old=1;		// 1 damit am Anfang eine 0 ausgegeben wird
	logic_control.ms_pacman.score=0;
	logic_control.ms_pacman.score_old=1;	// 1 damit am Anfang eine 0 ausgegeben wird
	logic_control.new_highscore_state=0;
	logic_control.result=0;
	logic_control.lives=3;					// Anzahl der Leben beim Spielstart
	logic_control.level=1;					// Anfangslevel
	logic_control.dot_counter=0;
	logic_control.fruits.timer=0;
	logic_control.fruits.timer_limit=9500;	// Zeit wie lange Früchte angezeigt werden (in ms)
	logic_control.fruits.fruit1_done=0;
	logic_control.fruits.fruit2_done=0;

	// Counter für Zeiten und die entsprechenden toggle flags setzen
	logic_control.ghost_counter=0;
	logic_control.ghost_logic_mode_toggle=0;
	logic_control.pacman_counter=0;
	logic_control.pacman_dinner_toggle=0;
	logic_control.update_pacman_position_flag=0;
	logic_control.update_pacman_time=20;			// Pacman/Ms.Pacman Aktualisierungszeit auf 20ms setzen

	logic_control.fruits.banana.show_in_map=0;
	logic_control.fruits.apple.show_in_map=0;
	logic_control.fruits.cherry.show_in_map=0;
	logic_control.fruits.strawberry.show_in_map=0;
	logic_control.fruits.pineapple.show_in_map=0;

	logic_control.ghost_home_time=4000;				// Zeit wie lange die Geister versuchen in ihre eigenen Ecke zu kommen
	logic_control.ghost_chase_time=6000;			// Zeit wie lange die Geister versuchen Pacman/Ms.Pacman zu "jagen"
	logic_control.pacman_dinner_time=10000;			// Zeit wie lange Pacman die Geister fressen kann (nachdem ein großer Punkt gefressen wurde)

	for (uint8_t i=0 ; i<=243 ; i++)
	{
		logic_control.dots[i]=1;					// Array in denen die anzuzeigenden Punkte gespeichert werden mit 1 vorbelegen (Punkte vorhanden)
	}

	dot_init();
}

void logic_init_next_level(void)
{
	// Spiellogik Variablen für das nächste Level zurücksetzen
	logic_control.game_state=gameplay;
	logic_control.level++;
	logic_control.dot_counter=0;
	logic_control.fruits.timer=0;
	logic_control.fruits.timer_limit=9500;
	logic_control.fruits.fruit1_done=0;
	logic_control.fruits.fruit2_done=0;

	logic_control.ghost_counter=0;
	logic_control.ghost_logic_mode_toggle=0;

	logic_control.pacman_counter=0;
	logic_control.pacman_dinner_toggle=0;

	logic_control.update_pacman_position_flag=0;

	logic_control.fruits.banana.show_in_map=0;
	logic_control.fruits.apple.show_in_map=0;
	logic_control.fruits.cherry.show_in_map=0;
	logic_control.fruits.strawberry.show_in_map=0;
	logic_control.fruits.pineapple.show_in_map=0;

	for (uint8_t i=0 ; i<=243 ; i++)
	{
		logic_control.dots[i]=1;
	}

	dot_init();
}

void bonus_fruit_display(void)
{
	// Fruits display
	switch (logic_control.level)				// Früchte in Abhängigkeit des Levels
	{
		case 1:
			logic_control.fruits.points=100;	// für die Frucht setzen
			if (logic_control.dot_counter==70 && !(logic_control.fruits.fruit1_done))  // Wenn 70 Punkte gefressen und noch nicht angezeigt
				{
					logic_control.fruits.cherry.show_in_map=1;		// Frucht anzeigen
					logic_control.fruits.fruit_display=1;			// Hilfsvariable für Fruchtanzeige
					logic_control.fruits.timer=0;					// timer zurücksetzen
					logic_control.fruits.fruit1_done=1;				// 1te Frucht pro Level wurde angezeigt
				}
			if (logic_control.dot_counter==170 && !(logic_control.fruits.fruit2_done))	// Wenn 170 Punkte gefressen und noch nicht angezeigt
				{
					logic_control.fruits.cherry.show_in_map=1;
					logic_control.fruits.fruit_display=1;
					logic_control.fruits.timer=0;
					logic_control.fruits.fruit2_done=1;				// 2te Frucht pro Level wurde angezeigt
				}
			break;

		case 2:
			logic_control.fruits.points=300;
			if (logic_control.dot_counter==70 && !(logic_control.fruits.fruit1_done))
				{
					logic_control.fruits.strawberry.show_in_map=1;
					logic_control.fruits.fruit_display=1;
					logic_control.fruits.timer=0;
					logic_control.fruits.fruit1_done=1;
				}
			if (logic_control.dot_counter==170 && !(logic_control.fruits.fruit2_done))
				{
					logic_control.fruits.strawberry.show_in_map=1;
					logic_control.fruits.fruit_display=1;
					logic_control.fruits.timer=0;
					logic_control.fruits.fruit2_done=1;
				}
			break;

		case 3:
			logic_control.fruits.points=500;
			if (logic_control.dot_counter==70 && !(logic_control.fruits.fruit1_done))
				{
					logic_control.fruits.banana.show_in_map=1;
					logic_control.fruits.fruit_display=1;
					logic_control.fruits.timer=0;
					logic_control.fruits.fruit1_done=1;
				}
			if (logic_control.dot_counter==170 && !(logic_control.fruits.fruit2_done))
				{
					logic_control.fruits.banana.show_in_map=1;
					logic_control.fruits.fruit_display=1;
					logic_control.fruits.timer=0;
					logic_control.fruits.fruit2_done=1;
				}
			break;

		case 4:
			logic_control.fruits.points=700;
			if (logic_control.dot_counter==70 && !(logic_control.fruits.fruit1_done))
				{
					logic_control.fruits.apple.show_in_map=1;
					logic_control.fruits.fruit_display=1;
					logic_control.fruits.timer=0;
					logic_control.fruits.fruit1_done=1;
				}
			if (logic_control.dot_counter==170 && !(logic_control.fruits.fruit2_done))
				{
					logic_control.fruits.apple.show_in_map=1;
					logic_control.fruits.fruit_display=1;
					logic_control.fruits.timer=0;
					logic_control.fruits.fruit2_done=1;
				}
			break;

		case 5:
			logic_control.fruits.points=1000;
			if (logic_control.dot_counter==70 && !(logic_control.fruits.fruit1_done))
				{
					logic_control.fruits.pineapple.show_in_map=1;
					logic_control.fruits.fruit_display=1;
					logic_control.fruits.timer=0;
					logic_control.fruits.fruit1_done=1;
				}
			if (logic_control.dot_counter==170 && !(logic_control.fruits.fruit2_done))
				{
					logic_control.fruits.pineapple.show_in_map=1;
					logic_control.fruits.fruit_display=1;
					logic_control.fruits.timer=0;
					logic_control.fruits.fruit2_done=1;
				}
			break;


		default:	// Wenn level > 5
			logic_control.fruits.points=1000;
			if (logic_control.dot_counter==70 && !(logic_control.fruits.fruit1_done))
				{
					logic_control.fruits.pineapple.show_in_map=1;
					logic_control.fruits.fruit_display=1;
					logic_control.fruits.timer=0;
					logic_control.fruits.fruit1_done=1;
				}
			if (logic_control.dot_counter==170 && !(logic_control.fruits.fruit2_done))
				{
					logic_control.fruits.pineapple.show_in_map=1;
					logic_control.fruits.fruit_display=1;
					logic_control.fruits.timer=0;
					logic_control.fruits.fruit2_done=1;
				}
			break;
	}
}

void logic_check(void)
{
	bonus_fruit_display();		// Früchte darstellen
	ghost_logic_toggle();		// überprüfen vom Ghost toggle für Home Position bzw. Pacman Verfolgung
	pacman_meal_toggle();		// überprüfen wann Geister wieder "normal" werden nachdem Pacman sie gefressen hat

	// ab bestimmten Punktzahlen Geister aus Käfig lassen, wenn sie im Käfig sind und nachdem sie gefressen würden mindestens wieder 5 neue Dots gegessen wurden
	if (logic_control.ghost_1.cage && logic_control.ghost_1.dotcounter_ghost>=0)
		{
			// Geister auf Position vor dem Käfig setzen
			logic_control.ghost_1.x_grid=logic_control.ghost_1.cage_outside_x_grid;
			logic_control.ghost_1.x_pixel=(logic_control.ghost_1.x_grid*grid_dim);
			logic_control.ghost_1.y_grid=logic_control.ghost_1.cage_outside_y_grid;
			logic_control.ghost_1.y_pixel=(logic_control.ghost_1.y_grid*grid_dim)+grid_center_offset;
			if (logic_control.ghost_1.graphic_state==filled_flash) logic_control.ghost_1.graphic_state=filled;				// Wenn Ghost ausgefüllt blinkt, dann ausgefüllt rausschicken
			if (logic_control.ghost_1.graphic_state==not_filled_flash) logic_control.ghost_1.graphic_state=not_filled;		// Wenn Ghost nicht ausgefüllt blinkt, dann nicht ausgefüllt rausschicken
			logic_control.ghost_1.cage=0;
		}

	if (logic_control.dot_counter>=5 && logic_control.ghost_2.cage && logic_control.ghost_2.dotcounter_ghost>=5)
		{
			logic_control.ghost_2.x_grid=logic_control.ghost_2.cage_outside_x_grid;
			logic_control.ghost_2.x_pixel=(logic_control.ghost_2.x_grid*grid_dim);
			logic_control.ghost_2.y_grid=logic_control.ghost_2.cage_outside_y_grid;
			logic_control.ghost_2.y_pixel=(logic_control.ghost_2.y_grid*grid_dim)+grid_center_offset;
			if (logic_control.ghost_2.graphic_state==filled_flash) logic_control.ghost_2.graphic_state=filled;
			if (logic_control.ghost_2.graphic_state==not_filled_flash) logic_control.ghost_2.graphic_state=not_filled;
			logic_control.ghost_2.cage=0;
		}

	if (logic_control.dot_counter>=30 && logic_control.ghost_3.cage && logic_control.ghost_3.dotcounter_ghost>=10)
		{
			logic_control.ghost_3.x_grid=logic_control.ghost_3.cage_outside_x_grid;
			logic_control.ghost_3.x_pixel=(logic_control.ghost_3.x_grid*grid_dim);
			logic_control.ghost_3.y_grid=logic_control.ghost_3.cage_outside_y_grid;
			logic_control.ghost_3.y_pixel=(logic_control.ghost_3.y_grid*grid_dim)+grid_center_offset;
			if (logic_control.ghost_3.graphic_state==filled_flash) logic_control.ghost_3.graphic_state=filled;
			if (logic_control.ghost_3.graphic_state==not_filled_flash) logic_control.ghost_3.graphic_state=not_filled;
			logic_control.ghost_3.cage=0;
		}

	if (logic_control.dot_counter>=90 && logic_control.ghost_4.cage && logic_control.ghost_4.dotcounter_ghost>=15)
		{
			logic_control.ghost_4.x_grid=logic_control.ghost_4.cage_outside_x_grid;
			logic_control.ghost_4.x_pixel=(logic_control.ghost_4.x_grid*grid_dim);
			logic_control.ghost_4.y_grid=logic_control.ghost_4.cage_outside_y_grid;
			logic_control.ghost_4.y_pixel=(logic_control.ghost_4.y_grid*grid_dim)+grid_center_offset;
			if (logic_control.ghost_4.graphic_state==filled_flash) logic_control.ghost_4.graphic_state=filled;
			if (logic_control.ghost_4.graphic_state==not_filled_flash) logic_control.ghost_4.graphic_state=not_filled;
			logic_control.ghost_4.cage=0;
		}


	if (logic_control.dot_counter==244)			// Wenn alle Punkte weg -> nächstes level
	{
		send_to_graphics1();
#if	TWOPLAYERMODE
		if (logic_control.player2_mode == 2) send_to_graphics2();
#endif
		_delay_ms(1000);						// 1s warten bis das nächste Level los geht...
		next_level();
	}

	if (logic_control.lives==0)					// Wenn keine Leben mehr übrig
		{
			logic_control.game_state=gameover;
			send_logic_data();
		}

	if (logic_control.level == 0)				// Wenn letztes Level geschafft
		{
			logic_control.game_state=the_end;
			send_logic_data();
		}
}

void next_level(void)
{
	logic_init_next_level();
	pacman_init();
#if TWOPLAYERMODE
	if (logic_control.player2_mode==2) ms_pacman_init();
#endif
	ghost_data_init();

	if (logic_control.level==3)		// Schwierigkeitsgrad ab Level 3 erhöhen, Home Zeit verkürzen
	{
		logic_control.ghost_home_time=3000;
		logic_control.ghost_chase_time=8000;
	}

	if (logic_control.level>=5)		// Schwierigkeitsgrad ab Level 5 erhöhen, Home Zeit erneut verkürzen
	{
		logic_control.ghost_home_time=1000;
		logic_control.ghost_chase_time=14000;
	}

	send_all_dots();				// wieder starten mit allen Dots
	send_logic_data();				// Logikdaten über seriell verschicken
}

void pacman_meal_toggle(void)
{
	if (logic_control.pacman_dinner_toggle)	// Wenn die Zeit für Pacman "kann Geister fressen" abgelaufen ist
	{
		if (logic_control.ghost_1.graphic_state == not_filled)	// Wenn nicht ausgefüllt dann Ziel Modus zurücksetzen und Ghost ausfüllen
			{
				logic_control.ghost_1.logic_mode=target_pacman;
				logic_control.ghost_1.update_time=20;				// Geschwingdigkeit auf Standard zurücksetzen
				logic_control.ghost_1.update_position_counter=0;
				logic_control.ghost_1.graphic_state=filled;			// nur auf ausgefüllt ändern, wenn nicht ausgefüllt oder im Käfig blinkend
			}
		else if (logic_control.ghost_1.graphic_state == not_filled_flash)	// wenn nicht ausgefüllt und blinkend, dann ausgefüllt und blinkend anzeigen
			{
				logic_control.ghost_1.update_time=20;				// Geschwingdigkeit auf Standard zurücksetzen
				logic_control.ghost_1.update_position_counter=0;
				logic_control.ghost_1.graphic_state=filled_flash;
			}


		if (logic_control.ghost_2.graphic_state == not_filled)
			{
				if (logic_control.player2_mode == 2) logic_control.ghost_2.logic_mode=target_ms_pacman;		// Wenn 2 Spieler Spiel läuft, dann verfolgt Ghost 2 Ms.Pacman
				else logic_control.ghost_2.logic_mode=target_pacman_4;
				logic_control.ghost_2.update_time=20;
				logic_control.ghost_2.update_position_counter=0;
				logic_control.ghost_2.graphic_state=filled;
			}
		else if (logic_control.ghost_2.graphic_state == not_filled_flash)
			{
				logic_control.ghost_2.update_time=20;
				logic_control.ghost_2.update_position_counter=0;
				logic_control.ghost_2.graphic_state=filled_flash;
			}


		if (logic_control.ghost_3.graphic_state == not_filled)
			{
				logic_control.ghost_3.logic_mode=random_direction;
				logic_control.ghost_3.update_time=20;
				logic_control.ghost_3.update_position_counter=0;
				logic_control.ghost_3.graphic_state=filled;
			}
		else if (logic_control.ghost_3.graphic_state == not_filled_flash)
			{
				logic_control.ghost_3.update_time=20;
				logic_control.ghost_3.update_position_counter=0;
				logic_control.ghost_3.graphic_state=filled_flash;
			}


		if (logic_control.ghost_4.graphic_state == not_filled)
			{
				logic_control.ghost_4.logic_mode=random_direction;
				logic_control.ghost_4.update_time=20;
				logic_control.ghost_4.update_position_counter=0;
				logic_control.ghost_4.graphic_state=filled;
			}
		else if (logic_control.ghost_4.graphic_state == not_filled_flash)
			{
				logic_control.ghost_4.update_time=20;
				logic_control.ghost_4.update_position_counter=0;
				logic_control.ghost_4.graphic_state=filled_flash;
			}
		logic_control.pacman_dinner_toggle=0;		// Pacman kann Geister fressen toggle zurücksetzen
	}
}

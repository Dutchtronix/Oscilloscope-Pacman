/**
 *  \file ghost_ai.c
 *
 *  \date: 14.12.2012
 *  \author: Christian Brommer & Dennis Schuldt
 */

#include "global.h"
#include "ghost_ai.h"
#include "character_logic.h"
#include <avr/io.h>
#include "math.h"

void ghost_data_init(void)
{
	// initialize Ghost 1
	logic_control.ghost_1.grid_valid=1;
	logic_control.ghost_1.next_grid_valid=0;
	logic_control.ghost_1.direction=neutral;
	logic_control.ghost_1.next_direction=neutral;
	logic_control.ghost_1.pixel_per_step=1;
	logic_control.ghost_1.graphic_state=filled;
	logic_control.ghost_1.logic_mode=target_pacman;
	logic_control.ghost_1.cage=0;						// Ghost 1 niemals im Käfig
	logic_control.ghost_1.dotcounter_ghost=0;
	logic_control.ghost_1.ghost_reverse_help=0;
	logic_control.ghost_1.update_position_flag=0;
	logic_control.ghost_1.update_time=20;				// Ghost Aktualisierungszeit auf 20ms setzen

	// Set positions
	logic_control.ghost_1.cage_inside_x_grid=18;
	logic_control.ghost_1.cage_inside_y_grid=18;
	logic_control.ghost_1.cage_outside_x_grid=18;
	logic_control.ghost_1.cage_outside_y_grid=21;
	logic_control.ghost_1.x_grid=logic_control.ghost_1.cage_outside_x_grid;
	logic_control.ghost_1.x_pixel=(logic_control.ghost_1.x_grid*grid_dim);
	logic_control.ghost_1.y_grid=logic_control.ghost_1.cage_outside_y_grid;
	logic_control.ghost_1.y_pixel=(logic_control.ghost_1.y_grid*grid_dim)+grid_center_offset;
	logic_control.ghost_1.home_corner_x=35;
	logic_control.ghost_1.home_corner_y=217;

	// initialize Ghost 2
	logic_control.ghost_2.grid_valid=1;
	logic_control.ghost_2.next_grid_valid=0;
	logic_control.ghost_2.direction=neutral;
	logic_control.ghost_2.next_direction=neutral;
	logic_control.ghost_2.pixel_per_step=1;
	logic_control.ghost_2.graphic_state=filled_flash;
	if (logic_control.player2_mode == 2) {
		logic_control.ghost_2.logic_mode=target_ms_pacman;
	}
	else {
		logic_control.ghost_2.logic_mode=target_pacman_4;
	}
	logic_control.ghost_2.cage=1;
	logic_control.ghost_2.dotcounter_ghost=0;
	logic_control.ghost_2.ghost_reverse_help=0;
	logic_control.ghost_2.update_position_flag=0;
	logic_control.ghost_2.update_time=20;

	logic_control.ghost_2.cage_inside_x_grid=18;
	logic_control.ghost_2.cage_inside_y_grid=18;
	logic_control.ghost_2.cage_outside_x_grid=18;
	logic_control.ghost_2.cage_outside_y_grid=21;
	logic_control.ghost_2.x_grid=logic_control.ghost_2.cage_inside_x_grid;
	logic_control.ghost_2.x_pixel=(logic_control.ghost_2.x_grid*grid_dim);
	logic_control.ghost_2.y_grid=logic_control.ghost_2.cage_inside_y_grid;
	logic_control.ghost_2.y_pixel=(logic_control.ghost_2.y_grid*grid_dim)+grid_center_offset;
	logic_control.ghost_2.home_corner_x=210;
	logic_control.ghost_2.home_corner_y=217;

	// initialize Ghost 3
	logic_control.ghost_3.grid_valid=1;
	logic_control.ghost_3.next_grid_valid=0;
	logic_control.ghost_3.direction=neutral;
	logic_control.ghost_3.next_direction=neutral;
	logic_control.ghost_3.pixel_per_step=1;
	logic_control.ghost_3.graphic_state=4;
	logic_control.ghost_3.logic_mode=random_direction;
	logic_control.ghost_3.cage=1;
	logic_control.ghost_3.dotcounter_ghost=0;
	logic_control.ghost_3.ghost_reverse_help=0;
	logic_control.ghost_3.update_position_flag=0;
	logic_control.ghost_3.update_time=20;

	logic_control.ghost_3.cage_inside_x_grid=16;
	logic_control.ghost_3.cage_inside_y_grid=18;
	logic_control.ghost_3.cage_outside_x_grid=18;
	logic_control.ghost_3.cage_outside_y_grid=21;
	logic_control.ghost_3.x_grid=logic_control.ghost_3.cage_inside_x_grid;
	logic_control.ghost_3.x_pixel=(logic_control.ghost_3.x_grid*grid_dim);
	logic_control.ghost_3.y_grid=logic_control.ghost_3.cage_inside_y_grid;
	logic_control.ghost_3.y_pixel=(logic_control.ghost_3.y_grid*grid_dim)+grid_center_offset;
	logic_control.ghost_3.home_corner_x=35;
	logic_control.ghost_3.home_corner_y=21;

	// initialize Ghost 4
	logic_control.ghost_4.grid_valid=1;
	logic_control.ghost_4.next_grid_valid=0;
	logic_control.ghost_4.direction=neutral;
	logic_control.ghost_4.next_direction=neutral;
	logic_control.ghost_4.pixel_per_step=1;
	logic_control.ghost_4.graphic_state=filled_flash;
	logic_control.ghost_4.logic_mode=random_direction;
	logic_control.ghost_4.cage=1;
	logic_control.ghost_4.dotcounter_ghost=0;
	logic_control.ghost_4.ghost_reverse_help=0;
	logic_control.ghost_4.update_position_flag=0;
	logic_control.ghost_4.update_time=20;

	logic_control.ghost_4.cage_inside_x_grid=20;
	logic_control.ghost_4.cage_inside_y_grid=18;
	logic_control.ghost_4.cage_outside_x_grid=18;
	logic_control.ghost_4.cage_outside_y_grid=21;
	logic_control.ghost_4.x_grid=logic_control.ghost_4.cage_inside_x_grid;
	logic_control.ghost_4.x_pixel=(logic_control.ghost_4.x_grid*grid_dim);
	logic_control.ghost_4.y_grid=logic_control.ghost_4.cage_inside_y_grid;
	logic_control.ghost_4.y_pixel=(logic_control.ghost_4.y_grid*grid_dim)+grid_center_offset;
	logic_control.ghost_4.home_corner_x=210;
	logic_control.ghost_4.home_corner_y=21;
}

uint8_t global_random_help = 90;				// Hilfsvariable für Zufallsgenerator

void randomize_me(volatile struct character_data *data)
{
	// Segmentsdefinitionen für den Zufallsgenerator
	#define segment1 64		// 255/5
	#define segment2 128	// 255/5  * 2
	#define segment3 192	// 255/5  * 3
	#define segment4 255	// 255/5  * 4

	// lokale temp Variablen
	uint8_t temp_1 = 0;
	uint8_t temp_2 = 0;

	// Zweistellen aus global_random_help "rausziehen/filtern"
	if(global_random_help & 0x40)
		temp_1 = 1;

	if(global_random_help & 0x04)
		temp_2 = 1;

	// Ergebnisse XOR verknüpfen
	uint8_t temp = temp_1 ^ temp_2;

	// global_random_help um eine Stelle schieben, "0 nachschieben"
	global_random_help = (global_random_help << 1);

	// Wenn die XOR Verknüpfung = 1, dann global_random_help + 1 = eine 1 nachgeschoben! Wenn temp=0 eine 0 nachgschoben!
	if(temp) {
		global_random_help++;
		temp = 0;
	}

	// Wenn neue Richtung übernommen oder Ghost startet mit Neutral oder Ghost steht vor Wand (ghost_help=1), dann neue Richtung übernehmen
	if ((data->direction == data->next_direction) || data->direction==neutral || data->ghost_help==1) {
		if (global_random_help < segment1) {
				if (!(data->direction==down) && !(data->next_direction==up)) data->next_direction=up;
		}
		else
			if(global_random_help < segment2) {
				if (!(data->direction==up) && !(data->next_direction==down))
					data->next_direction=down;
			}
		else
			if(global_random_help < segment3) {
				if (!(data->direction==left) && !(data->next_direction==right))
					data->next_direction=right;
			}
		else
			if(global_random_help < segment4) {
				if (!(data->direction==right) && !(data->next_direction==left)) data->next_direction=left;
			}
		data->ghost_help=0;			// Wenn Ghost vor einer Wand stand entsprechend wieder zurücksetzen
	}
}

void ghost_target_pacman(volatile struct character_data *ghost, uint8_t mode)
{
	uint8_t possible_grid[5][5] =		// lokales Array für {x,y,x entfernung, y entfernung, gesamt entfernung}
	{
			{0,0,0,0,0},
			{0,0,0,0,0},
			{0,0,0,0,0},
			{0,0,0,0,0},
			{0,0,0,0,0}
	};

	//Ghost special data
	// 0=nächstes in gleicher position/wird nicht benötigt, 1=right, 2=down, 3=left, 4=up
	uint8_t next_grid_possible[5] =	{0, 0, 0, 0, 0};

	uint16_t kleinster_wert=600;	// Hilfsvariable zur Entscheidung welche Entfernung die kleinste ist (einfach groß vorbelegt, damit das errechnete aufjedenfall kleiner ist)

	uint16_t math_help_x=0;			// 16-bit Hilfsvariable zur Entfernungsberechnung
	uint16_t math_help_y=0;			// 16-bit Hilfsvariable zur Entfernungsberechnung

	switch(ghost->direction)
	{
						// Wenn der Ghost Augen sind oder nicht ausgefüllt, darf er auch einmal in die entgegengesetzte Richtung
		case up:		if ((ghost->graphic_state==eyes || ghost->graphic_state==not_filled) && ghost->ghost_reverse_help) {
							 //check nächstes Feld in entsprechende Richtung
							next_grid_possible[down] = check_grid_valid(possible_grid[down][0] = ghost->x_pixel, possible_grid[down][1] = ghost->y_pixel-grid_dim);
							ghost->ghost_reverse_help=0;
						}
						next_grid_possible[right] = check_grid_valid(possible_grid[right][0] = ghost->x_pixel+grid_dim, possible_grid[right][1] = ghost->y_pixel);  //check nächstes Feld in entsprechende Richtung
						next_grid_possible[left] = check_grid_valid(possible_grid[left][0] = ghost->x_pixel-grid_dim, possible_grid[left][1] = ghost->y_pixel); 	//check nächstes Feld in entsprechende Richtung
						next_grid_possible[up] = check_grid_valid(possible_grid[up][0] = ghost->x_pixel, possible_grid[up][1] = ghost->y_pixel+grid_dim); 			//check nächstes Feld in entsprechende Richtung
						break;

		case down:		if ((ghost->graphic_state==eyes || ghost->graphic_state==not_filled) && ghost->ghost_reverse_help)
						{
							next_grid_possible[up] = check_grid_valid(possible_grid[up][0] = ghost->x_pixel, possible_grid[up][1] = ghost->y_pixel+grid_dim);
							ghost->ghost_reverse_help=0;
						}
						next_grid_possible[right] = check_grid_valid(possible_grid[right][0] = ghost->x_pixel+grid_dim, possible_grid[right][1] = ghost->y_pixel);
						next_grid_possible[left] = check_grid_valid(possible_grid[left][0] = ghost->x_pixel-grid_dim, possible_grid[left][1] = ghost->y_pixel);
						next_grid_possible[down] = check_grid_valid(possible_grid[down][0] = ghost->x_pixel, possible_grid[down][1] = ghost->y_pixel-grid_dim);
						break;

		case right:		if ((ghost->graphic_state==eyes || ghost->graphic_state==not_filled) && ghost->ghost_reverse_help)
						{
							next_grid_possible[left] = check_grid_valid(possible_grid[left][0] = ghost->x_pixel-grid_dim, possible_grid[left][1] = ghost->y_pixel);
							ghost->ghost_reverse_help=0;
						}
						next_grid_possible[up] = check_grid_valid(possible_grid[up][0] = ghost->x_pixel, possible_grid[up][1] = ghost->y_pixel+grid_dim);
						next_grid_possible[down] = check_grid_valid(possible_grid[down][0] = ghost->x_pixel, possible_grid[down][1] = ghost->y_pixel-grid_dim);
						next_grid_possible[right] = check_grid_valid(possible_grid[right][0] = ghost->x_pixel+grid_dim, possible_grid[right][1] = ghost->y_pixel);
						break;

		case left:		if ((ghost->graphic_state==eyes || ghost->graphic_state==not_filled) && ghost->ghost_reverse_help)
						{
							next_grid_possible[right] = check_grid_valid(possible_grid[right][0] = ghost->x_pixel+grid_dim, possible_grid[right][1] = ghost->y_pixel);
							ghost->ghost_reverse_help=0;
						}
						next_grid_possible[up] = check_grid_valid(possible_grid[up][0] = ghost->x_pixel, possible_grid[up][1] = ghost->y_pixel+grid_dim);
						next_grid_possible[down] = check_grid_valid(possible_grid[down][0] = ghost->x_pixel, possible_grid[down][1] = ghost->y_pixel-grid_dim);
						next_grid_possible[left] = check_grid_valid(possible_grid[left][0] = ghost->x_pixel-grid_dim, possible_grid[left][1] = ghost->y_pixel);
						break;

		case neutral:	next_grid_possible[right] = check_grid_valid(possible_grid[right][0] = ghost->x_pixel+grid_dim, possible_grid[right][1] = ghost->y_pixel);
						next_grid_possible[left] = check_grid_valid(possible_grid[left][0] = ghost->x_pixel-grid_dim, possible_grid[left][1] = ghost->y_pixel);
						break;
	}

	// Schleife zur Berechnung der Entfernung und Entscheidung des kürzesten Wegs
	// Loop for calculating the distance and deciding on the shortest path
	for (uint8_t a=1; a<=4; a++) {
		if(next_grid_possible[a])		// Wenn die Richtung Möglich ist
		{
			if (mode==target_pacman)
			{
				if (logic_control.pacman.x_pixel > possible_grid[a][0])
					possible_grid[a][2] = logic_control.pacman.x_pixel - possible_grid[a][0];
				else possible_grid[a][2] = possible_grid[a][0] - logic_control.pacman.x_pixel;		// x Entfernung in Pixeln zum Ziel

				if (logic_control.pacman.y_pixel > possible_grid[a][1])
					possible_grid[a][3] = logic_control.pacman.y_pixel - possible_grid[a][1];		// beachten das immer das größte vom kleineren abgezogen werden muss
				else possible_grid[a][3] = possible_grid[a][1] - logic_control.pacman.y_pixel;		// y Entfernung in Pixeln zum Ziel
			}
			else if (mode==target_pacman_4)
				{
				switch(logic_control.pacman.direction)
					{
					case up:
							if (logic_control.pacman.y_pixel+4*grid_dim > possible_grid[a][1])					//4*grid_dim => ist 4 Kästchen vor Pacman in Laufrichtung
								possible_grid[a][3] = logic_control.pacman.y_pixel+4*7 - possible_grid[a][1];
							else possible_grid[a][3] = possible_grid[a][1] - logic_control.pacman.y_pixel+4*7;
							break;

					case down:
							if (logic_control.pacman.y_pixel-4*grid_dim > possible_grid[a][1])
								possible_grid[a][3] = logic_control.pacman.y_pixel-4*7 - possible_grid[a][1];
							else possible_grid[a][3] = possible_grid[a][1] - logic_control.pacman.y_pixel-4*7;
							break;

					case left:
							if (logic_control.pacman.x_pixel-4*grid_dim > possible_grid[a][0])
								possible_grid[a][2] = logic_control.pacman.x_pixel-4*7 - possible_grid[a][0];
							else possible_grid[a][2] = possible_grid[a][0] - logic_control.pacman.x_pixel-4*7;
							break;

					case right: break;
							if (logic_control.pacman.x_pixel+4*grid_dim > possible_grid[a][0])
								possible_grid[a][2] = logic_control.pacman.x_pixel+4*7 - possible_grid[a][0];
							else possible_grid[a][2] = possible_grid[a][0] - logic_control.pacman.x_pixel+4*7;
							break;

					case neutral: break;
					}
				}
			else if (mode==target_ms_pacman)
			{
				if (logic_control.ms_pacman.x_pixel > possible_grid[a][0])
					possible_grid[a][2] = logic_control.pacman.x_pixel - possible_grid[a][0];		//x Entfernung zu target berechnen
				else possible_grid[a][2] = possible_grid[a][0] - logic_control.pacman.x_pixel;		//x Entfernung zu target berechnen

				if (logic_control.ms_pacman.y_pixel > possible_grid[a][1])
					possible_grid[a][3] = logic_control.pacman.y_pixel - possible_grid[a][1];		// beachten was von was abgezogen werden muss
				else possible_grid[a][3] = possible_grid[a][1] - logic_control.pacman.y_pixel;		//x Entfernung zu target berechnen
			}
			else if (mode==home)
			{
				if (ghost->home_corner_x > possible_grid[a][0])
					possible_grid[a][2] = ghost->home_corner_x  - possible_grid[a][0];		//x Entfernung zu target berechnen
				else possible_grid[a][2] = possible_grid[a][0] - ghost->home_corner_x ;		//x Entfernung zu target berechnen

				if (ghost->home_corner_y > possible_grid[a][1])
					possible_grid[a][3] = ghost->home_corner_y - possible_grid[a][1];		// beachten was von was abgezogen werden muss
				else possible_grid[a][3] = possible_grid[a][1] - ghost->home_corner_y;		//x Entfernung zu target berechnen
			}
			else if (mode==eyes_home)
			{
				if (ghost->cage_outside_x_grid*grid_dim > possible_grid[a][0])
					possible_grid[a][2] = ghost->cage_outside_x_grid*grid_dim - possible_grid[a][0];		//x Entfernung zu target berechnen
				else possible_grid[a][2] = possible_grid[a][0] - ghost->cage_outside_x_grid*grid_dim;		//x Entfernung zu target berechnen

				if (ghost->cage_outside_y_grid*grid_dim > possible_grid[a][1])
					possible_grid[a][3] = ghost->cage_outside_y_grid*grid_dim - possible_grid[a][1];		// beachten was von was abgezogen werden muss
				else possible_grid[a][3] = possible_grid[a][1] - ghost->cage_outside_y_grid*grid_dim;		//x Entfernung zu target berechnen
			}

			// Berechnung Entfernung zum Ziel
			math_help_x = possible_grid[a][2]*possible_grid[a][2];
			math_help_y = possible_grid[a][3]*possible_grid[a][3];
			possible_grid[a][4]=sqrt(math_help_x + math_help_y);

			if (possible_grid[a][4] < kleinster_wert)		// Entscheidung für kürzesten Weg
			{
				kleinster_wert = possible_grid[a][4];
				ghost->next_direction = a;
			}
		}
	}
}

uint8_t check_ghosts(logic_data *data)
{

	// Positionsvergleich für Pacman & Ms.Pacman & Ghost1-4 -> Wenn Geister Pacman fressen wollen
	if (data->ghost_1.graphic_state==filled || data->ghost_2.graphic_state==filled || data->ghost_3.graphic_state==filled || data->ghost_4.graphic_state==filled)
	{
		if ( ((data->pacman.x_pixel == data->ghost_1.x_pixel) && (data->pacman.y_pixel==data->ghost_1.y_pixel && data->ghost_1.graphic_state==filled)) ||
			 ((data->pacman.x_pixel == data->ghost_2.x_pixel) && (data->pacman.y_pixel==data->ghost_2.y_pixel && data->ghost_2.graphic_state==filled)) ||
			 ((data->pacman.x_pixel == data->ghost_3.x_pixel) && (data->pacman.y_pixel==data->ghost_3.y_pixel && data->ghost_3.graphic_state==filled)) ||
			 ((data->pacman.x_pixel == data->ghost_4.x_pixel) && (data->pacman.y_pixel==data->ghost_4.y_pixel && data->ghost_4.graphic_state==filled))    )
		{
			data->pacman.dead=1;
			data->ghost_1.graphic_state=dont_show;
			data->ghost_2.graphic_state=dont_show;
			data->ghost_3.graphic_state=dont_show;
			data->ghost_4.graphic_state=dont_show;

			return 1;		// Pacman tod
		}else if ( ((data->ms_pacman.x_pixel == data->ghost_1.x_pixel) && (data->ms_pacman.y_pixel==data->ghost_1.y_pixel && data->ghost_1.graphic_state==filled)) ||
				   ((data->ms_pacman.x_pixel == data->ghost_2.x_pixel) && (data->ms_pacman.y_pixel==data->ghost_2.y_pixel && data->ghost_2.graphic_state==filled)) ||
				   ((data->ms_pacman.x_pixel == data->ghost_3.x_pixel) && (data->ms_pacman.y_pixel==data->ghost_3.y_pixel && data->ghost_3.graphic_state==filled)) ||
				   ((data->ms_pacman.x_pixel == data->ghost_4.x_pixel) && (data->ms_pacman.y_pixel==data->ghost_4.y_pixel && data->ghost_4.graphic_state==filled))    )
				{
					data->ms_pacman.dead=1;
					data->ghost_1.graphic_state=dont_show;
					data->ghost_2.graphic_state=dont_show;
					data->ghost_3.graphic_state=dont_show;
					data->ghost_4.graphic_state=dont_show;

					return 2;		// Ms.Pacman tod
				}
	}

	// Positionsvergleich für Pacman & Ms.pacman & Ghost 1-4 -> Wenn Pacman/Ms.Pacman Geister fressen können
	if (data->ghost_1.graphic_state==not_filled || data->ghost_2.graphic_state==not_filled || data->ghost_3.graphic_state==not_filled || data->ghost_4.graphic_state==not_filled)
		{
			if ((data->pacman.x_pixel == data->ghost_1.x_pixel) && (data->pacman.y_pixel==data->ghost_1.y_pixel) && data->ghost_1.graphic_state==not_filled)	// Positions und Zustand abfrage
				{
					data->pacman.score+=200;				// Extra Punkte für gefressenen Ghost
					data->ghost_1.graphic_state=eyes;		// Als Augen darstellen
					data->ghost_1.ghost_reverse_help=1; 	// Augen dürfen einmal entgegengesetzte Richtung
					data->ghost_1.logic_mode=eyes_home;		// Logik Modus auf Ziel "vor dem Käfig" umstellen
					data->ghost_1.update_time=1;			// Da Augen, Geschwindigkeit erhöhen, 1ms (so schnell wie möglich, technisch sind ca. 10ms Möglich, vom Senden abhängig)
				}
			if ((data->pacman.x_pixel == data->ghost_2.x_pixel) && (data->pacman.y_pixel==data->ghost_2.y_pixel) && data->ghost_2.graphic_state==not_filled)
				{
					data->pacman.score+=200;
					data->ghost_2.graphic_state=eyes;
					data->ghost_2.ghost_reverse_help=1;
					data->ghost_2.logic_mode=eyes_home;
					data->ghost_2.update_time=1;
				}
			if ((data->pacman.x_pixel == data->ghost_3.x_pixel) && (data->pacman.y_pixel==data->ghost_3.y_pixel) && data->ghost_3.graphic_state==not_filled)
				{
					data->pacman.score+=200;
					data->ghost_3.graphic_state=eyes;
					data->ghost_3.ghost_reverse_help=1;
					data->ghost_3.logic_mode=eyes_home;
					data->ghost_3.update_time=1;
				}
			if ((data->pacman.x_pixel == data->ghost_4.x_pixel) && (data->pacman.y_pixel==data->ghost_4.y_pixel) && data->ghost_4.graphic_state==not_filled)
				{
					data->pacman.score+=200;
					data->ghost_4.graphic_state=eyes;
					data->ghost_4.ghost_reverse_help=1;
					data->ghost_4.logic_mode=eyes_home;
					data->ghost_4.update_time=1;
				}

			if ((data->ms_pacman.x_pixel == data->ghost_1.x_pixel) && (data->ms_pacman.y_pixel==data->ghost_1.y_pixel) && data->ghost_1.graphic_state==not_filled)
				{
					data->ms_pacman.score+=200;
					data->ghost_1.graphic_state=eyes;
					data->ghost_1.ghost_reverse_help=1;
					data->ghost_1.logic_mode=eyes_home;
					data->ghost_1.update_time=1;
				}
			if ((data->ms_pacman.x_pixel == data->ghost_2.x_pixel) && (data->ms_pacman.y_pixel==data->ghost_2.y_pixel) && data->ghost_2.graphic_state==not_filled)
				{
					data->ms_pacman.score+=200;
					data->ghost_2.graphic_state=eyes;
					data->ghost_2.ghost_reverse_help=1;
					data->ghost_2.logic_mode=eyes_home;
					data->ghost_2.update_time=1;
				}
			if ((data->ms_pacman.x_pixel == data->ghost_3.x_pixel) && (data->ms_pacman.y_pixel==data->ghost_3.y_pixel) && data->ghost_3.graphic_state==not_filled)
				{
					data->ms_pacman.score+=200;
					data->ghost_3.graphic_state=eyes;
					data->ghost_3.ghost_reverse_help=1;
					data->ghost_3.logic_mode=eyes_home;
					data->ghost_3.update_time=1;
				}
			if ((data->ms_pacman.x_pixel == data->ghost_4.x_pixel) && (data->ms_pacman.y_pixel==data->ghost_4.y_pixel) && data->ghost_4.graphic_state==not_filled)
				{
					data->ms_pacman.score+=200;
					data->ghost_4.graphic_state=eyes;
					data->ghost_4.ghost_reverse_help=1;
					data->ghost_4.logic_mode=eyes_home;
					data->ghost_4.update_time=1;
				}
		}

	// Positionsvergleich für Ghost 1-4 vorm Käfig -> Wenn Ghost er als Auge dargestellt wird
	if (data->ghost_1.graphic_state==eyes || data->ghost_2.graphic_state==eyes || data->ghost_3.graphic_state==eyes || data->ghost_4.graphic_state==eyes)
	{
		// Augen vor Käfig und wenn es Augen sind (sonst könnte es zufällig aufblinken wenn der Ghost dort ist)
		if ((data->ghost_1.x_pixel == data->ghost_1.cage_outside_x_grid*grid_dim) && (data->ghost_1.y_pixel == (data->ghost_1.cage_outside_y_grid*grid_dim)+grid_center_offset) && data->ghost_1.graphic_state==eyes)
		{
			logic_control.ghost_1.x_grid=logic_control.ghost_1.cage_inside_x_grid;				// Ghost in entsprechende Käfig Position setzen
			logic_control.ghost_1.x_pixel=(logic_control.ghost_1.x_grid*grid_dim);
			logic_control.ghost_1.y_grid=logic_control.ghost_1.cage_inside_y_grid;
			logic_control.ghost_1.y_pixel=(logic_control.ghost_1.y_grid*grid_dim)+grid_center_offset;
			logic_control.ghost_1.graphic_state=filled_flash;		// Ghost wieder ausfüllen und blinken (ohne Funktion bei Ghost 1, da er niemals im Käfig ist)
			logic_control.ghost_1.logic_mode=target_pacman;			// Bewegungslogik zurücksetzen
			logic_control.ghost_1.cage=1;							// Hilfsvarialbe Ghost im Käfig setzen
			logic_control.ghost_1.update_time=20;					// Geschwingdigkeit auf Standard zurücksetzen
			logic_control.ghost_1.ghost_reverse_help=1;
			logic_control.ghost_1.dotcounter_ghost=0;
		}
		if ((data->ghost_2.x_pixel == data->ghost_2.cage_outside_x_grid*grid_dim) && (data->ghost_2.y_pixel == (data->ghost_2.cage_outside_y_grid*grid_dim)+grid_center_offset) && data->ghost_2.graphic_state==eyes)
		{
			logic_control.ghost_2.x_grid=logic_control.ghost_2.cage_inside_x_grid;
			logic_control.ghost_2.x_pixel=(logic_control.ghost_2.x_grid*grid_dim);
			logic_control.ghost_2.y_grid=logic_control.ghost_2.cage_inside_y_grid;
			logic_control.ghost_2.y_pixel=(logic_control.ghost_2.y_grid*grid_dim)+grid_center_offset;
			logic_control.ghost_2.graphic_state=filled_flash;
			if (logic_control.player2_mode==2)
				logic_control.ghost_2.logic_mode=target_ms_pacman;		// Wenn 2Player Ms.Pacman verfolgen, bei 1 Spieler Pacman+4
			else
			{
				
			} logic_control.ghost_2.logic_mode=target_pacman_4;
			logic_control.ghost_2.cage=1;
			logic_control.ghost_2.update_time=20;
			logic_control.ghost_2.ghost_reverse_help=1;
			logic_control.ghost_2.dotcounter_ghost=0;
		}
		if ((data->ghost_3.x_pixel == data->ghost_3.cage_outside_x_grid*grid_dim) && (data->ghost_3.y_pixel == (data->ghost_3.cage_outside_y_grid*grid_dim)+grid_center_offset) && data->ghost_3.graphic_state==eyes)
		{
			logic_control.ghost_3.x_grid=logic_control.ghost_3.cage_inside_x_grid;
			logic_control.ghost_3.x_pixel=(logic_control.ghost_3.x_grid*grid_dim);
			logic_control.ghost_3.y_grid=logic_control.ghost_3.cage_inside_y_grid;
			logic_control.ghost_3.y_pixel=(logic_control.ghost_3.y_grid*grid_dim)+grid_center_offset;
			logic_control.ghost_3.graphic_state=filled_flash;		//
			logic_control.ghost_3.logic_mode=random_direction;
			logic_control.ghost_3.cage=1;
			logic_control.ghost_3.update_time=20;
			logic_control.ghost_3.ghost_reverse_help=1;
			logic_control.ghost_3.dotcounter_ghost=0;
		}
		if ((data->ghost_4.x_pixel == data->ghost_4.cage_outside_x_grid*grid_dim) && (data->ghost_4.y_pixel == (data->ghost_4.cage_outside_y_grid*grid_dim)+grid_center_offset) && data->ghost_4.graphic_state==eyes)
		{
			logic_control.ghost_4.x_grid=logic_control.ghost_4.cage_inside_x_grid;													// start coordinates
			logic_control.ghost_4.x_pixel=(logic_control.ghost_4.x_grid*grid_dim);
			logic_control.ghost_4.y_grid=logic_control.ghost_4.cage_inside_y_grid;
			logic_control.ghost_4.y_pixel=(logic_control.ghost_4.y_grid*grid_dim)+grid_center_offset;
			logic_control.ghost_4.graphic_state=filled_flash;
			logic_control.ghost_4.logic_mode=random_direction;
			logic_control.ghost_4.cage=1;
			logic_control.ghost_4.update_time=20;
			logic_control.ghost_4.ghost_reverse_help=1;
			logic_control.ghost_4.dotcounter_ghost=0;
		}
		return 0;
	}
	else return 0;
}

void ghost_logic_toggle(void)
{
	if (logic_control.ghost_logic_mode_toggle)
	{
		// nur Logikmodi ändern, wenn Geister Pacman/Ms.Pacman jagen
		// Only change logic modes when ghosts are chasing Pacman/Ms. Pacman.
		if (logic_control.ghost_1.graphic_state==filled) logic_control.ghost_1.logic_mode=home;
		if (logic_control.ghost_2.graphic_state==filled) logic_control.ghost_2.logic_mode=home;
		if (logic_control.ghost_3.graphic_state==filled) logic_control.ghost_3.logic_mode=home;
		if (logic_control.ghost_4.graphic_state==filled) logic_control.ghost_4.logic_mode=home;
	}

	if (!logic_control.ghost_logic_mode_toggle) {
		// nur Logikmodi ändern, wenn Geister als Ziel ihre Heimposition erreichen wollen
		if (logic_control.ghost_1.graphic_state==filled)
			logic_control.ghost_1.logic_mode=target_pacman;
		if (logic_control.ghost_2.graphic_state==filled && logic_control.player2_mode==2)
			logic_control.ghost_2.logic_mode=target_ms_pacman;	//Wenn 2 player dann ms.pacman verfolgen
		else
			if (logic_control.ghost_2.graphic_state==filled) logic_control.ghost_2.logic_mode=target_pacman_4;
		if (logic_control.ghost_3.graphic_state==filled) logic_control.ghost_3.logic_mode=random_direction;
		if (logic_control.ghost_4.graphic_state==filled) logic_control.ghost_4.logic_mode=random_direction;
	}
}

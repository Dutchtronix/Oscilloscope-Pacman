/**
 *	\file map_functions.c
 *
 *	\date 14.12.2012
 * 	\author Christian Brommer & Dennis Schuldt
 *
 * Update by JdR (Dutchtronix) to run the graphics processor
 * on the Dutchtronix AVR Oscilloscope Clock. This clock
 * has a D to A converter needed for rendering the graphics
 * on an analog oscilloscope.
 *
 * Issues to deal with:
 *	- new clock speed is 20 MHz
 *	- lots of tables need to be move to code space
 *	- German comments
 *	- option to replace Pacman char generator with Dutchtronix char generator
 *	- must replace Pacman gbham() with Dutchtronix line drawing code
 *	- make coordinates uint8_t type
 *	- Use Dutchtronix Serial driver but Pacman uses 9 bits data
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <inttypes.h>

#include "global.h"
#include "map_functions.h"
#include "map_data.h"
#include "draw_engine.h"
#include "timerG.h"

//
// performance_test is unnecessary since GNU-C generates almost identical code for both cases
//
//#define performance_test	///zum testen von Performance-Unterschieden in verscheidenen Funktionen. Bei definition werden die möglicherweise schnelleren Programmabschnitte compiliert

void draw_map_komponent(uint8_t a)
{
	gbham(
		pgm_read_byte(&outer_top_frame[a][0]),
		pgm_read_byte(&outer_top_frame[a][1]),
		pgm_read_byte(&outer_top_frame[a+1][0]),
		pgm_read_byte(&outer_top_frame[a+1][1])
	);
}

void draw_map_elements(const uint8_t (*data)[2], uint8_t array_size)	// (*data)[2] ist ein Pointer auf ein zweidimensionales Array. Beim übergeben an die Funktion, ist es auf Grund des Pointers unerheblich wie lang das Array ist. In der Funktion wird dann mit data[][] darauf zu gegriffen.
{
#ifndef performance_test

		uint8_t a,b;	// b erstellen damit nicht in jedem Schritt, für "jedes" Array Segment "a+1" gerechnet werden muss, sondern nur einmal pro Schritt

		for(a = 0, b = a+1; a < array_size; a++, b = a+1) // zwei Variablen vorbelegen und in der Ausführung beide Variablen berechnen
		{
			gbham(
				pgm_read_byte(&data[a][0]),
				pgm_read_byte(&data[a][1]),
				pgm_read_byte(&data[b][0]),
				pgm_read_byte(&data[b][1])
			);	//Linie von Punkt a und seinem direkt nachfolgendem Punkt b bzw. a+1 berechnen
		};

#else //nur eine Variable erstellen und dafür zweimal "a+1" in jedem Schleifen-Durchgang berechnen

		for(uint_fast8_t a = 0; a < array_size; a++)
		{
			gbham(
				pgm_read_byte(&data[a][0]),
				pgm_read_byte(&data[a][1]),
				pgm_read_byte(&data[a+1][0]),
				pgm_read_byte(&data[a+1][1])
			);	//Linie von Punkt a und seinem direkt nachfolgendem Punkt a+1
		};

#endif
}

void draw_relative_element(const uint8_t (*data)[2], uint8_t array_size, uint8_t x_position, uint8_t y_position)
{
	//offset Position aus dem ersten Arraysegment lesen:
	uint8_t x_offset = pgm_read_byte(&data[0][0]);
	uint8_t y_offset = pgm_read_byte(&data[0][1]);

	//absolute Position auf dem Bildschirm, differenz für die Datenpunkte
	uint8_t x_diff = x_position - x_offset;
	uint8_t y_diff = y_position - y_offset;


	//bei 1 beginnen da in 0 die Offsetposition steht
#ifndef performance_test

		uint8_t a,b;	// b erstellen damit nicht in jedem Schritt, für "jedes" Array Segment "a+1" gerechnet werden muss, sondern nur einmal pro Schritt

		for(a = 1, b = a+1; a < array_size; a++, b = a+1) // zwei Variablen vorbelegen und in der Ausführung beide Variablen berechnen
		{
				gbham(
					(x_diff + pgm_read_byte(&data[a][0])),
					(y_diff + pgm_read_byte(&data[a][1])),
					(x_diff + pgm_read_byte(&data[b][0])),
					(y_diff + pgm_read_byte(&data[b][1]))
				); //Linie von Punkt a und seinem direkt nachfolgendem Punkt b bzw. a+1 berechnen

		};

#else //nur eine Variable erstellen und dafür zweimal "a+1" in jedem Schleifen-Durchgang berechnen

		for(uint_fast8_t a = 1; a < array_size; a++)
		{
			gbham(
				(x_diff + pgm_read_byte(&data[a][0])),
				(y_diff + pgm_read_byte(&data[a][1])),
				(x_diff + pgm_read_byte(&data[a+1][0])),
				(y_diff + pgm_read_byte(&data[a+1][1]))
			); //Linie von Punkt a und seinem direkt nachfolgendem Punkt a+1

		};

#endif
}

void draw_relative_element_s(uint8_t (*data)[2], uint8_t array_size, uint8_t x_position, uint8_t y_position)
{
	//offset Position from dem ersten Arraysegment lesen:
	//absolute Position auf dem Bildschirm, differenz für die Datenpunkte
	uint8_t x_diff = x_position - data[0][0];
	uint8_t y_diff = y_position - data[0][1];


	//bei 1 beginnen da in 0 die Offsetposition steht

	uint8_t a;

	for(a = 1; a < array_size; a++) {
		 //Linie von Punkt a und seinem direkt nachfolgendem Punkt b bzw. a+1 berechnen
		gbham((x_diff + data[a][0]), (y_diff + data[a][1]), (x_diff + data[a+1][0]), (y_diff + data[a+1][1]));
	};
}

void draw_map(void)
{
	//Array-Größe ist nicht gleich größte Array-Position, Array mit 2 Elementen -> [0] und [1] also im Maximum eins weniger als die angegebene Größe

	draw_map_elements(outer_top_frame, 17);
	draw_map_elements(inner_top_frame, 25);
	draw_map_elements(outer_bottom_frame, 17);
	draw_map_elements(inner_bottom_frame, 33);

	draw_map_elements(cage,12);

	draw_map_elements(cage_gate,1);


	draw_map_elements(barrier_1,8);
	draw_map_elements(barrier_2,8);
	draw_map_elements(barrier_3,8);
	draw_map_elements(barrier_4,8);
	draw_map_elements(barrier_5,8);

	draw_map_elements(barrier_6,16);
	draw_map_elements(barrier_7,16);
	draw_map_elements(barrier_8,16);

	draw_map_elements(barrier_9,8);
	draw_map_elements(barrier_10,8);

	draw_map_elements(barrier_11,16);

	draw_map_elements(barrier_12,8);

	draw_map_elements(barrier_13,12);

	draw_map_elements(barrier_14,8);

	draw_map_elements(barrier_15,8);

	draw_map_elements(barrier_16,12);

	draw_map_elements(barrier_17,16);
	draw_map_elements(barrier_18,16);
	draw_map_elements(barrier_19,16);
}

void draw_pacman_logo(void)
{
	draw_relative_element(relative_start_P, 109, 20, 150);
	draw_relative_element(relative_start_A, 114, 50, 150);
	draw_relative_element(relative_start_C, 79, 82, 150);

	draw_relative_element(relative_start_M, 189, 130, 150);
	draw_relative_element(relative_start_A, 114, 188, 150);
	draw_relative_element(relative_start_N, 141, 225, 150);
}


void place_pacman(uint8_t x_position, uint8_t y_position, uint8_t direction, uint8_t mouth)
{
	switch (direction) {

		case 0:	//neutral
			draw_relative_element(relative_pacman_open_HL, 24, x_position, y_position);
			break;

		case 1:	//right
			if(mouth)
				draw_relative_element(relative_pacman_open_HR, 24, x_position, y_position);
			else
				draw_relative_element(relative_pacman_close_HR, 24, x_position, y_position);
			break;

		case 2:	//down
			if(mouth)
				draw_relative_element(relative_pacman_open_VD, 24, x_position, y_position);
			else
				draw_relative_element(relative_pacman_close_VD, 24, x_position, y_position);
			break;

		case 3:	//left
			if(mouth)
				draw_relative_element(relative_pacman_open_HL, 24, x_position, y_position);
			else
				draw_relative_element(relative_pacman_close_HL, 24, x_position, y_position);
			break;

		case 4:	//up
			if(mouth)
				draw_relative_element(relative_pacman_open_VU, 24, x_position, y_position);
			else
				draw_relative_element(relative_pacman_close_VU, 24, x_position, y_position);
			break;

		default:	//bei anderen Werten, wird kein PacMan angezeigt
			break;
	}
}

void place_ms_pacman(uint8_t x_position, uint8_t y_position, uint8_t direction, uint8_t mouth)
{
	switch (direction) {
		case 0:	//neutral
			draw_relative_element(relative_ms_pacman_open_HL, 36, x_position, y_position);
			break;

		case 1:	//right
			if(mouth)
				draw_relative_element(relative_ms_pacman_open_HR, 36, x_position, y_position);
			else
				draw_relative_element(relative_ms_pacman_close_HR, 34, x_position, y_position);
			break;

		case 2:	//down
			if(mouth)
				draw_relative_element(relative_ms_pacman_open_VD, 36, x_position, y_position);
			else
				draw_relative_element(relative_ms_pacman_close_VD, 34, x_position, y_position);
			break;

		case 3:	//left
			if(mouth)
				draw_relative_element(relative_ms_pacman_open_HL, 36, x_position, y_position);
			else
				draw_relative_element(relative_ms_pacman_close_HL, 34, x_position, y_position);
			break;

		case 4:	//up
			if(mouth)
				draw_relative_element(relative_ms_pacman_open_VU, 36, x_position, y_position);
			else
				draw_relative_element(relative_ms_pacman_close_VU, 34, x_position, y_position);
			break;

		default:	//bei anderen Werten, wird kein PacMan angezeigt
			break;
	}
}

void place_ghost(uint8_t x_position, uint8_t y_position, uint8_t fill)
{
	if(fill)
		draw_relative_element(relative_ghost_2, 44, x_position, y_position);
	else
		draw_relative_element(relative_ghost_1, 32, x_position, y_position);
}

void place_eyes(uint8_t x_position, uint8_t y_position)
{
		draw_relative_element(relative_eyes, 23, x_position, y_position);

}

void place_fruit(uint8_t x_position, uint8_t y_position, uint8_t fruit)
{
	switch (fruit) {
		case 1:	//cherry
			draw_relative_element(relative_cherry, 18, x_position, y_position);
			break;

		case 2:	//banana
			draw_relative_element(relative_banana, 27, x_position, y_position);
			break;

		case 3:	//pineapple
			draw_relative_element(relative_pineapple, 15, x_position, y_position);
			break;

		case 4:	//apple
			draw_relative_element(relative_apple, 21, x_position, y_position);
			break;

		case 5:	//strawberry
			draw_relative_element(relative_strawberry, 36, x_position, y_position);
			break;

		default: //cherry
			draw_relative_element(relative_cherry, 18, x_position, y_position);
			break;
	}

}

uint8_t destroy_pacman(uint8_t x_position, uint8_t y_position)
{
	// pacman_destroy.value wird im Timer runtergezählt

	if(pacman_destroy.value>24)		//Werte die größer sind als 24, auf 24 begrenzen
		pacman_destroy.value = 24;

	// da pacman_destroy.value vom Timer runtergezählt wird, löst sich der PacMan mir jedem Schritt weiter auf
	draw_relative_element(relative_pacman_close_HL, pacman_destroy.value, x_position, y_position);

	if(pacman_destroy.value == 0) {	//Ist der PacMan aufgelöst, gibt die Funktion eine 1 zurück
		pacman_destroy.value = 24;
		return 1;
	}
	else	//Ist der PacMan noch nicht aufgelöst, gibt die Funktion eine 0 zurück
		return 0;
}

uint8_t destroy_ms_pacman(uint8_t x_position, uint8_t y_position)
{
	// pacman_destroy.value wird im Timer runtergezählt

	if(ms_pacman_destroy.value>34)		//Werte die größer sind als 24, auf 24 begrenzen
		ms_pacman_destroy.value = 34;

	// da pacman_destroy.value vom Timer runtergezählt wird, löst sich der PacMan mir jedem Schritt weiter auf
	draw_relative_element(relative_ms_pacman_close_HL, ms_pacman_destroy.value, x_position, y_position);

	if(ms_pacman_destroy.value == 0) {	//Ist der PacMan aufgelöst, gibt die Funktion eine 1 zurück
		ms_pacman_destroy.value = 34;
		return 1;
	}
	else	//Ist der PacMan noch nicht aufgelöst, gibt die Funktion eine 0 zurück
		return 0;
}

void place_dot(uint8_t dot_number, uint8_t choice)
{
	if(choice)
		draw_relative_element_s(relative_small_dot, 5, dot_coordinates[dot_number][0], dot_coordinates[dot_number][1]);
	else
		draw_relative_element_s(relative_big_dot, 14, dot_coordinates[dot_number][0],  dot_coordinates[dot_number][1]);

}

void place_small_dot(uint8_t dot_number)
{
	draw_relative_element_s(relative_small_dot, 5, dot_coordinates[dot_number][0],  dot_coordinates[dot_number][1]);
}


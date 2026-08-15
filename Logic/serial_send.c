/**
 *  \file serial_send.c
 *
 *  \date: 14.12.2012
 *  \author: Christian Brommer & Dennis Schuldt
 */

#include "serial_send.h"
#include "global.h"
#include "serial.h"
#include "character_logic.h"

#include "string.h"
#include "stdlib.h"

/* alle seriellen Sendebefehle sind hier vorhanden. Ausnahme bildet nur der Case "Scores" in der main switch-Anweisung
 * sowie die Funktionen display_1player_game() und display_2player_game()*/

void send_graphics_init(void)
{
	// send graphic init
	serial_send(game_state, 0);
	serial_send(game_state, 1);

	serial_send(new_highscore_state, 0);
	serial_send(new_highscore_state, 1);

	serial_send(result, 0);
	serial_send(result, 1);

	serial_send(dots, 0);
	serial_send(dots, 1);

	ltoa(logic_control.highscore, (char *)string_highscore, 10);
	strcat((char *)string_highscore, "\n");
	serial_send(highscore, 0);

	ltoa(logic_control.highscore, (char *)string_highscore, 10);
	strcat((char *)string_highscore, "\n");
	serial_send(highscore, 1);
}

void send_logic_data(void)
{
	// send gameplay data
	serial_send(level, 0);
	serial_send(level, 1);

	serial_send(lives, 0);
	serial_send(lives, 1);

	serial_send(game_state, 0);
	if (logic_control.player2_mode==2) serial_send(game_state, 1);	// damit 2tes Oszilloskop im 1 Spieler Modus bei "insert coin" hängen bleibt

	logic_control.player_mode=logic_control.player1_mode;
	serial_send(player_mode, 0);
	logic_control.player_mode=logic_control.player2_mode;
	serial_send(player_mode, 1);
}

void send_to_graphics1(void)
{
	// prepare to send
	// Punkte
	send_dot_segment(logic_control.pacman.global_dots, logic_control.dots[logic_control.pacman.global_dots], 0);
#if TWOPLAYERMODE
	send_dot_segment(logic_control.ms_pacman.global_dots, logic_control.dots[logic_control.ms_pacman.global_dots], 0);
#endif

	// character position data
	serial_send(pacman_x_position, 0);
	serial_send(pacman_y_position, 0);
	serial_send(pacman_direction, 0);
	serial_send(pacman_mouth_lock, 0);
	serial_send(pacman_dead,0);

#if TWOPLAYERMODE
	serial_send(ms_pacman_x_position, 0);
	serial_send(ms_pacman_y_position, 0);
	serial_send(ms_pacman_direction, 0);
	serial_send(ms_pacman_mouth_lock, 0);
	serial_send(ms_pacman_dead,0);
#endif

	// ghost data
	serial_send(ghost_1_x_position, 0);
	serial_send(ghost_1_y_position, 0);
	serial_send(ghost_1_state, 0);

	serial_send(ghost_2_x_position, 0);
	serial_send(ghost_2_y_position, 0);
	serial_send(ghost_2_state, 0);

	serial_send(ghost_3_x_position, 0);
	serial_send(ghost_3_y_position, 0);
	serial_send(ghost_3_state, 0);

	serial_send(ghost_4_x_position, 0);
	serial_send(ghost_4_y_position, 0);
	serial_send(ghost_4_state, 0);

	// character score
	if (logic_control.pacman.score != logic_control.pacman.score_old)	// nur schicken, wenn sich der Score geändert hat
	{
		ltoa(logic_control.pacman.score, (char *)string_pacman_score, 10);
		strcat((char *)string_pacman_score, "\n");
		serial_send(pacman_score, 0);
	}
	if (logic_control.pacman.score > logic_control.pacman.score_old) logic_control.pacman.score_old=logic_control.pacman.score;	// Score Änderung überprüfen

	// fruits data
	serial_send(cherry_show_in_map, 0);
	serial_send(strawberry_show_in_map, 0);
	serial_send(apple_show_in_map, 0);
	serial_send(pineapple_show_in_map, 0);
	serial_send(banana_show_in_map, 0);
}

#if TWOPLAYERMODE
void send_to_graphics2(void)
{
	// prepare to send
	// Punkte
	send_dot_segment(logic_control.pacman.global_dots, logic_control.dots[logic_control.pacman.global_dots], 1);
	send_dot_segment(logic_control.ms_pacman.global_dots, logic_control.dots[logic_control.ms_pacman.global_dots], 1);

	// character position data
	serial_send(pacman_x_position, 1);
	serial_send(pacman_y_position, 1);

	serial_send(pacman_direction, 1);
	serial_send(pacman_mouth_lock, 1);

	serial_send(pacman_dead,1);

	serial_send(ms_pacman_x_position, 1);
	serial_send(ms_pacman_y_position, 1);

	serial_send(ms_pacman_direction, 1);
	serial_send(ms_pacman_mouth_lock, 1);

	serial_send(ms_pacman_dead,1);

	// ghost data
	serial_send(ghost_1_x_position, 1);
	serial_send(ghost_1_y_position, 1);
	serial_send(ghost_1_state, 1);

	serial_send(ghost_2_x_position, 1);
	serial_send(ghost_2_y_position, 1);
	serial_send(ghost_2_state, 1);

	serial_send(ghost_3_x_position, 1);
	serial_send(ghost_3_y_position, 1);
	serial_send(ghost_3_state, 1);

	serial_send(ghost_4_x_position, 1);
	serial_send(ghost_4_y_position, 1);
	serial_send(ghost_4_state, 1);

	// character score
	if (logic_control.ms_pacman.score != logic_control.ms_pacman.score_old)	// nur schicken, wenn sich der Score geändert hat
	{
		ltoa(logic_control.ms_pacman.score, (char *)string_ms_pacman_score, 10);
		strcat((char *)string_ms_pacman_score, "\n");
		serial_send(ms_pacman_score, 1);
	}
	if (logic_control.ms_pacman.score > logic_control.ms_pacman.score_old) logic_control.ms_pacman.score_old=logic_control.ms_pacman.score; // Score Änderung überprüfen

	// fruits data
	serial_send(cherry_show_in_map, 1);
	serial_send(strawberry_show_in_map, 1);
	serial_send(apple_show_in_map, 1);
	serial_send(pineapple_show_in_map, 1);
	serial_send(banana_show_in_map, 1);
}
#endif

void send_all_dots()
{
	// alle Punkte an beide Spieler senden
	serial_send(dots, 0);
	serial_send(dots, 1);
}

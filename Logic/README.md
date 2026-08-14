This is the code running on the Logic AVR.

This code sends encoded graphics commands to the "graphics" AVR to render the game and provides the joystick interface to play the game.

Two joysticks are theoretically supported though this repository only supports one joystick.

This project was built using Atmel Studio 7. You may be able to use the provided PacmanLogic.cproj directly or
just add all C source files to a project, set the microcontroller to ATmega1284p and build the project.

The analog Joystick is connected to analog input PA0 (40) for Y input, analog input PA1 (39) for X input
and PA4 (36) for the control button. The Joystick will also need +5V and GND from the logic AVR.

Connect the Serial out pin of the logic AVR to the Serial in pin of the graphics AVR directly (TTL levels)
as well as GND.

To run Pacman, connect the graphics AVR to an XY display, intensity control is optional but recommended,
and start it. The graphics AVR will be waiting for serial data from this logic AVR.

This is the code running on the Logic AVR.

This code sends encoded graphics commands to the "graphics" AVR to render the game and provides the joystick interface to play the game.

Two joysticks are theoretically supported though this repository only supports one joystick.

This project was built using Atmel Studio 7. You may be able to use the provided PacmanLogic.cproj directly or
just add all C source files to a project, set the microcontroller to ATmega1284p and build the project.

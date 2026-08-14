This is the code running on the Dutchtronix AVR Oscilloscope Clock acting as the Graphics AVR for Oscilloscope Pacman.

This code accepts encoded graphics commands from the logic AVR and renders them on an XY monitor using X and Y BNC cables or probes.
Intensity control (Z) is provided but optional.

This project was built using Atmel Studio 7. You may be able to use the provided graphics.cproj directly or just add all C source files to a project, set the microcontroller to ATmega328p and build the project.

Connect the Serial out pin of the logic AVR to the Serial in pin of the graphics AVR directly (TTL levels) as well as GND.

To run Pacman, connect the graphics AVR to an XY display, intensity control is optional but recommended, and start it. The graphics AVR will be waiting for serial data from the logic AVR.

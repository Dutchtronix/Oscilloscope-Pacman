# Oscilloscope-Pacman

This is a port of the Oscilloscope Pacman program created by Christian Brommer and Dennis Schuldt. This port has been tested for 1 player only. The program runs on 2 separate AVRs, one AVR does all the logic and joystick controls, the Dutchtronix AVR Oscilloscope clock hardware renders the graphics.

The subdirectory "logic" runs on a standard ATmega1284P microcontroller but it will also work on smaller ATmegas since the code and ram requirements are minimal. In 2 player mode, the AVR will need 2 serial ports. 

The subdirectory "graphics" is tuned for the Dutchtronix AVR Oscilloscope clock hardware, specifically the ATmega328p microcontroller and the AD7302 DA converter.

Both AVRs run at 20 MHz.

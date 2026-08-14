/**
 *	\file text.c
 *
 * replacment of file text-generator.c by JdR (Dutchtronix) to run the graphics processor
 * on the Dutchtronix AVR Oscilloscope Clock. This clock has a D to A converter needed
 * for rendering the graphics on an analog oscilloscope.
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <inttypes.h>

#include "global.h"
#include "text_generator.h"
#include "draw_engine.h"

void CDrawTextStringfromFlash(const char crt[]);
void CDrawTextStringfromSRam(char crt[]);

void print_gbham(char crt[], uint8_t x_position, uint8_t y_position)
{
	_r9_ = x_position;						//XOffsetr9
	_r10_ = y_position;						//YOffsetr10
	CDrawTextStringfromSRam(crt);
	_r9_ = _r10_ = 0;
}
#include "font8x5.inc"


void print_gbham_F(const char crt[], uint8_t x_position, uint8_t y_position)
{
	_r9_ = x_position;						//XOffsetr9
	_r10_ = y_position;						//YOffsetr10
	CDrawTextStringfromFlash(crt);
	_r9_ = _r10_ = 0;
}


uint8_t XOffset, YOffset, GXOffset, GYOffset;
uint8_t LastChanA, LastChanB;

/**
 * \brief Signum Funktion
 *
 * \param x Eingabewert
 *
 * \return berechneter Signum Wert
*/
int sgn(int16_t x)
{
  return (x > 0) ? 1 : (x < 0) ? -1 : 0;
}

//
// use Dutchtronix clock DA converter code
//
void CDrawVector(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

void gbham(uint8_t xstart,uint8_t ystart,uint8_t xend,uint8_t yend)
{
	CDrawVector(xstart, ystart, xend, yend);
}
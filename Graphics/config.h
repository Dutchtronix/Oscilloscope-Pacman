#ifndef CONFIG_H_
#define CONFIG_H_

#define F_CPU 20000000
#define DUTCHTRONIX	1
#define PACMAN 1

#define MAPGOFFSETTOREG 1
#define TESTREGS 0

#if defined( __ASSEMBLER__ )
#if MAPGOFFSETTOREG
#define	GXOffset	r7
#define	GYOffset	r8
#define	XOffsetr9	r9
#define	YOffsetr10	r10
#endif
#define	__zero_reg_ r1
//
//-- map the IO register back into the IO address space
//
#define PA_DDR		(_SFR_IO_ADDR(PORTA) - 1)
#define PB_DDR		(_SFR_IO_ADDR(PORTB) - 1)
#define PC_DDR		(_SFR_IO_ADDR(PORTC) - 1)
#define PD_DDR		(_SFR_IO_ADDR(PORTD) - 1)
#define PA_OUT		_SFR_IO_ADDR(PORTA)
#define PB_OUT		_SFR_IO_ADDR(PORTB)
#define PC_OUT		_SFR_IO_ADDR(PORTC)
#define PD_OUT		_SFR_IO_ADDR(PORTD)
#define PA_IN		(_SFR_IO_ADDR(PORTA) - 2)
#define PB_IN		(_SFR_IO_ADDR(PORTB) - 2)
#define PC_IN		(_SFR_IO_ADDR(PORTC) - 2)
#define PD_IN		(_SFR_IO_ADDR(PORTD) - 2)

#ifndef _cbi
	#define _cbi(reg,bit)	andi reg,~(_BV(bit))
#endif
#ifndef _sbi
	#define _sbi(reg,bit)	ori reg,_BV(bit)
#endif

#else
#if !TESTREGS
// Registers r2..r6 are used in the render functions.
    register unsigned char _r2_ asm("r2"); 
    register unsigned char _r3_ asm("r3"); 
    register unsigned char _r4_ asm("r4"); 
    register unsigned char _r5_ asm("r5"); 
    register unsigned char _r6_ asm("r6");
#endif

#if MAPGOFFSETTOREG
	//
	// GXOffset and GYOffset can easily be ram variables
	// but that results in more code and slower access
	//
    register unsigned char GXOffset asm("r7"); 
    register unsigned char GYOffset asm("r8");
	//
	// Warning: using r10 as a register variable with Winavr is not safe.
	// For certain math operations (e.g. long long multiply) r10 is used as
	// an argument register, but not saved because it is a register variable.
	//
	// In general, register variables are not safe in ISRs when using
	// library functions. We only reserve these registers so we don't
	// have to save them, ever. r7/r8, also unsafe in ISRs, are not used there.
	// Update XOffset and YOffset now mapped to r9 and r10
	//
    register unsigned char _r9_ asm("r9"); 
    register unsigned char _r10_ asm("r10"); 
#endif

#ifndef cbi
#define cbi(reg,bit)	reg &= ~(_BV(bit))
#endif
#ifndef sbi
#define sbi(reg,bit)	reg |= (_BV(bit))
#endif

#endif

#define MajorVersion 4
#define MinorVersion 4

#define HARDWARE31 1			//Enhanced Oscilloscope Clock
#define HARDWARE324 0			//Atmega324P/644 based Oscilloscope Clock with MCP4802

#define TWOSWPORTS 0			//Only for latest Atmega644 design

#define SPLASHSCREEN 0			//normally not enough space
#define USETIMER2 0				//Enable if needed

#define DEBUG 1
#define EXTDEBUG 0
#define	PIXELDELAY 0
#define MILLIS (PS2KBD)

#if HARDWARE31
#define Mega328p 1
#define Mega324p 0
#define MCP4802 0
#define AD7302 1
#define	ZINVERTED 0
#define PS2KBD 0

#elif HARDWARE324

#undef F_CPU
#define F_CPU 32000000

#undef TWOSWPORTS
#define TWOSWPORTS 1

#define Mega328p 0
#define Mega324p 1
#define MCP4802 1
#define AD7302 0
#define	ZINVERTED 0
#define PS2KBD 0
#else
#message UNSUPPORTED
#endif
//
// AD7302 parallel DAC:
// Mega328p:
//	8 bit parallel data (7..0) is mapped to PC3..PC0 PB3..PB0
//	DACA/DACB select is mapped to PD3 (low is A)
//	WR (active low) is mapped to PD4
//	LED (active high) or NET input is mapped to PD6
//	LDAC is mapped to PD5
//	Z control is mapped to PD7
//	Tactile Switch 1 is mapped to PB4
//	Tactile Switch 2 is mapped to PB5
//
// Mega324p:
//	8 bit parallel data (7..0) is mapped to PA7..PA0
//	PD0..PD1 is Serial, PD2 is Clock Interrupt (hardware)
//	DACA/DACB select is mapped to PD3 (low is A)
//	WR (active low) is mapped to PD4
//	LDAC is mapped to PD5
//	LED (active high) or NET input is mapped to PD6
//	Z control is mapped to PD7
//	Free: PB0..PB3, PB6..PB7
//	Tactile Switch 1 is mapped to PB4
//	Tactile Switch 2 is mapped to PB5
//	hardware: PC0,PC1 is I2C. PC2..PC5 is JTAG, 
//	PS2 keyboard support: PC6 and PC7
//  TODO: compute average number of pixels set per screen.
//

#define	LEDDDR DDRD
#define	LEDPort PORTD
#define	LEDPin PIND
#define	LEDBit 6

#define DACCTLPort PORTD
#define DACCTL PD_OUT
#define DACCTLDDR PD_DDR
#define PPSPort PORTD
#define DACSELBit 3
#define PPSBit	2
#define DACWRBit 4
#define DACWR PD_OUT,DACWRBit
#define DACSELECT	PD_OUT,DACSELBit

#if PS2KBD
#define PS2PORT PORTB
#define PS2PORTIN PINB
#define PS2DATABIT 4
#define PS2CLKBIT 3
#define PS2DDR DDRB
#endif

#if AD7302

#define DACHIDATA PC_OUT
#define DACLODATA PB_OUT
#define	DACHIDDR  PC_DDR
#define	DACLODDR  PB_DDR

#define DACLDACBit	5
#define DACLDAC PD_OUT,DACLDACBit
#define DACZXBit	7
#define DACZX PD_OUT,DACZXBit
#define	DACZXDDR  PD_DDR,7
#define ZX_DDR_PORT DDRD
#define ZX_PORT_BIT	7
//
// use PB4 and PB5 as input lines
//
#define SWDDR	DDRB
#define SWPort	PORTB
#define SWPin 	PINB
#define SW1Bit	4
#define SW2Bit	5

#else
ERROR
#endif
//
// Constants
//
#define	SRAM_START	0x100

#ifndef NULL
#define	NULL  0
#endif
#define	TRUE	1
#define	FALSE	0
#define	BS		8
#define	TAB		9
#define	CR		13
#define	LF		10

#define FIRST_CHAR 32
#define	CHAR_WIDTH 6
#define CHAR_HEIGHT 8

//
// Timer computations
//
#if F_CPU==20000000
//
// timer0 set for  1.024 mSec.
//
// the prescaler is set so that timer0 ticks every 256 clock cycles, and
// the match handler is called every Timer0CountUp ticks.
//
#define Timer0Clock (F_CPU/256)
#define Timer0CountUp (1024UL * Timer0Clock / 1000000UL)
#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )
#define MICROSECONDS_PER_TIMER0_MATCH (clockCyclesToMicroseconds(256 * Timer0CountUp))
#endif

#if 0
//
// timer1 set for 16.666 mSec (60 Hz)
//
#define Timer1Clock (F_CPU/256)
#define Timer1CountUp (16666UL * Timer1Clock / 1000000UL)
//
// timer2 set for 4.992 mSec
//
#define Timer2Clock (F_CPU/1024)
#define Timer2CountUp (4992UL * Timer2Clock / 1000000UL)
#endif

#endif

#if MILLIS
//
// Timer1 also used for millis.
//
#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )
//
// the prescaler is set so that timer0 ticks every 256 clock cycles, and the
// the match handler is called every Timer1CountUp ticks.
#define MICROSECONDS_PER_TIMER1_MATCH (clockCyclesToMicroseconds(256 * Timer1CountUp))
//
// the whole number of milliseconds per timer1 overflow
//
#define MILLIS_INC (MICROSECONDS_PER_TIMER1_MATCH / 1000)
//
// the fractional number of milliseconds per timer1 overflow.
//
#define FRACT_INC (MICROSECONDS_PER_TIMER1_MATCH % 1000)
#define FRACT_MAX 1000 >> 3
#endif		//MILLIS

#if 0		//later
#if F_CPU == 16000000
//
// 16 Mhz. 62.5 nSec per cycle
//
#if	(Timer1CountUp != 1041) || (Timer0CountUp != 102) || (Timer2CountUp != 78)
# error Incorrect Timer computation
#endif
#elif F_CPU == 20000000
//
// 20 Mhz. 50 nSec per cycle
//
#if (Timer1CountUp != 1302) || (Timer0CountUp  != 19) || (Timer2CountUp  !=97)
# error Incorrect Timer computation
#endif
#elif F_CPU == 32000000
//
// 32 Mhz. 31.25 nSec per cycle
//
#if (Timer1CountUp != 2083) || (Timer0CountUp  != 204) || (Timer2CountUp  != 156)
# error Incorrect Timer computation
#endif
#else
//UNSUPPORTED
#endif
#endif
//
// GPIOR0 bits
//
#define		fRcvdChar	0
#define		fDebouncing	1
#define		fUpdSecond	2
#define		fUPS1		3
#define		fTimeoutS1	4
#define		fUPS2		5
#define		fTimeoutS2	6
#define		fFLASH		7

#define	DEFCHARWIDTH		12
#define MENUSPACINGWIDTH	6
#define	HALFSPACECHARWIDTH	4
#define	COMMACHARWIDTH		6
#define	COLONCHARWIDTH		8
#define	DASHCHARWIDTH		10
#if PACMAN
#define LOWERACHARWIDTH		12
#define LOWERICHARWIDTH		6
#define LOWERNCHARWIDTH		10
#endif
#define	MINNUMERICH			6
#define	MINNUMERICV			8
#define	NUMERICVPOS			(81 - MINNUMERICV)
#define	TERMNUMERICVPOS		0

#define	ScanTblStrFlash (ScanTbl + stStrFlash * 4)
#define	ScanTblStrRam (ScanTbl + stStrSRam * 4)
#define	ScanTblNumBuf (ScanTbl + stNumBuf * 4)

#if ZINVERTED
#define BEAMON	cbi DACZX				;low implies no Z-axis control
#define BEAMOFF	sbi	DACZX				;high means decreased intensity
#else
#define BEAMON	sbi DACZX
#define BEAMOFF	cbi	DACZX
#endif

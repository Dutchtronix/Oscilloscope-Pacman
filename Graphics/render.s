; ClkRender.s -- source for Dutchtronix Oscilloscope Clock
;
;  Copyright © 2010-2026 JdR
;
;  All Rights Reserved
;
;  This file is part of the Dutchtronix Oscilloscope Clock Distribution.
;  Use, modification, and re-distribution is permitted subject to the
;  terms in the file named "LICENSE.TXT", which contains the full text
;  of the legal notices and should always accompany this Distribution.
;
;  This software is provided "AS IS" with NO WARRANTY OF ANY KIND.
;
;  This notice (including the copyright and warranty disclaimer)
;  must be included in all copies or derivations of this software.
;
; reminder: AVR GCC Call used registers: r18..r27, r30..r31 (Z)
;					Called saved registers: r2..r17, r28..r29 (Y)
;
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/portpins.h>

#include "./avr.inc"
#include "./config.h"

	.text
	.global	SetMem
	.global	DACinit
	.global	CDACXYPos
	.global	CDrawVector
	.global	CDrawTextStringfromFlash
	.global	CDrawTextStringfromSRam
	.global	TurnBeamOff
	.global RestoreBeam
	.global bintobcd
#if SPLASHSCREEN
	.global SplashRefresh
#endif
	.balign 2

SetMem:
#if DEBUG
	ser		r18
	ldiw	Z,SRAM_START			;Preset RAM
	in		r19,(_SFR_IO_ADDR(SPL))
	in		r20,(_SFR_IO_ADDR(SPH))
1:
	st		Z+,r18
	cp		ZL,r19
	cpc		ZH,r20
	brne	1b
#else
	// the C startup code clears .bss
#endif
	ret

DACinit:
#if MAPGOFFSETTOREG
	clr		XOffsetr9				;Scan Table offset
	clr		YOffsetr10
	clr		GXOffset
	clr		GYOffset
#else
	sts		XOffset,__zero_reg_		;Scan Table offset
	sts		YOffset,__zero_reg_
	sts		GXOffset,__zero_reg_
	sts		GYOffset,__zero_reg_
#endif
	sts		LastChanA, __zero_reg_
	sts		LastChanB, __zero_reg_

;
; Upper 4 bits can be safely ignored here.
; PB4, PB5 are used for push buttons; set as input ports
; PB6, PB7 are unconnected (XTAL)
;  
	in		r24,DACLODDR
	ori		r24,0b00001111
	out		DACLODDR,r24
;
; Upper 4 bits can be safely ignored here.
; PC4, PC5 are used for I2C access to the RTC but are set as input ports otherwise
; PC6, PC7 are unconnected/non-existant
;
	in		r24,DACHIDDR
	ori		r24,0b00001111
	out		DACHIDDR,r24

	in		r24,DACCTLDDR
	ldi		r25,(_BV(DACSELBit)|_BV(DACWRBit)|_BV(DACLDACBit))
	or		r24,r25
	out		DACCTLDDR,r24
	// r25 was loaded above
	out		DACCTL,r25
	sbi		DACZXDDR		;Z-Axis control output
	BEAMOFF
	cbi		_SFR_IO_ADDR(GPIOR0),0		;remember beam state
	ret
;
; Turn Beam off when processing an interrupt
; Keep current state in GPIOR0
;
TurnBeamOff:
	BEAMOFF
	ret
RestoreBeam:
	sbic _SFR_IO_ADDR(GPIOR0),0
	BEAMON
	ret
;
; CDACXYPos(A (r24), B (r22))
;
CDACXYPos:
	BEAMON
	sbi		_SFR_IO_ADDR(GPIOR0),0		;remember beam state
	mov		r23,r22

#define	X0r24				r24
#define	Y0r23				r23
#define	X1r18				r18
#define	Y1r19				r19
;
; DACXYPos		Channel A position in X0r24, Channel B position in Y0r23
;				XOffsetr9, YOffsetr10 preloaded
;
; TODO			Supress X0 or Y0 if unchanged.
;				preload DACCTL values
; USED:			r22, r0.
;
; Must preserve: r20,r21
;
DACXYPos:
;
; X first (DACA)
;
	mov		r22,X0r24				;preserve X0r24
#if INVERT
	neg		r22						;map X origin
#endif
#if MAPGOFFSETTOREG
	add		r22,XOffsetr9			;Scan Table offset
#else
	lds		r0,XOffset				;Scan Table offset
	add		r22,r0
#endif
#if 1
;	compare with LastChanA, update LastChanA
	lds		r0,LastChanA
	cp		r22,r0
	breq	SkipChanA
	sts		LastChanA,r22
#endif
	cbi		DACSELECT				;select DACA (active low)
	out		DACLODATA,r22			;High nibble of DACLODATA modified here: undo Button pull-up resistors.
	swap	r22
	out		DACHIDATA,r22			;no need to preserve High nibble of DACHIDATA
	cbi		DACWR					;DACCTL,DACWRBit			;latch DACA data
	sbi		DACWR					;DACCTL,DACWRBit			;hold
SkipChanA:
;
; Y second (DACB)
;
	mov		r22,Y0r23				;preserve Y0r23 by using r22
#if INVERT
	neg		r22						;map Y origin
#endif
#if MAPGOFFSETTOREG
	add		r22,YOffsetr10			;Scan Table offset
#else
	lds		r0,YOffset				;Scan Table offset
	add		r22,r0
#endif
#if 0
;	compare with LastChanB, update LastChanB. Problem is LDAC if a channel is skipped
	lds		r0,LastChanB
	cp		r22,r0
	breq	1f						;SkipChanB
	sts		LastChanB,r22
#endif
	sbi		DACSELECT				;select DACB (active high)
	out		DACLODATA,r22			;High nibble of DACLODATA modified here: undo Button pull-up resistors.
	swap	r22
	out		DACHIDATA,r22			;no need to preserve High nibble of DACHIDATA
	cbi		DACWR					;DACCTL,DACWRBit			;latch DACB data
	sbi		DACWR					;DACCTL,DACWRBit			;hold
;SkipChanB:
1:
	cbi		DACLDAC					;DACCTL,DACLDACBit		;update DAC channels
	sbi		DACLDAC					;DACCTL,DACLDACBit
	ret
;
; DACYXPos		Channel A position in Y0r23, Channel B position in X0r24
;				XOffsetr9, YOffsetr10 preloaded
;
; USED:	r22, r0
;
; Must preserve: r20,r21
;
DACYXPos:
;
; Y first (DACA)
;
	mov		r22,Y0r23				;save Y0r23
#if INVERT
	neg		r22						;map Y origin
#endif
#if MAPGOFFSETTOREG
	add		r22,XOffsetr9			;Scan Table offset
#else
	lds		r0,XOffset				;Scan Table offset
	add		r22,r0
#endif
#if 1
;	compare with LastChanA, update LastChanA
	lds		r0,LastChanA
	cp		r22,r0
	breq	SkipChanA1
	sts		LastChanA,r22
#endif
	cbi		DACSELECT				;select DACA (active low)
	out		DACLODATA,r22			;High nibble of DACLODATA modified here: undo Button pull-up resistors.
	swap	r22
	out		DACHIDATA,r22			;no need to preserve High nibble of DACHIDATA
	cbi		DACWR					;DACCTL,DACWRBit			;latch DACA data
	sbi		DACWR					;DACCTL,DACWRBit			;hold
SkipChanA1:
;
; X second (DACB)
;
	mov		r22,X0r24				;preserve X0r24 by using r22
#if INVERT
	neg		r22						;map X origin
#endif
#if MAPGOFFSETTOREG
	add		r22,YOffsetr10			;Scan Table offset
#else
	lds		r0,YOffset				;Scan Table offset
	add		r22,r0
#endif
#if 1
;	compare with LastChanB, update LastChanB
	lds		r0,LastChanB
	cp		r22,r0
	breq	1f						;SkipChanB1
	sts		LastChanB,r22
#endif
	sbi		DACSELECT				;select DACB (active high)
	out		DACLODATA,r22			;High nibble of DACLODATA modified here: undo Button pull-up resistors.
	swap	r22
	out		DACHIDATA,r22			;no need to preserve High nibble of DACHIDATA
	cbi		DACWR					;DACCTL,DACWRBit			;latch DACB data
	sbi		DACWR					;DACCTL,DACWRBit			;hold
;SkipChanB1:
1:
	cbi		DACLDAC					;DACCTL,DACLDACBit		;update DAC channels
	sbi		DACLDAC					;DACCTL,DACLDACBit
	ret
;
;  C interface to DrawVector
;
; Must declare MAPGOFFSETTOREG (r7..r8) and TESTREGS (r2..r6) in C
;
;	CDrawVector(uint8_t x0, y0, x1, y1)
;	Confirm these register allocations: Call used registers
;	x0: r24
;	y0: r22
;	x1: r20
;	y1: r18
;
CDrawVector:
	mov		Y0r23,r22			;y0
	mov		Y1r19,r18			;y1
	mov		X1r18,r20			;x1
;	cbi		TIMSK0, OCIE0A		;Disable TC0.matchA
    ldiw	Z,TIMSK0
    ld		r20,Z
    andi	r20, ~0x02
    st		Z,r20
	call	DrawVector
;	sbi		TIMSK0, OCIE0A		;Enable TC0.matchA
    ld		r20,Z
    ori		r20, 0x02
    st		Z,r20
	ret
;
;	Vector Drawing Function: Bresenham's Algorithm
;
;	Arguments:
;	   r24 - X0r24-Start
;	   r23 - Y0r23-Start
;	   r18 - X1r18-End
;	   r19 - Y1r19-End
;
; Not used:
;
;	unprotected "Call Saved" registers r11..r17, Y
;	X, Z(saved)
;
; When done, X0r24,Y0r23 and X1r18,Y1r19 are identical. TODO is this used?
;
; registers r9/r10 are XOffsetr9/YOffsetr10. Don't change
#if MAPGOFFSETTOREG
; registers r7/r8 are GXOffset/GYOffset. Don't change
#endif
; if called from internal XXXRefresh():
; Don't use r25
; Register r22 used by pixel set function (DACXYPos, DACYXPos)
;
; r20,r21 is used to save Z
; Z is used to hold the plot function to call
;
; register r0 used; work registers r2..r6 defined here:
;
#define	LineErrorL			r2
#define	LineErrorH			r3
#define	DeltaX				r4
#define	DeltaY				r5
#define	YStep				r6

#if 0
void (*pfPlot)(x,y);
function line(x0, y0, x1, y1)
     boolean steep := abs(y1 - y0) > abs(x1 - x0)
     if steep then
	 	pfPlot = DACYXPOS;
		swap(x0, y0)
		swap(x1, y1)
     if x0 > x1 then
	 	pfPlot = DACXYPos;
		swap(x0, x1)
		swap(y0, y1)
     int deltax := x1 - x0
     int deltay := abs(y1 - y0)
     int LineError := 0
     int YStep
     int y := y0
     if y0 < y1 then YStep := 1 else YStep := -1
     for x from x0 to x1
		pfPlot();
        LineError := LineError + deltay
        if 2×LineError >= deltax then
             y := y + YStep
             LineError := LineError - deltax
#endif
DrawVector:
	movw	r20,ZL
;     boolean steep := abs(y1 - y0) > abs(x1 - x0)
	ldiw	Z,pm(DACXYPos)		;preset word address
	mov		DeltaY,Y1r19
	sub		DeltaY,Y0r23
	brcc	Line100
	neg		DeltaY				;Take absolute value Y1r19-Y0r23 -> Dx
Line100:
	mov		DeltaX,X1r18
	sub		DeltaX,X0r24
	brcc	Line200
	neg		DeltaX				;Take absolute value X1r18-X0r24 ->  Dy
Line200:
	cp		DeltaX,DeltaY		;Inverted: if (Dx >= Dy) jmp
	brsh	Line300
	ldiw	Z,pm(DACYXPos)		; preset word address
;     if steep then
;         swap(x0, y0)
;         swap(x1, y1)
;XOR swap algorithm
	eor		X0r24,Y0r23				;swap X0r24,Y0r23
	eor		Y0r23,X0r24
	eor		X0r24,Y0r23
	eor		X1r18,Y1r19				;swap X1r18,Y1r19
	eor		Y1r19,X1r18
	eor		X1r18,Y1r19
Line300:
;     if x0 > x1 then
;         swap(x0, x1)
;         swap(y0, y1)
	cp		X1r18,X0r24				;inverted: if (X1r18 >= X0r24) jmp
	brsh	Line400
	eor		X0r24,X1r18				;swap X0r24, X1r18
	eor		X1r18,X0r24
	eor		X0r24,X1r18
	eor		Y0r23,Y1r19				;swap Y0r23,Y1r19
	eor		Y1r19,Y0r23
	eor		Y0r23,Y1r19
Line400:
	mov		DeltaX,X1r18			;int deltax := X1r18 - X0r24
	sub		DeltaX,X0r24
	mov		DeltaY,Y1r19			;int deltay := abs(Y1r19 - Y0r23)
	sub		DeltaY,Y0r23
	brcc	Line500
	neg		DeltaY				;Take absolute value
Line500:
	clr		LineErrorL			;int LineError := 0
	clr		LineErrorH
	clr		YStep				;int YStep
	cp		Y0r23,Y1r19				;if Y0r23 < Y1r19 then ystep := 1 else ystep := -1
	brsh	Line600
	inc		YStep
	rjmp	Line750
Line600:
	dec		YStep
;     int y := y0
;     for x from x0 to x1
Line750:
	cp		X1r18,X0r24
	brlo	Line1100
	BEAMON
	sbi		_SFR_IO_ADDR(GPIOR0),0		;remember beam state
Line775:
;		pfPlot();
	cli
	icall
	sei
;         LineError := LineError + deltay
	add		LineErrorL,DeltaY
	adc		LineErrorH,__zero_reg_
Line997:
;         if 2×LineError >= deltax then
	mov		r0,LineErrorL
	add		r0,LineErrorL
	cp		r0,DeltaX
	cpc		LineErrorH,__zero_reg_
Line998:
	brlt	 Line1000
;             y := y + ystep
	add		Y0r23,YStep
;             LineError := LineError - deltax
	sub		LineErrorL,DeltaX
	sbc		LineErrorH,__zero_reg_
Line1000:
	inc		X0r24
	breq	Line1100			;X0r24 overflows so we're done. Handles the X1r18==0xff case
	cp		X1r18,X0r24			;inverted test
	brsh	Line775
Line1100:
	BEAMOFF
	cbi		_SFR_IO_ADDR(GPIOR0),0		;remember beam state
	movw	ZL,r20
	ret
#undef	LineErrorL
#undef	LineErrorH
#undef	DeltaX
#undef	DeltaY
#undef	YStep
#undef	X0r24
#undef	Y0r23
#undef	X1r18
#undef	Y1r19

#if 1
;
; C interface to DrawTextStringfromFlash()
; r24/r25 ptr to Flash based string
; assume Z is caller saved
;
CDrawTextStringfromSRam:
	movw	ZL,r24
;
; Draw a String
;
; in Z: ptr to 0-terminated string
;
; Must preserve r25,X
;
DrawTextStringfromSRam:
	// Move the beam to the correct position first
	ldi		r22,1
	sts		FirstInString,r22
1:
	ld		r24,Z+				;get next Text Code from SRam
	tst		r24					;end of string
	breq	2f					;done
	rcall	DrawChar			;must preserve r25,X,Z
	rjmp	1b
2:
	ret
#endif
;
; C interface to DrawTextStringfromFlash()
; r24/r25 ptr to Flash based string
; assume Z is caller saved
;
CDrawTextStringfromFlash:
	movw	ZL,r24
;
; Z points to Flash based string
; preserves r25
;
DrawTextStringfromFlash:
	// Move the beam to the correct position first
	ldi		r22,1
	sts		FirstInString,r22
1:
	lpm		r24,Z+				;get next Text Code from flash
	tst		r24					;end of string
	breq	2f					;done
	rcall	DrawChar			;must preserve r25,X,Z
	rjmp	1b
2:
	ret
;
; DrawChar
;
; IN:	r24	-- char to draw. Valid range is 32..127 + HALFSPACE
;
; Must preserve r25,X,Z
;
; USES: r4,r5,r6,r18,r19,r20,r21,r22,r23,r24 (OLD)
; CharPixel uses: r0, r2, r3. Expects r17,r19,r20,r21 preloads
;
; Preserves: r7, r8, r9, r10, r25, X, Z (NEW)
;	r17 also saved WHY?
;	r18 preserved if MAPGOFFSETTOREG (for DrawVTChar)
;
; Also called from DrawVTChar in VectorTable Drawing
;
; TESTREGS true means r2..r6 are available for use as long as DrawVector() is not called
; MAPGOFFSETTOREG means r7..r10 are reserved
;
DrawChar:
	push	r25					;required for TextTableScan()
#if MAPGOFFSETTOREG				//unclear why only if MAPGOFFSETTOREG
	push	r18
#endif
	movw	r4,ZL				;save Z to r4,r5
	sbrc	r24,7				;bit 1 high is out of range
	rjmp	DrawCL900			;includes HALFSPACE
	subi	r24,FIRST_CHAR+1	;rebase
	brcc	1f
	rjmp	DrawCL900			;char was out of range, includes SPACE
1:
	mov		r20,r24				;compute 8 * char index
	ldi		r21,8
	mul		r20,r21				;result in r1:r0
	ldiw	Z,Newfonttbl
	add		ZL,r0
	adc		ZH,r1
	clr		r1					;__zero_reg_
	ldi		r22,CHAR_HEIGHT		;First Y pos, Also Row Counter
	push	r17					;WHY?
//
// preload r18 with XOffsetr9
// preload r25 with YOffsetr10
//
#if MAPGOFFSETTOREG
	//
	// Dumb. Use a second register pair now. Use macros to cover both cases
	//
	mov		r18,XOffsetr9 						//X Scan Table offset
	mov		r25,YOffsetr10						//Y Scan Table offset
#else
	lds		r18,XOffset 						//X Scan Table offset
	lds		r25,YOffset							//Y Scan Table offset
#endif
//
// Preloads:
// Combine bit actions on DACCTL port
// This requires caching the value of port D, but the LED bit
// was changed in an ISR (LED Morse code). Solved by using
// Flags.UpdateLed and doing the LED change in ClockWorks()
//
	BEAMOFF
	cbi		_SFR_IO_ADDR(GPIOR0),0							;remember beam state
	in		r20,PD_IN							//TODO sb PORTD? same reading?
#if ZINVERTED
	_sbi(r20,DACZXBit)							//set one bit:BEAMOFF
#else
	_cbi(r20,DACZXBit)							//clear one bit:BEAMOFF
#endif
#if AD7302
	mov		r19,r20
	andi	r19,~(_BV(DACLDACBit))				//clear one bit
#endif
	mov		r21,r20
	andi	r21,~(_BV(DACSELBit)|_BV(DACWRBit))	//clear two bits
	mov		r17,r20
	andi	r17,~(_BV(DACWRBit))				//clear one bit
//
// load r23 with DOUBLED bit position.
//
// Draw ONLY the first dot with BEAMOFF iff FirstInString is TRUE
// This moves the beam while it's off. Delay still too short
// Repaint the whole character when done (Z,r22 restored)
//
	lds		r6,FirstInString
	and		r6,r6
	breq	DrawCL175
	pushw	Z
DrawCL100:
	lpm		r6,Z+								//bits for current row
	;
	; early out test for r6 == 0. Timing shows slight improvement.
	;
	tst		r6
	breq	1f
	clr		r23									//start x pos, 6 pixels per row
	sbrc	r6,7
	rjmp	CharDot								//will return to DrawCL150
	ldi		r23,2
	sbrc	r6,6
	rjmp	CharDot								//will return to DrawCL150
	ldi		r23,4
	sbrc	r6,5
	rjmp	CharDot								//will return to DrawCL150
	ldi		r23,6
	sbrc	r6,4
	rjmp	CharDot								//will return to DrawCL150
	ldi		r23,8
	sbrc	r6,3
	rjmp	CharDot								//will return to DrawCL150
	ldi		r23,10
	sbrc	r6,2
	rjmp	CharDot								//will return to DrawCL150
1:
	dec		r22									//next Y pos, row counter
	brne	DrawCL100							//finish all rows
DrawCL150:
// reload Z and r22
	popw	Z
	ldi		r22,CHAR_HEIGHT						//First Y pos, Also Row Counter

	sts		FirstInString,__zero_reg_
DrawCL175:
//
// preloads again: Need to reread PortD
// Combine bit actions on DACCTL port
// This requires caching the value of port D, but the LED bit
// may be changed in an ISR (LED Morse code). Solved by using
// Flags.UpdateLed and doing the LED change in ClockWorks()
//
	BEAMON
	sbi		_SFR_IO_ADDR(GPIOR0),0		;remember beam state
	in		r20,PD_IN
#if ZINVERTED
	andi	r20,~(_BV(DACZXBit))				//clear one bit:BEAMON
#else
	ori		r20,_BV(DACZXBit)					//set one bit:BEAMON
#endif
#if AD7302
	mov		r19,r20
	andi	r19,~(_BV(DACLDACBit))				//clear one bit
#endif
	mov		r21,r20
	andi	r21,~(_BV(DACSELBit)|_BV(DACWRBit))	//clear two bits
	mov		r17,r20
	andi	r17,~(_BV(DACWRBit))				//clear one bit
//
//	r18 and r25 still preloaded with XOffsetr9 and YOffsetr10.
//	Related to MAPGOFFSETTOREG
//
DrawCL200:
	lpm		r6,Z+				;bits for current row
	;
	; early out test for r6 == 0. Timing shows slight improvement.
	;
	tst		r6
	breq	1f
	clr		r23					;start x pos, 6 pixels per row
	sbrc	r6,7
	rcall	CharPixel
	ldi		r23,2
	sbrc	r6,6
	rcall	CharPixel
	ldi		r23,4
	sbrc	r6,5
	rcall	CharPixel
	ldi		r23,6
	sbrc	r6,4
	rcall	CharPixel
	ldi		r23,8
	sbrc	r6,3
	rcall	CharPixel
	ldi		r23,10
	sbrc	r6,2
	rcall	CharPixel
1:
	dec		r22					;next Y pos, row counter
	brne	DrawCL200			;finish all rows
	pop		r17					;WHY?
DrawCL900:
	BEAMOFF
	cbi		_SFR_IO_ADDR(GPIOR0),0		;remember beam state
	movw	ZL,r4			;restore
;
; compute GetCharWidth, r24 holds char
; TODO: use table
;
	cpi		r24,25		;':'-(FIRST_CHAR+1)
	brne	1f			;GetCWL100
	ldi		r24,COLONCHARWIDTH
	rjmp	10f
1:
;GetCWL100:
	cpi		r24,128		;HALFSPACE (not rebased)
	brne	2f
	ldi		r24,HALFSPACECHARWIDTH
	rjmp	10f
2:
	cpi		r24,12		;'-'-(FIRST_CHAR+1)
	brne	3f
	ldi		r24,DASHCHARWIDTH
	rjmp	10f
3:
	cpi		r24,11		;','-(FIRST_CHAR+1)
	brne	4f			;GetCWL400
	ldi		r24,COMMACHARWIDTH
	rjmp	10f			;GetCWL800
4:
#if PACMAN
	cpi		r24,64		;'a'-(FIRST_CHAR+1)
	brne	5f
	ldi		r24,LOWERACHARWIDTH
	rjmp	10f
5:
	cpi		r24,72		;'i'-(FIRST_CHAR+1)
	brne	6f
	ldi		r24,LOWERICHARWIDTH
	rjmp	10f
6:
	cpi		r24,77		;'i'-(FIRST_CHAR+1)
	brne	7f
	ldi		r24,LOWERNCHARWIDTH
	rjmp	10f
7:
	cpi		r24,255		;SPACE
	brne	8f
	ldi		r24,HALFSPACECHARWIDTH
	rjmp	10f
8:
#endif
	ldi		r24,DEFCHARWIDTH
10:
#if MAPGOFFSETTOREG
	add		XOffsetr9,r24		;ignored if called from DrawVTChar()
#else
	lds		r0,XOffset
	add		r0,r24
	sts		XOffset,r0
#endif
#if MAPGOFFSETTOREG
	pop		r18
#endif
	pop		r25
	ret
;
; in r22,r23. r22 unchanged on exit
; preloads: r17,r19,r20,r21
; r18 and r25 still preloaded with XOffsetr9 and YOffsetr10. Related to MAPGOFFSETTOREG
; uses r0,r2,r3,r23
; don't use r4,r5,r6,r24
;
CharPixel:
	mov		r0,r22
;
; X first (DACA)
;
	add		r23,r18				 	;X Scan Table offset
	mov		r3,r23					;get high nibble into r3
	swap	r3
;
; Y second (DACB)
;
	lsl		r22						;times 2
	add		r22,r25		 			;Y Scan Table offset
	mov		r2,r22					;get high nibble into r2
	swap	r2
//
// 52 cycles per fat pixel: 2.6 uSec. Could run with interrupts OFF since
// Timer1 may interrupt, causing flicker. Need to save interrupt status first.
// Reduced time spent in Timer1.
// Issue: is there a chance we'll miss an INT0 interrupt, coming in on PORTD2,
// since we write constantly to PORTD here. ASSERT in TIMEOUT code to test.
//
#if 1
;	in		r1,_SFR_IO_ADDR(SREG)
	cli
#endif
;
; Pixel 1
;
	out		DACLODATA,r23			;High nibble of DACLODATA modified here: undo Button pull-up resistors.
	out		DACHIDATA,r3			;no need to preserve High nibble of DACHIDATA
	// select DACA (active low). latch DACA data
	out		DACCTL,r21				;001	DACSELBit low DACWRBit low
	// hold latch. select DACB (active high)
	out		DACCTL,r20				;111	DACSELBit high DACWRBit high
	out		DACLODATA,r22			;High nibble of DACLODATA modified here: undo Button pull-up resistors.
	out		DACHIDATA,r2			;no need to preserve High nibble of DACHIDATA
	//latch DACB data
	out		DACCTL,r17				;101	DACWRBit low
	//hold
	out		DACCTL,r20				;111
	//update DAC channels
	out		DACCTL,r19				;110	DACLDACBit low
	//hold
	out		DACCTL,r20				;111
;
; Pixel 2
;
	inc		r23						;next row
	mov		r3,r23
	swap	r3
	out		DACLODATA,r23			;High nibble of DACLODATA modified here: undo Button pull-up resistors.
	out		DACHIDATA,r3			;no need to preserve High nibble of DACHIDATA
	// select DACA (active low). latch DACA data
	out		DACCTL,r21				;001	DACSELBit low DACWRBit low
	// hold latch. select DACB (active high)
	out		DACCTL,r20				;111
	out		DACLODATA,r22			;High nibble of DACLODATA modified here: undo Button pull-up resistors.
	out		DACHIDATA,r2			;no need to preserve High nibble of DACHIDATA
	//latch DACB data
	out		DACCTL,r17				;101	DACWRBit low
	//hold
	out		DACCTL,r20				;111
	//update DAC channels
	out		DACCTL,r19				;110	DACLDACBit low
	//hold
	out		DACCTL,r20				;111
;
; Pixel 3
;
	dec		r23						;back to initial row
	inc		r22						;next column
	mov		r3,r23
	swap	r3
	mov		r2,r22
	swap	r2
	out		DACLODATA,r23			;High nibble of DACLODATA modified here: undo Button pull-up resistors.
	out		DACHIDATA,r3			;no need to preserve High nibble of DACHIDATA
	// select DACA (active low). latch DACA data
	out		DACCTL,r21				;001	DACSELBit low DACWRBit low
	// hold latch. select DACB (active high)
	out		DACCTL,r20				;111
	out		DACLODATA,r22			;High nibble of DACLODATA modified here: undo Button pull-up resistors.
	out		DACHIDATA,r2			;no need to preserve High nibble of DACHIDATA
	//latch DACB data
	out		DACCTL,r17				;101	DACWRBit low
	//hold
	out		DACCTL,r20				;111
	//update DAC channels
	out		DACCTL,r19				;110	DACLDACBit low
	//hold
	out		DACCTL,r20				;111
;
; Pixel 4
;
;	This last pixel can easily be skipped without barely visible difference
;
	inc		r23						;next row
	mov		r3,r23
	swap	r3
	out		DACLODATA,r23			;High nibble of DACLODATA modified here: undo Button pull-up resistors.
	out		DACHIDATA,r3			;no need to preserve High nibble of DACHIDATA
	// select DACA (active low). latch DACA data
	out		DACCTL,r21				;001	DACSELBit low DACWRBit low
	// hold latch. select DACB (active high)
	out		DACCTL,r20				;111
	out		DACLODATA,r22			;High nibble of DACLODATA modified here: undo Button pull-up resistors.
	out		DACHIDATA,r2			;no need to preserve High nibble of DACHIDATA
	//latch DACB data
	out		DACCTL,r17				;101	DACWRBit low
	//hold
	out		DACCTL,r20				;111
	//update DAC channels
	out		DACCTL,r19				;110	DACLDACBit low
	//hold
	out		DACCTL,r20				;111
	mov		r22,r0
#if 1
;	out		_SFR_IO_ADDR(SREG),r1
;	clr		__zero_reg_
	sei
#endif
	ret
//
// in r23 (X) doubled already, r22 (Y)
// PortD values preloaded in r21, r20, r17, r19
// XOffsetr9 to r18 and YOffsetr10 to r25 preloaded 
// uses r2, r3
//
CharDot:
;
; X first (DACA). r23 was doubled by caller.
;
	add		r23,r18				 	;X Scan Table offset
	mov		r3,r23
	swap	r3
;
; Y second (DACB)
;
	lsl		r22						;times 2
	add		r22,r25		 			;Y Scan Table offset
	mov		r2,r22
	swap	r2
	out		DACLODATA,r23			;High nibble of DACLODATA modified here: undo Button pull-up resistors.
	out		DACHIDATA,r3			;no need to preserve High nibble of DACHIDATA
	// select DACA (active low). latch DACA data
	out		DACCTL,r21				;001	DACSELBit low DACWRBit low
	// hold latch. select DACB (active high)
	out		DACCTL,r20				;111	DACSELBit high DACWRBit high
	out		DACLODATA,r22			;High nibble of DACLODATA modified here: undo Button pull-up resistors.
	out		DACHIDATA,r2			;no need to preserve High nibble of DACHIDATA
	//latch DACB data
	out		DACCTL,r17				;101	DACWRBit low
	//hold
	out		DACCTL,r20				;111
	//update DAC channels
	out		DACCTL,r19				;110	DACLDACBit low
	//hold
	out		DACCTL,r20				;111
	rjmp	DrawCL150

#undef	XOffsetr9
#undef	YOffsetr10

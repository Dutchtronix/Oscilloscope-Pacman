#if defined(__GNUC__)
;
; WinAVR
;
#define	XL	r26
#define	XH	r27
#define	YL	r28
#define	YH	r29
#define	ZL	r30
#define	ZH	r31

.macro ldiw arg1 arg2
	.if \arg1 == X
	ldi	XL,lo8(\arg2)
	ldi	XH,hi8(\arg2)
	.elseif \arg1 == Y
	ldi	YL,lo8(\arg2)
	ldi	YH,hi8(\arg2)
	.elseif \arg1 == Z
	ldi	ZL,lo8(\arg2)
	ldi	ZH,hi8(\arg2)
	.elseif \arg1 == A
	ldi	r16,lo8(\arg2)
	ldi	r17,hi8(\arg2)
	.elseif \arg1 == r24
	ldi	r24,lo8(\arg2)
	ldi	r25,hi8(\arg2)
	.elseif \arg1 == r22
	ldi	r22,lo8(\arg2)
	ldi	r23,hi8(\arg2)
	.elseif \arg1 == r18
	ldi	r18,lo8(\arg2)
	ldi	r19,hi8(\arg2)
	.else
		error
	.endif
.endm

.macro	ldsw arg1 arg2
	.if \arg1 == X
	lds	XL,\arg2
	lds	XH,\arg2\()+1
	.elseif \arg1 == Y
	lds	YL,\arg2
	lds	YH,\arg2\()+1
	.elseif \arg1 == Z
	lds	ZL,\arg2
	lds	ZH,\arg2\()+1
	.elseif \arg1 == A
	lds	r16,\arg2
	lds	r17,\arg2\()+1
	.elseif \arg1 == r24
	lds	r24,\arg2
	lds	r25,\arg2\()+1
	.else
		error
	.endif
.endm

.macro	lddw arg1 arg2
	.if \arg1 == X
	ldd	XL,\arg2
	ldd	XH,\arg2\()+1
	.elseif \arg1 == Y
	ldd	YL,\arg2
	ldd	YH,\arg2\()+1
	.elseif \arg1 == Z
	ldd	ZL,\arg2
	ldd	ZH,\arg2\()+1
	.elseif \arg1 == A
	ldd	r16,\arg2
	ldd	r17,\arg2\()+1
	.else
		error
	.endif
.endm


.macro	stsw arg1 arg2
	.if \arg2 == X
	sts	\arg1,XL
	sts	\arg1\()+1,XH
	.elseif \arg2 == Y
	sts	\arg1,YL
	sts	\arg1\()+1,YH
	.elseif \arg2 == Z
	sts	\arg1,ZL
	sts	\arg1\()+1,ZH
	.elseif \arg2 == A
	sts	\arg1,r16
	sts	\arg1\()+1,r17
	.elseif \arg2 == r24
	sts	\arg1,r24
	sts	\arg1\()+1,r25
	.else
		error
	.endif
.endm

.macro	stdw arg1 arg2
	.if \arg2 == X
	std	\arg1,XL
	std	\arg1\()+1,XH
	.elseif \arg2 == Y
	std	\arg1,YL
	std	\arg1\()+1,YH
	.elseif \arg2 == Z
	std	\arg1,ZL
	std	\arg1\()+1,ZH
	.elseif \arg2 == A
	std	\arg1,r16
	std	\arg1\()+1,r17
	.else
		error
	.endif
.endm

.macro	pushw arg1
	.if \arg1 == X
	push	XH
	push	XL
	.elseif \arg1 == Y
	push	YH
	push	YL
	.elseif \arg1 == Z
	push	ZH
	push	ZL
	.elseif \arg1 == A
	push	r17
	push	r16
	.else
		error
	.endif
.endm

.macro	popw arg1
	.if \arg1 == X
	pop		XL
	pop		XH
	.elseif \arg1 == Y
	pop		YL
	pop		YH
	.elseif \arg1 == Z
	pop		ZL
	pop		ZH
	.elseif \arg1 == A
	pop		r16
	pop		r17
	.else
		error
	.endif
.endm
#if 0
;------------------------------------------------;
; Output port immediate via r16
;
;	outi	port,var

.macro	outi arg1 arg2
	ldi	r16,\arg2
	out	\arg1,r16
.endm

;------------------------------------------------;
; Store immediate into direct memory via r16
;
;	stsi	var,imm

.macro	stsi arg1 arg2
	ldi	r16,\arg2
	sts	\arg1,r16
.endm

;------------------------------------------------;
; Store immediate into indirect memory via r16
;
;	sti	Z,imm
;	stdi	Z+d,imm

.macro	sti arg1 arg2
	ldi	r16,\arg2
	st	\arg1,r16
.endm

.macro	stdi arg1 arg2
	ldi	r16,\arg2
	std	\arg1,r16
.endm
#else

; Store immediate into indirect memory via r24
;
;	sti	Z,imm

.macro	sti arg1 arg2
	ldi	r24,\arg2
	st	\arg1,r24
.endm

#endif

;------------------------------------------------;
; add/sub/subc/cp/cpc/lsl/lsr/rol/ror to register pair
;

.macro	addiw arg1 arg2
	.if \arg1 == X
	subi	XL,lo8(-(\arg2))
	sbci	XH,hi8(-(\arg2))
	.elseif \arg1 == Y
	subi	YL,lo8(-(\arg2))
	sbci	YH,hi8(-(\arg2))
	.elseif \arg1 == Z
	subi	ZL,lo8(-(\arg2))
	sbci	ZH,hi8(-(\arg2))
	.elseif \arg1 == A
	subi	r16,lo8(-(\arg2))
	sbci	r17,hi8(-(\arg2))
	.else
		error
	.endif
.endm

.macro	clrw arg1
	.if \arg1 == X
	clr	XL
	clr	XH
	.elseif \arg1 == Y
	clr	YL
	clr	YH
	.elseif \arg1 == Z
	clr	ZL
	clr	ZH
	.elseif \arg1 == A
	clr	r16
	clr	r17
	.else
		error
	.endif
.endm

;------------------------------------------------;
; Add immediate to register

.macro	addi arg1 arg2
	subi	\arg1,-(\arg2)
.endm

;------------------------------------------------;
; Long branch


.macro	rjne arg1
	breq	1f
	rjmp	\arg1
1:
.endm

.macro	rjeq arg1
	brne	1f
	rjmp	\arg1
1:
.endm

.macro	rjcc
	brcs	PC+2
	rjmp	@0
.endm

.macro	rjcs
	brcc	PC+2
	rjmp	@0
.endm

.macro	rjtc
	brts	PC+2
	rjmp	@0
.endm

.macro	rjts
	brtc	PC+2
	rjmp	@0
.endm


.macro	retcc
	brcs	PC+2
	ret
.endm

.macro	retcs
	brcc	PC+2
	ret
.endm

.macro	reteq
	brne	PC+2
	ret
.endm

.macro	retne
	breq	PC+2
	ret
.endm

#else			//Atmel Assembler
;------------------------------------------------;
; Constants

.def	AL	= r16
.def	AH	= r17
.def	BL	= r18
.def	BH	= r19
.def	CL	= r20
.def	CH	= r21
.def	DL	= r22
.def	DH	= r23
.def	EL	= r24
.def	EH	= r25

.equ	RAMTOP = 0x60		; RAM top address
.equ	RAMTOP100 = 0x100	; For only M64/128 native mode


.equ	bit0	= 0b00000001
.equ	bit1	= 0b00000010
.equ	bit2	= 0b00000100
.equ	bit3	= 0b00001000
.equ	bit4	= 0b00010000
.equ	bit5	= 0b00100000
.equ	bit6	= 0b01000000
.equ	bit7	= 0b10000000


;------------------------------------------------;
; Push/Pop register pair
;
; 	pushw	Z

.macro	pushw
	push	@0H
	push	@0L
.endm

.macro	popw
	pop	@0L
	pop	@0H
.endm


;------------------------------------------------;
; Load/store word from/to direct memory/immediate
;
;	ldsw	Z,mem
;	ldiw	Z,imm

.macro	ldiw
	ldi	@0L,low(@1)
	ldi	@0H,high(@1)
.endm

.macro	ldsw
	lds	@0L,@1
	lds	@0H,@1+1
.endm

.macro	lddw
	ldd	@0L,@1
	ldd	@0H,@1+1
.endm

.macro	stsw
	sts	@0,@1L
	sts	@0+1,@1H
.endm

.macro	stdw
	std	@0,@1L
	std	@0+1,@1H
.endm

.macro	clrsw
	sts	@0,r15
	sts	@0+1,r15
.endm

.macro	clrdw
	std	@0,r15
	std	@0+1,r15
.endm


;------------------------------------------------;
; Store immediate into indirect memory via r16
;
;	sti	Z,imm
;	stdi	Z+d,imm

.macro	sti
	ldi	r16,@1
	st	@0,r16
.endm

.macro	stdi
	ldi	r16,@1
	std	@0,r16
.endm


;------------------------------------------------;
; add/sub/subc/cp/cpc/lsl/lsr/rol/ror to register pair
;

.macro	addiw
	subi	@0L,low(-(@1))
	sbci	@0H,high(-(@1))
.endm

.macro	subiw
	subi	@0L,low(@1)
	sbci	@0H,high(@1)
.endm

.macro	addw
	add	@0L,@1L
	adc	@0H,@1H
.endm

.macro	adcw
	adc	@0L,@1L
	adc	@0H,@1H
.endm

.macro	subw
	sub	@0L,@1L
	sbc	@0H,@1H
.endm

.macro	sbcw
	sbc	@0L,@1L
	sbc	@0H,@1H
.endm

.macro	cpw
	cp	@0L,@1L
	cpc	@0H,@1H
.endm

.macro	cpcw
	cpc	@0L,@1L
	cpc	@0H,@1H
.endm

.macro	andw
	and	@0L,@1L
	and	@0H,@1H
.endm

.macro	andiw
	andi	@0L,low(@1)
	andi	@0H,high(@1)
.endm

.macro	orw
	or	@0L,@1L
	or	@0H,@1H
.endm

.macro	oriw
	ori	@0L,low(@1)
	ori	@0H,high(@1)
.endm

.macro	lslw
	lsl	@0L
	rol	@0H
.endm

.macro	lsrw
	lsr	@0H
	ror	@0L
.endm

.macro	rolw
	rol	@0L
	rol	@0H
.endm

.macro	rorw
	ror	@0H
	ror	@0L
.endm

.macro	clrw
	clr	@0L
	clr	@0H
.endm

.macro	comw
	com	@0L
	com	@0H
.endm

.macro	movew
	mov	@0L, @1L
	mov	@0H, @1H
.endm


;------------------------------------------------;
; Store immediate into direct memory via r16
;
;	stsi	var,imm

.macro	stsi
	ldi	r16,@1
	sts	@0,r16
.endm


;------------------------------------------------;
; Output port immediate via r16
;
;	outi	port,var

.macro	outi
	ldi	r16,@1
	out	@0,r16
.endm


;------------------------------------------------;
; Add immediate to register

.macro	addi
	subi	@0,-(@1)
.endm


;------------------------------------------------;
; Long branch


.macro	rjne
	breq	PC+2
	rjmp	@0
.endm

.macro	rjeq
	brne	PC+2
	rjmp	@0
.endm

.macro	rjcc
	brcs	PC+2
	rjmp	@0
.endm

.macro	rjcs
	brcc	PC+2
	rjmp	@0
.endm

.macro	rjtc
	brts	PC+2
	rjmp	@0
.endm

.macro	rjts
	brtc	PC+2
	rjmp	@0
.endm


.macro	retcc
	brcs	PC+2
	ret
.endm

.macro	retcs
	brcc	PC+2
	ret
.endm

.macro	reteq
	brne	PC+2
	ret
.endm

.macro	retne
	breq	PC+2
	ret
.endm


;------------------------------------------------;
; Move single bit between two registers
;
;	bmov	dstreg,dstbit,srcreg.srcbit

.macro	bmov
	bst	@2,@3
	bld	@0,@1
.endm

#endif

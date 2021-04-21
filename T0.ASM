;:ts=8
; General purpose ROM startup code 
;
; First we set up the stack pointer.
; Then we copy the initialized data.
; Then we clear out the uninitialized data.
; Then we call main.

	chip	65C02

	include	"zpage.inc"

	****************************************************************
	*                                                              *
	*   Define a special section in case most of the               *
	*      code is not in bank 0.                                  *
	*                                                              *
	****************************************************************

MCH_STACK	EQU	$FF 
C_STACK		EQU	$1EFF

STARTUP	SECTION	OFFSET $FF00

	xref  _BEG_DATA
	xref  _END_DATA
	xref  _ROM_BEG_DATA
	xref  _BEG_UDATA
	xref  _END_UDATA

	xdef	_START
_START:
START:
	cld			; clear decimal mode
	ldx	#MCH_STACK	; get the hardware stack address
	txs			; and set the stack to it
	lda	#<C_STACK	; get the software stack address
	sta	SP
	lda	#>C_STACK
	sta	SP+1

	****************************************************************
	*                                                              *
	*    Next, we want to copy the initialized data from           *
	*      ROM to RAM.                                             *
	*                                                              *
	****************************************************************

	lda	#<_END_DATA-_BEG_DATA	;number of bytes to copy
	sta	R0
	lda	#>_END_DATA-_BEG_DATA
	sta	R0+1
	ora	R0
	beq	copydone		; nothing to do
	lda	#<_ROM_BEG_DATA		; get source into R1
	sta	R1
	lda	#>_ROM_BEG_DATA
	sta	R1+1
	lda	#<_BEG_DATA		; get dest into R2
	sta	R2
	lda	#>_BEG_DATA
	sta	R2+1
	ldy	#0
copyloop:
	lda	(R1),Y
	sta	(R2),Y
	iny
	bne	copyskip
	inc	R1+1
	inc	R2+1
copyskip:
	lda	R0
	bne	copyskip2
	dec	R0+1
	bmi	copydone
copyskip2:
	dec	R0
	bra	copyloop
copydone:

	****************************************************************
	*                                                              *
	*   Now, clear out the uninitialized data area.                *
	*    We assume that it is in the same bank as DATA.            *
	*                                                              *
	****************************************************************

	lda	#<_END_UDATA-_BEG_UDATA  ;get number of bytes to clear
	sta	R0
	lda	#>_END_UDATA-_BEG_UDATA
	sta	R0+1
	ora	R0
	beq	done
	lda	#<_BEG_UDATA		; get beginning of zeroes
	sta	R1
	lda	#>_BEG_UDATA
	sta	R1+1
	lda	#0			;get a zero for storing
	tay
loop:
	tya
	sta	(R1),y			; clear memory
	inc	R1
	bne	skip
	inc	R1+1
skip:
	lda	R0
	bne	skip2
	dec	R0+1
	bmi	done
skip2:
	dec	R0
	bra	loop			;continue till done
done:

	****************************************************************
	*                                                              *
	*   Finally, transfer control to the real program.             *
	*                                                              *
	****************************************************************

        xref	_main
        jsr	_main
	lda	R0+1
        pha
	lda	R0
        pha
        xref	__exit
        jsr	__exit

	xdef	__exit
__exit:
	brk

	xdef	IRQ
	xref	_IRQHandler
IRQ:	
	pha		; save hardware registers
	phx
	phy
	ldx	#19
1?:
	lda	R0,X
	pha
	dex
	bpl	1?
	ldx	#8
2?:
	lda	VAL-1,X
	pha
	dex
	bne	2?
	jsr	_IRQHandler
	ldx	#-8
3?:
	pla
	sta	VAL+8,X
	inx
	bne	3?
	ldx	#-20
4?:
	pla
	sta	R0+20,X
	inx
	bne	4?
	ply
	plx
	pla
	rti

    xdef	NMI
	xref	_NMIHandler
NMI:	
	pha		; save hardware registers
	phx
	phy
	ldx	#19
1?:
	lda	R0,X
	pha
	dex
	bpl	1?
	ldx	#8
2?:
	lda	VAL-1,X
	pha
	dex
	bne	2?
	jsr	_NMIHandler
	ldx	#-8
3?:
	pla
	sta	VAL+8,X
	inx
	bne	3?
	ldx	#-20
4?:
	pla
	sta	R0+20,X
	inx
	bne	4?
	ply
	plx
	pla
	rti

	****************************************************************
	*                                                              *
	*   This section defines the interrupt and reset vectors.      *
	*      The reset vectors always start at FFFA. Replace the     *
	*      zeroes with any routines that handle interrupts.        *
	*                                                              *
	****************************************************************

VECTORS	SECTION OFFSET $FFFA

V_NMI	dw	NMI
V_RESET	dw	START
V_IRQ	dw	IRQ

	ENDS

	END


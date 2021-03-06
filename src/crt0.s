; ---------------------------------------------------------------------------
; crt0.s
; ---------------------------------------------------------------------------
;
; Startup code for cc65 (GameTank version)

.export   _init, _exit
.import   _main

.export   __STARTUP__ : absolute = 1        ; Mark as startup
.import   __RAM_START__, __RAM_SIZE__       ; Linker generated

.import    copydata, zerobss, initlib, donelib

.PC02

BankReg = $2005
VIA = $2800
ORB = 0
ORAr = 1
DDRB = 2
DDRA = 3
T1C = 5
ACR = $B
PCR = $C
IFR = $D
IER = $E

DMA_Flags = $2007
DMA_VX = $4000
DMA_VY = $4001
DMA_GX = $4002
DMA_GY = $4003
DMA_WIDTH = $4004
DMA_HEIGHT = $4005
DMA_Status = $4006
DMA_Color = $4007

.include  "zeropage.inc"

; ---------------------------------------------------------------------------
; Place the startup code in a special segment

.segment  "STARTUP"

; ---------------------------------------------------------------------------
; A little light 6502 housekeeping

_init:    LDX     #$FF                 ; Initialize stack pointer to $01FF
          TXS
          CLD                          ; Clear decimal mode

    ldx #0
viaWakeup:
	inx
	bne viaWakeup

	LDA #0
	STA BankReg

	SEI
	LDA #%11111101
	STA DMA_Flags
	LDA #$7F
	STA DMA_WIDTH
	STA DMA_HEIGHT
	STZ DMA_VX
	STZ DMA_VY
	STZ DMA_GX
	STZ DMA_GY
	LDA #$FF
	STA DMA_Color
	LDA #1
	STA DMA_Status
	WAI

	LDA #%00000111
	STA VIA+DDRA
    LDA #$FF
    STA VIA+ORAr
	lda #$7E
	jsr ShiftHighBits

; ---------------------------------------------------------------------------
; Set cc65 argument stack pointer

          LDA     #<(__RAM_START__ + __RAM_SIZE__)
          STA     sp
          LDA     #>(__RAM_START__ + __RAM_SIZE__)
          STA     sp+1

; ---------------------------------------------------------------------------
; Initialize memory storage

          JSR     zerobss              ; Clear BSS segment
          JSR     copydata             ; Initialize DATA segment
          JSR     initlib              ; Run constructors

; ---------------------------------------------------------------------------
; Call main()

          JSR     _main

; ---------------------------------------------------------------------------
; Back from main (this is also the _exit entry):  force a software break

_exit:    JSR     donelib              ; Run destructors
          BRK

ShiftHighBits:
	STA $0
	LDA #$FF
	STA VIA+ORAr

	LDA $0
	ROR
	ROR
	ROR
	ROR
	ROR
	AND #2
	ORA #%00000100
	STA VIA+ORAr
	ORA #1
	STA VIA+ORAr

	LDA $0
	ROR
	ROR
	ROR
	ROR
	AND #2
	ORA #%00000100
	STA VIA+ORAr
	ORA #1
	STA VIA+ORAr

	LDA $0
	ROR
	ROR
	ROR
	AND #2
	ORA #%00000100
	STA VIA+ORAr
	ORA #1
	STA VIA+ORAr

	LDA $0
	ROR
	ROR
	AND #2
	ORA #%00000100
	STA VIA+ORAr
	ORA #1
	STA VIA+ORAr
	
	LDA $0
	ROR
	AND #2
	ORA #%00000100
	STA VIA+ORAr
	ORA #1
	STA VIA+ORAr

	LDA $0
	AND #2
	ORA #%00000100
	STA VIA+ORAr
	ORA #1
	STA VIA+ORAr

	LDA $0
	ROL
	AND #2
	STA VIA+ORAr
	ORA #1
	STA VIA+ORAr

	ORA #4
	STA VIA+ORAr

	RTS
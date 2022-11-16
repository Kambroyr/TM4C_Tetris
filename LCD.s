; LCD.s
; David Lau
; Last modification date: 3/29/22

; Runs on TM4C123
; Use SSI0 to send an 8-bit code to the ST7735 160x128 pixel LCD.

; Backlight (pin 10) connected to +3.3 V
; MISO (pin 9) unconnected
; SCK (pin 8) connected to PA2 (SSI0Clk)
; MOSI (pin 7) connected to PA5 (SSI0Tx)
; TFT_CS (pin 6) connected to PA3 (SSI0Fss)
; CARD_CS (pin 5) unconnected
; Data/Command (pin 4) connected to PA6 (GPIO)
; RESET (pin 3) connected to PA7 (GPIO)
; VCC (pin 2) connected to +3.3 V
; Gnd (pin 1) connected to ground

GPIO_PORTA_DATA_R       EQU   0x400043FC
SSI0_DR_R               EQU   0x40008008
SSI0_SR_R               EQU   0x4000800C

      EXPORT   writecommand
      EXPORT   writedata

      AREA    |.text|, CODE, READONLY, ALIGN=2
      THUMB
      ALIGN

; The Data/Command pin must be valid when the eighth bit is
; sent.  The SSI module has hardware input and output FIFOs
; that are 8 locations deep.  Based on the observation that
; the LCD interface tends to send a few commands and then a
; lot of data, the FIFOs are not used when writing
; commands, and they are used when writing data.  This
; ensures that the Data/Command pin status matches the byte
; that is actually being transmitted.
; The write command operation waits until all data has been
; sent, configures the Data/Command pin for commands, sends
; the command, and then waits for the transmission to
; finish.
; The write data operation waits until there is room in the
; transmit FIFO, configures the Data/Command pin for data,
; and then adds the data to the transmit FIFO.
; NOTE: These functions will crash or stall indefinitely if
; the SSI0 module is not initialized and enabled.

; This is a helper function that sends an 8-bit command to the LCD.
; Input: R0  8-bit command to transmit
; Output: none
; Assumes: SSI0 and port A have already been initialized and enabled
writecommand
;; --UUU-- Code to write a command to the LCD
;1) Read SSI0_SR_R and check bit 4, 
;2) If bit 4 is high, loop back to step 1 (wait for BUSY bit to be low)
;3) Clear D/C=PA6 to zero
;4) Write the command to SSI0_DR_R
;5) Read SSI0_SR_R and check bit 4, 
;6) If bit 4 is high, loop back to step 5 (wait for BUSY bit to be low)
	
	LDR R1, =SSI0_SR_R				
cloop1	
	LDR R2, [R1]					;	read SSI0_SR_R				
	AND R2, #0x10					;	mask out all bits except bit 4
	CMP R2, #0						;	if BUSY bit high, continuously check SSI0_SR_R
	BNE cloop1
	LDR R3, =GPIO_PORTA_DATA_R		;	else, set D/C to write a command
	LDR R2, [R3]
	BIC R2, #0x40					;	clear PA6
	STR R2, [R3]
	LDR R3, =SSI0_DR_R
	STR R0, [R3]					; 	write R0 to SSI0_DR_R
cloop2
	LDR R2, [R1]					;	read SSI0_SR_R
	AND R2, #0x10					;	mask out all bits except bit 4
;	LSR R2, #4
	CMP R2, #0
	BNE cloop2						;	if BUSY bit high, continuously check SSI0_SR_R
    BX  LR                          ;   else, return

; This is a helper function that sends an 8-bit data to the LCD.
; Input: R0  8-bit data to transmit
; Output: none
; Assumes: SSI0 and port A have already been initialized and enabled
writedata
;; --UUU-- Code to write data to the LCD
;1) Read SSI0_SR_R and check bit 1, 
;2) If bit 1 is low loop back to step 1 (wait for TNF bit to be high)
;3) Set D/C=PA6 to one
;4) Write the 8-bit data to SSI0_DR_R

    LDR R1, =SSI0_SR_R
dloop	
	LDR R2, [R1]			;	read SSI0_SR_R
	AND R2, #0x2			;	mask out all bits except bit 1
	LSR R2, #1
	CMP R2, #1			;	if TNF bit low, continuously check SSI0_SR_R
	BNE dloop
	LDR R3, =GPIO_PORTA_DATA_R	;	else, set D/C to write data
	LDR R2, [R3]
	ORR R2, #0x40			;	set PA6
	STR R2, [R3]
	LDR R3, =SSI0_DR_R
	STR R0, [R3]			; 	write R0 to SSI0_DR_R
	BX  LR                          ;   	return


    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file

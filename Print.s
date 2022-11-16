; Print.s
; David Lau
; Last modification date: 3/29/22
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
	PRESERVE8
    THUMB

;	Binding
FP RN 11
COUNT EQU 0
NUM EQU 4
POINT EQU 46
RISK EQU 42

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
; R0=0,    then output "0"
; R0=3,    then output "3"
; R0=89,   then output "89"
; R0=123,  then output "123"
; R0=9999, then output "9999"
; R0=4294967295, then output "4294967295"
LCD_OutDec
	PUSH {R4-R9, R11, LR}
	SUB SP, #8						;	Allocate two 32-bit variables
    MOV FP, SP						;	Set frame pointer
    CMP R0, #0						;	Check if R0 is zero
	BNE nonzero 					
	ADD R0, #0x30					;	If zero, out zero
	BL ST7735_OutChar
	B done							;	Return to main
nonzero
	STR R0, [FP, #NUM]				;	Store number on the stack
    MOV R4, #0
    STR R4, [FP, #COUNT]			;	Initialize count to zero
    MOV R5, #10
loop
    LDR R4, [FP, #NUM]				;	Access
    CMP R4, #0						;	Check to see if every digit has been outputted
    BEQ next
    UDIV R6, R4, R5					;	Modulo to isolate each digit of the number
    MUL R7, R6, R5
    SUB R8, R4, R7					;	Store remainder on the stack
    PUSH {R8}
    STR R6, [FP, #NUM]				;	Update NUM with the number sans least significant decimal digit
    LDR R6, [FP, #COUNT]			;	Increment the digit counter
    ADD R6, #1
    STR R6, [FP, #COUNT]
    B loop
next
    LDR R4, [FP, #COUNT]
    CMP R4, #0						;	Check the digit counter to see if all the digits have been printed
    BEQ done
    POP {R0}						;	Pop the digit off the stack
    ADD R0, #0x30					;	Convert to ASCII
    BL ST7735_OutChar				;	Output the character
    SUB R4, #1						;	Decrement the digit counter
    STR R4, [FP, #COUNT]
    B next
done
	ADD SP ,#8						;	Deallocate the stack
	POP {R4-R9, R11, LR}
	BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000"
;       R0=3,    then output "0.003"
;       R0=89,   then output "0.089"
;       R0=123,  then output "0.123"
;       R0=9999, then output "9.999"
;       R0>9999, then output "*.***"
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix
	PUSH {R4-R9, R11, LR}
	SUB SP, #8						;	Allocation
    MOV FP, SP
    CMP R0, #0						;	Check if R0 is zero
	BNE nonnull					
	ADD R0, #0x30					;	If zero, out zero decimal string "0.000"
	BL ST7735_OutChar
	MOV R0, #POINT
	BL ST7735_OutChar
	MOV R0, #0x30
	BL ST7735_OutChar
	MOV R0, #0x30
	BL ST7735_OutChar
	MOV R0, #0x30
	BL ST7735_OutChar
	B exit							;	Return to main
nonnull
	LDR R1, =9999
	CMP R0, R1						;	Check if R0 < 9999
	BLS range
	MOV R0, #RISK					;	If R0 > 9999, print string "*.***"
	BL ST7735_OutChar
	MOV R0, #POINT
	BL ST7735_OutChar
	MOV R0, #RISK
	BL ST7735_OutChar
	MOV R0, #RISK
	BL ST7735_OutChar
	MOV R0, #RISK
	BL ST7735_OutChar
	B exit							;	Return to main
range
	STR R0, [FP, #NUM]				;	Store number on the stack
    MOV R4, #0
    STR R4, [FP, #COUNT]			;	Initialize count to zero
    MOV R5, #10
	MOV R9, #4						;	Max number of printable digits
	MOV R7, #0						;	Current number of digits printed
floop
	LDR R4, [FP, #NUM]
    CMP R4, #0						;	Check to see if every digit has been outputted
	BEQ cont
	UDIV R6, R4, R5					;	Modulo to isolate each digit of the number
    MUL R7, R6, R5
    SUB R8, R4, R7					;	Store remainder on the stack
    PUSH {R8}
	SUB R9, #1						;	Decrement max digit counter
    STR R6, [FP, #NUM]				;	Update NUM with the number sans least significant decimal digit
    LDR R6, [FP, #COUNT]			;	Increment the digit counter
    ADD R6, #1
    STR R6, [FP, #COUNT]
    B floop
cont
    CMP R7, #1						;	Check if first digit has been printed
	BNE decimal						;	If decimal hasn't been printed, print the decimal point
	MOV R0, #POINT					;	Print the decimal point
	BL ST7735_OutChar
decimal	
	CMP R9, #0						;	Check if R0 originally had less than 4 digits
	BEQ max							;	If R0 had four digits, don't print filler zeroes
	MOV R0, #0x30
	BL ST7735_OutChar
	SUBS R9, #1						;	Decrement number of filler zeroes needed
	ADD R7, #1						;	Increment number of digits printed
	B cont
max	
	LDR R4, [FP, #COUNT]
    CMP R4, #0						;	Check the digit counter to see if all the digits have been printed
	BEQ exit
    POP {R0}						;	Pop the digit off the stack
    ADD R0, #0x30					;	Convert to ASCII
    BL ST7735_OutChar				;	Output the character
	SUB R4, #1						;	Decrement the digit counter
    STR R4, [FP, #COUNT]
	ADD R7, #1						;	Increment number of digits printed
    B cont
exit
	ADD SP ,#8						;	Deallocate the stack
	POP {R4-R9, R11, LR}
	BX   LR

	ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

	ALIGN                           ; make sure the end of this section is aligned
	END                             ; end of file

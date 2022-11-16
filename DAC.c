// dac.c
// This software configures DAC output
// Runs on LM4F120 or TM4C123
// David Lau
// Date Created: 3/6/17 
// Last Modified: 11/15/2021 

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data

// **************DAC_Init*********************
// Initialize 6-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void){
	// Initializes DAC on PB5-0
	SYSCTL_RCGCGPIO_R |= 0x00000002;	
	__nop();				
	__nop();
	GPIO_PORTB_AMSEL_R &= ~0x3F;		
	GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; 	
	GPIO_PORTB_DIR_R |= 0x3F;		
	GPIO_PORTB_AFSEL_R &= 0x3F; 		
	GPIO_PORTB_DEN_R |= 0x3F;		
	// Enables 8mA drive for Port B
	GPIO_PORTB_DR8R_R |= 0x3F;		
}

/* **************DAC_Out*********************
 output to DAC
 Input: 6-bit data, 0 to 63 
 Input=n is converted to n*3.3V/63
 Output: none */
void DAC_Out(uint32_t data){
	GPIO_PORTB_DATA_R = data;
}

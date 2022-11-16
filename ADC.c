// ADC.c
// Runs on TM4C123
// Provide functions that initialize ADC0
// Last Modified: 4/12/2022 
// David Lau

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

// ADC initialization function using PD2 
// Input: none
// Output: none
void ADC_Init(void){ 
// Initialize ADC to sample using busy-wait
	SYSCTL_RCGCADC_R |= 0x01;
	SYSCTL_RCGCGPIO_R |= 0x08;
	while((SYSCTL_PRGPIO_R&0x08) == 0){};
	GPIO_PORTD_DIR_R &= ~0x04;
	GPIO_PORTD_AFSEL_R |= 0x04;					
	GPIO_PORTD_DEN_R &= ~0x04;					
	GPIO_PORTD_AMSEL_R |= 0x04;					
	ADC0_PC_R = 0x01;							
	ADC0_SSPRI_R = 0x0123;						
	ADC0_ACTSS_R &= ~0x0008;						
	ADC0_EMUX_R &= ~0xF000;							
	ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0) + 5;
	ADC0_SSCTL3_R = 0x0006;
	ADC0_IM_R &= ~0x0008;
	ADC0_ACTSS_R |= 0x0008;
//	ADC0_SAC_R = 5;
}

/*------------ADC_In------------
 Busy-wait Analog to digital conversion
 Input: none
 Output: 12-bit result of ADC conversion
 measures from PD2, analog channel 5 */
uint32_t ADC_In(void){  
	uint32_t result;
	ADC0_PSSI_R = 0x0008;							
	while((ADC0_RIS_R&0x08) == 0){};					
	result = ADC0_SSFIFO3_R & 0xFFF;					
	ADC0_ISC_R = 0x0008;							
	return result;
}



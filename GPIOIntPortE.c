#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

unsigned long Touch;     // true on touch
unsigned long Release;   // true on release
unsigned long Last;      // previous
uint32_t fallingEdges = 0;

void GPIOArm(void);
	
void PortE_Init(void){
	// Configure PE2-0 for falling edge interrupts
	SYSCTL_RCGCTIMER_R |= 0x01;
	SYSCTL_RCGCGPIO_R |= 0x10;
	__nop();
	__nop();
	fallingEdges = 0;
	GPIO_PORTE_DIR_R &= ~0x07;
	GPIO_PORTE_AFSEL_R &= 0x07;
	GPIO_PORTE_DEN_R |= 0x07;
	GPIO_PORTE_PCTL_R &= ~0x0FFF;
	GPIO_PORTE_AMSEL_R = 0;
	GPIO_PORTE_PUR_R |= 0x07;
	GPIO_PORTE_IS_R &= ~0x07;
	GPIO_PORTE_IBE_R &= ~0x07;
	GPIO_PORTE_IEV_R &= ~0x07;
	GPIO_PORTE_ICR_R = 0x07;
	GPIO_PORTE_IM_R |= 0x07;
	// Priority 5
	NVIC_PRI1_R = (NVIC_PRI1_R & 0xFFFFFF0F)|0x000000A0;
	NVIC_EN0_R = 0x00000010;
	GPIOArm();
}

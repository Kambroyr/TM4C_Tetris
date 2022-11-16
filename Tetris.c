// Tetris.c
// Runs on TM4C123
// Last Modified: 5/4/2022 by David Lau

// ******* Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// buttons connected to PE0-PE3
// 32*R resistor DAC bit 0 on PB0 (least significant bit)
// 16*R resistor DAC bit 1 on PB1
// 8*R resistor DAC bit 2 on PB2 
// 4*R resistor DAC bit 3 on PB3
// 2*R resistor DAC bit 4 on PB4
// 1*R resistor DAC bit 5 on PB5 (most significant bit)

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Print.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "Images.h"
#include "Sound.h"
#include "Systick.h"
#include "GPIOIntPortE.h"
#include "Timer0.h"
#include "Timer1.h"

#define drop 0
#define clear 1

const uint16_t TetrisLogo[] = {
 0x74D2, 0x74B1, 0x74B1, 0x74B1, 0x74D1, 0x7D13, 0x7D33, 0x7D33, 0x7CF2, 0x74B1, 0x74B1, 0x7CF2, 0x7D33, 0x7D33, 0x74D1, 0x7D33,
 0x7D33, 0x7D13, 0x74B1, 0x74B1, 0x74B1, 0x74D2, 0x7D33, 0x7D33, 0x74D2, 0x74B1, 0x74B1, 0x7CF2, 0x7D33, 0x7D13, 0x7D13, 0x7D33,
 0x7D33, 0x7D33, 0x74F2, 0x7CF2, 0x7D33, 0x7D33, 0x7CF2, 0x7D33, 0x7D33, 0x7D33, 0x7CF2, 0x7D33, 0x7D12, 0x74D1, 0x74B1, 0x74B1,
 0x74B1, 0x74D1, 0x8574, 0xB73B, 0xAED9, 0xAEFA, 0xA6B9, 0x640F, 0x536D, 0x536D, 0x8D74, 0xB73B, 0xAF1A, 0x8574, 0x534C, 0x534C,
 0x95D5, 0x4AEB, 0x538D, 0x534C, 0xBFBD, 0xB71A, 0xAEFA, 0xA698, 0x4B2C, 0x4B2C, 0xA678, 0xAEFA, 0xB73B, 0x7CF2, 0x534C, 0x640F,
 0x7471, 0x536D, 0x5B8D, 0x4B2C, 0x7CF2, 0x8D74, 0x534C, 0x63EE, 0x8DB5, 0x536D, 0x536D, 0x534C, 0x8DB5, 0x4B2B, 0x7CF2, 0xB71A,
 0xAEF9, 0xAED9, 0xB73B, 0x7D13, 0x7D53, 0xA6B9, 0x9E78, 0xA698, 0x9E37, 0x10C2, 0x0000, 0x0000, 0x5BAD, 0xB71A, 0xAED9, 0x5BAD,
 0x0000, 0x0000, 0xB73B, 0x3228, 0x0000, 0x0000, 0xCFFE, 0xAED9, 0xA6B8, 0x95D5, 0x0000, 0x0000, 0x8DB5, 0xA698, 0xB71A, 0x42AA,
 0x0000, 0x10C2, 0x2185, 0x0000, 0x0000, 0x0020, 0x9E57, 0x5B8D, 0x0000, 0x0861, 0x640F, 0x0000, 0x0000, 0x6C30, 0x2186, 0x0000,
 0x0000, 0x95D5, 0xA6B9, 0x9E78, 0xAED9, 0x7512, 0x7D53, 0xAED9, 0xA678, 0xA698, 0x9E37, 0x2165, 0x0861, 0x0861, 0x5BCE, 0xB71A,
 0xAED9, 0x63CE, 0x0020, 0x0000, 0xB73B, 0xC7DD, 0x63EF, 0x0000, 0xC7FE, 0xAED9, 0xA6B9, 0x95D6, 0x0000, 0x0000, 0x8DB5, 0xA6B9,
 0xAF1A, 0x4AEB, 0x0041, 0x2144, 0x3228, 0x0041, 0x08A2, 0x9E57, 0xBFBD, 0x5B8D, 0x0020, 0x1924, 0x6C4F, 0x0000, 0x4AEB, 0x4289,
 0x0040, 0x0020, 0x0000, 0x8D95, 0xA6B9, 0xA678, 0xAED9, 0x7D12, 0x7D53, 0xAED9, 0xA678, 0xA698, 0x95F6, 0x4B2B, 0x5BEE, 0x430B,
 0x534C, 0xB73A, 0xAED9, 0x63EF, 0x53AE, 0x10A2, 0xBF7C, 0x63EE, 0x7470, 0x7CD2, 0xAEFA, 0xA698, 0xA6B9, 0x8D94, 0x42EB, 0x1965,
 0x8D74, 0xA6B9, 0xAEFA, 0x536C, 0x53CE, 0x3228, 0x3A69, 0x2145, 0xA698, 0xB75B, 0xAF1A, 0x5B8D, 0x53AD, 0x3A89, 0x534C, 0x3A69,
 0x6C70, 0x4B4C, 0x5C4F, 0x0000, 0x4AEB, 0xAED9, 0xA678, 0xA678, 0xAED9, 0x7D12, 0x7D53, 0xAED9, 0xA678, 0xA698, 0x95F6, 0x42EB,
 0x53AD, 0x42CA, 0x532C, 0xB73A, 0xAED9, 0x63EF, 0x4B8D, 0x1103, 0x7CF2, 0x2185, 0x536C, 0xAF1A, 0xA678, 0xA678, 0xA6B9, 0x8574,
 0x3AAA, 0x1944, 0x8D74, 0xA6B9, 0xAEFA, 0x536C, 0x4B8D, 0x3228, 0x3207, 0x10A2, 0x4ACA, 0x7CF2, 0xBF9C, 0x5B6D, 0x4B6C, 0x3207,
 0x7CF2, 0x7CD2, 0x3ACA, 0x53CE, 0x29C6, 0x8D95, 0x9E37, 0xA6B9, 0xA678, 0xA678, 0xAED9, 0x7D12, 0x7D53, 0xAED9, 0xA698, 0xA6B9,
 0x95D6, 0x4B0B, 0x53EE, 0x430B, 0x532C, 0xB73A, 0xB71A, 0x63EF, 0x53CD, 0x1924, 0x7CF2, 0x2185, 0x536D, 0xAF1A, 0xA678, 0xA698,
 0xAED9, 0x8574, 0x42EA, 0x1965, 0x8574, 0xA6B9, 0xB73A, 0x536D, 0x53CE, 0x3228, 0x3A69, 0x29A6, 0x29A6, 0x63EF, 0xBF7B, 0x5B8D,
 0x53AD, 0x3207, 0x8DB5, 0x74B1, 0x432B, 0x4B6D, 0x3A48, 0x8D94, 0x9616, 0xA6B9, 0xA678, 0xA678, 0xAED9, 0x7D12, 0x7D33, 0xB73A,
 0x6C50, 0xA699, 0x95D6, 0x5BAD, 0x7533, 0x5BCE, 0x4B0B, 0xC7FD, 0x6C2F, 0x6C70, 0x7512, 0x1903, 0xBF5B, 0x5BCE, 0x640F, 0x8D74,
 0x7D13, 0x7CF2, 0xA698, 0x8574, 0x5BEE, 0x3227, 0x8554, 0xA6D9, 0x8D74, 0x534C, 0x7512, 0x3A69, 0x534C, 0x5C0F, 0x7512, 0x3A48,
 0x8DB5, 0x63CE, 0x74F2, 0x42CA, 0x7470, 0x5BEE, 0x6C90, 0x4AEB, 0x8D74, 0x0000, 0x4B2B, 0xB73A, 0x9E78, 0x9E78, 0xAED9, 0x7D12,
 0x7D33, 0xBF5B, 0x4B0B, 0x5B8D, 0xA698, 0x536C, 0x6CD1, 0x538C, 0x536D, 0xA6B8, 0x0000, 0x7CF2, 0x6490, 0x10E3, 0xAEFA, 0xB71A,
 0x74B1, 0x0881, 0x8533, 0x4AEB, 0x8D94, 0x8D95, 0x538D, 0x29E6, 0x8D94, 0x95F6, 0x2985, 0x536C, 0x6CB1, 0x3248, 0x4B0B, 0x4B6C,
 0x6CD1, 0x3289, 0x8533, 0x5BCE, 0x6470, 0x42CA, 0x3207, 0x7532, 0x42CA, 0x536D, 0x74F2, 0x0000, 0x536D, 0xBF9C, 0xA6B8, 0xAED9,
 0xB71A, 0x7512, 0x7D33, 0xB73B, 0x640F, 0x7D13, 0x6C50, 0x7D13, 0xBF9C, 0x8D95, 0x3207, 0x95D6, 0x1924, 0x7CD2, 0xB75B, 0x2985,
 0xA698, 0x6C70, 0xAED9, 0x1903, 0x6C70, 0x8DB5, 0x640F, 0x7CF2, 0x9616, 0x532C, 0x8D95, 0x532B, 0x5BAD, 0x536C, 0xB75B, 0x42AA,
 0x5BAD, 0x9595, 0xA6B9, 0x6C0F, 0x9E57, 0x5BAD, 0xB75B, 0x536C, 0x536C, 0x7CD2, 0x4ACA, 0x8D94, 0xD7FF, 0x0000, 0x536D, 0x74B1,
 0x8D95, 0x6C30, 0x8D74, 0x7D33, 0x7D53, 0xBF5B, 0x7CF2, 0x8574, 0x6C0F, 0x8513, 0x8554, 0x7CD2, 0x5BAE, 0x8D74, 0x5B6D, 0x8D95,
 0x8553, 0x5B8D, 0x8D74, 0x7470, 0x8DB5, 0x534C, 0x95D6, 0x8574, 0x7CF2, 0x7491, 0x7D12, 0x63EE, 0x7CD2, 0x7CD2, 0x74B1, 0x7CD2,
 0x8553, 0x6C0F, 0x7491, 0x7CD2, 0x6C2F, 0x8D75, 0xB75B, 0x7CF2, 0x8533, 0x6C0F, 0xA678, 0x5BAD, 0x8533, 0x7D12, 0x95B5, 0x42AA,
 0x7CD2, 0x7470, 0x8D94, 0x6C30, 0x9DF6, 0x7D33, 0x8574, 0x8594, 0x8DB5, 0x8574, 0x8DB5, 0x8594, 0x8574, 0x8594, 0x8DD5, 0x8574,
 0x8DD5, 0x8DB5, 0x8574, 0x8DB5, 0x8594, 0x8594, 0x8574, 0x8DD5, 0x8DB5, 0x8594, 0x8574, 0x8DB5, 0x8594, 0x8DB5, 0x8DB5, 0x8594,
 0x8DB5, 0x8DB5, 0x8574, 0x8DB5, 0x8DB5, 0x8594, 0x8D94, 0x8DB5, 0x8574, 0x8DB5, 0x8574, 0x8DB5, 0x8DB5, 0x8DD5, 0x8595, 0x8574,
 0x8574, 0x8DD6, 0x8DB5, 0x8594, 0x8594, 0x8DD5, 0x8DB5, 0x7D53,
};





const uint16_t blackSquare[] = {
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000,
};
const uint16_t pearSquare[] = {
	0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678,
	0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678,
	0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678,
	0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678,
	0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678,
	0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678, 0xA678,
	0xA678, 0xA678, 0xA678, 0xA678,
};

struct orientation{
	uint8_t coord[4][2];
};
typedef struct orientation orientation_t;

struct piece{
	const uint16_t *color;
	orientation_t pos[4];
};
typedef struct piece piece_t;

const piece_t pieces[7] = {
	// Line piece
	{pearSquare, {{0, 10, 10, 10, 20, 10, 30, 10},			
	{20, 10, 20, 20, 20, 30, 20, 40},
	{0, 20, 10, 20, 20, 20, 30, 20},
	{10, 10, 10, 20, 10, 30, 10, 40}}},
	// S piece
	{pearSquare, {{10, 10, 20, 10, 0, 20, 10, 20},			
	{10, 10, 10, 20, 20, 20, 20, 30},
	{10, 20, 20, 20, 0, 30, 10, 30},
	{0, 10, 0, 20, 10, 20, 10, 30}}},
	// L piece
	{pearSquare, {{0, 10, 0, 20, 10, 20, 20, 20}, 			
	{10, 10, 20, 10, 10, 20, 10, 30}, 
	{0, 20, 10, 20, 20, 20, 20, 30}, 
	{10, 10, 10, 20, 0, 30, 10, 30}}}, 
	// J piece
	{pearSquare, {{20, 10, 0, 20, 10, 20, 20, 20},			
	{10, 10, 10, 20, 10, 30, 20, 30},
	{0, 20, 10, 20, 20, 20, 0, 30},
	{0, 10, 10, 10, 10, 20, 10, 30}}},
	// T piece
	{pearSquare, {{10, 10, 0, 20, 10, 20, 20, 20},			
	{10, 10, 10, 20, 20, 20, 10, 30},
	{0, 20, 10, 20, 20, 20, 10, 30},
	{10, 10, 0, 20, 10, 20, 10, 30}}}, 
	// Z piece
	{pearSquare, {{0, 10, 10, 10, 10, 20, 20, 20},			
	{20, 10, 10, 20, 20, 20, 10, 30},
	{0, 20, 10, 20, 10, 30, 20, 30},
	{10, 10, 0, 20, 10, 20, 0, 30}}},
	// Square piece
	{pearSquare, {{0, 10, 10, 10, 0, 20, 10, 20}, 			
	{0, 10, 10, 10, 0, 20, 10, 20}, 
	{0, 10, 10, 10, 0, 20, 10, 20}, 
	{0, 10, 10, 10, 0, 20, 10, 20}}},
};

uint16_t rows[16]; 

static int16_t y0 = -20, y1 = -20, y2 = -20, y3 = -20;
static int16_t dx0 = 50, dx1 = 50, dx2 = 50, dx3 = 50;

// Used to keep track of score
static uint32_t linesCleared = 0;

static uint8_t language = 0;

// Index of current shape
uint32_t shape;						
// Index of current shape's orientation
static int8_t way = 0;					
// ADC mailbox
uint32_t ADCMail;					

void DisableInterrupts(void);				
void EnableInterrupts(void);				
// Initialize Port E to be used for edge-triggered interrupts
void PortE_Init(void);					
void drawPiece(uint8_t i, uint8_t rot);
void clearPiece(void);
void clearRotPiece(void);
void GPIOPortE_Handler(void);
void rowClear(int16_t ele);

// Flags for which switch has been pressed
static uint32_t sw0, sw1, sw2 = 0;				
void GPIOPortE_Handler(void){
	GPIO_PORTE_IM_R &= ~0x07;
	Timer0Arm();
	clearRotPiece();
	// Sets either clockwise rotation, counterclockwise rotation, or drop flag
	if((GPIO_PORTE_RIS_R & 0x01) == 1){
		sw0++;
	} else if((GPIO_PORTE_RIS_R & 0x02) >> 1 == 1){
		sw1++;
	} else if((GPIO_PORTE_RIS_R & 0x04) >> 2 == 1){
		sw2++;
	}		
	// Acknowledge GPIO interrupt
	GPIO_PORTE_ICR_R = 0x07;				
	fallingEdges++;
}

static int16_t savedY0 = -10, savedY1 = -10, savedY2 = -10, savedY3 = -10;
static int16_t savedX0 = 0, savedX1 = 0, savedX2 = 0, savedX3 = 0;
void clearPiece(void){
	// Clears the previous four squares drawn
	if(savedY0 != y0){
		ST7735_DrawBitmap(savedX0, savedY0, blackSquare, 10, 10);
	}
	if(savedY1 != y1){
		ST7735_DrawBitmap(savedX1, savedY1, blackSquare, 10, 10);
	}
	if(savedY2 != y2){
		ST7735_DrawBitmap(savedX2, savedY2, blackSquare, 10, 10);
	}
	if(savedY3 != y3){
		ST7735_DrawBitmap(savedX3, savedY3, blackSquare, 10, 10);
	}
}

// Clears the four old squares that were printed
void clearRotPiece(void){
	ST7735_DrawBitmap(savedX0, y0, blackSquare, 10, 10);
	ST7735_DrawBitmap(savedX1, y1, blackSquare, 10, 10);
	ST7735_DrawBitmap(savedX2, y2, blackSquare, 10, 10);
	ST7735_DrawBitmap(savedX3, y3, blackSquare, 10, 10);
}

// Draws the four squares that make up each piece
void drawPiece(uint8_t i, uint8_t rot){
	ST7735_DrawBitmap(dx0, y0, pieces[i].color, 10, 10);
	ST7735_DrawBitmap(dx1, y1, pieces[i].color, 10, 10);
	ST7735_DrawBitmap(dx2, y2, pieces[i].color, 10, 10);
	ST7735_DrawBitmap(dx3, y3, pieces[i].color, 10, 10);
}

// Read ADC input and sets semaphore to increase drop speed if previous interrupt is finished
void SysTick_Handler(void){
	ADCMail = ADC_In();		
	if(sysFlag == 0){
		dy += 2;
		sysFlag = 1;
	}
}

// Function to clear rows that are completely filled
void rowClear(int16_t ele){
	linesCleared++;
	playsound(clear);
	ST7735_FillRect(0, (ele-10), 100, 10, ST7735_BLACK);
	rows[(ele/10)-1] = 0x0000;
	uint16_t temp = rows[(ele/10)-1];
	for(uint8_t i = ((ele/10)-1); i > 0; i--){
		rows[i] = rows[i - 1];
	}
	rows[0] = temp;
	static int16_t bound = 0;
	for(int16_t i = 15; i > 0; i--){
		if(rows[i] == 0){
			bound = i;
			break;
		}
	}
	// Draw black rectangle of width 100 and height (15 - bound) * 10 at x = 0, and y = (bound + 1) * 10
	int16_t height = (16 - bound) * 10;
	bound *= 10;
	ST7735_FillRect(0, bound, 100, height, ST7735_BLACK);
	// Then redraw all the squares from ele to bound shifted down by 10 pixels
	for(int8_t num = 15; num > (15 - ((height/10) - 1)); num--){
		for(uint8_t shift = 0; shift < 10; shift++){
			uint16_t bit = ((rows[num] >> shift) & 0x0001);
			if(bit == 1){
				ST7735_DrawBitmap((shift*10), ((num + 1)*10), pearSquare, 10, 10); 
			}
		}
	}
}


// Main game engine
int main(void){						
	// Initialize game board buffer to be empty
	for(uint16_t i = 0; i < 16; i++){		
		rows[i] = 0;
	}
	DisableInterrupts();
	// Enable inputs, outputs, and interrupts
	TExaS_Init(NONE);
	PortE_Init();					
	Output_Init();					
	ADC_Init();					
	// Set random seed by waiting for button to be pressed to exit start menu
	uint32_t seed = 0;
	EnableInterrupts();
	ST7735_FillScreen(0xA678);
	ST7735_DrawBitmap(40, 60, TetrisLogo, 50, 12);
	ST7735_SetCursor(3, 7);
	ST7735_OutString("Press any button");
	ST7735_SetCursor(7, 8);
	ST7735_OutString("to begin.");
	// Start screen
	while(1){
		if(sw0 == 1 || sw1 == 1 || sw2 == 1){
			break;
		} else {
			seed++;
		}
	}
	sw0 = 0, sw1 = 0, sw2 = 0;
	ST7735_FillScreen(0xA678);
	ST7735_SetCursor(0, 0);
	ST7735_OutString("Choose a language:");
	ST7735_SetCursor(0, 1);
	ST7735_OutString("English (red button)");
	ST7735_SetCursor(0, 2);
	ST7735_OutString("Espanol (boton azul)");
	// Language select screen
	while(1){
		if(sw0 == 1){
			language = 0;
			break;
		}
		if(sw1 ==1){
			language = 1;
			break;
		}
	}
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_FillRect(100, 0, 28, 160, 0xA678);
	sw0 = 0, sw1 = 0, sw2 = 0;
	DisableInterrupts();
	// Enable RNG, sound interrupts, and piece updates
	Random_Init(seed);					
	SysTick_Init();						
	Sound_Init();						
	EnableInterrupts();
	shape = (Random32() >> 24)%7;				
	while(1){
		if(rows[0] != 0){				
			break;
		}
		// Check to see if piece has hit bottom of the board or another piece
		if(y0 == 160||y1 == 160||y2 == 160||y3 == 160||((rows[((y0 + 10)/10)-1]>>(dx0/10))&0x0001) == 1||((rows[((y1 + 10)/10)-1]>>(dx1/10))&0x0001) == 1||((rows[((y2 + 10)/10)-1]>>(dx2/10))&0x0001) == 1||((rows[((y3 + 10)/10)-1]>>(dx3/10))&0x0001) == 1){
			clearPiece();
			drawPiece(shape, way);
			// Update game board buffer and check if each row is full
			rows[(y0/10)-1] = rows[(y0/10)-1] | (0x0001 << (dx0/10));
			// If row is full, clear the array and update the board
			if(rows[(y0/10)-1] == 0x03FF){
				rowClear(y0);
			}
			rows[(y1/10)-1] = rows[(y1/10)-1] | (0x0001 << (dx1/10));
			if(rows[(y1/10)-1] == 0x03FF){
				rowClear(y1);
			}
			rows[(y2/10)-1] = rows[(y2/10)-1] | (0x0001 << (dx2/10));
			if(rows[(y2/10)-1] == 0x03FF){
				rowClear(y2);
			}
			rows[(y3/10)-1] = rows[(y3/10)-1] | (0x0001 << (dx3/10));
			if(rows[(y3/10)-1] == 0x03FF){
				rowClear(y3);
			}
			dy = -20;
			// Choose a new piece to be dropped
			shape = (Random32() >> 4)%7;	
			// Reset y coordinate
			y0 = -10;						
			y1 = -10;
			y2 = -10;
			y3 = -10;
			savedY0 = -10;
			savedY1 = -10;
			savedY2 = -10;
			savedY3 = -10;
			// Reset x coordinates
			dx0 = 0;						
			dx1 = 0;
			dx2 = 0;
			dx3 = 0;
			savedX0 = -10;
			savedX1 = -10;
			savedX2 = -10;
			savedX3 = -10;
		}
		clearPiece();
		drawPiece(shape, way);
		/* Acknowledge rotate request by clearing semaphore and either rotate piece in correct direction or 
		drop the piece */
		if(sw0 == 1){
			sw0 = 0;		
			way = (way + 1)%4;
		} else if(sw1 == 1){
			sw1 = 0;
			way = (way - 1);
			if(way < 0){				
				way *= -3;
			}
			way = way%4;
		} else if(sw2 == 1){
			sw2 = 0;
			savedX0 = dx0;
			savedX1 = dx1;
			savedX2 = dx2;
			savedX3 = dx3;
			clearRotPiece();
			while(y0 != 160&&y1 != 160&&y2 != 160&&y3 != 160&&((rows[((y0 + 10)/10)-1]>>(dx0/10))&0x0001) != 1&&((rows[((y1 + 10)/10)-1]>>(dx1/10))&0x0001) != 1&&((rows[((y2 + 10)/10)-1]>>(dx2/10))&0x0001) != 1&&((rows[((y3 + 10)/10)-1]>>(dx3/10))&0x0001) != 1){
				// Increase y until it hits the bottom border or another piece
				dy += 2;
				y0 = pieces[shape].pos[way].coord[0][1] + dy;
				y1 = pieces[shape].pos[way].coord[1][1] + dy;
				y2 = pieces[shape].pos[way].coord[2][1] + dy;
				y3 = pieces[shape].pos[way].coord[3][1] + dy;
			}
			// Draw the piece at its new location
			playsound(drop);
			drawPiece(way, shape);
			// Update game board buffer and check if each row is full
			rows[(y0/10)-1] = rows[(y0/10)-1] | (0x0001 << (dx0/10));
			// If row is full, clear the array and update the board
			if(rows[(y0/10)-1] == 0x03FF){
				rowClear(y0);
			}
			rows[(y1/10)-1] = rows[(y1/10)-1] | (0x0001 << (dx1/10));
			if(rows[(y1/10)-1] == 0x03FF){
				rowClear(y1);
			}
			rows[(y2/10)-1] = rows[(y2/10)-1] | (0x0001 << (dx2/10));
			if(rows[(y2/10)-1] == 0x03FF){
				rowClear(y2);
			}
			rows[(y3/10)-1] = rows[(y3/10)-1] | (0x0001 << (dx3/10));
			if(rows[(y3/10)-1] == 0x03FF){
				rowClear(y3);
			}
			dy = -20;
			// Choose a new piece to be generated and reset both x and y coordinates
			shape = (Random32() >> 4)%7;	
			y0 = -10;
			y1 = -10;
			y2 = -10;
			y3 = -10;
			savedY0 = -10;
			savedY1 = -10;
			savedY2 = -10;
			savedY3 = -10;
			dx0 = 0;
			dx1 = 0;
			dx2 = 0;
			dx3 = 0;
			savedX0 = -10;
			savedX1 = -10;
			savedX2 = -10;
			savedX3 = -10;
		}
		// Update old x and y coordinates
		savedX0 = dx0;				
		savedX1 = dx1;
		savedX2 = dx2;
		savedX3 = dx3;
		savedY0 = y0;
		savedY1 = y1;
		savedY2 = y2;
		savedY3 = y3;
		if(sysFlag == 1){
			// Convert ADC input to proper column of the board
			if(ADCMail < 410){
				dx0 = 0;
			} else if(ADCMail < 820 && ADCMail > 410){
				dx0 = 10;
			} else if(ADCMail < 1230 && ADCMail > 820){
				dx0 = 20;
			} else if(ADCMail < 1640 && ADCMail > 1230){
				dx0 = 30;
			} else if(ADCMail < 2050 && ADCMail > 1640){
				dx0 = 40;
			} else if(ADCMail < 2460 && ADCMail > 2050){
				dx0 = 50;
			} else if(ADCMail < 2870 && ADCMail > 2460){
				dx0 = 60;
			} else if(ADCMail < 3280 && ADCMail > 2870){
				dx0 = 70;
			} else if(ADCMail < 3690 && ADCMail > 3280){
				dx0 = 80;
			} else if(ADCMail > 3690){
				dx0 = 90;
			}
			dx1 = pieces[shape].pos[way].coord[1][0] - pieces[shape].pos[way].coord[0][0] + dx0;
			dx2 = pieces[shape].pos[way].coord[2][0] - pieces[shape].pos[way].coord[0][0] + dx0;
			dx3 = pieces[shape].pos[way].coord[3][0] - pieces[shape].pos[way].coord[0][0] + dx0;
			// Check to make sure piece is within horizontal bounds of (0,90) and isn't hitting another piece to its right/left
			while(dx1 > 90 || dx2 > 90 || dx3 > 90 ||((rows[((y0 + 10)/10)-1]>>(dx0/10))&0x0001) == 1||((rows[((y1 + 10)/10)-1]>>(dx1/10))&0x0001) == 1||((rows[((y2 + 10)/10)-1]>>(dx2/10))&0x0001) == 1||((rows[((y3 + 10)/10)-1]>>(dx3/10))&0x0001) == 1){
				dx0 -=10;
				dx1 = pieces[shape].pos[way].coord[1][0] - pieces[shape].pos[way].coord[0][0] + dx0;
				dx2 = pieces[shape].pos[way].coord[2][0] - pieces[shape].pos[way].coord[0][0] + dx0;
				dx3 = pieces[shape].pos[way].coord[3][0] - pieces[shape].pos[way].coord[0][0] + dx0;
			}
			while(dx1 < 0 || dx2 < 0 || dx3 < 0 ||((rows[((y0 + 10)/10)-1]>>(dx0/10))&0x0001) == 1||((rows[((y1 + 10)/10)-1]>>(dx1/10))&0x0001) == 1||((rows[((y2 + 10)/10)-1]>>(dx2/10))&0x0001) == 1||((rows[((y3 + 10)/10)-1]>>(dx3/10))&0x0001) == 1){
				dx0 += 10;
				dx1 = pieces[shape].pos[way].coord[1][0] - pieces[shape].pos[way].coord[0][0] + dx0;
				dx2 = pieces[shape].pos[way].coord[2][0] - pieces[shape].pos[way].coord[0][0] + dx0;
				dx3 = pieces[shape].pos[way].coord[3][0] - pieces[shape].pos[way].coord[0][0] + dx0;
			}
			// Update new y coordinates to be printed
			y0 = pieces[shape].pos[way].coord[0][1] + dy;
			y1 = pieces[shape].pos[way].coord[1][1] + dy;
			y2 = pieces[shape].pos[way].coord[2][1] + dy;
			y3 = pieces[shape].pos[way].coord[3][1] + dy;
			// Acknowledge sysTick interrupt by clearing flag
			sysFlag = 0;		
		}
	}
	// Ensures proper language output based off of player choice
	if(language == 0){
		ST7735_FillScreen(0xA678);
		ST7735_SetCursor(6,4);
		ST7735_OutString("Game Over");
		ST7735_SetCursor(1,5);
		ST7735_OutString("You cleared ");
		LCD_OutDec(linesCleared);
		ST7735_OutString(" lines");
	} else if(language == 1){
		ST7735_FillScreen(0xA678);
		ST7735_SetCursor(3,4);
		ST7735_OutString("Juego terminado");
		ST7735_SetCursor(3,5);
		ST7735_OutString("usted despejo ");
		LCD_OutDec(linesCleared);
		ST7735_SetCursor(7, 6);
		ST7735_OutString(" lineas");
	}
}


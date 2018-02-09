#include "stm32l476xx.h"
#include "lcd.h"

/* Spencer Goulette
	 02/07/18
	 ECE 271 LAB2
	 Objective: Have 6 Letters of last name to display on LCD, have 'ECE217' display on LCD, and be able to scroll left and right on a string that is displayed on the LCD
	 Clock Frequency: 32KHz
*/

void System_Clock_Init(void);

int main(void){
	LCD_Initialization();	//Initializes LCD
	//Function that takes in string and will allow you to scroll left and right to view it
	LCD_Joystick((uint8_t*)"TOOK YOU LONG ENOUGH TO CHECK OFF LAB 2", 39);	//Also displays two pairs of 6 characters on the LCD
}

void System_Clock_Init(void){
	
	// Enable High Speed Internal Clock (HSI = 16 MHz)
  RCC->CR |= ((uint32_t)RCC_CR_HSION);
	
  // wait until HSI is ready
  while ( (RCC->CR & (uint32_t) RCC_CR_HSIRDY) == 0 );
	
  // Select HSI as system clock source 
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSI;  //01: HSI16 oscillator used as system clock

  // Wait till HSI is used as system clock source 
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) == 0 );
		
}


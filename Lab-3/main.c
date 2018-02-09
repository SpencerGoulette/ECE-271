#include "stm32l476xx.h"
#include "lcd.h"

/* Spencer Goulette
	 02/08/18
	 ECE 271 LAB3
	 Objective: Have 6 Letters of last name to display on LCD, have 'ECE217' display on LCD, and be able to scroll left and right on a string that is displayed on the LCD
	 Pins: Port A pins used for Input of Buttons, and Port E pins used for output of Buttons
	 Clock Frequency: 16MHz
*/

void System_Clock_Init(void);

int main(void){
	LCD_Initialization();	//Initializes LCD
	Keypad_Print();	//Function that checks for press of button on keypad and prints that button to LCD
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


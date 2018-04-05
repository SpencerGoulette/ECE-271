#include "stm32l476xx.h"
#include "lcd.h"

/* Spencer Goulette
	 02/15/18
	 ECE 271 LAB4
	 Objective: Have the Motor Turn using the half step and full step method. Also be able to enter degree that you want the motor to rotate and half it rotate that amount of degrees
	 Clock Frequency: 32KHz
*/

void System_Clock_Init(void);
int keyPad(int degree);

int keyPad(int degree){
	double rotate;
	LCD_Initialization();	//Initializes LCD
	GPIOB->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3 | GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
	GPIOB->MODER |= (GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0 | GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0);
	while(1)
	{
		LCD_Clear();
		rotate = Keypad_Print();
		degree = (int)(rotate/360*512);
		return degree;
	}
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
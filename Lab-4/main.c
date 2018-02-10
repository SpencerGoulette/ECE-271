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
	int i, j, timeDelay;
	double degree, rotations;
	LCD_Initialization();	//Initializes LCD
	GPIOB->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3 | GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
	GPIOB->MODER |= (GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0 | GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0);
	while(1)
	{
		degree = Keypad_Print();
		rotations = degree/360*512;
		
		for(j = 0; j < rotations; j++)
		{
			for(i = 0; i < 4; i++)
			{
				for(timeDelay = 0; timeDelay < 2500; timeDelay++);
				GPIOB->ODR &= ~(GPIO_ODR_OD2 | GPIO_ODR_OD3 | GPIO_ODR_OD6 | GPIO_ODR_OD7);
				switch(i)
				{
					case 0:
						GPIOB->ODR |= (GPIO_ODR_OD2);
						break;
					case 1:
						GPIOB->ODR |= (GPIO_ODR_OD6);
						break;
					case 2:
						GPIOB->ODR |= (GPIO_ODR_OD3);
						break;
					case 3:
						GPIOB->ODR |= (GPIO_ODR_OD7);
						break;
					default:
						break;
				}
			}
		}
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


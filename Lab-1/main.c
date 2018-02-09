#include "stm32l476xx.h"

int main(void){
	
	uint32_t input;
	uint32_t input2;
	uint32_t input3;
	uint32_t input4;
	uint32_t input5;
	uint32_t repetition;
	uint32_t timeDelay;
	// Enable High Speed Internal Clock (HSI = 16 MHz)
  RCC->CR |= ((uint32_t)RCC_CR_HSION);
	
  // wait until HSI is ready
  while ( (RCC->CR & (uint32_t) RCC_CR_HSIRDY) == 0 ) {;}
	
  // Select HSI as system clock source 
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSI;     // 01: HSI16 oscillator used as system clock

  // Wait till HSI is used as system clock source 
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) == 0 ) {;}
  
  // Enable the clock to GPIO Port B	
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;   

	// MODER:00: Input mode,              01: General purpose output mode
  //       10: Alternate function mode, 11: Analog mode (reset state)
  GPIOB->MODER &= ~0x00000030 ;   // Clear bit 4 and bit 5
  GPIOB->MODER |= 0x00000010;   // Set bit 4
		
	GPIOB->ODR |= GPIO_ODR_ODR_2;
  
  // PUPDR:00: NO PUPD (reset state),   01: Pullup
  //       10: Pulldown,                11: Reserved
	GPIOB->PUPDR &= ~0x00000030;  // Clear bit 4 and bit 5
	
	// Enable the clock to GPIO Port E	
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;   

	// MODER:00: Input mode,              01: General purpose output mode
  //       10: Alternate function mode, 11: Analog mode (reset state)
  GPIOE->MODER &= ~0x00030000 ;   // Clear bit 17 and bit 16
  GPIOE->MODER |= 0x00010000;   // Set bit 16
		
	GPIOE->ODR |= GPIO_ODR_ODR_8;
		
  // PUPDR:00: NO PUPD (reset state),   01: Pullup
  //       10: Pulldown,                11: Reserved
	GPIOB->PUPDR &= ~0x00030000;  //Clear bit 16 and bit 17
		
	// Enable the clock to GPIO Port A	
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;   

	// MODER:00: Input mode,              01: General purpose output mode
  //       10: Alternate function mode, 11: Analog mode (reset state)
  GPIOA->MODER &= ~0x00000CFF;   // Clear bits 0-7, and bit 10 and bit 11
  
  // PUPDR:00: NO PUPD (reset state),   01: Pullup
  //       10: Pulldown,                11: Reserved
	GPIOA->PUPDR &= ~0x00000CFF;  // Clear bits 0-7, and bit 10 and bit 11
	GPIOA->PUPDR |= 0x000008AA; // Set bits 1, 3, 5, 7, and 11
	
	while(1)
	{
		input = (GPIOA->IDR & 0x00000001);
		input2 = (GPIOA->IDR & 0x00000002);
		input3 = (GPIOA->IDR & 0x00000004);
		input4 = (GPIOA->IDR & 0x00000008);
		input5 = (GPIOA->IDR & 0x00000020);

		if(input == 0)  // Center of Joystick is not pressed
		{
			
		}
		else  // Center of Joystick is pressed
		{
			if(GPIOE->ODR == 0 && GPIOB->ODR == 0)
			{
			GPIOB->ODR |= GPIO_ODR_ODR_2; // Turn on Red LED and turn off Green LED
			GPIOE->ODR |= GPIO_ODR_ODR_8; // Turn off Red LED and turn on Green LED
			}
			else
			{
				GPIOE->ODR = 0;
				GPIOB->ODR = 0;
			}
			while(input != 0)
			{
				input = (GPIOA->IDR & 0x00000001);
			}
		}
		if(input2 == 0)  // Left of Joystick is not pressed
		{
			
		}
		else  // Left of Joystick is pressed
		{
			if(GPIOE->ODR == 0 && GPIOB->ODR == 0)
			{
			GPIOB->ODR |= GPIO_ODR_ODR_2; // Turn on Red LED and turn off Green LED
			GPIOE->ODR |= GPIO_ODR_ODR_8; // Turn off Red LED and turn on Green LED
			}
			else
			{
				GPIOE->ODR = 0;
				GPIOB->ODR = 0;
			}
			for(timeDelay = 0; timeDelay <400000; timeDelay++)
			{
				
			}
		}
		if(input3 == 0)  // Left of Joystick is not pressed
		{
			
		}
		else  // Left of Joystick is pressed
		{
			if(GPIOE->ODR == 0 && GPIOB->ODR != 0)
			{
				GPIOB->ODR = 0;				// Turn on Red LED and turn off Green LED
				GPIOE->ODR |= GPIO_ODR_ODR_8; // Turn off Red LED and turn on Green LED
			}
			else
			{
				GPIOE->ODR = 0;
				GPIOB->ODR |= GPIO_ODR_ODR_2;
			}
			for(timeDelay = 0; timeDelay <400000; timeDelay++)
			{
				
			}
		}
		if(input4 == 0)  // Left of Joystick is not pressed
		{
			
		}
		else  // Left of Joystick is pressed
		{
			if(GPIOE->ODR != 0 || GPIOB->ODR != 0)
			{
				GPIOB->ODR = 0;				// Turn on Red LED and turn off Green LED
				GPIOE->ODR = 0;
			}
			for(timeDelay = 0; timeDelay <1000000; timeDelay++)
			{
				
			}
			GPIOB->ODR |= GPIO_ODR_ODR_2; // Turn on Red LED and turn off Green LED
			GPIOE->ODR |= GPIO_ODR_ODR_8;
			for(timeDelay = 0; timeDelay <400000; timeDelay++)
			{
				
			}
			GPIOB->ODR = 0; // Turn on Red LED and turn off Green LED
			GPIOE->ODR = 0;
			for(timeDelay = 0; timeDelay <2000000; timeDelay++)
			{
				
			}
			for(repetition = 0; repetition < 2; repetition++)
			{
				GPIOB->ODR |= GPIO_ODR_ODR_2; // Turn on Red LED and turn off Green LED
				GPIOE->ODR |= GPIO_ODR_ODR_8;
				for(timeDelay = 0; timeDelay <1000000; timeDelay++)
				{
				
				}
				GPIOB->ODR = 0; // Turn on Red LED and turn off Green LED
				GPIOE->ODR = 0;
				for(timeDelay = 0; timeDelay <1000000; timeDelay++)
				{
				
				}
				GPIOB->ODR |= GPIO_ODR_ODR_2; // Turn on Red LED and turn off Green LED
				GPIOE->ODR |= GPIO_ODR_ODR_8;
				for(timeDelay = 0; timeDelay <400000; timeDelay++)
				{
				
				}
				GPIOB->ODR = 0; // Turn on Red LED and turn off Green LED
				GPIOE->ODR = 0;
				for(timeDelay = 0; timeDelay <1000000; timeDelay++)
				{
				
				}
			}
			for(timeDelay = 0; timeDelay <1000000; timeDelay++)
			{
				
			}
			GPIOB->ODR |= GPIO_ODR_ODR_2; // Turn on Red LED and turn off Green LED
			GPIOE->ODR |= GPIO_ODR_ODR_8;
			for(timeDelay = 0; timeDelay <400000; timeDelay++)
			{
				
			}
			GPIOB->ODR = 0; // Turn on Red LED and turn off Green LED
			GPIOE->ODR = 0;
		}
		if(input5 == 0)  // Center of Joystick is not pressed
		{
			
		}
		else  // Center of Joystick is pressed
		{
			if(GPIOE->ODR != 0 && GPIOB->ODR == 0)
			{
				GPIOB->ODR |= GPIO_ODR_ODR_2; // Turn on Red LED and turn off Green LED
				GPIOE->ODR = 0;
			}
			else
			{
				GPIOE->ODR |= GPIO_ODR_ODR_8; // Turn off Red LED and turn on Green LED
				GPIOB->ODR = 0;
			}
			while(input5 != 0)
			{
				input5 = (GPIOA->IDR & 0x00000020);
			}
		}
	}
}
#include "stm32l476xx.h"

/* Spencer Goulette
	 03/01/18
	 ECE 271 LAB6
	 Objective: Get LED to blink with
	 Pins: Port B pins used for red LED
	 Clock Frequency: 8MHz
	 SysTick Clock Frequency: 1MHz
*/

void clockInit(void);
void portInit(void);
void EXTI_Init(void);
void EXTI3_IRQHandler(void);

int main(void)
{
	int i;
	clockInit();
	
	portInit();
	
	EXTI_Init();
	
	while(1)
	{
		GPIOB->ODR ^= GPIO_ODR_ODR_2;
		for(i = 0; i < 1000000; i++)
		{
			
		}
	}
}	

void clockInit(void)
{
	// Enable High Speed Internal Clock (HSI = 16 MHz)
  RCC->CR |= ((uint32_t)RCC_CR_HSION);
	
  // wait until HSI is ready
  while ( (RCC->CR & (uint32_t) RCC_CR_HSIRDY) == 0 ) {;}
	
  // Select HSI as system clock source // Enable High Speed Internal Clock (HSI = 16 MHz)
  RCC->CR |= ((uint32_t)RCC_CR_HSION);
	
  // wait until HSI is ready
  while ( (RCC->CR & (uint32_t) RCC_CR_HSIRDY) == 0 ) {;}
	
  // Select HSI as system clock source 
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSI;     // 01: HSI16 oscillator used as system clock

  // Wait till HSI is used as system clock source 
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) == 0 ) {;}
}
void portInit(void)
{
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
	
	// Enable the clock to GPIO Port A	
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;   

	// MODER:00: Input mode,              01: General purpose output mode
  //       10: Alternate function mode, 11: Analog mode (reset state)
  GPIOA->MODER &= ~0x00000CFF;   // Clear bits 0-7, and bit 10 and bit 11
  
  // PUPDR:00: NO PUPD (reset state),   01: Pullup
  //       10: Pulldown,                11: Reserved
	GPIOA->PUPDR &= ~0x00000CFF;  // Clear bits 0-7, and bit 10 and bit 11
	GPIOA->PUPDR |= 0x000008AA; // Set bits 1, 3, 5, 7, and 11
	
	// Enable the clock to GPIO Port E	
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;   

	// MODER:00: Input mode,              01: General purpose output mode
  //       10: Alternate function mode, 11: Analog mode (reset state)
  GPIOE->MODER &= ~0x00030000 ;   // Clear bit 17 and bit 16
  GPIOE->MODER |= 0x00010000;   // Set bit 16
}

void EXTI_Init(void)
{
	// Enable SYSCFG clock
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	
	// Select PA.3 as the trigger source of EXTI 3
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI3;
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI3_PA;
	SYSCFG->EXTICR[0] &= ~(0x000F);
	
	// Enable rising edge trigger for EXTI3
	// Rising trigger selection register (RSTR)
	// 0 = disabled, 1 = enabled
	EXTI->RTSR1 |= EXTI_RTSR1_RT3;
	
	// Disable falling edge trigger for EXTI3
	// Falling trigger selection register (FSTR)
	// 0 = disabled, 1 = enabled
	EXTI->FTSR1 &= ~EXTI_FTSR1_FT3;
	
	// Enable EXTI 3 interrupt
	// Interupt mask register: 0 = masked, 1 = unmasked
	// "Masked" means that processor ignores the corresponding interrupt.
	EXTI->IMR1 |= EXTI_IMR1_IM3;
	
	// Enable EXTI 3 interrupt
	NVIC_EnableIRQ(EXTI3_IRQn);
	
	// Set EXTI 3 priority to 1
	NVIC_SetPriority(EXTI3_IRQn, 1);
	
}

void EXTI3_IRQHandler(void)
{
	// Check for EXTI 3 interrupt flag
	if((EXTI->PR1 & EXTI_PR1_PIF3) == EXTI_PR1_PIF3)
	{
		// Toggle LED
		GPIOE->ODR ^= GPIO_ODR_ODR_8;
		
		// Clear interrupt pending request
		EXTI->PR1 |= EXTI_PR1_PIF3;
	}
}


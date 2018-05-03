#include "stm32l476xx.h"
#include "lcd.h"
/* Spencer Goulette
	 05/03/18
	 ECE 271 Bonus Lab 8
	 Objective: Get LED to blink with another LED using external interrupt
	 Pins: Port B pins used for red LED and Port E pins used for green LED
	 Clock Frequency: 16MHz
*/

void clockInit(void);
void portInit(void);
void EXTI_Init(void);
void EXTI9_5_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI0_IRQHandler(void);
void delay(int time);

int main(void)
{
	int i;
	LCD_Initialization();	// Intitializing everything
	clockInit();
	portInit();
	EXTI_Init();
	
	while(1)
	{
		GPIOB->ODR ^= GPIO_ODR_ODR_2;	// Toggles LED
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
	
	// Select the trigger sources of the EXTIs
	SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI3 | SYSCFG_EXTICR1_EXTI2 | SYSCFG_EXTICR1_EXTI1 | SYSCFG_EXTICR1_EXTI0);
	SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PA | SYSCFG_EXTICR1_EXTI2_PA | SYSCFG_EXTICR1_EXTI1_PA | SYSCFG_EXTICR1_EXTI0_PA);
	SYSCFG->EXTICR[0] &= ~(0x000F);
	
	SYSCFG->EXTICR[1] &= ~(SYSCFG_EXTICR2_EXTI5);
	SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PA);
	SYSCFG->EXTICR[1] &= ~(0x000F);
	
	// Enable rising edge trigger 
	// Rising trigger selection register (RSTR)
	// 0 = disabled, 1 = enabled
	EXTI->RTSR1 |= (EXTI_RTSR1_RT3 | EXTI_RTSR1_RT2 | EXTI_RTSR1_RT1 | EXTI_RTSR1_RT0);
	
	EXTI->RTSR1 |= EXTI_RTSR1_RT5;
	
	// Disable falling edge trigger
	// Falling trigger selection register (FSTR)
	// 0 = disabled, 1 = enabled
	EXTI->FTSR1 &= ~(EXTI_FTSR1_FT3 | EXTI_FTSR1_FT2 | EXTI_FTSR1_FT1 | EXTI_FTSR1_FT0);
	
	EXTI->FTSR1 &= ~EXTI_FTSR1_FT5;
	
	// Enable EXTI interrupts
	// Interupt mask register: 0 = masked, 1 = unmasked
	// "Masked" means that processor ignores the corresponding interrupt.
	EXTI->IMR1 |= (EXTI_IMR1_IM3 | EXTI_IMR1_IM2 | EXTI_IMR1_IM1 | EXTI_IMR1_IM0);
	
	EXTI->IMR1 |= (EXTI_IMR1_IM5);
	
	// Enable EXTI interrupts
	NVIC_EnableIRQ(EXTI9_5_IRQn);
	NVIC_EnableIRQ(EXTI3_IRQn);
	NVIC_EnableIRQ(EXTI2_IRQn);
	NVIC_EnableIRQ(EXTI1_IRQn);
	NVIC_EnableIRQ(EXTI0_IRQn);
	
	// Set EXTI priority to 1
	NVIC_SetPriority(EXTI9_5_IRQn, 1);
	NVIC_SetPriority(EXTI3_IRQn, 1);
	NVIC_SetPriority(EXTI2_IRQn, 1);
	NVIC_SetPriority(EXTI1_IRQn, 1);
	NVIC_SetPriority(EXTI0_IRQn, 1);
	
}

void EXTI9_5_IRQHandler(void)
{
	if((EXTI->PR1 & EXTI_PR1_PIF5) == EXTI_PR1_PIF5)
	{
		
		// Display LCD
		GPIOB->ODR &= ~GPIO_ODR_ODR_2;	// Turn both off
		GPIOE->ODR &= ~GPIO_ODR_ODR_8;
		
		// Clear interrupt pending request
		EXTI->PR1 |= EXTI_PR1_PIF5;
	}
}

void EXTI0_IRQHandler(void)
{
	if((EXTI->PR1 & EXTI_PR1_PIF0) == EXTI_PR1_PIF0)
	{
		
		// Display LCD
		GPIOB->ODR |= GPIO_ODR_ODR_2;	// Turn both on
		GPIOE->ODR |= GPIO_ODR_ODR_8;
		
		// Clear interrupt pending request
		EXTI->PR1 |= EXTI_PR1_PIF0;
	}
}

void EXTI1_IRQHandler(void)
{
	// Check for EXTI 3 interrupt flag
	if((EXTI->PR1 & EXTI_PR1_PIF1) == EXTI_PR1_PIF1)
	{
		
		// Display LCD
		delay(500);
		LCD_DisplayString((uint8_t*)"BONUS");
		delay(500);
		LCD_DisplayString((uint8_t*)"ONUS ");
		delay(500);
		LCD_DisplayString((uint8_t*)"NUS L");
		delay(500);
		LCD_DisplayString((uint8_t*)"US LA");
		delay(500);
		LCD_DisplayString((uint8_t*)"S LAB");
		delay(500);
		
		// Clear LCD
		LCD_Clear();
		
		// Clear interrupt pending request
		EXTI->PR1 |= EXTI_PR1_PIF1;
	}
}

void EXTI2_IRQHandler(void)
{
	static int toggle = 1;
	// Check for EXTI 3 interrupt flag
	if((EXTI->PR1 & EXTI_PR1_PIF2) == EXTI_PR1_PIF2)
	{
		if(toggle == 1)
		{
			// Display LCD
			LCD_DisplayString((uint8_t*)"BONUS");
			
			// Toggle
			toggle = 0;
		}
		else{
			// Clear LCD
			LCD_Clear();
			
			// Toggle
			toggle = 1;
		}
		// Clear interrupt pending request
		EXTI->PR1 |= EXTI_PR1_PIF2;
	}
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

void delay(int time)	// Delay function
{
	volatile int i;
	for(i = 0; i < time * 1000; i++);
}


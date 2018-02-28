#include "stm32l476xx.h"

void sysTick_Initialize(uint32_t ticks);
void delay(uint32_t nTime);
void SysTick_Handler(void);

void toggle_red_led(void);

volatile uint32_t TimeDelay;
int x;

int main(void){
	
	// Enable High Speed Internal Clock (HSI = 16 MHz)
  RCC->CR |= ((uint32_t)RCC_CR_HSION);
	
  // wait until HSI is ready
  while ( (RCC->CR & (uint32_t) RCC_CR_HSIRDY) == 0 ) {;}
	
  // Select HSI as system clock source 
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSI;     // 01: HSI16 oscillator used as system clock
	
	// Enable the clock to GPIO Port E	
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;   

	// MODER:00: Input mode,              01: General purpose output mode
  //       10: Alternate function mode, 11: Analog mode (reset state)
  GPIOB->MODER &= ~0x00000030 ;   // Clear bit 4 and bit 5
  GPIOB->MODER |= 0x00000010;   // Set bit 4
		
	GPIOB->ODR |= GPIO_ODR_ODR_2;
  
  // PUPDR:00: NO PUPD (reset state),   01: Pullup
  //       10: Pulldown,                11: Reserved
	GPIOB->PUPDR &= ~0x00000030;  // Clear bit 4 and bit 5
		
	GPIOB->PUPDR &= ~0x00030000;  //Clear bit 16 and bit 17
	
	sysTick_Initialize(8000);
	
	while(1)
	{
		delay(5000);
		toggle_red_led();
		delay(500);
		toggle_red_led();
		delay(500);
		toggle_red_led();
		delay(500);
		toggle_red_led();
		delay(500);
		toggle_red_led();
		delay(500);
		toggle_red_led();
		delay(500);
		toggle_red_led();
		
		delay(500);
		toggle_red_led();
		delay(1500);
		toggle_red_led();
		delay(500);
		toggle_red_led();
		delay(1500);
		toggle_red_led();
		delay(500);
		toggle_red_led();
		delay(1500);
		toggle_red_led();
		delay(500);
		
		toggle_red_led();
		delay(500);
		toggle_red_led();
		delay(500);
		toggle_red_led();
		delay(500);
		toggle_red_led();
		delay(500);
		toggle_red_led();
		delay(500);
		toggle_red_led();
		
		delay(1000);
		toggle_red_led();
	}
}	

void delay(uint32_t nTime)
{
	TimeDelay = nTime;
	while(TimeDelay != 0);
}

void toggle_red_led(void)
{	
	if(GPIOB->ODR == 0)
	{
	GPIOB->ODR |= GPIO_ODR_ODR_2; // Turn on Red LED
	}
	else
	{
		GPIOB->ODR = 0; // Turn off Green LED
	}
}

void SysTick_Handler(void)
{
	if(TimeDelay > 0)
	{
		TimeDelay--;
	}
}

void sysTick_Initialize(uint32_t ticks)
{
	SysTick->CTRL = 0;
	
	SysTick->LOAD = ticks - 1;
	
	
	
	NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
	
	
	SysTick->VAL = 0;
	
	
	
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
	
	
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	
	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

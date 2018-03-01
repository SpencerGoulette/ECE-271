#include "stm32l476xx.h"

/* Spencer Goulette
	 03/01/18
	 ECE 271 LAB6
	 Objective: Get LED to blink with
	 Pins: Port B pins used for red LED
	 Clock Frequency: 8MHz
	 SysTick Clock Frequency: 1MHz
*/

void sysTick_Initialize(uint32_t ticks);	//Initializes SysTick
void delay(uint32_t nTime);		//Time to Delay
void SysTick_Handler(void);		//The interrupt to create the system delay

void toggle_red_led(void);		//Code to toggle a red LED

volatile uint32_t TimeDelay;

int main(void){
	
	// Enable High Speed Internal Clock (HSI = 8 MHz)
	
	RCC->CR &= ~RCC_CR_MSION;		//Turn MSI off so clock speed can be changed
	RCC->CR |= RCC_CR_MSIRANGE_7;	//Change MSI clock to 8MHz
	RCC->CR |= RCC_CR_MSIRGSEL;	//Select the MSI clock as the system clock
	RCC->CR |= RCC_CR_MSION;	//Enable the MSI clock
	
  // wait until MSI is ready
  while ( (RCC->CR & (uint32_t) RCC_CR_MSIRDY) == 0 ) {;}
	
	// Enable the clock to GPIO Port B	
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;   

	// MODER:00: Input mode,              01: General purpose output mode
  //       10: Alternate function mode, 11: Analog mode (reset state)
  GPIOB->MODER &= ~0x00000030 ;   // Clear bit 4 and bit 5
  GPIOB->MODER |= 0x00000010;   // Set bit 4
  
  // PUPDR:00: NO PUPD (reset state),   01: Pullup
  //       10: Pulldown,                11: Reserved
	GPIOB->PUPDR &= ~0x00000030;  // Clear bit 4 and bit 5
		
	GPIOB->PUPDR &= ~0x00030000;  //Clear bit 16 and bit 17
	
	sysTick_Initialize(1000);	//Initializes SysTick
	
	while(1)
	{
		delay(3000);
		toggle_red_led();	//Toggles red LED each time code is run
		delay(250);
		toggle_red_led();	//Does S in Morse Code
		delay(250);
		toggle_red_led();
		delay(250);
		toggle_red_led();
		delay(250);
		toggle_red_led();
		delay(250);
		toggle_red_led();
		
		delay(250);
		toggle_red_led();	//Does O in Morse Code
		delay(1000);
		toggle_red_led();
		delay(250);
		toggle_red_led();
		delay(1000);
		toggle_red_led();
		delay(250);
		toggle_red_led();
		delay(1000);
		toggle_red_led();
		delay(250);
		
		toggle_red_led();	//Does S in Morse Code
		delay(250);
		toggle_red_led();
		delay(250);
		toggle_red_led();
		delay(250);
		toggle_red_led();
		delay(250);
		toggle_red_led();
		delay(250);
		toggle_red_led();
		
	}
}	

void delay(uint32_t nTime)	//Delay the nTime passed in in milliseconds
{
	TimeDelay = nTime;		//By setting TimeDelay to nTime, the SysTick_Handler occurs
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
		GPIOB->ODR = 0; // Turn off Red LED
	}
}

void SysTick_Handler(void)	//Interrupt to decrease the TimeDelay
{
	if(TimeDelay > 0)
	{
		TimeDelay--;
	}
}

void sysTick_Initialize(uint32_t ticks)	//Initializes SysTick
{
	SysTick->CTRL = 0;	//Disable SysTick IRQ and SysTick timer
	
	SysTick->LOAD = ticks - 1;	//Set Load Value
	
	
	
	NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);		//Set Interrupt Priority
	
	
	SysTick->VAL = 0;		//Clear SysTick current value register
	
	
	
	SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;	//Set CLKSOURCE to 0 to use the MSI AHB clock frequency/8
	
	
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;	//Enable Systick Interrupt
	
	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;	//Set Systick Control and Status Register
}

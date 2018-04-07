#include "stm32l476xx.h"
#include "lcd.h"
#include <string.h>

/* Spencer Goulette
	 02/08/18
	 ECE 271 LAB3
	 Objective: Have 6 Letters of last name to display on LCD, have 'ECE217' display on LCD, and be able to scroll left and right on a string that is displayed on the LCD
	 Pins: Port A pins used for Input of Buttons, and Port E pins used for output of Buttons
	 Clock Frequency: 16MHz
*/

volatile uint32_t pulse_width = 0;
volatile double timeSpan = 0;
volatile uint8_t OC = 0;
volatile uint32_t last_captured = 0;
volatile uint32_t signal_polarity = 0; // Assume input is low initially

void System_Clock_Init(void);
void TIM4_IRQHandler(void);
void sysTick_Initialize(uint32_t ticks);	//Initializes SysTick
void delay(uint32_t nTime);		//Time to Delay
void SysTick_Handler(void);		//The interrupt to create the system delay

volatile uint32_t TimeDelay;

int main(void){
	System_Clock_Init();
	LCD_Initialization();	//Initializes LCD
	sysTick_Initialize(1999);	//Initializes SysTick
	
	// Enable the clock to GPIO Port B and GPIO Port E	
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;

	// MODER:00: Input mode,              01: General purpose output mode
  //       10: Alternate function mode, 11: Analog mode (reset state)
  GPIOB->MODER &= ~GPIO_MODER_MODE6;   // Clear bit 4 and bit 5
  GPIOB->MODER |= GPIO_MODER_MODE6_1;   // Set bit 4
	
	GPIOE->MODER &= ~GPIO_MODER_MODE11; //Setting up trigger
	GPIOE->MODER |= GPIO_MODER_MODE11_0;
  
  // PUPDR:00: NO PUPD (reset state),   01: Pullup
  //       10: Pulldown,                11: Reserved
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD6;  // Clear bit 4 and bit 5
	
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL6);	// Selecting the alternative function for the timer
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_1;

	// Enable the clock to timer 4
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;
	
	// Set up an appropriate prescaler to slowdown the timer's input clock
	TIM4->PSC = 15;
	
	// Set Auto-reload value to maximum value
	TIM4->ARR = 0xFFFF; // Maximum 16-bit value
	
	// Set the direction of channel 1 as input, and select the active input
	TIM4->CCMR1 &= ~TIM_CCMR1_CC1S; // Clear capture/compare 1 selection bits
	TIM4->CCMR1 |= TIM_CCMR1_CC1S_0; // CC1S[1:0] for channel 1:
		//	00 = output
		//	01 = input, CC1 is mapped on timer input 1
		//	10 = input, CC1 is mapped on timer input 2
		//	11 = input, CC1 is mapped on slave timer
		
		// Program the input filter duration: Disable digital filtering by clearing
		// IC1F[3:0] bits becaude we want to capture every event
		TIM4->CCMR1 &= ~TIM_CCMR1_IC1F; // No filtering
		
		// Set the active transition as both rising and falling edges
		// CC1NP: CC1P bits: 00 = rising edge	01 = falling edge
		//									 10 = reserved		11 = both edges
		TIM4->CCER |= (TIM_CCER_CC1P | TIM_CCER_CC1NP); // Both edges generate interrupts
		
		// Program the input prescaler: clear prescaler to capture each transition
		TIM4->CCMR1 &= ~TIM_CCMR1_IC1PSC;
		
		// Enable capture for channel 1
		// CC1E: 0 = disabled, 1 = enabled
		TIM4->CCER |= TIM_CCER_CC1E;
		
		// Allow channel 1 of timer 4 to generate interrupts
		TIM4->DIER |= TIM_DIER_CC1IE;
		
		// Allow channel 1 of timer 4 to generate DMA requests
		TIM4->DIER |= TIM_DIER_CC1DE; // Optional. Required if DMA is used
		
		// Enable the timer counter
		TIM4->CR1 |= TIM_CR1_CEN;
		
		// Set priority of timer 4 interrupt to 0 (highest urgency)
		NVIC_SetPriority(TIM4_IRQn, 0);
		
		// Enable timer 4 interrupt in the interrupr controller (NVIC)
		NVIC_EnableIRQ(TIM4_IRQn);
		
		while(1)
		{
			GPIOE->ODR |= GPIO_ODR_OD11;	//Trigger the sensor to get the distance
			delay(1);
			GPIOE->ODR &= ~GPIO_ODR_OD11;
			delay(1);
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
	
	return;
}

void TIM4_IRQHandler(void)
{
	uint32_t current_captured;
	char str[6];
	if((TIM4->SR & TIM_SR_CC1IF) != 0)	// Check interrupt flag set
	{
		// Reading CCR1 clears CC1IF interrupt flag
		current_captured = TIM4->CCR1;
		
		// Toggle the polarity flag
		signal_polarity = 1 - signal_polarity;
		
		if(signal_polarity == 0) // Calculate only when the current input is low
		{
			pulse_width = (current_captured - last_captured); // Assume up-counting
			timeSpan = ((pulse_width + ((1 + 65535) * OC))/100);
			sprintf(str, "%d", (pulse_width / 148));	// Give distance in inches
			//sprintf(str, "%d", (int)timeSpan); // Give Time Span in seconds
			LCD_DisplayString((uint8_t*)str);	//Display
			TIM4->CNT = 0;
			OC = 0;
		}
		
		last_captured = current_captured;
	}
	
	if((TIM4->SR & TIM_SR_UIF) != 0) // Check if overflow has taken place
	{
		TIM4->SR &= ~TIM_SR_UIF; // Clear UIF flag to prevent re-entering
		OC++;
	}
}

void delay(uint32_t nTime)	//Delay the nTime passed in in milliseconds
{
	TimeDelay = nTime;		//By setting TimeDelay to nTime, the SysTick_Handler occurs
	while(TimeDelay != 0);
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

#include "stm32l476xx.h"

/* Spencer Goulette
	 03/17/18
	 ECE 271 LAB7
	 Objective: Get LED to dim and motor to turn -90, 0, and 90 degrees
	 Pins: Port E pins used for green LED
	 Clock Frequency: 8MHz
	 SysTick Clock Frequency: 1MHz
*/

void sysTick_Initialize(uint32_t ticks);	//Initializes SysTick
void delay(uint32_t nTime);		//Time to Delay
void SysTick_Handler(void);		//The interrupt to create the system delay
void TIM1_Init(void);

uint32_t switchCase = 99;
volatile uint32_t TimeDelay;

int main(void)
{	
	int pauseCounter = 1;
	int brightness = 1;
	int stepSize = 1;   
	
	// Enable High Speed Internal Clock (HSI = 8 MHz)
	
	RCC->CR &= ~RCC_CR_MSION;		//Turn MSI off so clock speed can be changed
	RCC->CR |= RCC_CR_MSIRANGE_7;	//Change MSI clock to 8MHz
	RCC->CR |= RCC_CR_MSIRGSEL;	//Select the MSI clock as the system clock
	RCC->CR |= RCC_CR_MSION;	//Enable the MSI clock
	
  // wait until MSI is ready
  while ( (RCC->CR & (uint32_t) RCC_CR_MSIRDY) == 0 ) {;}
	
	sysTick_Initialize(1000);	//Initializes SysTick
		
	// Enable the clock to GPIO Port B	
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
		
	// MODER:00: Input mode,              01: General purpose output mode
  //       10: Alternate function mode, 11: Analog mode (reset state)
  GPIOE->MODER &= ~GPIO_MODER_MODE8;   // Clear MODER 8
  GPIOE->MODER |= GPIO_MODER_MODE8_1;   // Set MODER 8 to Alternate Function
  
	GPIOE->AFR[1] &= ~GPIO_AFRH_AFSEL8; // Clear AFRH8
		
	GPIOE->AFR[1] |= GPIO_AFRH_AFSEL8_0; // Set the Alternative Function to TIM1_CH1N
	
	
	TIM1_Init(); // Initializing TIMER 1
		
	while(pauseCounter <= 10)
	{
		if((brightness >= 99) || (brightness <= 0))
		{
			stepSize = -stepSize;	// Reverse direction
			pauseCounter += 1;
		}
		
		brightness += stepSize; // Change brightness
		
		TIM1->CCR1 = brightness; // Change duty cycle of channel 1 outputs
		
		delay(10);	// Delay 1 ms
	}
	
	switchCase = 1999;
	TIM1_Init();
	brightness = 140;	// 0
	TIM1->CCR1 = brightness;
	delay(1000);
	brightness = 50;	// -90
	TIM1->CCR1 = brightness;
	delay(1000);
	brightness = 250;	// 90
	TIM1->CCR1 = brightness;
	delay(1000);

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

void TIM1_Init(void)
{
	// Enable TIMER 1 Clock
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	
	//Counting direction: 0 = Up-counting, 1= Down-Counting
	TIM1->CR1 &= ~TIM_CR1_DIR; // Select Up-counting
	
	// Prescaler, slow down the input clock by a factor of (1 + prescaler)
	TIM1->PSC = 79; // 8 MHz / (1 + 79) = 100 KHz
	
	// Auto-reload
	TIM1->ARR = switchCase; // PWM period = (99 + 1) * 1/100KHz = 0.001s
	
	// Clear output compare mose bits for channel 1
	TIM1->CCMR1 &= ~TIM_CCMR1_OC1M;
	
	// Select PWM Mode 1 output on channel 1 (OC1M = 110)
	TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	
	// Output 1 preload enable
	TIM1->CCMR1 |= TIM_CCMR1_OC1PE;
	
	// Select output polarity: 0 = Active High, 1 = Active Low
	TIM1->CCMR1 &= ~TIM_CCER_CC1NP; // OC1N = OCREF + CC1NP
	
	// Enable complementary output of channel 1 (CH1N)
	TIM1->CCER |= TIM_CCER_CC1NE;
	
	// Main output enable (MOE): 0 = Disable, 1 = Enable
	TIM1->BDTR |= TIM_BDTR_MOE;
	
	// Output Compare Register for channel 1
	TIM1->CCR1 = 50; // Initial duty cycle 50%
	
	// Enable counter
	TIM1->CR1 |= TIM_CR1_CEN;
}

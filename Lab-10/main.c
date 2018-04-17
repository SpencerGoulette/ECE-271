#include "stm32l476xx.h"
#include "lcd.h"
#include "adc.h"

/* Spencer Goulette
	 02/15/18
	 ECE 271 LAB10
	 Objective: Get LED to light up when voltage higher than 2V and turn off when voltage is lower than 1V
	 Clock Frequency: 16MHz
*/

void System_Clock_Init(void);

volatile uint32_t Result, Result_1, Result_2, Result_3, Result_4;

int main(void){
	char str[6] = {' ',' ',' ',' ',' ',' '};
	System_Clock_Init();
	LCD_Initialization();
	// Enable the clock to GPIO Port B	
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;   

	// MODER:00: Input mode,              01: General purpose output mode
  //       10: Alternate function mode, 11: Analog mode (reset state)
  GPIOB->MODER &= ~GPIO_MODER_MODE2;   // Clear bit 4 and bit 5
  GPIOB->MODER |= GPIO_MODER_MODE2_0;   // Set bit 4
  
  // PUPDR:00: NO PUPD (reset state),   01: Pullup
  //       10: Pulldown,                11: Reserved
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD2;  // Make PUPD
	
	// Enable the clock to GPIO Port A
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	// MODER:00: Input mode,              01: General purpose output mode
  //       10: Alternate function mode, 11: Analog mode (reset state)
  GPIOA->MODER &= ~(GPIO_MODER_MODE1 | GPIO_MODER_MODE2);
  GPIOA->MODER |= GPIO_MODER_MODE1 | GPIO_MODER_MODE2_0;   // Set mode to analog
	
	GPIOA->ASCR |= GPIO_ASCR_ASC1;	// Set bit 1 in ASCR to close the analog switch
	
	GPIOA->ODR |= GPIO_ODR_ODR_2;
	
	ADC_init();
	
	while(1)
	{
		ADC1->CR |= ADC_CR_ADSTART;
		while((ADC123_COMMON->CSR & ADC_CSR_EOC_MST) !=  ADC_CSR_EOC_MST);
		if(ADC1->DR/13 < 285)
		{
			GPIOB->ODR |= GPIO_ODR_ODR_2;
		}
		else if(ADC1->DR/13 > 285)
		{
			GPIOB->ODR &= ~GPIO_ODR_ODR_2;
		}
		sprintf(str, "%d", (ADC1->DR/13));
		LCD_DisplayString((uint8_t*) str);
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

void ADC1_2_IRQHandler(void) {
	if((ADC1->ISR & ADC_ISR_EOC) == ADC_ISR_EOC) {
		// For a regular channel, check End of Conversion (EOC) flag
		// Reading ADC data register (DR) clears the EOC flag
		Result = ADC1->DR;
	}
	else if ((ADC1->ISR & ADC_ISR_JEOS) == ADC_ISR_JEOS) {
		// For injected channels, check End of Sequence (JEOS) flag
		// Reading injected data registers does not clear the JEOS flag
		// Each injected channel has a dedicated data register
		Result_1 = ADC1->JDR1;	// Injected channel 1
		Result_2 = ADC1->JDR2;	// Injected channel 2
		Result_3 = ADC1->JDR3;	// Injected channel 3
		Result_4 = ADC1->JDR4;	// Injected channel 4
		ADC1->ISR |= ADC_ISR_EOS; // Clear the flag by writing 1 to it
	}
}
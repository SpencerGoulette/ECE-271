#include "adc.h"
#include "stm32l476xx.h"
#include <stdint.h>

/* Spencer Goulette
	 02/15/18
	 ECE 271 LAB4
	 Objective: Have the Motor Turn using the half step and full step method. Also be able to enter degree that you want the motor to rotate and half it rotate that amount of degrees
	 Clock Frequency: 32KHz
*/

void ADC_init(void){
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;  // Enable ADC clock
	
	ADC1->CR &= ~ADC_CR_ADEN; // Disable ADC1
	
	SYSCFG->CFGR1 |= SYSCFG_CFGR1_BOOSTEN; // Enable I/O analog voltage booster
	ADC123_COMMON->CCR |= ADC_CCR_VREFEN;	// Enabled the conversion of internal channels
	ADC123_COMMON->CCR &= ~ADC_CCR_PRESC;	// Set Prescaler
	ADC123_COMMON->CCR &= ~ADC_CCR_CKMODE;
	ADC123_COMMON->CCR |= ADC_CCR_CKMODE_0;	// Select Synchronous Clock Mode (HCLK/1)
	ADC123_COMMON->CCR &= ~ADC_CCR_DUAL; // Configure all ADCs as independent
	ADC1_Wakeup();
	ADC1->CFGR &= ~ADC_CFGR_RES; // Set resolution to 12 bits
	ADC1->CFGR &= ~ADC_CFGR_ALIGN;	// Right Alignment
	ADC1->SQR1 &= ~ADC_SQR1_L;	// 1 Conversion on the regular conversion channel sequence
	ADC1->SQR1 &= ~ADC_SQR1_SQ1;
	ADC1->SQR1 |= ADC_SQR1_SQ1_1 | ADC_SQR1_SQ1_2; // Specify channel 6 as the 1st conversion
	ADC1->DIFSEL &= ~ADC_DIFSEL_DIFSEL_6; // Configure Channel 6 as single-ended
	ADC1->SMPR1 &= ~ADC_SMPR1_SMP6;
	ADC1->SMPR1 |= ADC_SMPR1_SMP6_2 | ADC_SMPR1_SMP6_0; // Sampling time set to 92.5 ADC Clock Cycles
	ADC1->CFGR &= ~ADC_CFGR_CONT; // Selecting ADC as discontinous mode
	ADC1->CFGR &= ~ADC_CFGR_EXTEN; // Selecting software trigger
	ADC1->CR |= ADC_CR_ADEN; // Enable ADC 1
	while((ADC1->ISR & ADC_ISR_ADRDY) != ADC_ISR_ADRDY);
	return;
}

void ADC1_Wakeup(void) {
	int wait_time;
	
	// To start ADC operations, the following sequence should be applied
	// DEEPPWD = 0; ADC not in deep-power down
	// DEEPPWD = 1; ADC in deep-power-down (default reset state)
	if((ADC1->CR & ADC_CR_DEEPPWD) == ADC_CR_DEEPPWD) {
		// Exit deep power down mode if still in that state
		ADC1->CR &= ~ADC_CR_DEEPPWD;
	}
	
	// Enable the ADC internal voltage regulator
	// Before performing any operation such as launchin a calibration of
	// enabling the ADC, the ADC voltage regulator must first be enabled and
	// the software must wait for the regulation start-up time.
	ADC1->CR |= ADC_CR_ADVREGEN;
	
	// Wait for ADC voltage regulator start-up time. The software must wait for
	// the startup time of the ADC voltage regulator (T_ADCVREG_STUP, i.e., 20us)
	// before launching a calibration of enabling the ADC.
	wait_time = 20 * (80000000 / 1000000);
	while(wait_time != 0) {
		wait_time--;
	}
	return;
}
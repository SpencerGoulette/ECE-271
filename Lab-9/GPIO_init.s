; Spencer Goulette
; 02/22/18
; ECE 271 LAB5
; Objective: Have the Motor Turn using Assembly.
; Clock Frequency: 32KHz

	INCLUDE core_cm4_constants.s		; Load Cortex-M4 Definitions
	INCLUDE stm32l476xx_constants.s     ; Load STM32L4 Definitions 

	AREA    GPIOinit, CODE
	EXPORT	GPIO_init
	ALIGN
	ENTRY			
				
GPIO_init	PROC

			; Enable the clock to GPIO Port B	
			LDR r0, =RCC_BASE
			LDR r1, [r0, #RCC_AHB2ENR]
			ORR r1, r1, #RCC_AHB2ENR_GPIOBEN
			STR r1, [r0, #RCC_AHB2ENR]
			
			; Enable the clock to GPIO Port E	
			LDR r0, =RCC_BASE
			LDR r1, [r0, #RCC_AHB2ENR]
			ORR r1, r1, #RCC_AHB2ENR_GPIOEEN
			STR r1, [r0, #RCC_AHB2ENR]

			; MODE: 00: Input mode,              01: General purpose output mode
			;       10: Alternate function mode, 11: Analog mode (reset state)
			; Setting GPIOB MODER
			LDR r0, =GPIOB_BASE
			LDR r1, [r0, #GPIO_MODER]
			BIC r1, r1, #GPIO_MODER_MODER6
			ORR r1, r1, #GPIO_MODER_MODER6_1
			STR r1, [r0, #GPIO_MODER]
			
			LDR r0, =GPIOE_BASE
			LDR r1, [r0, #GPIO_MODER]
			BIC r1, r1, #GPIO_MODER_MODER11
			ORR r1, r1, #GPIO_MODER_MODER11_0
			STR r1, [r0, #GPIO_MODER]
			
			LDR r0, =GPIOB_BASE
			LDR r1, [r0, #GPIO_PUPDR]
			BIC r1, r1, #GPIO_PUPDR_PUPDR6
			STR r1, [r0, #GPIO_PUPDR]
			
			LDR r0, =GPIOB_BASE
			LDR r1, [r0, #GPIO_AFR0]
			BIC r1, r1, #GPIO_AFRL_AFRL6
			ORR r1, r1, #GPIO_BSRR_BR_9
			STR r1, [r0, #GPIO_AFR0]
			
			BX LR
			ENDP
			ALIGN			

			AREA    myData, DATA, READWRITE
			ALIGN
			END

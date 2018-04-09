; Spencer Goulette
; 02/22/18
; ECE 271 LAB5
; Objective: Have the Motor Turn using Assembly.
; Clock Frequency: 32KHz

	INCLUDE core_cm4_constants.s		; Load Cortex-M4 Definitions
	INCLUDE stm32l476xx_constants.s     ; Load STM32L4 Definitions 

	AREA    main, CODE
	EXPORT	__main				; make __main visible to linker
	IMPORT	HSI_init
	IMPORT	TIM4_IRQHandler
	IMPORT	keyPad
	ALIGN
	ENTRY			
				
__main	PROC
		
		BL HSI_init
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
		AND r1, #0
		ORR r1, r1, #GPIO_MODER_MODER6_1
		STR r1, [r0, #GPIO_MODER]
		
		LDR r0, =GPIOE_BASE
		LDR r1, [r0, #GPIO_MODER]
		AND r1, #0
		ORR r1, r1, #GPIO_MODER_MODER11_0
		STR r1, [r0, #GPIO_MODER]
		
		LDR r0, =GPIOB_BASE
		LDR r1, [r0, #GPIO_PUPDR]
		AND r1, #4294955007
		STR r1, [r0, #GPIO_PUPDR]
		
		LDR r0, =GPIOB_BASE
		LDR r1, [r0, #GPIO_MODER]
		AND r1, #0
		ORR r1, r1, #GPIO_MODER_MODER6_1
		STR r1, [r0, #GPIO_MODER]
		
		; Enable the clock to GPIO Port E	
		LDR r0, =RCC_BASE
		LDR r1, [r0, #RCC_APB1ENR1]
		ORR r1, r1, #RCC_APB1ENR1_TIM4EN
		STR r1, [r0, #RCC_APB1ENR1]
		
		B inf	;infinite while loop
		
stop 	B 		stop     ; dead loop & program hangs here
			
		ENDP
					
		ALIGN			

		AREA    myData, DATA, READWRITE
		ALIGN
array	DCD    1, 2, 3, 4
timespan	DCD    0	; Pulse width
lastcounter	DCD    0	; Timer counter value of last capture event
overflow	DCD	   0	; Counter for the number of overflows
		END

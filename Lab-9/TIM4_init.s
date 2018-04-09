; Spencer Goulette
; 02/22/18
; ECE 271 LAB5
; Objective: Have the Motor Turn using Assembly.
; Clock Frequency: 32KHz

	INCLUDE core_cm4_constants.s		; Load Cortex-M4 Definitions
	INCLUDE stm32l476xx_constants.s     ; Load STM32L4 Definitions 

	AREA    main, CODE
	EXPORT	TIM4_init
	ALIGN
	ENTRY			
				
TIM4_init	PROC
		
		; Enable the clock to GPIO Port E	
		LDR r0, =RCC_BASE
		LDR r1, [r0, #RCC_APB1ENR1]
		ORR r1, r1, #RCC_APB1ENR1_TIM4EN
		STR r1, [r0, #RCC_APB1ENR1]
		
		LDR r0, =TIM4_BASE
		LDR r1, [r0, #TIM_PSC]
		MOV r1, #15
		STR r1, [r0, #TIM_PSC]
		
		LDR r0, =TIM4_BASE
		LDR r1, [r0, #TIM_ARR]
		MOV r1, #65536
		STR r1, [r0, #TIM_ARR]
		
		LDR r0, =TIM4_BASE
		LDR r1, [r0, #TIM_CCMR1]
		BIC r1, r1, #TIM_CCMR1_CC1S
		ORR r1, r1, #TIM_CCMR1_CC1S_0
		BIC r1, r1, #TIM_CCMR1_IC1F
		STR r1, [r0, #TIM_CCMR1]
		
		LDR r0, =TIM4_BASE
		LDR r1, [r0, #TIM_CCER]
		ORR r1, r1, #TIM_CCER_CC1P
		ORR r1, r1, #TIM_CCER_CC1NP
		STR r1, [r0, #TIM_CCER]
		
		LDR r0, =TIM4_BASE
		LDR r1, [r0, #TIM_CCMR1]
		BIC r1, r1, #TIM_CCMR1_IC1PSC
		STR r1, [r0, #TIM_CCMR1]
		
		LDR r0, =TIM4_BASE
		LDR r1, [r0, #TIM_CCER]
		ORR r1, r1, #TIM_CCER_CC1E
		STR r1, [r0, #TIM_CCER]
		
		LDR r0, =TIM4_BASE
		LDR r1, [r0, #TIM_DIER]
		ORR r1, r1, #TIM_DIER_CC1IE
		ORR r1, r1, #TIM_DIER_CC1DE
		STR r1, [r0, #TIM_DIER]
		
		LDR r0, =TIM4_BASE
		LDR r1, [r0, #TIM_CR1]
		ORR r1, r1, #TIM_CR1_CEN
		STR r1, [r0, #TIM_CR1]
	
		LDR r2, =SCB_BASE
		ADD r2, r2, #SCB_SHP
		MOV r3, #1<<4
		STRB r3, [r2, #11]
		
		BX LR
		
stop 	B 		stop     ; dead loop & program hangs here
			
		ENDP
					
		ALIGN			

		AREA    myData, DATA, READWRITE
		ALIGN
array	DCD    1, 2, 3, 4
		END

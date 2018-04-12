; Spencer Goulette
; 02/22/18
; ECE 271 LAB5
; Objective: Have the Motor Turn using Assembly.
; Clock Frequency: 32KHz

	INCLUDE core_cm4_constants.s		; Load Cortex-M4 Definitions
	INCLUDE stm32l476xx_constants.s     ; Load STM32L4 Definitions 

	AREA	handler, CODE
	EXPORT	TIM4_IRQHandler
	EXPORT	timespan
	EXPORT	lastCounter
	EXPORT	overflow
	ALIGN		

TIM4_IRQHandler PROC
					
				PUSH {r4, r6, r10, lr}
				
				LDR r0, =TIM4_BASE		; Psuedo instruction
				LDR r2, [r0, #TIM_SR]	; Read status register
				AND r3, r2, #TIM_SR_UIF	; Check update event flag
				CBZ r3, check_CCFlag	; Compare and branch on zero
				
				LDR r3, =overflow
				LDR r1, [r3]			; Read overflow from memory
				ADD r1, r1, #1			; Increment overflow counter
				STR r1, [r3]			; Save overflow to memory
				
				BIC r2, r2, #TIM_SR_UIF	; Clear update event flag
				STR r2, [r0, #TIM_SR]	; Update status register
			
check_CCFlag	AND r2, r2, #TIM_SR_CC1IF ; Check capture event flag
				CBZ r2, exit			; Compare and branch on zero
				
				LDR r0, =TIM4_BASE		; Load base memory address
				LDR r1, [r0, #TIM_CCR1]	; Read the new captured value
				
				LDR r2, =lastCounter
				LDR r0, [r2]			; Load the last counter value
				STR r1, [r2]			; Save the new counter value
				CBZ r0, clearOverflow	; compare and branch on zero
				
				LDR r3, =overflow
				LDR r4, [r3]			; Load the overflow value
				LSL r4, r4, #16			; Multiply by 2^16
				ADD r6, r1, r4
				SUB r10, r6, r0			; r10 = timer counter difference
				LDR r2, =timespan
				STR r10, [r2]			; Update timespan memory
				
clearOverflow 	MOV r0, #0
				LDR r3, =overflow
				STR r0, [r3]			; Clear overflow counter
exit			POP {r4, r6, r10, pc}
				ENDP
					
		ALIGN			

		AREA    myData, DATA, READWRITE
		ALIGN
array	DCD    1, 2, 3, 4
timespan	DCD	0
lastCounter	DCD	0
overflow	DCD	0
		END

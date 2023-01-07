 /** 
 **************************************************************
 * @file mylib/s4642286_irremote.c
 * @author Ethan Pinto - 46422860
 * @date 01042022
 * @brief Infrared Remote HAL Peripheral Driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_irremote_init() - Initialises the IR Receiver hardware.
 * s4642286_irremote_recv() - Processes the recieved input waveform.
 * s4642286_irremote_readkey() - Reads the key that has been detected.
 *************************************************************** 
 */

#include "board.h"
#include "processor_hal.h"
#include "s4642286_irremote.h"

uint32_t timerValue, currentTimerValue = 0, lastTimerValue = 0, bitPattern = 0;
int pulseCount = 0;


/**
 * @brief Initialises the IR Receiver hardware.
 * 
 */
void s4642286_irremote_init() {
    // Enable GPIO B Clock
    __GPIOB_CLK_ENABLE();

	// Connect PB10 to timer output compare channel
	GPIOB->OSPEEDR |= (GPIO_SPEED_FAST << (10 * 2));
	GPIOB->PUPDR &= ~(0x03 << (10 * 2));					// Clear bits for no push/pull
	GPIOB->MODER &= ~(0x03 << (10 * 2));					// Clear bits
	GPIOB->MODER |= (GPIO_MODE_AF_PP << (10 * 2));  		// Set Alternate Function Push Pull Mode
	
	GPIOB->AFR[1] &= ~(0x0F << (2 * 4));					// Clear Alternate Function bits for pin (Higher ARF register)
	GPIOB->AFR[1] |= (GPIO_AF1_TIM2 << (2 * 4));			// Set Alternate Function 1 for pin (Higher ARF register)

	// Enable Timer 2
    __TIM2_CLK_ENABLE();

	/* Compute the prescaler value
	Set the clock prescaler to 50kHz
	SystemCoreClock is the system clock frequency */
	TIM2->PSC = ((SystemCoreClock / 2) / TIMER_COUNTER_FREQ) - 1;

	// Set timer to count up.
	TIM2->CR1 &= ~TIM_CR1_DIR;

	// Select direction as input and select the active input.
	TIM2->CCMR2 &= ~TIM_CCMR2_CC3S;
	TIM2->CCMR2 |= TIM_CCMR2_CC3S_0;

	// Disable digital filtering since we want to capture event.
	TIM2->CCMR2 &= ~TIM_CCMR2_IC3F;

	// Select falling edge as edge of active transition.
	TIM2->CCER &= ~(TIM_CCER_CC3NP);
	TIM2->CCER |= TIM_CCER_CC3P;

	// Set input prescaler to zero to capture each valid transition.
	TIM2->CCMR2 &= ~TIM_CCMR2_IC3PSC;

	// Enable capture for Channel 3.
	TIM2->CCER |= TIM_CCER_CC3E;

	// Enable interrupts and enable Capture interrupts for Channel 3.
	TIM2->DIER |= TIM_DIER_CC3IE; 

	// Enable update interrupt to occur on timer counter overflow or underflow.
	TIM2->DIER |= TIM_DIER_UIE;

	// Enable priority (10) and interrupt callback. Do not set a priority lower than 5.
	HAL_NVIC_SetPriority(TIM2_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

	// Enable Timer to start counting.
	TIM2->CR1 |= TIM_CR1_CEN;
}

/**
 * @brief Processes the recieved input waveform. This function is 
 *		  called by the timer input capture ISR.
 * 
 */
void s4642286_irremote_recv() {
	// Check for overflow
	if ((TIM2->SR & TIM_SR_UIF) != TIM_SR_UIF) {	 
		TIM2->SR &= ~TIM_SR_UIF; 	// Clear the UIF Flag
	}

	// Check if input capture has taken place 
	if ((TIM2->SR & TIM_SR_CC3IF) == TIM_SR_CC3IF) { 
		currentTimerValue = TIM2->CCR3;

		pulseCount++;

		timerValue = (currentTimerValue - lastTimerValue) / 2000;
		// If the pulse width is greater than 500ms, this will mark the SOF
		if (timerValue >= 500) {
			// First 2 counts needs to be skipped hence pulse count is set to -2 
			pulseCount = -2;
			bitPattern = 0;
		} else if ((pulseCount >= 0) && (pulseCount < 32)) {  
			// Accumulate the bit values between 0-31.
			// pulse width greater than 12ms is considered as LOGIC1
			if (timerValue >= 12) {
				bitPattern |= (uint32_t)1 << (31 - pulseCount);
			}
		}
		
		if (pulseCount >= 32) {
			// This will mark the End of frame as 32 pulses are received
			pulseCount = 0;
			keyPressedFlag = 1;
		}
		lastTimerValue = currentTimerValue;
	}
}

/**
 * @brief Interrupt handler (ISR) for TIM 2 IRQ Handler.
 * 
 */
void TIM2_IRQHandler(void) {
	s4642286_irremote_recv();
}

/**
 * @brief Checks if a certain key has been detected.
 * 
 * @param value a pointer to the buffer to store the character in.
 * @return int Returns 1 if a key has been detected, else 0.
 */
int s4642286_irremote_readkey(char *value) {
	if (keyPressedFlag) {
		switch (0xFFFFFF & bitPattern) {
			case ZERO:
				*value = '0';
				break;
			case ONE:
				*value = '1';
				break;
			case TWO:
				*value = '2';
				break;
			case THREE:
				*value = '3';
				break;
			case FOUR:
				*value = '4';
				break;
			case FIVE:
				*value = '5';
				break;
			case SIX:
				*value = '6';
				break;
			case SEVEN:
				*value = '7';
				break;
			case EIGHT:
				*value = '8';
				break;
			case NINE:
				*value = '9';
				break;
			default:
				break;
		}
		// Clear keyPressedFlag
		keyPressedFlag = 0;
		return 1;
	}
	return 0;
}

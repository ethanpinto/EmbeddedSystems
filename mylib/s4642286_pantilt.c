 /** 
 **************************************************************
 * @file mylib/s4642286_pantilt.c
 * @author Ethan Pinto - 46422860
 * @date 15032022
 * @brief Pan and Tilt peripheral driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_reg_pantilt_init() - Initialise servo (GPIO, PWM, etc)
 *************************************************************** 
 */

#include "board.h"
#include "processor_hal.h"
#include "s4642286_pantilt.h"

// Timer Macro Definitions
#define TIMER_COUNTING_FREQ 50000
#define PWM_PULSE_WIDTH_TICKS 2000
#define PWM_PERCENT2TICKS_DUTYCYCLE(value) (uint16_t)(value * PWM_PULSE_WIDTH_TICKS / 100)
#define PWM_TICKS2PERCENT_DUTYCYCLE(value) (double)((100.0 / (double)PWM_PULSE_WIDTH_TICKS) * value)
#define DUTYCYCLE_TO_ANGLE(dutyCycle) (int)((18 * dutyCycle) - 130.5)
#define ANGLE_TO_DUTYCYCLE(angle) (double)((1.0/18.0) * (double)angle + 7.25)
#define ZERO_DUTY_CYCLE 7.25

/**
 * @brief Initialises the pan and tilt servos and set up GPIO and PWM.
 * 
 */
void s4642286_reg_pantilt_init(void) {
    // Enable GPIO E Clock
    __GPIOE_CLK_ENABLE();

    // Make Pins PE9 and PE11 outputs.
    GPIOE->OSPEEDR |= ((GPIO_SPEED_FAST << (9 * 2)) | (GPIO_SPEED_FAST << (11 * 2)));	// Set fast speed.
	GPIOE->PUPDR &= ~((0x03 << (9 * 2)) | (0x03 << (11 * 2)));							// Clear bits for no push/pull
	GPIOE->MODER &= ~((0x03 << (9 * 2)) | (0x03 << (11 * 2)));							// Clear bits
	GPIOE->MODER |= ((GPIO_MODE_AF_PP << (9 * 2)) | GPIO_MODE_AF_PP << (11 * 2));  	// Set Alternate Function Push Pull Mode
	
	GPIOE->AFR[1] &= ~((0x0F << (1 * 4)) | (0x0F << (3 * 4)));					// Clear Alternate Function for pin (Higher ARF register)
	GPIOE->AFR[1] |= ((GPIO_AF1_TIM1 << (1 * 4)) | (GPIO_AF1_TIM1 << (3 * 4)));	// Set Alternate Function for pin (Higher ARF register) 

    // Enable Timer 1
    __TIM1_CLK_ENABLE();

    /* Compute the prescaler value
	   Set the clock prescaler to 50kHz
	   SystemCoreClock is the system clock frequency */
	TIM1->PSC = ((SystemCoreClock / 2) / TIMER_COUNTING_FREQ) - 1;

	// Set timer to count up.
	TIM1->CR1 &= ~TIM_CR1_DIR; 

	TIM1->ARR = PWM_PULSE_WIDTH_TICKS; 								// Set PWM waveform period 
	TIM1->CCR1 = PWM_PERCENT2TICKS_DUTYCYCLE(ZERO_DUTY_CYCLE);		// Set waveform duty cycle for Pan
	TIM1->CCR2 = PWM_PERCENT2TICKS_DUTYCYCLE(ZERO_DUTY_CYCLE);		// Set waveform duty cycle for Tilt

	TIM1->CCMR1 &= ~(TIM_CCMR1_OC1M | TIM_CCMR1_OC2M); 	// Clear OC1M and OC2M (Channels 1 and 2) 
	TIM1->CCMR1 |= ((0x06 << 4) | (0x06 << 12)); 		// Enable PWM Mode 1, upcounting, on Channels 1 and 2
	TIM1->CCMR1 |= (TIM_CCMR1_OC1PE | TIM_CCMR1_OC2PE); // Enable output preload bit for Channels 1 and 2 
	
	TIM1->CR1  |= (TIM_CR1_ARPE); 						// Set Auto-Reload Preload Enable 
	TIM1->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC2E); 		// Set CC1E and CC2E Bits
	TIM1->CCER &= ~(TIM_CCER_CC1NE | TIM_CCER_CC2NE); 	// Clear CC2NE Bit for active high output
	
	/* Set Main Output Enable (MOE) bit
	   Set Off-State Selection for Run mode (OSSR) bit
	   Set Off-State Selection for Idle mode (OSSI) bit */
	TIM1->BDTR |= TIM_BDTR_MOE | TIM_BDTR_OSSR | TIM_BDTR_OSSI; 

	TIM1->CR1 |= TIM_CR1_CEN;	// Enable the counter
}

/**
 * @brief Writes an angle to pan or tilt servo.
 * 
 * @param type Type is 0 for pan or 1 for tilt
 * @param angle the angle that will be written to either pan or tilt 
 */
void s4642286_pantilt_angle_write(int type, int angle) {
	// Convert angle to duty cycle.
	double dutyCycle = ANGLE_TO_DUTYCYCLE(angle);
	uint16_t numTicks = (uint16_t) PWM_PERCENT2TICKS_DUTYCYCLE(dutyCycle);

	if (type == 0) {
		// Adjust pan angle by changing CCR1 register value;.
		TIM1->CCR1 = numTicks;
	} else if (type == 1) {
		// Adjust tilt angle by changing CCR2 register value.
		TIM1->CCR2 = numTicks;
	}
}

/**
 * @brief Read the current pan or tilt servo angle.
 * 
 * @param type Type is 0 for pan or 1 for tilt
 */
int s4642286_pantilt_angle_read(int type) {
	uint16_t tickNum;
	
	if (type == 0) {
		tickNum = TIM1->CCR1;
	} else if (type == 1) {
		tickNum = TIM1->CCR2;
	}
	double dutyCycle = PWM_TICKS2PERCENT_DUTYCYCLE(tickNum);
	return DUTYCYCLE_TO_ANGLE(dutyCycle);
}
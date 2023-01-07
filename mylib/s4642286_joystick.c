 /** 
 **************************************************************
 * @file mylib/s4642286_joystick.c
 * @author Ethan Pinto - 46422860
 * @date 16032022
 * @brief Joystick peripheral driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_reg_joystick_pb_init() - Intialise the Joystick pushbutton source.
 * s4642286_reg_joystick_pb_isr() - Joystick Pushbutton Interrupt service routine.
 * s4642286_reg_joystick_press_get() - Returns the value of the Joystick pushbutton press counter.
 * s4642286_reg_joystick_press_reset() - Reset the Joystick press counter value to 0.
 * s4642286_reg_joystick_init() - Intialise GPIO pins and ADC.
 * s4642286_tsk_joystick_init() - Creates the Joystick controlling task.
 *************************************************************** 
 */

#include "board.h"
#include "processor_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "s4642286_joystick.h"
#include "s4642286_CAG_joystick.h"

// Global Variables for joystick press counter and falling edge time.
static int joystickPressCounter;
static uint32_t fallingEdgeTime;

/**
 * @brief The controlling task for the Joystick. This task uses semaphores
 * 		  to manipulate the Joystick. It calls Joystick register functions
 * 		  and creates semaphores.
 * 
 */
void s4642286TaskJoystick(void) {
	// Initialise the Joystick x and y signals and the pushbutton.
	s4642286_reg_joystick_init();
	s4642286_reg_joystick_pb_init();

	// Create Semaphore for pushbutton.
	if (pbSem == NULL) {
		pbSem = xSemaphoreCreateBinary();
	}

	// Cyclic Executive (CE) loop
	for (;;) {
		// Check Joystick x and y values.
		int x = S4642286_REG_JOYSTICK_X_READ();
		int y = S4642286_REG_JOYSTICK_Y_READ();

		JoystickValues sendValues;

		if (JoystickQueue != NULL) {
			sendValues.x_joystick = x;
			sendValues.y_joystick = y;
			xQueueSendToFront(JoystickQueue, ( void * ) &sendValues, ( portTickType ) 10);
		}
		
		// Delay the task for 10ms.
		vTaskDelay(10);
	}
}

/**
 * @brief Creates the Joystick controlling task.
 * 
 */
void s4642286_tsk_joystick_init(void) {
    xTaskCreate( (void *) &s4642286TaskJoystick, (const signed char *) "JOYSTICK", JOYSTICKTASK_STACK_SIZE, NULL, JOYSTICKTASK_PRIORITY, NULL);
}

/**
 * @brief This function initialises pin A0 as an input and sets up interrupts.
 * 
 */
void s4642286_reg_joystick_pb_init(void) {
    // Enable clock for Port A
    __GPIOA_CLK_ENABLE();

    // Make Pin A0 an Input
    GPIOA->OSPEEDR |= (GPIO_SPEED_FAST << (3 * 2));	// Set fast speed.
	GPIOA->PUPDR &= ~(0x03 << (3 * 2));				// Clear bits for no push/pull
	GPIOA->MODER &= ~(0x03 << (3 * 2));				// Clear bits for input mode

    // Enable EXTI clock
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// Select trigger source (Port A, Pin 3) on EXTICR1.
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI3;
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI3_PA;

	EXTI->RTSR |= EXTI_RTSR_TR3;	// Enable rising edge
	EXTI->FTSR |= EXTI_FTSR_TR3;	// Enable falling edge
	EXTI->IMR |= EXTI_IMR_IM3;		// Enable external interrupt

	HAL_NVIC_SetPriority(EXTI3_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);
}


/**
 * @brief Initialises GPIO analog pins PC0 and PC3 as analog inputs and configures ADC Channels.
 * 
 */
void s4642286_reg_joystick_init(void) {
    // Enable clock for Port C
    __GPIOC_CLK_ENABLE();

    // Make Pins PC0 and PC3 Analog Inputs.
    GPIOC->OSPEEDR |= ((GPIO_SPEED_FAST << (0 * 2)) | (GPIO_SPEED_FAST << (3 * 2)));	// Set fast speed.
	GPIOC->PUPDR &= ~((0x03 << (0 * 2)) | (0x03 << (3 * 2)));							// Clear bits for no push/pull
	GPIOC->MODER |= ((0x03 << (0 * 2)) | (0x03 << (3 * 2)));							// Set bits for analog input mode

	// Enable ADC1 clock
	__ADC1_CLK_ENABLE();

	adcHandlerX.Instance = (ADC_TypeDef *)(ADC1_BASE);						// Use ADC1
	adcHandlerX.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;	// Set clock prescaler
	adcHandlerX.Init.Resolution            = ADC_RESOLUTION12b;				// Set 12-bit data resolution
	adcHandlerX.Init.ScanConvMode          = DISABLE;
	adcHandlerX.Init.ContinuousConvMode    = DISABLE;
	adcHandlerX.Init.DiscontinuousConvMode = DISABLE;
	adcHandlerX.Init.NbrOfDiscConversion   = 0;
	adcHandlerX.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;	// No Trigger
	adcHandlerX.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;	// No Trigger
	adcHandlerX.Init.DataAlign             = ADC_DATAALIGN_RIGHT;			// Right align data
	adcHandlerX.Init.NbrOfConversion       = 1;
	adcHandlerX.Init.DMAContinuousRequests = DISABLE;
	adcHandlerX.Init.EOCSelection          = DISABLE;

	HAL_ADC_Init(&adcHandlerX);		// Initialise ADC1

	// Configure ADC Channel for Board pin A1
	adcChanConfigX.Channel = ADC_CHANNEL_10;					// PC0 has Analog Channel 10 connected
	adcChanConfigX.Rank         = 1;
	adcChanConfigX.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	adcChanConfigX.Offset       = 0;

	// Enable ADC2 clock
	__ADC2_CLK_ENABLE();

	adcHandlerY.Instance = (ADC_TypeDef *)(ADC2_BASE);						// Use ADC2
	adcHandlerY.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;	// Set clock prescaler
	adcHandlerY.Init.Resolution            = ADC_RESOLUTION12b;				// Set 12-bit data resolution
	adcHandlerY.Init.ScanConvMode          = DISABLE;
	adcHandlerY.Init.ContinuousConvMode    = DISABLE;
	adcHandlerY.Init.DiscontinuousConvMode = DISABLE;
	adcHandlerY.Init.NbrOfDiscConversion   = 0;
	adcHandlerY.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;	// No Trigger
	adcHandlerY.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;	// No Trigger
	adcHandlerY.Init.DataAlign             = ADC_DATAALIGN_RIGHT;			// Right align data
	adcHandlerY.Init.NbrOfConversion       = 1;
	adcHandlerY.Init.DMAContinuousRequests = DISABLE;
	adcHandlerY.Init.EOCSelection          = DISABLE;

	HAL_ADC_Init(&adcHandlerY);		//Initialise ADC2

	// Configure ADC Channel for Board pin A2
	adcChanConfigY.Channel = ADC_CHANNEL_13;					// PC3 has Analog Channel 13 connected
	adcChanConfigY.Rank         = 1;
	adcChanConfigY.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	adcChanConfigY.Offset       = 0;

	HAL_ADC_ConfigChannel(&adcHandlerX, &adcChanConfigX);		// Initialise ADC Channel for X input
	HAL_ADC_ConfigChannel(&adcHandlerY, &adcChanConfigY);		// Initialise ADC Channel for Y input
}


/**
 * @brief This function reads and returns the X or Y joystick value.
 * 
 * @param adcHandler The ADC Handler for the X or Y joystick analog input. 
 * @return int The X or Y joystick ADC value.
 */
int s4642286_joystick_readxy(ADC_HandleTypeDef adcHandler) {
	int adcValue;

	HAL_ADC_Start(&adcHandler); // Start ADC conversion

	// Wait for ADC conversion to finish
	while (HAL_ADC_PollForConversion(&adcHandler, 10) != HAL_OK);
	if (adcHandler.Instance == (ADC_TypeDef *)ADC1_BASE) {
		// Read converted value from ADC1 Data Register
		adcValue = ADC1->DR;
		
	} else if (adcHandler.Instance == (ADC_TypeDef *)ADC2_BASE) {
		// Read converted value from ADC2 Data Register
		adcValue = ADC2->DR;
	}
	return adcValue;
}



/**
 * @brief This function implements pushbutton debouncing and is
 * called by the EXTI 3 IRQ Handler.
 * 
 */
void s4642286_reg_joystick_pb_isr(void) {
	
	BaseType_t xHigherPriorityTaskWoken;

    // Check if button has been pressed.
    if ((GPIOA->IDR & (0x01 << 3)) == (0x01 << 3)) {
		fallingEdgeTime = HAL_GetTick();
    } else {
        if ((HAL_GetTick() - fallingEdgeTime) > 20) {
            // Button is high again (button been released)
			joystickPressCounter++;
			
			// Is it time for another Task() to run?
			xHigherPriorityTaskWoken = pdFALSE;

			// Check if semaphore exists and if so, release it.
			if (pbSem != NULL) {
				xSemaphoreGiveFromISR( pbSem, &xHigherPriorityTaskWoken );
			}

			// Perform context switching, if required.
			portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
        }
    }
}

/**
 * @brief Interrupt handler (ISR) for EXTI 3 IRQ Handler.
 * 
 */
void EXTI3_IRQHandler(void) {

	NVIC_ClearPendingIRQ(EXTI3_IRQn);

	if ((EXTI->PR & EXTI_PR_PR3) == EXTI_PR_PR3) {
		//Clear interrupt flag.
		EXTI->PR |= EXTI_PR_PR3;

		s4642286_reg_joystick_pb_isr();   // Call the Joystick ISR
	}
}

/**
 * @brief This function returns the current press counter of the joystick.
 * 
 * @return int - returns the joystick press counter.
 */
int s4642286_reg_joystick_press_get(void) {
	return joystickPressCounter;
}

/**
 * @brief This function resets the joystick press counter to 0.
 * 
 */
void s4642286_reg_joystick_press_reset(void) {
	joystickPressCounter = 0; // Set Joystick press count to 0.
}

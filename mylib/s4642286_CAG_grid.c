 /** 
 **************************************************************
 * @file mylib/s4642286_CAG_grid.c
 * @author Ethan Pinto - 46422860
 * @date 5/05/2022
 * @brief CAG grid mylib Task Peripheral Driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_tsk_CAGGrid_init() - Creates the CAGGrid controlling task.
 *************************************************************** 
 */

#include "board.h"
#include "processor_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "debug_log.h"
#include <ctype.h>
#include "s4642286_lta1000g.h"
#include "s4642286_CAG_grid.h"
#include "s4642286_CAG_simulator.h"

/**
 * @brief The controlling task for CAG Grid. This task is used to control
 * 		  the grid which allows the user to select individual cells using
 * 		  keyboard keys.
 *  
 */
void s4642286TaskCAGGrid(void) {
	// Initialisation of User PB.
	s4642286_reg_user_pb_init();

	// Create the grid/mnemonic semaphore and the event group for key presses.
	modeSelectSem = xSemaphoreCreateBinary();
	keyPresses = xEventGroupCreate();

	// Variables to store character from input, event bits, cursor positions and current mode.
	char recvChar;
	EventBits_t uxBits;
	xpos = 0;
	ypos = 0;
	gridOn = 1;

	// Structs used for queues to LED Bar and simulator.
	xyposition positionSend;
	caMessage_t sendCell;
	
	for (;;) {	
		if (gridOn) {
			if ((recvChar = BRD_debuguart_getc()) != '\0') {
				// Process input character.
				switch (toupper(recvChar)) {
					case 'W':
						xEventGroupSetBits(keyPresses, UP);
						if (ypos - 1 >= 0 && ypos - 1 < SIMULATION_HEIGHT) {
							ypos--;
						}
						break;

					case 'A':
						xEventGroupSetBits(keyPresses, LEFT);
						// Move the cursor left (if within bounds)
						if (xpos - 1 >= 0 && xpos - 1 < SIMULATION_WIDTH) {
							xpos--;
						}	
						break;

					case 'S':
						xEventGroupSetBits(keyPresses, DOWN);
						// Move the cursor down (if within bounds)
						if (ypos + 1 >= 0 && ypos + 1 < SIMULATION_HEIGHT) {
							ypos++;
						}
						break;

					case 'D':
						xEventGroupSetBits(keyPresses, RIGHT);
						// Move the cursor right (if within bounds)
						if (xpos + 1 >= 0 && xpos + 1 < SIMULATION_WIDTH) {
							xpos++;
						}	
						break;

					case 'X':
						xEventGroupSetBits(keyPresses, SELECT);
						if (SimQueue != NULL) {
							// Select the cell and make it alive.
							sendCell.cell_x = xpos;
							sendCell.cell_y = ypos;
							sendCell.type = CELL_ALIVE;
							xQueueSendToFront(SimQueue, ( void * ) &sendCell, ( portTickType ) 10);
						}
						break;

					case 'Z':
						xEventGroupSetBits(keyPresses, UNSELECT);
						if (SimQueue != NULL) {
							// Unselect the cell and make it dead.
							sendCell.cell_x = xpos;
							sendCell.cell_y = ypos;
							sendCell.type = CELL_DEAD;
							xQueueSendToFront(SimQueue, ( void * ) &sendCell, ( portTickType ) 10);
						}
						break;

					case 'P':
						// Toggle Start/Stop
						if (specialFunctions != NULL) {
							if (gameRunning) {
								// Game is currently running, so stop it.
								xEventGroupSetBits(specialFunctions, STOP_SIM);

							} else {
								// Game is currently paused, so start it.
								xEventGroupSetBits(specialFunctions, START_SIM);
							}	
						}
						break;

					case 'O':
						xEventGroupSetBits(keyPresses, MOVE_ORIGIN);
						// Move cursor to origin.
						xpos = 0;
						ypos = 0;
						break;

					case 'C':
						xEventGroupSetBits(specialFunctions, CLEAR_GRID);
						break;

					default:
						break;
				}

				// Send current cursor position to LED Bar.
				if (positionQueue != NULL) {
					positionSend.value = (xpos << 4 | ypos);
					xQueueSendToFront(positionQueue, ( void * ) &positionSend, ( portTickType ) 10);
				}
			}
		}

		// Delay the task for 10ms.
		vTaskDelay(10);
	}
}


/**
 * @brief Creates the CAGDisplay controlling task.
 * 
 */
void s4642286_tsk_CAGGrid_init(void) {
    xTaskCreate( (void *) &s4642286TaskCAGGrid, (const signed char *) "GRID", GRIDTASK_STACK_SIZE, NULL, GRIDTASK_PRIORITY, NULL);
}

/**
 * @brief This function initialises pin C13 as an input and sets up interrupts.
 * 
 */
void s4642286_reg_user_pb_init(void) {

	BRD_LEDGreenOn();

	// Enable GPIO Clock
	__GPIOC_CLK_ENABLE();

	// Make Pin C13 an input
    GPIOC->OSPEEDR |= (GPIO_SPEED_FAST << 13);		//Set fast speed.
	GPIOC->PUPDR &= ~(0x03 << (13 * 2));			//Clear bits for no push/pull
	GPIOC->MODER &= ~(0x03 << (13 * 2));			//Clear bits for input mode

	// Enable EXTI clock
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	//select trigger source (Port C, Pin 13) on EXTICR4.
	SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;

	EXTI->RTSR |= EXTI_RTSR_TR13;	// Enable rising dedge
	EXTI->FTSR &= ~EXTI_FTSR_TR13;	// Disable falling edge
	EXTI->IMR |= EXTI_IMR_IM13;		// Enable external interrupt

	//Enable priority (10) and interrupt callback.
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/**
 * @brief This function implements pushbutton debouncing and is
 * 		  called by the EXTI15_10 IRQ Handler.
 * 
 */
void s4642286_reg_user_pb_isr(void) {
	BaseType_t xHigherPriorityTaskWoken;

    // Check if button has been pressed.
    if ((GPIOC->IDR & (0x01 << 13)) == (0x01 << 13) && (xTaskGetTickCount() - fallingEdgeTime) > DEBOUNCE_TIME) {	
		// Is it time for another Task() to run?
		xHigherPriorityTaskWoken = pdFALSE;

		// Toggle grid mode.
		gridOn = ~gridOn & 0x01;

		if (gridOn == 0) {
			// Mnemonic mode is on, thus Green LED should be OFF.
			BRD_LEDGreenOff();

			// Check if semaphore exists and if so, release it to change grid to mnemonic.
			if (modeSelectSem != NULL) {
				xSemaphoreGiveFromISR( modeSelectSem, &xHigherPriorityTaskWoken );
			}
		} else {
			// Grid mode is on, thus green LED should be ON.
			BRD_LEDGreenOn();
		}

		// Perform context switching, if required.
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
		fallingEdgeTime = xTaskGetTickCount();
    }
}

/**
 * @brief Interrupt handler (ISR) for EXTI 15 to 10 IRQ Handler
 * 
 */
void EXTI15_10_IRQHandler(void) {

	NVIC_ClearPendingIRQ(EXTI15_10_IRQn);

	if ((EXTI->PR & EXTI_PR_PR13) == EXTI_PR_PR13) {

		// Clear interrupt flag.
		EXTI->PR |= EXTI_PR_PR13;

		s4642286_reg_user_pb_isr();
	}
}

/**
  ******************************************************************************
  * @file    repo/s4/main.c
  * @author  Ethan Pinto
  * @date    17/04/2022
  * @brief   A program which runs using FreeRTOS that is used to create a dual
  *          timer displayed on an OLED board. The start/stop of the timer is
  * 	     controlled with the joystick Z signal.
  * 
  ******************************************************************************
  *
  */

#include "board.h"
#include "processor_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "s4642286_oled.h"
#include "s4642286_joystick.h"

struct dualTimerMsg {
	char type;	// type is either l or r
	unsigned short timerValue;
};

// Create a queue to be used to send left and right timer values to TaskTimerDisplay.
QueueHandle_t queueTimerDisplay;

// Function and Task Declarations
void hardware_init(void);
void TaskTimerLeft(void);
void TaskTimerRight(void);
void TaskTimerDisplay(void);

// Task Priorities 
#define TASKTIMERLEFT_PRIORITY		( tskIDLE_PRIORITY + 4 )
#define TASKTIMERRIGHT_PRIORITY		( tskIDLE_PRIORITY + 4 )
#define TASKTIMERDISPLAY_PRIORITY	( tskIDLE_PRIORITY + 4 )

// Task Stack Allocations 
#define TASKTIMERLEFT_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )
#define TASKTIMERRIGHT_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )
#define TASKTIMERDISPLAY_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )

// Timer x and y Macro definitions.
#define LEFT_START_X 25
#define LEFT_START_Y 10
#define RIGHT_START_X 75
#define RIGHT_START_Y 10

uint8_t mode = 1;

/*
 * Starts all the tasks, then starts the scheduler.
 */
int main( void ) {

	HAL_Init();

	// Create the OLED and Joystick Controlling Tasks
	s4642286_tsk_oled_init();
	s4642286_tsk_joystick_init();

	// Start the Dual Timer Tasks
	xTaskCreate( (void *) &TaskTimerLeft, (const signed char *) "LEFT", TASKTIMERLEFT_STACK_SIZE, NULL, TASKTIMERLEFT_PRIORITY, NULL );
	xTaskCreate( (void *) &TaskTimerRight, (const signed char *) "RIGHT", TASKTIMERRIGHT_STACK_SIZE, NULL, TASKTIMERRIGHT_PRIORITY, NULL );
	xTaskCreate( (void *) &TaskTimerDisplay, (const signed char *) "DISPLAY", TASKTIMERDISPLAY_STACK_SIZE, NULL, TASKTIMERDISPLAY_PRIORITY, NULL );

	// Start the scheduler
	vTaskStartScheduler();

  	return 0;
}


/**
 * @brief A task representing the left timer which counts every 1s.
 * 
 */
void TaskTimerLeft(void) {
	int count = 0;

	struct dualTimerMsg SendMessage;
	TickType_t xLastWakeTime = xTaskGetTickCount();

	hardware_init();

	// Adjust message to identify left timer.
	SendMessage.type = 'l';

	for (;;) {
		
		if (pbSem != NULL) {	// Check if semaphore exists

			/* See if we can obtain the PB semaphore for the left timer. If the semaphore
			is not available wait 10 ticks to see if it becomes free. */
			if( xSemaphoreTake( pbSem, 10 ) == pdTRUE ) {
            	// We were able to obtain the semaphore and can now access the shared resource. 

            	// Invert mode to stop or start timer
				mode = ~mode & 0x01;
        	}
		}
		
		// If the mode is 1, the timer runs, if the mode is 0, the timer is stopped.
		if (mode == 1) {
			if (queueTimerDisplay != NULL) {	// Check if queue exists 
				count++;

				// Send message to the front of the queue - wait atmost 1000 ticks 
				SendMessage.timerValue = count;

				xQueueSendToFront(queueTimerDisplay, ( void * ) &SendMessage, ( portTickType ) 1000 );
			}

		}
		// Wait for 1s 
		vTaskDelayUntil( &xLastWakeTime, 1000);
	}
}


/**
 * @brief A task representing the right timer which counts every 0.01s.
 * 
 */
void TaskTimerRight(void) {
	int count = 0;
	uint8_t mode = 1;

	struct dualTimerMsg SendMessage;
	TickType_t xLastWakeTime = xTaskGetTickCount();

	hardware_init();

	// Adjust message to identify right timer.
	SendMessage.type = 'r';

	for (;;) {

		// If the mode is 1, the timer runs, if the mode is 0, the timer is stopped.
		if (mode == 1) {
			count++;

			if (count > 99) {
				count = 0;
			}

			if (queueTimerDisplay != NULL) {	// Check if queue exists 

				// Send message to the front of the queue - wait atmost 10 ticks 
				SendMessage.timerValue = count;

				xQueueSendToFront(queueTimerDisplay, ( void * ) &SendMessage, ( portTickType ) 10 );
			}

		}
		vTaskDelayUntil( &xLastWakeTime, 10);
	}
}


/**
 * @brief A task which shows the timer values on the OLED.
 * 		  3 digits for the left timer. 2 digits for right timer.
 * 
 */
void TaskTimerDisplay(void) {

	hardware_init();
	
	struct dualTimerMsg RecvMessage;
	TickType_t xLastWakeTime = xTaskGetTickCount();

	// Create the timer display queue.
	queueTimerDisplay = xQueueCreate(30, sizeof(RecvMessage));

	for (;;) {

		if (queueTimerDisplay != NULL && OLEDQueue != NULL) {	// Check if queue exists

			// Check for item received - block atmost for 10 ticks
			if (mode == 1 && xQueueReceive( queueTimerDisplay, &RecvMessage, 10 )) {
				struct oledTextMsg oledMessageSend;
				
				if (RecvMessage.type == 'l') {
					oledMessageSend.startX = LEFT_START_X;
					oledMessageSend.startY = LEFT_START_Y;

				} else if (RecvMessage.type == 'r') {
					oledMessageSend.startX = RIGHT_START_X;
					oledMessageSend.startY = RIGHT_START_Y;

				}
				// Convert timer value to an array of characters
				sprintf(oledMessageSend.displayText, "%d", RecvMessage.timerValue);

				xQueueSendToFront(OLEDQueue, ( void * ) &oledMessageSend, ( portTickType ) 10 );
        	}
		}
		// Delay for 55ms
		vTaskDelay(55);
	}
}

/*
 * Hardware Initialisation.
 */
void hardware_init( void ) {

	portDISABLE_INTERRUPTS();	//Disable interrupts
	
	BRD_LEDInit();				//Initialise LEDs

	portENABLE_INTERRUPTS();	//Enable interrupts

}
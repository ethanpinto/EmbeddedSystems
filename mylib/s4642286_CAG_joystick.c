 /** 
 **************************************************************
 * @file mylib/s4642286_CAG_joystick.c
 * @author Ethan Pinto - 46422860
 * @date 5/05/2022
 * @brief CAG Joystick mylib Task Peripheral Driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_tsk_CAGJoystick_init() - Creates the CAGJoystick controlling task.
 *************************************************************** 
 */

#include "board.h"
#include "processor_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "s4642286_joystick.h"
#include "s4642286_CAG_joystick.h"
#include "s4642286_CAG_simulator.h"

/**
 * @brief The controlling task for the CAG Joystick. This function will
 * 		  control the simulation update time of the CAG simulator as well
 * 		  as pausing, resetting and clearing the simulation.
 *  
 */
void s4642286TaskCAGJoystick(void) {
	// Variables to store x and y position of joystick.
	int x, y;
	
	// Create queue to receive joystick postions.
	JoystickValues joystickRecieve;
	JoystickQueue = xQueueCreate(10, sizeof(joystickRecieve));

	// Cyclic Executive (CE) loop
	for (;;) {
		if (JoystickQueue != NULL) {
			if (xQueueReceive(JoystickQueue, &joystickRecieve, 10)) {
				x = joystickRecieve.x_joystick;
				y = joystickRecieve.y_joystick;

				// Check x value.
				if (x < XY_MIN) {
					// Joystick x signal is at extreme minimum. Thus, pause simulation.
					xEventGroupSetBits(specialFunctions, STOP_SIM);

				} else if (x > XY_MAX) {
					// Joystick x signal is at extreme maximum. Thus, start simulation.
					xEventGroupSetBits(specialFunctions, START_SIM);
				}

				// Check y value.
				if (y < XY_MIN) {
					// Joystick y signal is at extreme minimum. Thus, set simulation time to 1s.
					xEventGroupSetBits(specialFunctions, UPDATE_SPEED_1);

				} else if (y > XY_MAX) {
					// Joystick y signal is at extreme maximum. Thus, set simulation time to 10s.
					xEventGroupSetBits(specialFunctions, UPDATE_SPEED_10);

				} else if (y > XY_MID_LOW && y < XY_MID_HIGH){
					// Joystick y signal is upright, thus, set simulation time to 2s.
					xEventGroupSetBits(specialFunctions, UPDATE_SPEED_2);
				
				} else if (y > XY_MID_HIGH && y < XY_MAX){
					// Joystick y signal is nearly at maximum, thus, set simulation time to 5s.
					xEventGroupSetBits(specialFunctions, UPDATE_SPEED_5);
				
				}
			}
		}

		if( xSemaphoreTake( pbSem, 10 ) == pdTRUE ) {
			// Joystick Pushbutton pressed, so clear display and reset simulator.
			xEventGroupSetBits(specialFunctions, STOP_SIM);
			xEventGroupSetBits(specialFunctions, CLEAR_GRID);
			xEventGroupClearBits(specialFunctions, UPDATE_SPEED_2);
        }
		
		// Delay the task for 10ms.
		vTaskDelay(10);
	}
}


/**
 * @brief Creates the CAGJoystick controlling task.
 * 
 */
void s4642286_tsk_CAGJoystick_init(void) {
    xTaskCreate( (void *) &s4642286TaskCAGJoystick, (const signed char *) "CAG JOY", CAGJOYSTICKTASK_STACK_SIZE, NULL, CAGJOYSTICKTASK_PRIORITY, &xJoystickHandle);
}


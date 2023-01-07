 /** 
 **************************************************************
 * @file mylib/s4642286_CAG_display.c
 * @author Ethan Pinto - 46422860
 * @date 5/05/2022
 * @brief CAG display mylib Task Peripheral Driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_tsk_CAGDisplay_init() - Creates the CAGDisplay controlling task.
 *************************************************************** 
 */

#include "board.h"
#include "processor_hal.h"
#include "oled_pixel.h"
#include "oled_string.h"
#include "fonts.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "s4642286_CAG_display.h"
#include "s4642286_oled.h"
#include "s4642286_CAG_simulator.h"

/**
 * @brief The controlling task for the CAG Display. This task will receive
 * 		  information about the current board and send it to the OLED via
 * 		  a queue to be displayed.
 *  
 */
void s4642286TaskCAGDisplay(void) {
    // Initialise the SSD1306 OLED display.
    s4642286_reg_oled_init();

	hardware_init();

	//Clear Screen	
	ssd1306_Fill(Black);

	// Create a queue of game display messages.
	displayMessage gameReceive;
	DisplayQueue = xQueueCreate(3, sizeof(gameReceive));

	// Cyclic Executive (CE) loop
	for (;;) {

		if (DisplayQueue != NULL) {	// Check if the Display Queue exists

			// Check for item received - block atmost for 10 ticks
			if (xQueueReceive( DisplayQueue, &gameReceive, 10 )) {
				// Check which cells are alive and which are dead.
				for (int x = 0; x < SIMULATION_WIDTH; x++) {
					for (int y = 0; y < SIMULATION_HEIGHT; y++) {
						int alive = gameReceive.currentSimulation[x][y] - '0';
						if (alive) {
							// Light up cell
							ssd1306_DrawPixel((2*x), (2*y), WHITE);
							ssd1306_DrawPixel((2*x + 1), (2*y), WHITE);
							ssd1306_DrawPixel((2*x), (2*y + 1), WHITE);
							ssd1306_DrawPixel((2*x + 1), (2*y + 1), WHITE);

						} else {
							// Cell is dead, so remove it.
							ssd1306_DrawPixel((2*x), (2*y), BLACK);
							ssd1306_DrawPixel((2*x + 1), (2*y), BLACK);
							ssd1306_DrawPixel((2*x), (2*y + 1), BLACK);
							ssd1306_DrawPixel((2*x + 1), (2*y + 1), BLACK);
						}
					}
				}
				ssd1306_UpdateScreen();	
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
void s4642286_tsk_CAGDisplay_init(void) {
    xTaskCreate( (void *) &s4642286TaskCAGDisplay, (const signed char *) "DISPLAY", DISPLAYTASK_STACK_SIZE, NULL, DISPLAYTASK_PRIORITY, NULL );
}

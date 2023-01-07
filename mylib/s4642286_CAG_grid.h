 /** 
 **************************************************************
 * @file mylib/s4642286_CAG_grid.h
 * @author Ethan Pinto - 46422860
 * @date 5/05/2022
 * @brief CAG grid mylib Task Peripheral Driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_tsk_CAGGrid_init() - Creates the CAGGrid controlling task.
 *************************************************************** 
 */

#ifndef S4642286_CAG_GRID_H
#define S4642286_CAG_GRID_H

#include "semphr.h"
#include "event_groups.h"

// Function Definitions
extern void s4642286_tsk_CAGGrid_init(void);
void s4642286_reg_user_pb_init(void);

// Task Priority
#define GRIDTASK_PRIORITY	( tskIDLE_PRIORITY + 4 )

// Task Stack Size
#define GRIDTASK_STACK_SIZE ( configMINIMAL_STACK_SIZE * 2 )

#define OLED_WIDTH 128  // In Pixels
#define OLED_HEIGHT 32  // In Pixels
#define SIMULATION_WIDTH (OLED_WIDTH / CELL_SIZE)
#define SIMULATION_HEIGHT (OLED_HEIGHT / CELL_SIZE)
#define CELL_SIZE 2

// Event Group Bit Definitions - CAG Grid
EventGroupHandle_t keyPresses;
#define UP		        1 << 0		// W key pressed
#define LEFT		    1 << 1		// A key pressed
#define DOWN		    1 << 2      // S key pressed
#define RIGHT           1 << 3      // D key pressed
#define SELECT          1 << 4      // X key pressed
#define UNSELECT        1 << 5      // Z key pressed
#define MOVE_ORIGIN     1 << 6      // O key pressed

// Define a debounce time
#define DEBOUNCE_TIME 15

// Create variables which represent the current x and y position of the cursor.
unsigned short xpos;
unsigned short ypos;

// Create a variable to identify if grid is running or not.
int gridOn;

// Semaphore to control grid and mnemonic mode.
SemaphoreHandle_t modeSelectSem;

// Global Variables for falling edge time.
static uint32_t fallingEdgeTime;



#endif
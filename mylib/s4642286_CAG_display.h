 /** 
 **************************************************************
 * @file mylib/s4642286_CAG_display.h
 * @author Ethan Pinto - 46422860
 * @date 5/05/2022
 * @brief CAG display mylib Task Peripheral Driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_tsk_CAGDisplay_init() - Creates the CAGDisplay controlling task.
 *************************************************************** 
 */

#ifndef S4642286_CAG_DISPLAY_H
#define S4642286_CAG_DISPLAY_H

// External Function Definitions
extern void s4642286_tsk_CAGDisplay_init(void);

// Task Priority
#define DISPLAYTASK_PRIORITY	( tskIDLE_PRIORITY + 3 )

// Task Stack Size
#define DISPLAYTASK_STACK_SIZE ( configMINIMAL_STACK_SIZE * 10 )

#define OLED_WIDTH 128  // In Pixels
#define OLED_HEIGHT 32  // In Pixels
#define SIMULATION_WIDTH (OLED_WIDTH / CELL_SIZE)
#define SIMULATION_HEIGHT (OLED_HEIGHT / CELL_SIZE)
#define CELL_SIZE 2

/**
 * @brief A struct for sending the game layout to the display.
 * 
 */
typedef struct {
    char currentSimulation[SIMULATION_WIDTH][SIMULATION_HEIGHT];
} displayMessage;

#endif
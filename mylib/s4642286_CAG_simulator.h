 /** 
 **************************************************************
 * @file mylib/s4642286_CAG_simulator.h
 * @author Ethan Pinto - 46422860
 * @date 5/05/2022
 * @brief CAG simulator mylib Task Peripheral Driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_tsk_CAGSimulator_init() - Creates the CAGSimulator controlling task.
 *************************************************************** 
 */

#ifndef S4642286_CAG_SIMULATOR_H
#define S4642286_CAG_SIMULATOR_H

#include "semphr.h"
#include "event_groups.h"

// External Function Definitions
extern void s4642286_tsk_CAGSimulator_init(void);
extern void hardware_init(void);

// Task Priority
#define SIMULATORTASK_PRIORITY	( tskIDLE_PRIORITY + 4 )

// Task Stack Size
#define SIMULATORTASK_STACK_SIZE ( configMINIMAL_STACK_SIZE * 5 )

/**
 * @brief A struct for the cellular automation message.
 * 
 */
typedef struct {
    int type;       // Type - Cell or Lifeform
    int cell_x;     // Cell/Lifeform x position
    int cell_y;     // Cell/Lifeform y position
} caMessage_t;

#define OLED_WIDTH 128  // In Pixels
#define OLED_HEIGHT 32  // In Pixels
#define SIMULATION_WIDTH (OLED_WIDTH / CELL_SIZE)
#define SIMULATION_HEIGHT (OLED_HEIGHT / CELL_SIZE)
#define CELL_SIZE 2

// Array of cells
static int cells[SIMULATION_WIDTH][SIMULATION_HEIGHT]; // Points in the array are in the form (x,y)

// Buffer to record the state of the cells.
static int cellsBuffer[SIMULATION_WIDTH][SIMULATION_HEIGHT]; 

// The queue for sending information to the simulation and to the display.
QueueHandle_t SimQueue;
QueueHandle_t DisplayQueue;

// Task handle for the simulator.
TaskHandle_t xSimHandle;

// Global variables which represent the speed of the simulation, and if it is running or not.
unsigned long simSpeed;
int gameRunning;

// Event Group Bit Definitions - Special Functions
EventGroupHandle_t specialFunctions;
#define CLEAR_GRID		    1 << 0		// Clear the Grid
#define START_SIM		    1 << 1		// Start the simulation
#define STOP_SIM		    1 << 2      // Stop the simulation
#define UPDATE_SPEED_1      1 << 3      // Update the simulation speed to 1s
#define UPDATE_SPEED_2      1 << 4      // Update the simulation speed to 2s
#define UPDATE_SPEED_5      1 << 5      // Update the simulation speed to 5s
#define UPDATE_SPEED_10     1 << 6      // Update the simulation speed to 10s
#define CONTROL_BITS 0x7F

// Define Type values
#define CELL_DEAD 0x10
#define CELL_ALIVE 0x11
#define STILL_BLOCK 0x20
#define STILL_BEEHIVE 0x21
#define STILL_LOAF 0x22
#define OSC_BLINKER 0x30
#define OSC_TOAD 0x31
#define OSC_BEACON 0x32
#define SPACESHIP_GLIDER 0x40

#endif
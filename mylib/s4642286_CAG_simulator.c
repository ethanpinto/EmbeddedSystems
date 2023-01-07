 /** 
 **************************************************************
 * @file mylib/s4642286_CAG_simulator.c
 * @author Ethan Pinto - 46422860
 * @date 5/05/2022
 * @brief CAG Simulator mylib Task Peripheral Driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_tsk_CAGSimulator_init() - Creates the CAGSimulator controlling task.
 *************************************************************** 
 */

#include "board.h"
#include "processor_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "s4642286_CAG_simulator.h"
#include "s4642286_CAG_display.h"

// Function Definitions
void check_special(void);
void reset_simulation(void);
void update_simulation(void);
void update_display(void);
void add_life(caMessage_t life);

/**
 * @brief The controlling task for the CAG Simulator. THis task handles the main
 * 		  flow of the program and takes in commands from CAG Grid, CAG Mnemonic,
 * 		  and CAG Joystick and processes them. It then sends information including
 * 		  the location of living cells to the CAG Display through a queue. It also
 * 		  uses and event group to perform special functions.
 *  
 */
void s4642286TaskCAGSimulator(void) {
	uint32_t previousTick = 0;
	int x, y;
	
	// Set initial simulator conditions (OFF and update speed of 2s)
	gameRunning = 0;
	simSpeed = 2000;

	// Create a queue of simulator messages.
	caMessage_t simReceive;

	hardware_init();

	// Create a queue to receive info for the simulator.
	if (SimQueue == NULL) {
		SimQueue = xQueueCreate(5, sizeof(simReceive));
	}

	// Create an event group to perform special functions on the simulator.
	if (specialFunctions == NULL) {
		specialFunctions = xEventGroupCreate();
	}

	// Initialise the simulation
	reset_simulation();
    
	for (;;) {
		// Check event bits to see if game is running and update simulation time.
		check_special();

		if (SimQueue != NULL) {
			if (xQueueReceive(SimQueue, &simReceive, 10)) {
				// Add new lifeform.
				add_life(simReceive);
			}
		}

		// Update the simulation.
		if ((xTaskGetTickCount() - previousTick) >= simSpeed) {		
			if (gameRunning) {
				// Apply the rules of cellular evolution to the current state of the game.
				update_simulation();
			}

			if (DisplayQueue != NULL) {
				// Update CAG display with the new game layout.
				update_display();
			}

			previousTick = xTaskGetTickCount();
		}
		vTaskDelay(10);
	}
}

/**
 * @brief This function check the event group bits for the special functions,
 *		  and adjusts the simulator parameters accordingly.
 * 
 */
void check_special(void) {
	EventBits_t uxBits;

	uxBits = xEventGroupWaitBits(specialFunctions, CONTROL_BITS, pdTRUE, pdFALSE, 10);

	// Check if Clear Grid Event flag is set
	if ((uxBits & CLEAR_GRID) != 0) {

		// Clear Grid
		reset_simulation();
		
		uxBits = xEventGroupClearBits(specialFunctions, CLEAR_GRID);
	}

	// Check if Start Simulation Event flag is set
	if ((uxBits & START_SIM) != 0) {

		// Start simulation
		gameRunning = 1;
		
		uxBits = xEventGroupClearBits(specialFunctions, START_SIM);
	}

	// Check if Stop Simulation Event flag is set
	if ((uxBits & STOP_SIM) != 0) {

		// Stop simulation
		gameRunning = 0;

		uxBits = xEventGroupClearBits(specialFunctions, STOP_SIM);
	}

	// Check if Update Simulation Speed to 1s Event flag is set
	if ((uxBits & UPDATE_SPEED_1) != 0) {

		// Update Simulation Speed to 1s
		simSpeed = 1000;

		uxBits = xEventGroupClearBits(specialFunctions, UPDATE_SPEED_1);
	}

	// Check if Update Simulation Speed to 2s Event flag is set
	if ((uxBits & UPDATE_SPEED_2) != 0) {

		// Update Simulation Speed to 2s
		simSpeed = 2000;

		uxBits = xEventGroupClearBits(specialFunctions, UPDATE_SPEED_2);
	}
	
	// Check if Update Simulation Speed to 5s Event flag is set
	if ((uxBits & UPDATE_SPEED_5) != 0) {

		// Update Simulation Speed to 5s
		simSpeed = 5000;

		uxBits = xEventGroupClearBits(specialFunctions, UPDATE_SPEED_5);
	}

	// Check if Update Simulation Speed to 10s Event flag is set
	if ((uxBits & UPDATE_SPEED_10) != 0) {

		// Update Simulation Speed to 10s
		simSpeed = 10000;

		uxBits = xEventGroupClearBits(specialFunctions, UPDATE_SPEED_10);
	}
}

/**
 * @brief Create a display message to send current layout to CAG Display
 * 
 */
void update_display(void) {
	displayMessage gameSend;

	for (int x = 0; x < SIMULATION_WIDTH; x++) {
		for (int y = 0; y < SIMULATION_HEIGHT; y++) {
			if (cells[x][y] == 1) {
				gameSend.currentSimulation[x][y] = '1';
			} else {
				gameSend.currentSimulation[x][y] = '0';
			}
		}
	}
	xQueueSendToBack(DisplayQueue, ( void * ) &gameSend, ( portTickType ) 10);
}

/**
 * @brief Add a life form to the current game map.
 * 
 * @param life the coordinates and the type of life to add.
 */
void add_life(caMessage_t life) {
	switch (life.type) {
		case CELL_DEAD:
			cells[life.cell_x][life.cell_y] = 0;
			break;

		case CELL_ALIVE:
			cells[life.cell_x][life.cell_y] = 1;
			break;

		case STILL_BLOCK:
			cells[life.cell_x][life.cell_y] = 1;
			cells[life.cell_x + 1][life.cell_y] = 1;
			cells[life.cell_x][life.cell_y + 1] = 1;
			cells[life.cell_x + 1][life.cell_y + 1] = 1;
			break;

		case STILL_BEEHIVE:
			cells[life.cell_x - 1][life.cell_y] = 1;
			cells[life.cell_x][life.cell_y - 1] = 1;
			cells[life.cell_x + 1][life.cell_y - 1] = 1;
			cells[life.cell_x][life.cell_y + 1] = 1;
			cells[life.cell_x + 1][life.cell_y + 1] = 1;
			cells[life.cell_x + 2][life.cell_y] = 1;
			break;

		case STILL_LOAF:
			cells[life.cell_x - 1][life.cell_y - 1] = 1;
			cells[life.cell_x][life.cell_y - 1] = 1;
			cells[life.cell_x - 2][life.cell_y] = 1;
			cells[life.cell_x + 1][life.cell_y] = 1;
			cells[life.cell_x - 1][life.cell_y + 1] = 1;
			cells[life.cell_x + 1][life.cell_y + 1] = 1;
			cells[life.cell_x][life.cell_y + 2] = 1;
			break;

		case OSC_BLINKER:
			cells[life.cell_x][life.cell_y] = 1;
			cells[life.cell_x + 1][life.cell_y] = 1;
			cells[life.cell_x - 1][life.cell_y] = 1;
			break;

		case OSC_TOAD:
			cells[life.cell_x][life.cell_y] = 1;
			cells[life.cell_x + 1][life.cell_y] = 1;
			cells[life.cell_x - 1][life.cell_y] = 1;
			cells[life.cell_x][life.cell_y + 1] = 1;
			cells[life.cell_x + 1][life.cell_y + 1] = 1;
			cells[life.cell_x + 2][life.cell_y + 1] = 1;
			break;
			
		case OSC_BEACON:
			cells[life.cell_x - 1][life.cell_y - 1] = 1;
			cells[life.cell_x - 1][life.cell_y - 2] = 1;
			cells[life.cell_x][life.cell_y - 2] = 1;
			cells[life.cell_x + 1][life.cell_y + 1] = 1;
			cells[life.cell_x + 2][life.cell_y + 1] = 1;
			cells[life.cell_x + 2][life.cell_y] = 1;
			break;

		case SPACESHIP_GLIDER:
			cells[life.cell_x][life.cell_y] = 1;
			cells[life.cell_x - 1][life.cell_y - 1] = 1;
			cells[life.cell_x + 1][life.cell_y] = 1;
			cells[life.cell_x - 1][life.cell_y + 1] = 1;
			cells[life.cell_x][life.cell_y + 1] = 1;
			break;	
	}
}

/**
 * @brief Initialise/Reset the simulation and sets all the cells to be empty.
 * 
 */
void reset_simulation(void) {
	for (int x = 0; x < SIMULATION_WIDTH; x++) {
		for (int y = 0; y < SIMULATION_HEIGHT; y++) {
			// Initialise each cell to be empty.
			cells[x][y] = 0;
		}
	}
}

/**
 * @brief Runs at each simulation clock tick, and updates the current state of the
 * 		  cells to dead or alive based on the rules of cellular evolution.
 * 		  -> This algorithm was based on the code written by Joan Soler-Adillon.
 * 
 */
void update_simulation(void) {
	// Save current cell layout to cell buffer.
	for (int x = 0; x < SIMULATION_WIDTH; x++) {
		for (int y = 0; y < SIMULATION_HEIGHT; y++) {
			cellsBuffer[x][y] = cells[x][y];
		}
	}

	// Check the current state of each cell and compare with the rules of cellular evolution.
	for (int x = 0; x < SIMULATION_WIDTH; x++) {
		for (int y = 0; y < SIMULATION_HEIGHT; y++) {
			// Create a variable for number of neighbours surrounding each cell.
			int numNeighbours = 0;

			for (int xx = x - 1; xx <= x + 1; xx++) {
				for (int yy = y - 1; yy <= y + 1; yy++) {

					// Ensure that cell is within the bounds of the simulation
					if (((xx >= 0) && (xx < SIMULATION_WIDTH)) &&((yy >= 0) && (yy < SIMULATION_HEIGHT))) {

						// Check that we are not referring to the current cell being checked.	
						if (!((xx == x) && (yy == y))) {

							// If the neighbouring cell is alive, add it to the count.
							if (cellsBuffer[xx][yy] == 1){
								numNeighbours++;
							}
						}
					}
				}
			}
			
			// Using information about surrounding cells, apply rules of cellular evolution.
			if (cellsBuffer[x][y] == 1) {
				// The cell is currently alive, we need to kill it if necessary.
				if (numNeighbours < 2 || numNeighbours > 3) {
					// Kill the cell due to lack of alive neighbours surrounding it.
					cells[x][y] = 0;
				}
			} else {
				// The cell is currently dead, we need to make it live if it has 3 living neighbours. 
				if (numNeighbours == 3) {
					cells[x][y] = 1;
				}
			}
		}
	}
}

/**
 * @brief Creates the CAGSimulator controlling task.
 * 
 */
void s4642286_tsk_CAGSimulator_init(void) {
	xTaskCreate( (void *) &s4642286TaskCAGSimulator, (const signed char *) "SIMULATOR", SIMULATORTASK_STACK_SIZE, NULL, SIMULATORTASK_PRIORITY, &xSimHandle);
}

/*
 * Hardware Initialisation.
 */
void hardware_init( void ) {

	portDISABLE_INTERRUPTS();	//Disable interrupts

	portENABLE_INTERRUPTS();	//Enable interrupts
}
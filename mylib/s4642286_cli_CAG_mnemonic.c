 /** 
 **************************************************************
 * @file mylib/s4642286_cli_CAG_mnemonic.c
 * @author Ethan Pinto - 46422860
 * @date 5/05/2022
 * @brief CAG Mnemonic mylib CLI Peripheral File
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_CAGMnemonic_cli_init() - Creates the CAGMnemonic CLI controlling task.
 *************************************************************** 
 */

#include "board.h"
#include "processor_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "FreeRTOS_CLI.h"
#include "s4642286_CAG_joystick.h"
#include "s4642286_CAG_simulator.h"
#include "s4642286_cli_CAG_mnemonic.h"

/** CAG Mnemonic CLI Function Definitions. */

// Still Command
CLI_Command_Definition_t xStill = {
	"still",							
	"still <type> <x> <y>: Draw a still life at cell (x, y).\r\n",	
	prvStillCommand,					
	3								
};

// Osc Command
CLI_Command_Definition_t xOsc = {
	"osc",							
	"osc <type> <x> <y>: Draw an oscillator at cell (x, y).\r\n",	
	prvOscCommand,					
	3								
};

// Glider Command
CLI_Command_Definition_t xGlider = {
	"glider",							
	"glider <x> <y>: Draw a glider at cell (x, y).\r\n",	
	prvGliderCommand,					
	2								
};

// Start Command
CLI_Command_Definition_t xStart = {
	"start",							
	"start: Start or restart CAG_simulator.\r\n",	
	prvStartCommand,					
	0								
};

// Stop Command
CLI_Command_Definition_t xStop = {
	"stop",							
	"stop: Stop CAG_simulator.\r\n",	
	prvStopCommand,					
	0								
};

// Clear Command
CLI_Command_Definition_t xClear = {
	"clear",							
	"clear: Clear CAG display.\r\n",	
	prvClearCommand,					
	0								
};

// Del Command
CLI_Command_Definition_t xDel = {
	"del",							
	"del <type>: Delete the specified driver.\r\n",	
	prvDelCommand,					
	1								
};

// Cre Command
CLI_Command_Definition_t xCre = {
	"cre",							
	"cre <type>: Create the specified driver.\r\n",	
	prvCreCommand,					
	1								
};

// System Command
CLI_Command_Definition_t xSystem = {
	"system",							
	"system: Show the current system time (since boot-up) in ms.\r\n",	
	prvSystemCommand,					
	0								
};


// Usage Command
CLI_Command_Definition_t xUsage = {
	"usage",							
	"usage: List the current number of tasks running, the respective task state and stack high water-mark usage.\r\n",	
	prvUsageCommand,					
	0								
};


/*
 *Private helper function for checking argument validity.
 */
int check_values(int x, int y) {
	if (x < 0 || x > SIMULATION_WIDTH || y < 0 || y > SIMULATION_HEIGHT) {
		return 0;
	}
	return 1;
}

/*
 * Still Command.
 */
static BaseType_t prvStillCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

	const char *type;
	const char *xpos;
	const char *ypos;

	/* Get parameters from command string */
	type = FreeRTOS_CLIGetParameter(pcCommandString, 1, NULL);
	xpos = FreeRTOS_CLIGetParameter(pcCommandString, 2, NULL);
	ypos = FreeRTOS_CLIGetParameter(pcCommandString, 3, NULL);

	/* Send command to CAG Simulator. */
	caMessage_t sendCommand;
	sendCommand.cell_x = atoi(xpos);
	sendCommand.cell_y = atoi(ypos);
	sendCommand.type = ((0x02 << 4) | (atoi(type)));

	if (check_values(sendCommand.cell_x, sendCommand.cell_y) && atoi(type) >= 0
		&& atoi(type) <= 2) {
		if (SimQueue != NULL) {
			xQueueSendToFront(SimQueue, ( void * ) &sendCommand, ( portTickType ) 10);
		}
	} else {
		xWriteBufferLen = sprintf((char *) pcWriteBuffer, "Invalid input arguments to still command.\r\n");
		return pdFALSE;
	}

	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "\r\n");

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}


/*
 * Osc Command.
 */
static BaseType_t prvOscCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

	const char *type;
	const char *xpos;
	const char *ypos;

	/* Get parameters from command string */
	type = FreeRTOS_CLIGetParameter(pcCommandString, 1, NULL);
	xpos = FreeRTOS_CLIGetParameter(pcCommandString, 2, NULL);
	ypos = FreeRTOS_CLIGetParameter(pcCommandString, 3, NULL);

	/* Send command to CAG Simulator. */
	caMessage_t sendCommand;
	sendCommand.cell_x = atoi(xpos);
	sendCommand.cell_y = atoi(ypos);
	sendCommand.type = ((0x03 << 4) | (atoi(type)));

	if (check_values(sendCommand.cell_x, sendCommand.cell_y) && atoi(type) >= 0
		&& atoi(type) <= 2) {
		if (SimQueue != NULL) {
			xQueueSendToFront(SimQueue, ( void * ) &sendCommand, ( portTickType ) 10);
		}
	} else {
		xWriteBufferLen = sprintf((char *) pcWriteBuffer, "Invalid input arguments to osc command.\r\n");
		return pdFALSE;
	}


	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "\r\n");

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/*
 * Glider Command.
 */
static BaseType_t prvGliderCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

	const char *xpos;
	const char *ypos;

	/* Get parameters from command string */
	xpos = FreeRTOS_CLIGetParameter(pcCommandString, 1, NULL);
	ypos = FreeRTOS_CLIGetParameter(pcCommandString, 2, NULL);

	/* Send command to CAG Simulator. */
	caMessage_t sendCommand;
	sendCommand.cell_x = atoi(xpos);
	sendCommand.cell_y = atoi(ypos);
	sendCommand.type = 0x40;

	if (check_values(sendCommand.cell_x, sendCommand.cell_y)) {
		if (SimQueue != NULL) {
			xQueueSendToFront(SimQueue, ( void * ) &sendCommand, ( portTickType ) 10);
		}
	} else {
		xWriteBufferLen = sprintf((char *) pcWriteBuffer, "Invalid input arguments to glider command.\r\n");
		return pdFALSE;
	}

	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "\r\n");

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/*
 * Start Command.
 */
static BaseType_t prvStartCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

	/* Set Start Simulation event bit. */
	xEventGroupSetBits(specialFunctions, START_SIM);

	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "\r\n");

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/*
 * Stop Command.
 */
static BaseType_t prvStopCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

	/* Set Stop Simulation event bit. */
	xEventGroupSetBits(specialFunctions, STOP_SIM);

	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "\r\n");

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/*
 * Clear Command.
 */
static BaseType_t prvClearCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

	/* Set Clear Grid event bit. */
	xEventGroupSetBits(specialFunctions, CLEAR_GRID);

	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "\r\n");

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/*
 * Del Command.
 */
static BaseType_t prvDelCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

	const char *input;
	int type;

	/* Get parameters from command string */
	input = FreeRTOS_CLIGetParameter(pcCommandString, 1, NULL);

	type = atoi(input);

	if (type != 1 && type != 0) {
		xWriteBufferLen = sprintf((char *) pcWriteBuffer, "Invalid input arguments to del command.\r\n");
		return pdFALSE;
	} else {	
		if (type == 1) {
			/* Delete the Joystick driver */
			vTaskDelete(xJoystickHandle);
		} else {
			/* Delete the Simulator driver */
			vTaskDelete(xSimHandle);
		}
	}

	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "\r\n");

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/*
 * Cre Command.
 */
static BaseType_t prvCreCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

	const char *input;
	int type;

	/* Get parameters from command string */
	input = FreeRTOS_CLIGetParameter(pcCommandString, 1, NULL);

	type = atoi(input);

	eTaskState state;

	if (type != 1 && type != 0) {
		xWriteBufferLen = sprintf((char *) pcWriteBuffer, "Invalid input arguments to cre command.\r\n");
		return pdFALSE;
	} else {
		if (type == 1) {
			// Check if joystick task has been deleted.
			state = eTaskGetState(xJoystickHandle);

			if (state == eDeleted) {
				/* Create the Joystick driver */
				s4642286_tsk_CAGJoystick_init();
			}
		} else {
			// Check if simulator task has been deleted.
			state = eTaskGetState(xSimHandle);
			
			if (state == eDeleted) {
				/* Create the Simulator driver */
				s4642286_tsk_CAGSimulator_init();
			}
		}
	}

	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "\r\n");

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/*
 * System Command.
 */
static BaseType_t prvSystemCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

	/* Write command echo output string to write buffer. */
	xWriteBufferLen = sprintf((char *) pcWriteBuffer, "%lu\r\n", xTaskGetTickCount());

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}

/*
 * Usage Command.
 */
static BaseType_t prvUsageCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

	vTaskList(pcWriteBuffer);

	/* Return pdFALSE, as there are no more strings to return */
	/* Only return pdTRUE, if more strings need to be printed */
	return pdFALSE;
}


/**
 * @brief This function will register all the CLI commands for CAG Mnemonic.
 * 
 */
void s4642286_cli_init(void) {
	FreeRTOS_CLIRegisterCommand(&xStill);
	FreeRTOS_CLIRegisterCommand(&xOsc);
	FreeRTOS_CLIRegisterCommand(&xGlider);
	FreeRTOS_CLIRegisterCommand(&xStart);
	FreeRTOS_CLIRegisterCommand(&xStop);
	FreeRTOS_CLIRegisterCommand(&xClear);
	FreeRTOS_CLIRegisterCommand(&xDel);
	FreeRTOS_CLIRegisterCommand(&xCre);
	FreeRTOS_CLIRegisterCommand(&xSystem);
	FreeRTOS_CLIRegisterCommand(&xUsage);
}
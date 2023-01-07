/**
  ******************************************************************************
  * @file    pf/main.c
  * @author  Ethan Pinto
  * @date    5/05/2022
  * @brief   The main source file for the Cellular Automation Graphics project
  * 		 project which uses FreeRTOS.
  *
  ******************************************************************************
  *
  */

#include "board.h"
#include "processor_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "s4642286_CAG_simulator.h"
#include "s4642286_CAG_display.h"
#include "s4642286_CAG_grid.h"
#include "s4642286_CAG_mnemonic.h"
#include "s4642286_CAG_joystick.h"
#include "s4642286_joystick.h"
#include "s4642286_lta1000g.h"

/*
 * Starts all the other tasks, then starts the scheduler.
 */
int main( void ) {

	HAL_Init();
	BRD_LEDInit();
	BRD_debuguart_init();

	// Create the tasks for the CAG program.
	s4642286_tsk_CAGSimulator_init();
	s4642286_tsk_CAGDisplay_init();
	s4642286_tsk_CAGGrid_init();
	s4642286_tsk_lta1000g_init();
	s4642286_tsk_CAGMnemonic_init();
	s4642286_tsk_CAGJoystick_init();
	s4642286_tsk_joystick_init();

	/* Start the scheduler.*/
	vTaskStartScheduler();

  	return 0;
}

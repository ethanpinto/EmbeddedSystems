/**
  ******************************************************************************
  * @file    repo/s1/main.c
  * @author  Ethan Pinto - 46422860
  * @date    25022022
  * @brief   A counter, displayed by the LTA1000G LED Bar, will show the number
  *          of times the Joystick Z pushbutton has been pressed. The display is
  *          refreshed every 10ms and the Joystick Z pushbutton is debounced.
  ******************************************************************************
  */

#include "board.h"
#include "processor_hal.h"
#include "s4642286_joystick.h"
#include "s4642286_lta1000g.h"

// Function Definition
void hardware_init(void);

/**
 * @brief The main program.
 * 
 */
void main(void) {

    uint32_t lastTick = 0;

    HAL_Init();			//Initialise board.
    hardware_init();	//Initialise hardware modules
    s4642286_reg_joystick_press_reset();

    // Main processing loop
    while(1) {
        // Refresh LED Display
        if ((HAL_GetTick() - lastTick) >= 10) {   
            s4642286_reg_lta1000g_write(s4642286_reg_joystick_press_get());
            lastTick =  HAL_GetTick();
        }
    }
    return;
}

/**
 * @brief Initialise Hardware
 * 
 */
void hardware_init(void) {
    // Initialise LED display and Joystick
    s4642286_reg_lta1000g_init();
    s4642286_reg_joystick_pb_init(); 
}
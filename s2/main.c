/**
  ******************************************************************************
  * @file    repo/s2/main.c
  * @author  Ethan Pinto - 46422860
  * @date    15032022
  * @brief   This program will allow the pan and tilt servomotors to be
  *          controlled using the joystick, and the current pan angle will be
  *          displayed on the LTA1000G LED Board.
  ******************************************************************************
  */

#include "board.h"
#include "processor_hal.h"
#include "s4642286_joystick.h"
#include "s4642286_lta1000g.h"
#include "s4642286_pantilt.h"
#include "math.h"

// Function Definitions
void hardware_init(void);

/**
 * @brief This function contains the main loop for the program.
 * 
 */
void main(void) {
  // Set up system hardware.
  HAL_Init();
  hardware_init();

  // Main Processing Loop
  while (1) {
    // Read the joystick's x and y analog inputs and convert them to angles.
    int panAngle = ADC_TO_ANGLE(S4642286_REG_JOYSTICK_X_READ() + S4642286_REG_JOYSTICK_X_ZERO_CAL_OFFSET);
    int tiltAngle = ADC_TO_ANGLE(S4642286_REG_JOYSTICK_Y_READ() + S4642286_REG_JOYSTICK_Y_ZERO_CAL_OFFSET);

    // Write angles to pan and tilt module.
    S4642286_REG_PANTILT_PAN_WRITE(panAngle + S4642286_REG_PANTILT_PAN_90_CAL_OFFSET);
    S4642286_REG_PANTILT_TILT_WRITE(tiltAngle + S4642286_REG_PANTILT_TILT_90_CAL_OFFSET);

    // Update LED Bar with current Pan Angle.
    int currentPanAngle = S4642286_REG_PANTILT_PAN_READ() + 90; // Shift by 90 degrees (90->180, 0->90, -90->0)
    int segment = currentPanAngle / 20;

    if (segment != 0) {
      s4642286_reg_lta1000g_write(0x01 << (segment - 1));
    } else {
      // If angle is less than 20 degrees, no segment is lit up.
      s4642286_reg_lta1000g_write(0);
    }

    HAL_Delay(50);
  }
}

/**
 * @brief Initialise the LED Bar, Joystick and Pantilt Module.
 * 
 */
void hardware_init(void) {
  s4642286_reg_lta1000g_init();
  s4642286_reg_joystick_init(); 
  s4642286_reg_pantilt_init();
}
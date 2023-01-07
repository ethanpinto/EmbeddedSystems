 /** 
 **************************************************************
 * @file mylib/s4642286_pantilt.h
 * @author Ethan Pinto - 46422860
 * @date 18032022
 * @brief Pan and Tilt Peripheral Driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_reg_pantilt_init() - Initialise servos, GPIO pins and PWM.
 *************************************************************** 
 */

#ifndef S4642286_PANTILT_H
#define S4642286_PANTILT_H

// Function definitions
extern void s4642286_reg_pantilt_init(void);
void s4642286_pantilt_angle_write(int type, int angle);
int s4642286_pantilt_angle_read(int type);

// Macro Function Definitions
#define S4642286_REG_PANTILT_PAN_WRITE(angle) s4642286_pantilt_angle_write(0, angle)
#define S4642286_REG_PANTILT_PAN_READ() s4642286_pantilt_angle_read(0)
#define S4642286_REG_PANTILT_TILT_WRITE(angle) s4642286_pantilt_angle_write(1, angle)
#define S4642286_REG_PANTILT_TILT_READ() s4642286_pantilt_angle_read(1)

// Define Configuration Parameters
#define S4642286_REG_PANTILT_PAN_90_CAL_OFFSET 20
#define S4642286_REG_PANTILT_TILT_90_CAL_OFFSET 10

#define ADC_TO_ANGLE(adcValue) (int)floor((45.0/1024.0) * (double)adcValue - 90.0)

#endif
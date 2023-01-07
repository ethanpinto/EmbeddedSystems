 /** 
 **************************************************************
 * @file mylib/s4642286_joystick.h
 * @author Ethan Pinto - 46422860
 * @date 22022022
 * @brief Joystick peripheral driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_reg_joystick_pb_init() - Intialise Lthe Joystick pushbutton source.
 * s4642286_reg_joystick_pb_isr() - Joystick Pushbutton Interrupt service routine.
 * s4642286_reg_joystick_press_get() - Returns the value of the Joystick pushbutton press counter.
 * s4642286_reg_joystick_press_reset() - Reset the Joystick event counter value to 0.
 * s4642286_reg_joystick_init() - Intialise GPIO pins and ADC.
 * s4642286_tsk_joystick_init() - Creates the Joystick controlling task.
 *************************************************************** 
 */

#ifndef S4642286_JOYSTICK_H
#define S4642286_JOYSTICK_H

#include "semphr.h"

// External function definitions
extern void s4642286_reg_joystick_pb_init(void);
extern void s4642286_reg_joystick_pb_isr(void);
extern int s4642286_reg_joystick_press_get(void);
extern void s4642286_reg_joystick_press_reset(void);
extern void s4642286_reg_joystick_init(void);
extern void s4642286_tsk_joystick_init(void);
int s4642286_joystick_readxy(ADC_HandleTypeDef adcHandler);

// Task Priority
#define JOYSTICKTASK_PRIORITY	( tskIDLE_PRIORITY + 4 )

// Task Stack Size
#define JOYSTICKTASK_STACK_SIZE ( configMINIMAL_STACK_SIZE * 3 )

// Joystick Pushbutton Semaphore to control timers.
SemaphoreHandle_t pbSem;

// Global Variable declarations for ADC Handlers and Channel Configurations.
ADC_HandleTypeDef adcHandlerX;
ADC_HandleTypeDef adcHandlerY;
ADC_ChannelConfTypeDef adcChanConfigX;
ADC_ChannelConfTypeDef adcChanConfigY;

// Macro Function Definitions
#define S4642286_REG_JOYSTICK_X_READ() (s4642286_joystick_readxy(adcHandlerX))
#define S4642286_REG_JOYSTICK_Y_READ() (s4642286_joystick_readxy(adcHandlerY))

// Define Configuration Parameters
#define S4642286_REG_JOYSTICK_X_ZERO_CAL_OFFSET 10
#define S4642286_REG_JOYSTICK_Y_ZERO_CAL_OFFSET 2

#endif
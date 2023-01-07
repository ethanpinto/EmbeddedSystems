 /** 
 **************************************************************
 * @file mylib/s4642286_CAG_joystick.h
 * @author Ethan Pinto - 46422860
 * @date 5/05/2022
 * @brief CAG joystick mylib Task Peripheral Driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_tsk_CAGJoystick_init() - Creates the CAGJoystick controlling task.
 *************************************************************** 
 */

#ifndef S4642286_CAG_JOYSTICK_H
#define S4642286_CAG_JOYSTICK_H

// External Function Definitions
extern void s4642286_tsk_CAGJoystick_init(void);

// Task Priority
#define CAGJOYSTICKTASK_PRIORITY	( tskIDLE_PRIORITY + 4 )

// Task Stack Size
#define CAGJOYSTICKTASK_STACK_SIZE ( configMINIMAL_STACK_SIZE * 5 )

// Joystick Minimum and Maximum Values
#define XY_MIN 20
#define XY_MAX 4060
#define XY_MID_LOW 2030
#define XY_MID_HIGH 2080

typedef struct {
    int x_joystick;
    int y_joystick;
} JoystickValues;

// The queue for sending joystick information to simulation.
QueueHandle_t JoystickQueue;

// Create a task handle for the Joystick.
TaskHandle_t xJoystickHandle;

#endif
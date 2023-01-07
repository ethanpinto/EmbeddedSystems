 /** 
 **************************************************************
 * @file mylib/s4642286_lta1000g.h
 * @author Ethan Pinto - 46422860
 * @date 22022022
 * @brief lta1000g LED Light Bar peripheral driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_reg_lta1000g_init() - Intialise LED Bar
 * s4642286_reg_lta1000g_write() - Write the LED Bar segments high or low
 * s4642286_tsk_lta1000g_init() - Creates the LED Bar controlling task.
 *************************************************************** 
 */

#ifndef S4642286_LTA1000G_H
#define S4642286_LTA1000G_H

// External function definitions
extern void s4642286_reg_lta1000g_init(void);
extern void s4642286_reg_lta1000g_write(unsigned short value);
extern void s4642286_tsk_lta1000g_init(void);

// Task Priority
#define LEDTASK_PRIORITY	( tskIDLE_PRIORITY + 4 )

// Task Stack Size
#define LEDTASK_STACK_SIZE ( configMINIMAL_STACK_SIZE * 2 )

typedef struct {
    unsigned short value;
} xyposition;

// Queue Handle for LED Bar to communicate with CAG Simulator.
QueueHandle_t positionQueue;

#endif
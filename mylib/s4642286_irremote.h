 /** 
 **************************************************************
 * @file mylib/s4642286_irremote.c
 * @author Ethan Pinto - 46422860
 * @date 01042022
 * @brief Infrared Remote HAL Peripheral Driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_irremote_init() - Initialises the IR Receiver hardware.
 * s4642286_irremote_recv() - Processes the recieved input waveform.
 * s4642286_irremote_readkey() - Reads the key that has been detected.
 *************************************************************** 
 */

#ifndef S4642286_IRREMOTE_H
#define S4642286_IRREMOTE_H

// Function definitions
extern void s4642286_irremote_init();
extern void s4642286_irremote_recv();
extern int s4642286_irremote_readkey();

// Global Variable Definitions for Key Presses
int keyPressedFlag;
char keyPressedValue;

// Define IR Remote Encoded Values
#define ZERO 0xFF6897        
#define ONE 0xFF30CF      
#define TWO 0xFF18E7       
#define THREE 0xFF7A85        
#define FOUR 0xFF10EF        
#define FIVE 0xFF38C7       
#define SIX 0xFF5AA5        
#define SEVEN 0xFF42BD         
#define EIGHT 0xFF4AB5         
#define NINE 0xFF52AD

#define TIMER_COUNTER_FREQ 50000

#endif
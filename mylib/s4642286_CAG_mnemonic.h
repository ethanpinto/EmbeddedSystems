 /** 
 **************************************************************
 * @file mylib/s4642286_CAG_mnemonic.h
 * @author Ethan Pinto - 46422860
 * @date 6/05/2022
 * @brief CAG mnemonic mylib Task Peripheral Driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_tsk_CAGMnemonic_init() - Creates the CAGMnemonic controlling task.
 *************************************************************** 
 */

#ifndef S4642286_CAG_MNEMONIC_H
#define S4642286_CAG_MNEMONIC_H

// External Function Definitions
extern void s4642286_tsk_CAGMnemonic_init(void);

// Task Priorities
#define MNEMONICTASK_PRIORITY		( tskIDLE_PRIORITY + 4 )

// Task Stack Allocations
#define MNEMONICTASK_STACK_SIZE		( configMINIMAL_STACK_SIZE * 3 )

#define DELETE_CHAR 127


#endif
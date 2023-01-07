 /** 
 **************************************************************
 * @file mylib/s4642286_CAG_mnemonic.c
 * @author Ethan Pinto - 46422860
 * @date 6/05/2022
 * @brief CAG mnemonic mylib Task Peripheral Driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_tsk_CAGMnemonic_init() - Creates the CAGMnemonic controlling task.
 *************************************************************** 
 */

#include "board.h"
#include "processor_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_CLI.h"
#include "debug_log.h"
#include <string.h>
#include "s4642286_CAG_grid.h"
#include "s4642286_CAG_mnemonic.h"
#include "s4642286_CAG_simulator.h"
#include "s4642286_cli_CAG_mnemonic.h"


/**
 * @brief The controlling task for CAG Mnemonic. This task uses CLI.
 * 
 */
void s4642286TaskCAGMnemonic(void) {

	int i;
	char cRxedChar;
	char cInputString[100];
	int InputIndex = 0;
	char *pcOutputString;
	BaseType_t xReturned;
	int mnemonicOn = 0;

	/* Register CLI commands */
	s4642286_cli_init();

	hardware_init();

	/* Initialise pointer to CLI output buffer. */
	memset(cInputString, 0, sizeof(cInputString));
	pcOutputString = FreeRTOS_CLIGetOutputBuffer();

	for (;;) {
		if (modeSelectSem != NULL) {
			/* See if we can obtain the mode select semaphore. If the semaphore
			is not available wait 10 ticks to see if it becomes free.*/
			if( xSemaphoreTake( modeSelectSem, 10 ) == pdTRUE ) {
            	// We were able to obtain the semaphore and can now access the shared resource. 
				mnemonicOn = ~mnemonicOn & 0x01;
        	}
		}

		if (mnemonicOn) {
			/* Receive character from terminal */
			cRxedChar = debug_getc();

			/* Process if character if not Null */
			if (cRxedChar != '\0') {

				/* Echo character */
				debug_putc(cRxedChar);

				/* Process only if return is received. */
				if (cRxedChar == '\r') {

					//Put new line and transmit buffer
					debug_putc('\n');
					debug_flush();

					/* Put null character in command input string. */
					cInputString[InputIndex] = '\0';

					xReturned = pdTRUE;
					/* Process command input string. */
					while (xReturned != pdFALSE) {

						/* Returns pdFALSE, when all strings have been returned */
						xReturned = FreeRTOS_CLIProcessCommand( cInputString, pcOutputString, configCOMMAND_INT_MAX_OUTPUT_SIZE );

						/* Display CLI command output string (not thread safe) */
						portENTER_CRITICAL();
						for (i = 0; i < (int) strlen(pcOutputString); i++) {
							debug_putc(*(pcOutputString + i));
						}
						portEXIT_CRITICAL();

						vTaskDelay(5);
					}

					memset(cInputString, 0, sizeof(cInputString));
					InputIndex = 0;

				} else {

					debug_flush();		//Transmit USB buffer

					if( cRxedChar == '\r' ) {

					/* Ignore the character. */
					} else if(cRxedChar == DELETE_CHAR) {
						/* Backspace was pressed.  Erase the last character in the
						string - if any.*/
						if( InputIndex > 0 ) {
							InputIndex--;
							cInputString[ InputIndex ] = '\0';
							debug_putc('\b');
							debug_putc(' ');
							debug_putc('\b');
							debug_flush();
						}

					} else {
						/* A character was entered.  Add it to the string
						entered so far.  When a \n is entered the complete
						string will be passed to the command interpreter. */
						if( InputIndex < 20 ) {
							cInputString[ InputIndex ] = cRxedChar;
							InputIndex++;
						}
					}
				}
			}
		}

		vTaskDelay(50);
		
	}
}

/**
 * @brief Create the CAG Mnemonic Controlling Task.
 * 
 */
void s4642286_tsk_CAGMnemonic_init(void) {
	/* Create CLI Receiving task */
	xTaskCreate( (void *) &s4642286TaskCAGMnemonic, (const signed char *) "MNEMONIC", MNEMONICTASK_STACK_SIZE, NULL, MNEMONICTASK_PRIORITY, NULL );
}

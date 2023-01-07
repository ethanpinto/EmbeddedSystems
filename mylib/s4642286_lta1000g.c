 /** 
 **************************************************************
 * @file mylib/s4642286_lta1000g.c
 * @author Ethan Pinto - 46422860
 * @date 22022022
 * @brief lta1000g LED Light Bar peripheral driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_reg_lta1000g_init() - Intialise LED Bar
 * s4642286_reg_lta1000g_write() - Write the LED Bar segments high or low. 
 * s4642286_tsk_lta1000g_init() - Creates the LED Bar controlling task.
 *************************************************************** 
 */
#include "board.h"
#include "processor_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "s4642286_lta1000g.h"

/**
 * @brief The controlling task for the LED Bar. It uses a queue to 
 *        recieve information from other tasks.
 * 
 */
void s4642286TaskLEDBar(void) {
    // Initialise the LED Bar.
    s4642286_reg_lta1000g_init();

    // Create a struct to hold recieved message.
    xyposition recievePosition;

	// Create a Queue
	positionQueue = xQueueCreate(10, sizeof(recievePosition));

	for (;;) {

        if (positionQueue != NULL) {
            if (xQueueReceive( positionQueue, &recievePosition, 10 )) {
                // Write position to led bar.
				s4642286_reg_lta1000g_write(recievePosition.value);
        	}
        }

		vTaskDelay(10);
	}
}

/**
 * @brief Creates the LED Bar controlling task.
 * 
 */
void s4642286_tsk_lta1000g_init(void) {
    xTaskCreate( (void *) &s4642286TaskLEDBar, (const signed char *) "LEDBAR", LEDTASK_STACK_SIZE, NULL, LEDTASK_PRIORITY, NULL );
}

/**
 * @brief initialises the LED Bar.
 * 
 */
void s4642286_reg_lta1000g_init(void) {
    // Enable the GPIO A, B and C Clocks
    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();

    //Initialise Pins as outputs
    GPIOA->MODER &= ~((0x03 << (4 * 2)) | (0x03 << (15 * 2)));  // Clear the bits for Pins 4 and 15 in Port A.
    GPIOA->MODER |= ((0x01 << (4 * 2)) | (0x01 << (15 * 2)));   // Set Pins to output

    GPIOB->MODER &= ~((0x03 << (3 * 2)) | (0x03 << (4 * 2)) | (0x03 << (5 * 2)) | (0x03 << (12 * 2))
                        | (0x03 << (13 * 2)) | (0x03 << (15 * 2)));  // Clear the bits for Pins 3, 4, 5, 12, 13, and 15 in Port B.

    GPIOB->MODER |= ((0x01 << (3 * 2)) | (0x01 << (4 * 2)) | (0x01 << (5 * 2)) | (0x01 << (12 * 2))
                        | (0x01 << (13 * 2)) | (0x01 << (15 * 2)));  // Set Pins to output

    GPIOC->MODER &= ~((0x03 << (6 * 2)) | (0x03 << (7 * 2)));  // Clear the bits for Pins 6 and 7 in Port C.
    GPIOC->MODER |= ((0x01 << (6 * 2)) | (0x01 << (7 * 2)));   // Set Pins to output

    //Clear Bits in OTYPE register for Push/Pull Output
    GPIOA->OTYPER &= ~((0x01 << 4) | (0x01 << 15));    
    GPIOB->OTYPER &= ~((0x01 << 3) | (0x01 << 4) | (0x01 << 5) | (0x01 << 12) | (0x01 << 13) | (0x01 << 15));
    GPIOC->OTYPER &= ~((0x01 << 6) | (0x01 << 7)); 
}

/**
 * @brief Set a particular variable to high or low.
 * 
 * @param segment - the segment which will be changed.
 * @param segment_value - the value of the segment (0 or 1).
 */
void lta1000g_set_seg(int segment, unsigned char segment_value) {
    switch (segment) {
        case 0:
            if (segment_value) {
                GPIOC->ODR |= (0x01 << 6);   // Set bit
            } else {
                GPIOC->ODR &= ~(0x01 << 6);   // Clear bit
            }
            break;

        case 1:
            if (segment_value) {
                GPIOB->ODR |= (0x01 << 15);   // Set bit
            } else {
                GPIOB->ODR &= ~(0x01 << 15);   // Clear bit
            }
            break;

        case 2:
            if (segment_value) {
                GPIOB->ODR |= (0x01 << 13);   // Set bit
            } else {
                GPIOB->ODR &= ~(0x01 << 13);   // Clear bit
            }
            break;

        case 3:
            if (segment_value) {
                GPIOB->ODR |= (0x01 << 12);   // Set bit
            } else {
                GPIOB->ODR &= ~(0x01 << 12);   // Clear bit
            }
            break;
    
        case 4:
            if (segment_value) {
                GPIOA->ODR |= (0x01 << 15);   // Set bit
            } else {
                GPIOA->ODR &= ~(0x01 << 15);   // Clear bit
            }
            break;
    
        case 5:
            if (segment_value) {
                GPIOC->ODR |= (0x01 << 7);   // Set bit
            } else {
                GPIOC->ODR &= ~(0x01 << 7);   // Clear bit
            }
            break;
        
        case 6:
            if (segment_value) {
                GPIOB->ODR |= (0x01 << 5);   // Set bit
            } else {
                GPIOB->ODR &= ~(0x01 << 5);   // Clear bit
            }
            break;

        case 7:
            if (segment_value) {
                GPIOB->ODR |= (0x01 << 3);   // Set bit
            } else {
                GPIOB->ODR &= ~(0x01 << 3);   // Clear bit
            }
            break;

        case 8:
            if (segment_value) {
                GPIOA->ODR |= (0x01 << 4);   // Set bit
            } else {
                GPIOA->ODR &= ~(0x01 << 4);   // Clear bit
            }
            break;

        case 9:
            if (segment_value) {
                GPIOB->ODR |= (0x01 << 4);   // Set bit
            } else {
                GPIOB->ODR &= ~(0x01 << 4);   // Clear bit
            }
            break;
    }

}

/**
 * @brief Update the whole LED Bar and change each segment to high or low.
 * 
 * @param value - a binary number which holds the state of each segment.
 */
void s4642286_reg_lta1000g_write(unsigned short value) {
    // Iterate through each digit in value to see if segment is high or low.
    for (int seg = 0; seg < 10; seg++) {
        if (((value & (0x0001 << seg)) == (0x0001 << seg))) {
            lta1000g_set_seg(seg, 1);
        } else {
            lta1000g_set_seg(seg, 0);
        }
    } 
}


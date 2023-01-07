 /** 
 **************************************************************
 * @file mylib/s4642286_oled.h
 * @author Ethan Pinto - 46422860
 * @date 18/04/2022
 * @brief OLED Mylib register driver
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_tsk_oled_init() - Creates the OLED controlling task.
 *************************************************************** 
 */

#ifndef S4642286_OLED_H
#define S4642286_OLED_H

// External Function Definitions
extern void s4642286_tsk_oled_init(void);
extern void s4642286_reg_oled_init(void);

// Task Priority
#define OLEDTASK_PRIORITY	( tskIDLE_PRIORITY + 4 )

// Task Stack Size
#define OLEDTASK_STACK_SIZE ( configMINIMAL_STACK_SIZE * 2 )

/**
 * @brief A struct for the queue message.
 * 
 */
struct oledTextMsg{
    int startX;
    int startY;
    char displayText[20];
};

#define WHITE SSD1306_WHITE
#define BLACK SSD1306_BLACK

QueueHandle_t OLEDQueue;	    // Queue used for Stage 4

#define I2C_DEV_SDA_PIN		9
#define I2C_DEV_SCL_PIN		8
#define I2C_DEV_GPIO		GPIOB
#define I2C_DEV_GPIO_AF 	GPIO_AF4_I2C1
#define I2C_DEV_GPIO_CLK()	__GPIOB_CLK_ENABLE()

#define I2C_DEV				I2C1
#define I2C_DEV_CLOCKSPEED 	100000

#endif
 /** 
 **************************************************************
 * @file mylib/s4642286_oled.c
 * @author Ethan Pinto - 46422860
 * @date 18/04/2022
 * @brief OLED Mylib register and task driver file.
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_tsk_oled_init() - Creates the OLED controlling task.
 *************************************************************** 
 */

#include "board.h"
#include "processor_hal.h"
#include "oled_pixel.h"
#include "oled_string.h"
#include "fonts.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "s4642286_oled.h"

/**
 * @brief The controlling task for the OLED. This task uses queues,
 *        semaphores, or group events to manipulate the SSD1306 OLED device.
 *        It calls OLED register functions and creates queues and semaphores.
 * 
 */
void s4642286TaskOLED(void) {
    // Initialise the SSD1306 OLED display.
    s4642286_reg_oled_init();

	//Clear Screen	
	ssd1306_Fill(Black);

    // Create a queue of oled messages.
	struct oledTextMsg oledMessageRecv;
	OLEDQueue = xQueueCreate(10, sizeof(oledMessageRecv));
	  
	// Cyclic Executive (CE) loop
	for (;;) {

		if (OLEDQueue != NULL) {	// Check if queue exists 
			
			// Check for item received - block atmost for 10 ticks
			if (xQueueReceive( OLEDQueue, &oledMessageRecv, 10 )) {

				// Write timer value to OLED
				ssd1306_SetCursor(oledMessageRecv.startX, oledMessageRecv.startY);
				ssd1306_WriteString(oledMessageRecv.displayText, Font_11x18, SSD1306_WHITE);
				ssd1306_SetCursor(60,10);
				ssd1306_WriteString(":", Font_11x18, SSD1306_WHITE);
				ssd1306_UpdateScreen();	
        	}
		}
		// Delay the task for 10ms.
		vTaskDelay(10);
	}
}

/**
 * @brief Creates the OLED controlling task.
 * 
 */
void s4642286_tsk_oled_init(void) {
    xTaskCreate( (void *) &s4642286TaskOLED, (const signed char *) "OLED", OLEDTASK_STACK_SIZE, NULL, OLEDTASK_PRIORITY, NULL );
}

/**
 * @brief Initialise the OLED SSD1306 device with the I2C interface.
 * 
 */
void s4642286_reg_oled_init(void) {
    uint32_t pclk1;
	uint32_t freqrange;

    // Enable GPIO clock
	I2C_DEV_GPIO_CLK();

	//Clear and Set Alternate Function for pin (lower ARF register) 
	MODIFY_REG(I2C_DEV_GPIO->AFR[1], ((0x0F) << ((I2C_DEV_SCL_PIN-8) * 4)) | ((0x0F) << ((I2C_DEV_SDA_PIN-8)* 4)), ((I2C_DEV_GPIO_AF << ((I2C_DEV_SCL_PIN-8) * 4)) | (I2C_DEV_GPIO_AF << ((I2C_DEV_SDA_PIN-8)) * 4)));
	
	//Clear and Set Alternate Function Push Pull Mode
	MODIFY_REG(I2C_DEV_GPIO->MODER, ((0x03 << (I2C_DEV_SCL_PIN * 2)) | (0x03 << (I2C_DEV_SDA_PIN * 2))), ((GPIO_MODE_AF_OD << (I2C_DEV_SCL_PIN * 2)) | (GPIO_MODE_AF_OD << (I2C_DEV_SDA_PIN * 2))));
	
	//Set low speed.
	SET_BIT(I2C_DEV_GPIO->OSPEEDR, (GPIO_SPEED_LOW << I2C_DEV_SCL_PIN) | (GPIO_SPEED_LOW << I2C_DEV_SDA_PIN));

	//Set Bit for Push/Pull output
	SET_BIT(I2C_DEV_GPIO->OTYPER, ((0x01 << I2C_DEV_SCL_PIN) | (0x01 << I2C_DEV_SDA_PIN)));

	//Clear and set bits for no push/pull
	MODIFY_REG(I2C_DEV_GPIO->PUPDR, (0x03 << (I2C_DEV_SCL_PIN * 2)) | (0x03 << (I2C_DEV_SDA_PIN * 2)), (GPIO_PULLUP << (I2C_DEV_SCL_PIN * 2)) | (GPIO_PULLUP << (I2C_DEV_SDA_PIN * 2)));

	// Configure the I2C peripheral
	// Enable I2C peripheral clock
	__I2C1_CLK_ENABLE();

	// Disable the selected I2C peripheral
	CLEAR_BIT(I2C_DEV->CR1, I2C_CR1_PE);

  	pclk1 = HAL_RCC_GetPCLK1Freq();			// Get PCLK1 frequency
  	freqrange = I2C_FREQRANGE(pclk1);		// Calculate frequency range 

  	//I2Cx CR2 Configuration - Configure I2Cx: Frequency range
  	MODIFY_REG(I2C_DEV->CR2, I2C_CR2_FREQ, freqrange);

	// I2Cx TRISE Configuration - Configure I2Cx: Rise Time
  	MODIFY_REG(I2C_DEV->TRISE, I2C_TRISE_TRISE, I2C_RISE_TIME(freqrange, I2C_DEV_CLOCKSPEED));

   	// I2Cx CCR Configuration - Configure I2Cx: Speed
  	MODIFY_REG(I2C_DEV->CCR, (I2C_CCR_FS | I2C_CCR_DUTY | I2C_CCR_CCR), I2C_SPEED(pclk1, I2C_DEV_CLOCKSPEED, I2C_DUTYCYCLE_2));

   	// I2Cx CR1 Configuration - Configure I2Cx: Generalcall and NoStretch mode
  	MODIFY_REG(I2C_DEV->CR1, (I2C_CR1_ENGC | I2C_CR1_NOSTRETCH), (I2C_GENERALCALL_DISABLE| I2C_NOSTRETCH_DISABLE));

   	// I2Cx OAR1 Configuration - Configure I2Cx: Own Address1 and addressing mode
  	MODIFY_REG(I2C_DEV->OAR1, (I2C_OAR1_ADDMODE | I2C_OAR1_ADD8_9 | I2C_OAR1_ADD1_7 | I2C_OAR1_ADD0), I2C_ADDRESSINGMODE_7BIT);

   	// I2Cx OAR2 Configuration - Configure I2Cx: Dual mode and Own Address2
  	MODIFY_REG(I2C_DEV->OAR2, (I2C_OAR2_ENDUAL | I2C_OAR2_ADD2), I2C_DUALADDRESS_DISABLE);

  	// Enable the selected I2C peripheral
	SET_BIT(I2C_DEV->CR1, I2C_CR1_PE);

	//Initialise SSD1306 OLED.
	ssd1306_Init();
}

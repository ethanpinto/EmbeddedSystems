 /** 
 **************************************************************
 * @file mylib/s4642286_cli_CAG_mnemonic.h
 * @author Ethan Pinto - 46422860
 * @date 5/05/2022
 * @brief CAG Mnemonic mylib CLI Peripheral File
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_CAGMnemonic_cli_init() - Creates the CAGMnemonic CLI controlling task.
 *************************************************************** 
 */

#ifndef S4642286_CLI_MNEMONIC_H
#define S4642286_CLI_MNEMONIC_H

#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_CLI.h"
#include "debug_log.h"
#include <string.h>

#define OLED_WIDTH 128  // In Pixels
#define OLED_HEIGHT 32  // In Pixels
#define SIMULATION_WIDTH (OLED_WIDTH / CELL_SIZE)
#define SIMULATION_HEIGHT (OLED_HEIGHT / CELL_SIZE)
#define CELL_SIZE 2

// Function Definitions
extern void s4642286_cli_init(void);
static BaseType_t prvStillCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvOscCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvGliderCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvStartCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvStopCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvClearCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvDelCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvCreCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvSystemCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvUsageCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

#endif
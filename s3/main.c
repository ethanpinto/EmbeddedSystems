/**
  ******************************************************************************
  * @file    repo/s3/main.c
  * @author  Ethan Pinto - 46422860
  * @date    01042022
  * @brief   Program involving a Hamming encoder and decoder that uses console
  *          input (from the keyboard or an IR remote control key press) and
  *          displays the output on the lta1000g LED Bar.
  ******************************************************************************
  */

#include "board.h"
#include "processor_hal.h"
#include "s4642286_joystick.h"
#include "s4642286_hamming.h"
#include "s4642286_lta1000g.h"
#include "s4642286_irremote.h"


// Function Definition
void hardware_init(void);

// Define FSM States
#define S0 0    // IDLE 
#define S1 1    // ENCODE
#define S2 2    // DECODE
#define S3 3    // REMOTE RECEIVE


/**
 * @brief Converts a character into an integer if it is a valid Hex numbers.
 * 
 * @param input the character to convert
 * @return int the corresponding hex representation if valid, else -1
 */
int convert(char input) {
    int converted = input - 48;
    if (converted >= 0 && converted <= 9) {
        return converted;
    } else {
        switch (input) {
            case 'A':
                return 10;
                break;
            case 'B':
                return 11;
                break;
            case 'C':
                return 12;
                break;
            case 'D':
                return 13;
                break;
            case 'E':
                return 14;
                break;
            case 'F':
                return 15;
                break;
            default:
                return -1;
                break;      
        }
    }
}

/**
 * @brief The main program.
 * 
 */
void main(void) {
    // Initialise variables for cyclic executive
    uint32_t lastTick = 0;

    // Initialise variables for FSM inputs and states
    char recvChar1, recvChar2;
    int currentState = S0; // Begin in IDLE state
    int inputCount = 0, numInputIR = 0, number1, number2;
    int lastJoystickPressCount = 0;
    unsigned short decodedValue, displayValue;

    HAL_Init();			//Initialise board.
    hardware_init();	//Initialise hardware modules

    // Main cyclic executive
    while(1) {
        // Process inputs every 100ms.
        if ((HAL_GetTick() - lastTick) >= 150) { 
            switch (currentState) {
                case S0: ;  // IDLE STATE
                    // Check if a key has been pressed on IR Remote
                    if (keyPressedFlag) {
                        // Move to remote receive state
                        s4642286_reg_lta1000g_write(0);
                        currentState = S3;
                        break;
                    }
                    // Read for two key presses (apart from null character)
                    if (inputCount == 0 && (recvChar1 = BRD_debuguart_getc()) != '\0') {
                        inputCount += 1;
                    }
                    if (inputCount == 1 && (recvChar2 = BRD_debuguart_getc()) != '\0') {
                        inputCount += 1;
                    }
                    
                    // Check if input is valid and move to new state if valid.
                    if (inputCount == 2) {
                        if (recvChar1 == 'E' && recvChar2 == 'E') {
                            // Move to Encoding State
                            currentState = S1;

                        } else if (recvChar1 == 'D' && recvChar2 == 'D') {
                            // Move to Decoding State
                            currentState = S2;
                            
                        } else {
                            // Remain in IDLE state
                            currentState = S0;
                        }
                        inputCount = 0;
                    }
                    break;

                case S1: ;  //Encoding State         
                    if (keyPressedFlag) {
                        // Move to remote receive state
                        s4642286_reg_lta1000g_write(0);
                        currentState = S3;
                        break;
                    }

                    unsigned short encodedByte;
                    uint8_t firstByte, secondByte;
                    int displayedByte;
                    // Read for two key presses (apart from null character)
                    if (inputCount == 0 && (recvChar1 = BRD_debuguart_getc()) != '\0') {
                        inputCount += 1;
                    }
                    if (inputCount == 1 && (recvChar2 = BRD_debuguart_getc()) != '\0') {
                        inputCount += 1;
                    }
                    
                    if (inputCount == 2) {
                        if (recvChar1 == 'F' && recvChar2 == 'F') {
                            // Move to IDLE State
                            s4642286_reg_lta1000g_write(0);
                            currentState = S0;

                        } else if (recvChar1 == 'D' && recvChar2 == 'D') {
                            // Move to Decoding State
                            s4642286_reg_lta1000g_write(0);
                            currentState = S2;

                        } else if (recvChar1 == 'E' && recvChar2 == 'E') {
                            // Remain in Encoding State
                            currentState = S1;

                        } else if ((number1 = convert(recvChar1)) != -1 &&
                                (number2 = convert(recvChar2)) != -1) {

                            // Combine characters into one byte and encode.
                            encodedByte = s4642286_lib_hamming_byte_encode(((number1 << 4) | number2));
                            
                            // Split bytes
                            firstByte = (encodedByte >> 8);
                            secondByte = (encodedByte & 0xFF);

                            s4642286_reg_lta1000g_write(firstByte);
                            displayedByte = 1;
                        } 
                        inputCount = 0;
                    }

                    // Display Encoded Bytes on LED Bar.
                    if (s4642286_reg_joystick_press_get() > lastJoystickPressCount) {
                        if (displayedByte == 1) {
                            s4642286_reg_lta1000g_write(secondByte);
                            displayedByte = 2;

                        } else {
                            s4642286_reg_lta1000g_write(firstByte);
                            displayedByte = 1;

                        }
                        // Update last joystick press count.
                        lastJoystickPressCount = s4642286_reg_joystick_press_get();
                    }
                    break;

                case S2: ;  // Decoding State
                    if (keyPressedFlag) {
                        // Move to remote receive state
                        s4642286_reg_lta1000g_write(0);
                        currentState = S3;
                        break;
                    }

                    // Read for two key presses (apart from null character)
                    if (inputCount == 0 && (recvChar1 = BRD_debuguart_getc()) != '\0') {
                        inputCount += 1;
                    }
                    if (inputCount == 1 && (recvChar2 = BRD_debuguart_getc()) != '\0') {
                        inputCount += 1;
                    }
                    
                    if (inputCount == 2) {
                        if (recvChar1 == 'F' && recvChar2 == 'F') {
                            // Move to IDLE State
                            s4642286_reg_lta1000g_write(0);
                            currentState = S0;

                        } else if (recvChar1 == 'E' && recvChar2 == 'E') {
                            // Move to Encoding State
                            s4642286_reg_lta1000g_write(0);
                            currentState = S1;

                        } else if (recvChar1 == 'D' && recvChar2 == 'D') {
                            // Remain in Decoding State
                            currentState = S2;

                        } else if ((number1 = convert(recvChar1)) != -1 &&
                                (number2 = convert(recvChar2)) != -1) {

                            uint8_t value = ((number1 << 4) | number2);

                            // Decode value
                            decodedValue = s4642286_lib_hamming_byte_decode(value);
                            displayValue = (decodedValue >> 4);

                            if (s4642286_lib_hamming_parity_error(value)) {
                                // A parity check error occurred
                                displayValue |= (0x02 << 8);

                            } else if (value != decodedValue) {
                                // A bit error must have occurred, since the bytes before and
                                // after error correction are different.
                                displayValue |= (0x01 << 8);
                            }
                            s4642286_reg_lta1000g_write(displayValue);
                        }
                        inputCount = 0;
                    }
                    break;

                case S3: ; // REMOTE RECEIVE STATE
                    char recvCharIR1, recvCharIR2;
                    uint8_t dataBits, hammingBits, errorBits = 0x00;

                    if (numInputIR == 0 && s4642286_irremote_readkey(&keyPressedValue)) {
                        recvCharIR1 = keyPressedValue;
                        numInputIR++;
                    }
                    if (numInputIR == 1 && s4642286_irremote_readkey(&keyPressedValue)) {
                        recvCharIR2 = keyPressedValue;
                        numInputIR++;
                    }

                    if (numInputIR == 2) {
                        uint8_t value = ((recvCharIR1 - 48) << 4) | ((recvCharIR2 - 48));
                        decodedValue = s4642286_lib_hamming_byte_decode(value);

                        // Extract Data and Hamming bits
                        dataBits =  (decodedValue >> 4);
                        hammingBits = (decodedValue & 0x0E);

                        if (s4642286_lib_hamming_parity_error(value)) {
                            // A parity check error occurred
                            errorBits = 0x02;

                        } else if (value != decodedValue) {
                            // A bit error must have occurred, since the bytes before and
                            // after error correction are different.
                            errorBits = 0x01;
                        }

                        debug_log("%X, %X, %X\r\n", dataBits, hammingBits, errorBits);
                        numInputIR = 0;
                        inputCount = 0;
                    }

                    /** STATE TRANSITION LOGIC **/

                    // Read for two key presses (apart from null character)
                    if (inputCount == 0 && (recvChar1 = BRD_debuguart_getc()) != '\0') {
                        inputCount += 1;
                    }
                    if (inputCount == 1 && (recvChar2 = BRD_debuguart_getc()) != '\0') {
                        inputCount += 1;
                    }
                    
                    if (inputCount == 2) {
                        if (recvChar1 == 'F' && recvChar2 == 'F') {
                            // Move to IDLE State
                            s4642286_reg_lta1000g_write(0);
                            currentState = S0;

                        } else if (recvChar1 == 'E' && recvChar2 == 'E') {
                            // Move to Encoding State
                            s4642286_reg_lta1000g_write(0);
                            currentState = S1;

                        } else if (recvChar1 == 'D' && recvChar2 == 'D') {
                            // Move to Decoding State
                            s4642286_reg_lta1000g_write(0);
                            currentState = S2;
                        }
                        inputCount = 0;
                        numInputIR = 0;
                    }
                    break;
        
                default:
                    break;
            }
            lastTick =  HAL_GetTick();
        }
    }
    return;
}

/**
 * @brief Initialise Hardware
 * 
 */
void hardware_init(void) {
    // Initialise LED display, Joystick, and IR Receiver
    s4642286_reg_lta1000g_init();
    s4642286_reg_joystick_pb_init();
    s4642286_irremote_init();
    BRD_debuguart_init(); 
}
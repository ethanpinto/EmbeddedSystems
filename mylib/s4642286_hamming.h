 /** 
 **************************************************************
 * @file mylib/s4642286_hamming.h
 * @author Ethan Pinto - 46422860
 * @date 01042022
 * @brief Hamming MyLib Header File
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_lib_hamming_byte_encode() - Return the 16 bit encoded value of a byte.
 * s4642286_lib_hamming_byte_decode() - Return the decoded half byte from a byte.
 *************************************************************** 
 */

#ifndef S4642286_HAMMING_H
#define S4642286_HAMMING_H

// Function definitions
extern unsigned short s4642286_lib_hamming_byte_encode(unsigned char value);
extern unsigned char s4642286_lib_hamming_byte_decode(unsigned char value);
unsigned char hamming_hbyte_encode(unsigned char value);
extern int s4642286_lib_hamming_parity_error(unsigned char value);

#endif
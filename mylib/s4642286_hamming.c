 /** 
 **************************************************************
 * @file mylib/s4642286_hamming.c
 * @author Ethan Pinto - 46422860
 * @date 01042022
 * @brief Hamming MyLib Source File
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4642286_lib_hamming_byte_encode() - Returns the 16 bit encoded value of a byte.
 * s4642286_lib_hamming_byte_decode() - Returns the decoded half byte from a byte.
 * s4642286_lib_hamming_parity_error() - Returns 1 if a parity error has occurred,
 * 										 or 0 for no error.
 *************************************************************** 
 */

#include "board.h"
#include "processor_hal.h"
#include "s4642286_hamming.h"

/**
  * @brief An internal function used to encode a half byte.
  * 
  * Hamming code is based on the following generator and parity check matrices
  * G = [ 0 1 1 | 1 0 0 0 ;
  *       1 0 1 | 0 1 0 0 ;
  *       1 1 0 | 0 0 1 0 ;
  *       1 1 1 | 0 0 0 1 ;
  *
  * H =
  * [ 1 0 0 | 0 1 1 1 ;
  *   0 1 0 | 1 0 1 1 ;
  *   0 0 1 | 1 1 0 1 ];
  * 
  * @param value the byte to encode (first 4 bits are the data bits).
  * @return unsigned char an encoded byte.
  */
unsigned char hamming_hbyte_encode(unsigned char value) {
	uint8_t d0, d1, d2, d3;
	uint8_t p0 = 0, h0, h1, h2;
	uint8_t z;
	unsigned char out;

	/* extract bits */
	d0 = !!(value & 0x1);
	d1 = !!(value & 0x2);
	d2 = !!(value & 0x4);
	d3 = !!(value & 0x8);

	/* calculate hamming parity bits */
	h0 = d1 ^ d2 ^ d3;
	h1 = d0 ^ d2 ^ d3;
	h2 = d0 ^ d1 ^ d3;

	/* generate out byte without parity bit P0 */
	out = (h0 << 1) | (h1 << 2) | (h2 << 3) |
		(d0 << 4) | (d1 << 5) | (d2 << 6) | (d3 << 7);

	/* calculate even parity bit */
	for (z = 1; z < 8; z++) {
		p0 = p0 ^ !!(out & (1 << z));
	}
	out |= p0;

	return out;
}

/**
 * @brief This function encodes a value using a hamming (7,4) coding.
 * 
 * @param value the byte that is to be encoded
 * @return unsigned short returns the hamming 16 bit encoded value of a byte of data.
 */
unsigned short s4642286_lib_hamming_byte_encode(unsigned char value) {
	unsigned short out;

	/* first encode D0..D3 (first 4 bits),
	 * then D4..D7 (second 4 bits).
	 * Place result into 16 bit encoded value
	 */
	out = hamming_hbyte_encode(value & 0xF) | (hamming_hbyte_encode(value >> 4) << 8);
	return out;
}


/**
 * @brief This function takes in an encoded byte value, and detects and corrects errors.
 * 
 * @param value the encoded value
 * @return unsigned char Return the decoded half byte from a byte.
 */
unsigned char s4642286_lib_hamming_byte_decode(unsigned char value) {
	uint8_t d0, d1, d2, d3, h0, h1, h2, s2, s1, s0;
	unsigned char decodedByte = 0x00;

	// Extract Hamming bits and Data bits.
	h0 = !!(value & (1 << 1));
	h1 = !!(value & (1 << 2));
	h2 = !!(value & (1 << 3));
	d0 = !!(value & (1 << 4));
	d1 = !!(value & (1 << 5));
	d2 = !!(value & (1 << 6));
	d3 = !!(value & (1 << 7));

	// Calculate the Syndrome.
	s0 = h0 ^ d1 ^ d2 ^ d3;
	s1 = h1 ^ d0 ^ d2 ^ d3;
	s2 = h2 ^ d0 ^ d1 ^ d3;

	// Correct errors if any.
	if (!s0 && s1 && s2) {
		// d3 bit error
		d0 = !d0;
	} else if (s0 && !s1 && s2) {
		// d2 bit error
		d1 = !d1;
	} else if (s0 && s1 && !s2) {
		// d1 bit error
		d2 = !d2;
	} else if (s0 && s1 && s2) {
		// d0 bit error
		d3 = !d3;
	} else if (s0 && !s1 && !s2) {
		// h0 bit error
		h0 = !h0;
	} else if (!s0 && s1 && !s2) {
		// h1 bit error
		h1 = !h1;
	} else if (!s0 && !s1 && s2) {
		// h2 bit error
		h2 = !h2;
	}

	decodedByte  = (h0 << 1) | (h1 << 2) | (h2 << 3) |
		(d0 << 4) | (d1 << 5) | (d2 << 6) | (d3 << 7);

	return decodedByte;
}


/**
 * @brief This function checks if the parity bit matches the parity 
 * 		  of the encoded message. If not, then a parity error has 
 * 		  occurred.
 * 
 * @param value the encoded message 
 * @return int 1 if a parity error has occurred, else 0.
 */
int s4642286_lib_hamming_parity_error(unsigned char value) {
	uint8_t z;
	uint8_t messageParity = 0;
	uint8_t p0 = (value & (1 << 0)); // extracts the parity bit.

	// Calculate the parity of the message.
	for (z = 1; z < 8; z++) {
		messageParity = messageParity ^ !!(value & (1 << z));
	}

	// Check if the parity bit matches the actual parity of the message.
	if (p0 != messageParity) {
		// A parity error has occurred.
		return 1;
	} else {
		return 0;
	}
}
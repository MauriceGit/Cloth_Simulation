/*********************************************************************
* lescegra - image loader                                            *
*                                                                    *
* http://geeky.kicks-ass.org/projects/lescegra.html                  *
*                                                                    *
* Copyright 2003-2005 by Enno Cramer <uebergeek@web.de>              *
*                                                                    *
* This library is free software; you can redistribute it and/or      *
* modify it under the terms of the GNU Library General Public        *
* License as published by the Free Software Foundation; either       *
* version 2 of the License, or (at your option) any later version.   *
*                                                                    *
* This library is distributed in the hope that it will be useful,    *
* but WITHOUT ANY WARRANTY; without even the implied warranty of     *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  *
* Library General Public License for more details.                   *
*                                                                    *
* You should have received a copy of the GNU Library General Public  *
* License along with this library; if not, write to the Free         *
* Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. *
*********************************************************************/

#ifndef CG_ENDIAN_H
#define CG_ENDIAN_H 1

#include <stdio.h>

/**
 * Class: CGEndian
 * Endianess conversion function
 */

/**
 * Enumeration: CGEndian_Endianess
 * Type representing the a hosts or values endianess.
 *
 *   CG_ENDIAN_LITTLE - little endian layout (x86)
 *   CG_ENDIAN_BIG    - big endian layout (Mips)
 */
typedef enum {
    CG_ENDIAN_LITTLE,
    CG_ENDIAN_BIG
} CGEndian_Endianess;

/**
 * Function: CGEndian_endianess
 * Determine the hosts endianess.
 *
 * Returns:
 *   Either CG_ENDIAN_LITTLE or CG_ENDIAN_BIG
 */
CGEndian_Endianess CGEndian_endianess(void);

/**
 * Function: CGEndian_swap
 * Swap endianess of a value.
 *
 * Parameters:
 *   buffer - pointer to value
 *   size   - size of value in bytes
 */
void CGEndian_swap(void* buffer, int size);

/**
 * Function: CGEndian_swapArray
 * Swap endianess of an array of values.
 *
 * Parameters:
 *   buffer - pointer to value array
 *   size   - size of a single value
 *   count  - number of values in buffer
 */
void CGEndian_swapArray(void* buffer, int size, int count);

/**
 * Function: CGEndian_swapShort
 * Swap endianess of a short int.
 *
 * Parameters:
 *   buffer - pointer to value
 */
void CGEndian_swapShort(short int* buffer);

/**
 * Function: CGEndian_swapInt
 * Swap endianess of an int.
 *
 * Parameters:
 *   buffer - pointer to value
 */
void CGEndian_swapInt(int* buffer);

/**
 * Function: CGEndian_swapLong
 * Swap endianess of a long int.
 *
 * Parameters:
 *   buffer - pointer to value
 */
void CGEndian_swapLong(long int* buffer);

/**
 * Function: CGEndian_readf
 * Read binary data and perform automatic endianess conversion as
 * necessary.  Data conversion is specified by a format string.
 *
 * The format string consists of of a series of tokens.  Each token is
 * a repetition count followed by a type specifier.
 *
 * Four different type specifier are defined, uppercase letters stand
 * for big endian format, lowercase letters for little endian format.
 *
 * b, B - Bytes
 * w, W - Words (2 Bytes)
 * d, D - DWords (4 Bytes)
 * q, Q - QWords (8 Bytes)
 *
 * Example:
 * (begin code)
 *   // read 2 words, 2 dwords and 20 bytes, converting from little endian
 *   // to host endianess if necessary
 *   short int id[2], int dim[2], char data[20];
 *   CGEndian_readf(stdin, "2w 2d 20b", id, dim, data);
 * (end)
 *
 * Parameters:
 *   stream - input stream
 *   format - data format string
 */
int CGEndian_readf(FILE* stream, const char* format, ...);

#endif

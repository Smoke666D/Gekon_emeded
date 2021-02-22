/*
 * utils.h
 *
 *  Created on: 18 февр. 2021 г.
 *      Author: igor.dymov
 */

#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include "main.h"
#include "menu.h"

void vStrCopy(char * dest, char * source);
void vStrAdd(char * dest, char * source);
void vUToStr( char * str, uint16_t data, signed char scale );
void vUCTOSTRING ( uint8_t * str, uint8_t data );
void vITOSTRING ( uint8_t * str, uint16_t data );
float fxParToFloat(uint16_t data, int8_t scale);
void vBitmapToString(uint8_t * dest, uint16_t bitmap);

#endif /* INC_UTILS_H_ */

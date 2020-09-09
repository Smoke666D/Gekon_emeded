/*
 * dataProces.h
 *
 *  Created on: 15 июн. 2020 г.
 *      Author: photo_Mickey
 */

#ifndef INC_DATAPROCES_H_
#define INC_DATAPROCES_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "fix16.h"
#include "config.h"
/*----------------------- Functions ------------------------------------*/
uint8_t getBitMap( const eConfigReg* reg, uint8_t bit );
fix16_t getValue( const eConfigReg* reg );
fix16_t fix16_pow ( fix16_t x, fix16_t y );
/*----------------------------------------------------------------------*/
#endif /* INC_DATAPROCES_H_ */

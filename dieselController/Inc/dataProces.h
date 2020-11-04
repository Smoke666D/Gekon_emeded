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
/*------------------------ Define --------------------------------------*/
#define SERIES_SIZE    5U
#define SERIES_LN_SIZE 15U
/*----------------------- Functions ------------------------------------*/
uint8_t  getBitMap( const eConfigReg* reg, uint8_t bit );
uint16_t getUintValue ( const eConfigReg* reg );
fix16_t  getValue( const eConfigReg* reg );
fix16_t  fix16_pow ( fix16_t x, fix16_t y );
/*----------------------------------------------------------------------*/
#endif /* INC_DATAPROCES_H_ */

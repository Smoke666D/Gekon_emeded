/*
 * mac.h
 *
 *  Created on: 4 февр. 2022 г.
 *      Author: 79110
 */
#ifndef INC_MAC_H_
#define INC_MAC_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "EEPROM.h"
/*------------------------ Define --------------------------------------*/
#define  MAC_EUI48_LENGTH  6U
#define  MAC_EUI64_LENGTH  8U
#define  MAC_EUI64_ADD3    0xFF
#define  MAC_EUI64_ADD4    0xFE
#define  MAC_ADR           0xFA
/*------------------------ Macros --------------------------------------*/
/*------------------------- Enum ---------------------------------------*/
/*----------------------- Structures -----------------------------------*/
/*----------------------- Functions ------------------------------------*/
void     vMACinit ( const EEPROM_TYPE* eeprom );
uint8_t* vMACget48 ( void );
uint8_t* vMACget64 ( void );
/*----------------------------------------------------------------------*/
#endif /* INC_MAC_H_ */

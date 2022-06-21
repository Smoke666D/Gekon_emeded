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
#define  MAC_EUI64_ADD3    0xFFU
#define  MAC_EUI64_ADD4    0xFEU
#define  MAC_ADR           0xFAU
#define  MAC_DEFAULT_0     0x00U
#define  MAC_DEFAULT_1     0x10U
#define  MAC_DEFAULT_2     0xFAU
#define  MAC_DEFAULT_3     0x6EU
#define  MAC_DEFAULT_4     0x38U
#define  MAC_DEFAULT_5     0x4AU
/*------------------------ Macros --------------------------------------*/
/*------------------------- Enum ---------------------------------------*/
/*----------------------- Structures -----------------------------------*/
/*----------------------- Functions ------------------------------------*/
EEPROM_STATUS eMACinit ( const EEPROM_TYPE* eeprom );
void          vMACsetDefault ( void );
uint8_t*      uMACget48 ( void );
uint8_t*      uMACget64 ( void );
/*----------------------------------------------------------------------*/
#endif /* INC_MAC_H_ */

/*
 * mbcrc.h
 *
 *  Created on: 13.11.2019
 *      Author: Mikhail.Mikhailov
 */

#ifndef INC_MBCRC_H_
#define INC_MBCRC_H_
/*----------------------- Includes ------------------------------------------------------------------*/
#include "mbport.h"
/*----------------------- Define --------------------------------------------------------------------*/
#define  CRC_SIZE  256U
/*----------------------- Functions -----------------------------------------------------------------*/
uint16_t usMBCRC16 ( volatile uint8_t* pucFrame, uint16_t usLen );
/*---------------------------------------------------------------------------------------------------*/
#endif /* INC_MBCRC_H_ */

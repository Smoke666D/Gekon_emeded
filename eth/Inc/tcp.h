/*
 * tcpip.h
 *
 *  Created on: 10 февр. 2020 г.
 *      Author: mikhail.mikhailov
 */

#ifndef INC_TCP_H_
#define INC_TCP_H_
/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_hal.h"
/* Functions -----------------------------------------------------------------*/
uint8_t uETHlistenRoutine( void );
void 		cETHgetStrIP( char* ipStr );
void 		vETHinitLwip( void );
/*----------------------------------------------------------------------------*/
#endif /* INC_TCP_H_ */

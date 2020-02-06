/*
 * sys.h
 *
 *  Created on: Feb 6, 2020
 *      Author: mikhail.mikhailov
 */

#ifndef INC_SYS_H_
#define INC_SYS_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
/*----------------------- Functions ------------------------------------*/
void vSYSInitSerial( void );
void vSYSSerial( char* msg );
/*----------------------------------------------------------------------*/
#endif /* INC_SYS_H_ */

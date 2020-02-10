/*
 * eth_common.h
 *
 *  Created on: 13.11.2019
 *      Author: Mikhail.Mikhailov
 */
#ifndef INC_ETH_COMMON_H_
#define INC_ETH_COMMON_H_
/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_hal.h"

#define	HTTP_INPUT_BUFFER_SIZE		512
#define	HTTP_OUTPUT_BUFFER_SIZE		256
/* Functions -----------------------------------------------------------------*/
uint8_t vETHlistenRoutine( void );
void 		cETHgetStrIP( char* ipStr );
void 		vETHinitLwip( void );
/*----------------------------------------------------------------------------*/
#endif /* INC_ETH_COMMON_H_ */

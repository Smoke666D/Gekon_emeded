/*
 * rest.h
 *
 *  Created on: 19 февр. 2020 г.
 *      Author: mikhail.mikhailov
 */

#ifndef INC_REST_H_
#define INC_REST_H_
/*----------------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "config.h"
/*------------------------------ Enum ----------------------------------------*/

/*------------------------------ Default -------------------------------------*/
#define	XML_START_STR		"<tsRequest>"
#define	XML_END_STR			"<\tsRequest>"

#define	QUOTES_ANCII		0x22U
/*----------------------------- Functions ------------------------------------*/
uint32_t 	uRESTmakeConfig( char* output, eConfigReg* reg );
/*---------------------------------------------------------------------------------------------------*/

#endif /* INC_REST_H_ */

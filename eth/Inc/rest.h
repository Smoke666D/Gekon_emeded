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
typedef enum
{
	REST_LAST_RECORD,
	REST_CONT_RECORD
} RESTrecordPos;

typedef enum
{
	REST_CONFIGS = 0x00U,
} RESTrequests;
/*------------------------------ Default -------------------------------------*/
#define	QUOTES_ANCII						0x22U

#define	REST_BUFFER_SIZE				150

#define	REST_REQUEST_NUMBER			1U
#define	REST_REQUEST_CONFIGS		"configs"
/*------------------------------ Extern --------------------------------------*/
extern const char 	*restRequeststr[REST_REQUEST_NUMBER];
/*----------------------------- Functions ------------------------------------*/
uint32_t 	uRESTmakeConfig( char* output, eConfigReg* reg );
/*---------------------------------------------------------------------------------------------------*/

#endif /* INC_REST_H_ */

/*
 * log.h
 *
 *  Created on: 16 мар. 2020 г.
 *      Author: mikhail.mikhailov
 */
#ifndef INC_LOG_H_
#define INC_LOG_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
/*------------------------ Define --------------------------------------*/
#define	LOG_RCORD_MESSAGE_LENGTH	30U
/*------------------------- Enum ---------------------------------------*/
typedef enum
{
	RECORD_SOURCE_CORE,
	RECORD_SOURCE_SERVER,
} RECORD_SOURCE;

typedef enum
{
	MESSAGE,
	WARNING,
	ERROR,
} RECORD_PRIORITY;
/*----------------------- Structures -----------------------------------*/
typedef struct
{
	uint32_t					id;
	RECORD_SOURCE			source;
	RECORD_PRIORITY		priority;
	char							message[LOG_RCORD_MESSAGE_LENGTH];
	uint32_t					value;
} LOG_RECORD;
/*----------------------------------------------------------------------*/
#endif /* INC_LOG_H_ */
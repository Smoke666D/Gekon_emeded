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
#include "controllerTypes.h"
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
  RECORD_PRIORITY_MESSAGE,
  RECORD_PRIORITY_WARNING,
  RECORD_PRIORITY_ERROR,
} RECORD_PRIORITY;
/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
	uint32_t					id;
	RECORD_SOURCE			source   : 1U;
	RECORD_PRIORITY		priority : 2U;
	char							message[LOG_RCORD_MESSAGE_LENGTH];
	uint32_t					value;
} LOG_RECORD;
/*----------------------------------------------------------------------*/
#endif /* INC_LOG_H_ */

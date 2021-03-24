/*
 * outputProcessing.h
 *
 *  Created on: 22 мар. 2021 г.
 *      Author: 79110
 */

#ifndef INC_OUTPUTPROCESSING_H_
#define INC_OUTPUTPROCESSING_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "dataAPI.h"
#include "controllerTypes.h"
#include "outputData.h"
/*------------------------ Define --------------------------------------*/
#define MEASUREMENT_CHANNEL_NUMBER       26U
#define CONTROL_CHANNEL_NUMBER           14U
#define CONTROL_CMD_NUMBER               6U
#define CONTROL_STATUS_NUMBER            6U
#define CONTROL_READ_ONLY_NUMBER         37U
#define OUTPUT_SEMAPHORE_TAKE_DELAY      ( ( TickType_t ) 1000U )
/*-------------------------- CHECK -------------------------------------*/
#if ( ( MEASUREMENT_CHANNEL_NUMBER + CONTROL_CHANNEL_NUMBER ) != OUTPUT_DATA_REGISTER_NUMBER )
 #error Wrong channel number
#endif
/*-------------------------- ENUM --------------------------------------*/
typedef enum
{
  OUTPUT_REGISTER_TYPE_RESERVED,
  OUTPUT_REGISTER_TYPE_READ_ONLY,
  OUTPUT_REGISTER_TYPE_READ_WRITE,
} OUTPUT_REGISTER_TYPE;

typedef enum
{
  OUTPUT_STATUS_OK,
  OUTPUT_STATUS_BUSY,
  OUTPUT_STATUS_INIT_ERROR,
  OUTPUT_STATUS_ACCESS_DENIED,
} OUTPUT_STATUS;
/*----------------------- Structures -----------------------------------*/
/*------------------------ Functions -----------------------------------*/
void                 vOUTPUTinit ( void );
OUTPUT_REGISTER_TYPE eOUTPUTgetType ( uint8_t number );
void                 vOUTPUTupdate ( uint8_t chanel );
OUTPUT_STATUS        vOUTPUTwrite ( uint8_t chanel, uint16_t data );

/*----------------------------------------------------------------------*/
#endif /* INC_OUTPUTPROCESSING_H_ */

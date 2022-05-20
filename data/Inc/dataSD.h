/*
 * dataSD.h
 *
 *  Created on: 17 нояб. 2021 г.
 *      Author: 79110
 */

#ifndef INC_DATASD_H_
#define INC_DATASD_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "fatsd.h"
/*------------------------ Define --------------------------------------*/
#define SD_QUEUE_LENGTH      20U
#define SD_CONFIG_MIN_LENGTH 18U

#if ( LOG_RECORD_SIZE > MEASUREMENT_RECORD_SIZE )
  #define SD_ROUTINE_DATA_SIZE ( LOG_RECORD_SIZE )
#else
  #define SD_ROUTINE_DATA_SIZE ( MEASUREMENT_RECORD_SIZE )
#endif
/*------------------------- Macros -------------------------------------*/
/*-------------------------- ENUM --------------------------------------*/
typedef enum
{
  SD_CONFIG_STATUS_OK,
  SD_CONFIG_STATUS_FAT_ERROR,
  SD_CONFIG_STATUS_FILE_ERROR
} SD_CONFIG_STATUS;

typedef enum
{
  SD_COMMAND_READ,
  SD_COMMAND_WRITE
} SD_COMMAND;
/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  FATSD_FILE file : 2U;
  SD_COMMAND cmd  : 1U;
  char*      buffer;
  uint32_t   length;
} SD_ROUTINE;
/*------------------------ Functions -----------------------------------*/
void     vSDinit ( void );
void     vSDsendRoutine ( SD_ROUTINE* routine );
/*----------------------------------------------------------------------*/
#endif /* INC_DATASD_H_ */

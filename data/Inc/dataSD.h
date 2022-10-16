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
#define SD_QUEUE_LENGTH      10U
#define SD_CONFIG_MIN_LENGTH 18U
#define SD_ROUTINE_DATA_SIZE 100U
#define SD_TIME_SIZE         6U
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
  FATSD_FILE file;
  SD_COMMAND cmd;
  uint8_t    data[SD_ROUTINE_DATA_SIZE];
  uint32_t   length;
} SD_ROUTINE;
/*------------------------ External ------------------------------------*/
#if defined ( UNIT_TEST )
  extern uint8_t configCheker;
#endif
/*------------------------ Functions -----------------------------------*/
#if defined ( UNIT_TEST )
  char*    cSDcheckConfigCallback ( uint16_t length );
  char*    cSDreadConfigCallback ( uint16_t length );
  uint32_t uSDcopyDigsToLine ( uint16_t* data, uint32_t length, char* output, char seporator, uint8_t end );
  uint32_t uSDmakeMeasurement ( void* input, uint8_t length, char* output );
#endif
void    vSDinit ( void );
void    vSDsendRoutine ( SD_ROUTINE* routine );
FRESULT eSDgetStatus ( void );
/*----------------------------------------------------------------------*/
#endif /* INC_DATASD_H_ */

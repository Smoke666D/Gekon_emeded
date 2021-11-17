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
/*------------------------- Macros -------------------------------------*/
/*-------------------------- ENUM --------------------------------------*/
typedef enum
{
  SD_CONFIG_STATUS_OK,
  SD_CONFIG_STATUS_FAT_ERROR,
  SD_CONFIG_STATUS_FILE_ERROR
} SD_CONFIG_STATUS;
/*----------------------- Structures -----------------------------------*/
/*------------------------ Functions -----------------------------------*/
FRESULT eSDsaveConfig ( void );
FRESULT eSDloadConfig ( void );
/*----------------------------------------------------------------------*/
#endif /* INC_DATASD_H_ */

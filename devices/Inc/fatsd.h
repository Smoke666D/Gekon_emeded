/*
 * fatsd.h
 *
 *  Created on: Nov 12, 2021
 *      Author: 79110
 */
#ifndef INC_FATSD_H_
#define INC_FATSD_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "common.h"
#include "fatfs.h"
/*------------------------ Define --------------------------------------*/
#define FATSD_BUFFER_SIZE      50U
#define MIN_FAT32              65526U
#define FILES_NUMBER           3U
#define SEMAPHORE_ACCSEE_DELAY ( ( TickType_t ) 1000U )
#define CONFIG_FILE_NAME       u8"config.jsn\0"
#define MEASUREMEMT_FILE_NAME  u8"measure.jsn\0"
#define LOG_FILE_NAME          u8"log.jsn\0"
/*------------------------- Macros -------------------------------------*/
/*-------------------------- ENUM --------------------------------------*/
typedef enum
{
  SD_EXTRACTED,
  SD_INSERTED
} SD_POSITION;
typedef enum
{
  FATSD_FILE_CONFIG,
  FATSD_FILE_MEASUREMENT,
  FATSD_FILE_LOG
} FATSD_FILE;
/*----------------------- Callbacks ------------------------------------*/
typedef char* ( *lineParserCallback )( uint16_t length );
/*----------------------- Structures -----------------------------------*/
/*------------------------ Functions -----------------------------------*/
void     vFATSDinit ( const GPIO_TYPE* cd, const SD_HandleTypeDef* sd );
uint8_t  uFATSDisMount ( void );
FRESULT  eFILEreadLineByLine ( FATSD_FILE n, lineParserCallback callback );
FRESULT  eFILEaddLine ( FATSD_FILE n, const char* line, uint32_t length );
uint32_t uFATSDgetFreeSpace ( void );
/*----------------------------------------------------------------------*/
#endif /* INC_FATSD_H_ */

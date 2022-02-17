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
  SD_EXTRACTED = SD_NOT_PRESENT,
  SD_INSERTED  = SD_PRESENT
} SD_POSITION;
typedef enum
{
  FATSD_FILE_CONFIG,
  FATSD_FILE_MEASUREMENT,
  FATSD_FILE_LOG
} FATSD_FILE;
typedef enum
{
  SD_STATUS_UNMOUNTED,
  SD_STATUS_MOUNTED,
  SD_STATUS_LOCKED,
  SD_STATUS_ERROR
} SD_STATUS;
/*----------------------- Callbacks ------------------------------------*/
typedef char* ( *lineParserCallback )( uint16_t length );
/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  FATSD_FILE file : 2U;
  uint8_t    lock : 1U;
} FATSD_CACHE;
typedef struct __packed
{
  SD_STATUS   status   : 2U;
  uint8_t     mounted  : 1U;
  SD_POSITION position : 1U;
} FATSD_TYPE;
/*------------------------ Functions -----------------------------------*/
void      vFATSDinit ( const SD_HandleTypeDef* sd );
FRESULT   eFILEreadLineByLine ( FATSD_FILE n, lineParserCallback callback );
FRESULT   eFILEaddLine ( FATSD_FILE n, const char* line, uint32_t length );
FRESULT   eFILEerase ( FATSD_FILE n );
uint32_t  uFATSDgetFreeSpace ( void );
uint32_t  uFATSDgetFullSpace ( void );
SD_STATUS eFATSDgetStatus ( void );
char*     cFATSDgetBuffer ( void );
/*----------------------------------------------------------------------*/
#endif /* INC_FATSD_H_ */

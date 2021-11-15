/*
 * fatsd.c
 *
 *  Created on: Nov 12, 2021
 *      Author: 79110
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "fatsd.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "system.h"
/*------------------------- Define ------------------------------------------------------------------*/
/*----------------------- Structures ----------------------------------------------------------------*/
static osThreadId_t      fatsdHandle          = NULL;
static SemaphoreHandle_t xFileAccessSemaphore = NULL;
static FILINFO           finfo                = { 0U };
static FIL               file                 = { 0U };
static GPIO_TYPE         sdCD                 = { 0U };
static SD_HandleTypeDef* hsd                  = NULL;
static HAL_SD_CardInfoTypeDef cardInfo = { 0U };
/*----------------------- Constant ------------------------------------------------------------------*/
static const char* fileNames[FILES_NUMBER] = { CONFIG_FILE_NAME, MEASUREMEMT_FILE_NAME, LOG_FILE_NAME };
/*----------------------- Variables -----------------------------------------------------------------*/
static uint32_t    fcount    = 0U;
static uint32_t    lineCount = 0U;
static SD_POSITION position  = SD_EXTRACTED;
static uint8_t     mounted   = 0U;

static char        fbuf[20U] = { 0U };
/*----------------------- Functions -----------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PRIVATE -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
FRESULT eFATSDcheckFile ( const char* path )
{
  FRESULT res = f_stat( path, &finfo );
  if ( res == FR_NO_FILE )
  {
    taskENTER_CRITICAL();
    res = f_open( &file, path, FA_CREATE_NEW );
    if ( res == FR_OK )
    {
      res = f_close( &file );
    }
    taskEXIT_CRITICAL();
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
FRESULT eFATSDmount ( void )
{
  FRESULT res = FR_OK;
  uint8_t i   = 0U;
  fcount    = 0U;
  lineCount = 0U;
  if ( xSemaphoreTake( xFileAccessSemaphore, SEMAPHORE_ACCSEE_DELAY ) == pdTRUE )
  {
    res = f_mount( &SDFatFS, SDPath, 1U );
    if ( res == FR_NO_FILESYSTEM )
    {
      taskENTER_CRITICAL();
      while ( res != FR_OK )
      {
        res = f_mkfs( SDPath, 0U, MIN_FAT32 );
      }
      taskEXIT_CRITICAL();
    }
    if ( res == FR_OK )
    {
      for ( i=0U; i<FILES_NUMBER; i++ )
      {
        res = eFATSDcheckFile( fileNames[i] );
        if ( res != FR_OK )
        {
          break;
        }
      }
    }
    xSemaphoreGive( xFileAccessSemaphore );
  }
  else
  {
    res = FR_BUSY;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
FRESULT eFATSDunmount ( void )
{
  xSemaphoreGive( xFileAccessSemaphore );
  return f_mount( NULL, SDPath, 1U );
}
/*---------------------------------------------------------------------------------------------------*/
void vFATSDtask ( void* argument )
{
  FRESULT res   = FR_OK;
  uint8_t retSD = 0U;
  HAL_SD_MspDeInit( hsd );
  for (;;)
  {
    if ( position == SD_EXTRACTED )
    {
      if ( HAL_GPIO_ReadPin( sdCD.port, sdCD.pin ) == GPIO_PIN_RESET )
      {
        position = SD_INSERTED;
        HAL_SD_MspInit( hsd );
        BSP_SD_GetCardInfo( &cardInfo );
        retSD = FATFS_LinkDriver( &SD_Driver, SDPath );
        res   = eFATSDmount();
        if ( res == FR_OK )
        {
          mounted = 1U;
          res = eFILEaddLine( FATSD_FILE_LOG, u8"SD inserted\n", 12U );
        }
      }
    }
    else
    {
      if ( HAL_GPIO_ReadPin( sdCD.port, sdCD.pin ) == GPIO_PIN_SET )
      {
        position = SD_EXTRACTED;
        res      = eFATSDunmount();
        retSD    = FATFS_UnLinkDriverEx( SDPath, 0 );
        HAL_SD_MspDeInit( hsd );
        mounted  = 0U;
      }
    }
    osDelay( 1000U );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLIC --------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
void vFATSDinit ( const GPIO_TYPE* cd, const SD_HandleTypeDef* sd )
{
  sdCD = *cd;
  hsd  = ( SD_HandleTypeDef* )sd;
  xFileAccessSemaphore = xSemaphoreCreateMutex();
  const osThreadAttr_t fatsdTask_attributes = {
    .name       = "fatsdTask",
    .priority   = ( osPriority_t ) FATSD_TASK_PRIORITY,
    .stack_size = FATSD_TASK_STACK_SIZE
  };
  fatsdHandle = osThreadNew( vFATSDtask, NULL, &fatsdTask_attributes );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uFATSDisMount ( void )
{
  return mounted;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Read file from file system to the buffer.
 * Callback return the pointer to the buffer. At first and last calling
 * callback get zero length. So callback need to parsing previous data set.
 * input: n        - name of the file from the list
 *        callback - function for parsing
 */
FRESULT eFILEreadLineByLine ( FATSD_FILE n, lineParserCallback callback )
{
  FRESULT  res      = FR_OK;
  char*    output   = NULL;
  uint32_t i        = 0U;
  uint8_t  counter  = 0U;
  uint32_t chars    = 0U;
  char     data[2U] = { 0U };
  if ( mounted > 0U )
  {
    if ( xSemaphoreTake( xFileAccessSemaphore, SEMAPHORE_ACCSEE_DELAY ) == pdTRUE )
    {
      /* Open file */
      res = f_open( &file, fileNames[n], FA_READ );
      if ( res == FR_OK )
      {
        if ( f_error( &file ) == 0U )
        {
          /* Get the strings and parsing */
          output = callback( 0U );
          if ( output != NULL )
          {
            for ( i=0U; i<file.fsize; i++ )
            {
              res = f_read( &file, &data, 1U, ( UINT* )&counter );
              if ( res == FR_OK )
              {
                if ( data[0U] == '\n')       /* EOL */
                {
                  output[chars] = 0x00;
                  chars++;
                  output = callback( chars );
                  chars  = 0U;
                }
                else if ( data[0U] == 0x00 ) /* EOF */
                {
                  output[chars] = 0x00;
                  chars++;
                  break;
                }
                else /*  */
                {
                  output[chars] = data[0U];
                  chars++;
                }
              }
              else
              {
                break;
              }
            }
            if ( res == FR_OK )
            {
              output = callback( chars );
              output = callback( 0U );
              res    = f_close( &file );
            }
          }
        }
        else
        {
          res = FR_INVALID_OBJECT;
        }
      }
      xSemaphoreGive( xFileAccessSemaphore );
    }
    else
    {
      res = FR_BUSY;
    }
  }
  else
  {
    res = FR_NO_MOUNT;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
FRESULT eFILEaddLine ( FATSD_FILE n, const char* line, uint32_t length )
{
  FRESULT  res     = FR_OK;
  uint32_t counter = 0U;
  if ( mounted > 0U )
  {
    if ( xSemaphoreTake( xFileAccessSemaphore, SEMAPHORE_ACCSEE_DELAY ) == pdTRUE )
    {
      taskENTER_CRITICAL();
      res = f_open( &file, fileNames[n], FA_WRITE );
      if ( res == FR_OK )
      {
        if ( f_error( &file ) == 0U )
        {
          res = f_lseek( &file, file.fsize );
          if ( res == FR_OK )
          {
            res = f_write( &file, line, length, ( UINT* )&counter );
            if ( res == FR_OK )
            {
              if ( length == counter )
              {
                res = f_close( &file );
                if ( ( res == FR_OK ) && ( length != counter ) )
                {
                  res = FR_WRITE_COUNTER_ERROR;
                }
              }
              else
              {
                res = FR_WRITE_COUNTER_ERROR;
              }
            }
          }
        }
        else
        {
          res = FR_INVALID_OBJECT;
        }
      }
      taskEXIT_CRITICAL();
      xSemaphoreGive( xFileAccessSemaphore );
    }
    else
    {
      res = FR_BUSY;
    }
  }
  else
  {
    res = FR_NO_MOUNT;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uFATSDgetFreeSpace ( void )
{
  uint32_t size = 0U;
  FRESULT  res  = FR_OK;
  FATFS*   fs   = NULL;
  if ( mounted > 0U )
  {
    res = f_getfree( SDPath, ( DWORD* )&size, &fs );
    if ( res == FR_OK )
    {
      size *= fs->csize;
    }
  }
  return size;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

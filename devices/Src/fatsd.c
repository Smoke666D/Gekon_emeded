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
#include "freeData.h"
/*------------------------- Define ------------------------------------------------------------------*/
/*----------------------- Structures ----------------------------------------------------------------*/
#if defined( FATSD )
static osThreadId_t           fatsdHandle          = NULL;
static SemaphoreHandle_t      xFileAccessSemaphore = NULL;
static FILINFO                finfo                = { 0U };
static FIL                    file                 = { 0U };
static SD_HandleTypeDef*      hsd                  = NULL;
static HAL_SD_CardInfoTypeDef cardInfo             = { 0U };
static FATSD_TYPE             fatsd                = { 0U };
/*----------------------- Constant ------------------------------------------------------------------*/
static const char* fileNames[FILES_NUMBER] = { CONFIG_FILE_NAME, MEASUREMEMT_FILE_NAME, LOG_FILE_NAME };
/*----------------------- Variables -----------------------------------------------------------------*/
static uint32_t fcount    = 0U;
static uint32_t lineCount = 0U;
static uint8_t  strCount  = 0U;
static char     fbuf[FATSD_BUFFER_SIZE] = { 0U };
#endif
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
  fcount    = 0U;
  lineCount = 0U;
  if ( xSemaphoreTake( xFileAccessSemaphore, SEMAPHORE_ACCSEE_DELAY ) == pdTRUE )
  {
    res = f_mount( &SDFatFS, SDPath, 1U );
    if ( res == FR_NO_FILESYSTEM )
    {
      while ( res != FR_OK )
      {
        res = f_mkfs( SDPath, 0U, MIN_FAT32 );
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
  FRESULT res = FR_OK;
  xSemaphoreGive( xFileAccessSemaphore );
  res = f_mount( NULL, SDPath, 1U );
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
char* vFILEstringCounter ( uint16_t length )
{
  if ( length > 0U )
  {
    strCount++;
  }
  return fbuf;
}
/*---------------------------------------------------------------------------------------------------*/
void vFATSDtask ( void* argument )
{
  FRESULT  res   = FR_OK;
  uint8_t  retSD = 0U;
  uint32_t size  = 0U;
  osDelay( 1000U );
  HAL_SD_MspDeInit( hsd );
  for (;;)
  {
    if ( fatsd.position == SD_EXTRACTED )
    {
      if ( HAL_GPIO_ReadPin( SD_DETECT_GPIO_PORT, SD_DETECT_PIN ) == GPIO_PIN_RESET )
      {
        fatsd.position = SD_INSERTED;
        HAL_SD_MspInit( hsd );
        retSD = FATFS_LinkDriver( &SD_Driver, SDPath );
        if ( retSD == 0 )
        {
          res = eFATSDmount();
          BSP_SD_GetCardInfo( &cardInfo );
          if ( res == FR_OK )
          {
            fatsd.mounted                     = 1U;
            fatsd.status                      = SD_STATUS_MOUNTED;
            size                              = uFATSDgetFullSpace();
            *freeDataArray[SD_SIZE_LOW_ADR]   = ( uint16_t )( size );
            *freeDataArray[SD_SIZE_HIGHT_ADR] = ( uint16_t )( size >> 16U );
          }
          else if ( res == FR_LOCKED )
          {
            fatsd.mounted = 1U;
            fatsd.status  = SD_STATUS_LOCKED;
          }
          else
          {
            fatsd.status = SD_STATUS_ERROR;
          }
        }
        else
        {
          res = FR_LINK_ERROR;
        }
      }
    }
    else
    {
      if ( HAL_GPIO_ReadPin( SD_DETECT_GPIO_PORT, SD_DETECT_PIN ) == GPIO_PIN_SET )
      {
        fatsd.position = SD_EXTRACTED;
        res            = eFATSDunmount();
        retSD          = FATFS_UnLinkDriverEx( SDPath, 0 );
        HAL_SD_DeInit( hsd );
        SDIO->POWER    = 0x00000000;
        SDIO->CLKCR    = 0x00000000;
        SDIO->ARG      = 0x00000000;
        SDIO->CMD      = 0x00000000;
        SDIO->DTIMER   = 0x00000000;
        SDIO->DLEN     = 0x00000000;
        SDIO->DCTRL    = 0x00000000;
        SDIO->ICR      = 0x00C007FF;
        SDIO->MASK     = 0x00000000;
        fatsd.mounted  = 0U;
        fatsd.status   = SD_STATUS_UNMOUNTED;
        *freeDataArray[SD_SIZE_LOW_ADR]   = 0U;
        *freeDataArray[SD_SIZE_HIGHT_ADR] = 0U;
        *freeDataArray[SD_FREE_LOW_ADR]   = 0U;
        *freeDataArray[SD_FREE_HIGHT_ADR] = 0U;
      }
    }
    osDelay( 1000U );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLIC --------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
void vFATSDinit ( const SD_HandleTypeDef* sd )
{
  #if defined( FATSD )
    hsd                  = ( SD_HandleTypeDef* )sd;
    fatsd.position       = SD_EXTRACTED;
    fatsd.status         = SD_STATUS_UNMOUNTED;
    xFileAccessSemaphore = xSemaphoreCreateMutex();
    const osThreadAttr_t fatsdTask_attributes = {
      .name       = "fatsdTask",
      .priority   = ( osPriority_t ) FATSD_TASK_PRIORITY,
      .stack_size = FATSD_TASK_STACK_SIZE
    };
    fatsdHandle = osThreadNew( vFATSDtask, NULL, &fatsdTask_attributes );
  #endif
  return;
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
  uint32_t length   = 0U;
  uint8_t  data[2U] = { 0U };
  if ( ( fatsd.status == SD_STATUS_MOUNTED ) || ( fatsd.status == SD_STATUS_LOCKED ) )
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
                  output[length] = 0x00;
                  length++;
                  output = callback( length );
                  length = 0U;
                }
                else if ( data[0U] == 0x00 ) /* EOF */
                {
                  output[length] = 0x00;
                  length++;
                  break;
                }
                else /*  */
                {
                  output[length] = data[0U];
                  length++;
                }
              }
              else
              {
                break;
              }
            }
            if ( res == FR_OK )
            {
              output = callback( length );
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
      if ( res != FR_OK )
      {
        fatsd.status = SD_STATUS_ERROR;
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
FRESULT eFILEerase ( FATSD_FILE n )
{
  FRESULT res = FR_OK;
  if ( fatsd.status == SD_STATUS_MOUNTED )
  {
    if ( xSemaphoreTake( xFileAccessSemaphore, SEMAPHORE_ACCSEE_DELAY ) == pdTRUE )
    {
      res = f_open( &file, fileNames[n], FA_WRITE );
      if ( res == FR_NO_FILE )
      {
        res = f_open( &file, fileNames[n], FA_CREATE_NEW );
        if ( res == FR_OK )
        {
          res = f_close( &file );
        }
      }
      else if ( res == FR_OK )
      {
        res = f_truncate( &file );
        if ( res == FR_OK )
        {
          res = f_close( &file );
        }
      }
      else
      {

      }
      if ( res != FR_OK )
      {
        fatsd.status = SD_STATUS_ERROR;
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
  if ( fatsd.status == SD_STATUS_MOUNTED )
  {
    if ( xSemaphoreTake( xFileAccessSemaphore, SEMAPHORE_ACCSEE_DELAY ) == pdTRUE )
    {
      res = f_open( &file, fileNames[n], ( FA_OPEN_ALWAYS | FA_WRITE ) );
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
      if ( res != FR_OK )
      {
        fatsd.status = SD_STATUS_ERROR;
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
/*
 * Return free space at SD card in Kb
 */
uint32_t uFATSDgetFreeSpace ( void )
{
  uint32_t size = 0U;
  FRESULT  res  = FR_OK;
  FATFS*   fs   = NULL;
  if ( ( fatsd.status == SD_STATUS_MOUNTED ) || ( fatsd.status == SD_STATUS_LOCKED ) )
  {
    res = f_getfree( SDPath, ( DWORD* )&size, &fs );
    if ( res == FR_OK )
    {
      size *= fs->csize / 2U;
    }
    if ( res != FR_OK )
    {
      fatsd.status = SD_STATUS_ERROR;
    }
  }
  return size;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Return full space at SD card in Kb
 */
uint32_t uFATSDgetFullSpace ( void )
{
  uint32_t size = 0U;
  FRESULT  res  = FR_OK;
  FATFS*   fs   = NULL;
  if ( ( fatsd.status == SD_STATUS_MOUNTED ) || ( fatsd.status == SD_STATUS_LOCKED ) )
  {
    res = f_getfree( SDPath, ( DWORD* )&size, &fs );
    if ( res == FR_OK )
    {
      size = ( fs->n_fatent - 2 ) * fs->csize / 2U;
    }
    if ( res != FR_OK )
    {
      fatsd.status = SD_STATUS_ERROR;
    }
  }
  return size;
}
/*---------------------------------------------------------------------------------------------------*/
SD_STATUS eFATSDgetStatus ( void )
{
  return fatsd.status;
}
/*---------------------------------------------------------------------------------------------------*/
char* cFATSDgetBuffer ( void )
{
  return fbuf;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

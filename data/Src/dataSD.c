/*
 * dataSD.c
 *
 *  Created on: 17 нояб. 2021 г.
 *      Author: 79110
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "dataSD.h"
#include "config.h"
#include "rest.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "system.h"
/*------------------------- Define ------------------------------------------------------------------*/
/*----------------------- Structures ----------------------------------------------------------------*/
static osThreadId_t  sdHandle                       = NULL;
static QueueHandle_t pSDqueue                       = NULL;
static StaticQueue_t xSDqueue                       = { 0U };
static SD_ROUTINE    sdQueueBuffer[SD_QUEUE_LENGTH] = { 0U };
/*----------------------- Constant ------------------------------------------------------------------*/
/*----------------------- Variables -----------------------------------------------------------------*/
static SD_CONFIG_STATUS status = SD_CONFIG_STATUS_FAT_ERROR;
static uint8_t          configCheker = 0U;
/*----------------------- Functions -----------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PRIVATE -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
char* cSDcheckConfigCallback ( uint16_t length )
{
  char* buf = cFATSDgetBuffer();
  if ( length > 18U )
  {
    configCheker++;
  }
  return buf;
}
/*---------------------------------------------------------------------------------------------------*/
char* cSDreadConfigCallback ( uint16_t length )
{
  REST_ERROR res = REST_OK;
  char*      buf = cFATSDgetBuffer();
  if ( length > 18U )
  {
    res = eRESTparsingShortConfig( buf );
    if ( res != REST_OK )
    {
      configCheker = 0U;
    }
  }
  return buf;
}
/*---------------------------------------------------------------------------------------------------*/
void vSDtask ( void* argument )
{
  SD_ROUTINE input = { 0U };
  FRESULT    res   = FR_OK;
  for (;;)
  {
    if ( xQueueReceive( pSDqueue, &input, 0U ) == pdPASS )
    {
      if ( eFATSDgetStatus() == SD_STATUS_MOUNTED )
      {
        switch ( input.file )
        {
          case FATSD_FILE_CONFIG:
            if ( input.cmd == SD_COMMAND_READ )
            {
              res = eSDloadConfig();
            }
            else
            {
              res = eSDsaveConfig();
            }
            break;
          case FATSD_FILE_MEASUREMENT:
            #if defined( MEASUREMENT )
              res = eFILEaddLine( FATSD_FILE_MEASUREMENT, input.buffer, input.length );
            #endif
            break;
          case FATSD_FILE_LOG:
            #if defined( WRITE_LOG_TO_SD )
              res = eFILEaddLine( FATSD_FILE_LOG, input.buffer, input.length );
            #endif
            break;
          default:
            break;
        }
      }
      res = FR_OK;
    }
    //osDelay( 1000U );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLIC --------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
void vSDinit ( void )
{
  #if defined ( FATSD )
    const osThreadAttr_t fatsdTask_attributes = {
      .name       = "fatsdTask",
      .priority   = ( osPriority_t ) FATSD_TASK_PRIORITY,
      .stack_size = FATSD_TASK_STACK_SIZE
    };
    pSDqueue = xQueueCreateStatic( SD_QUEUE_LENGTH,
                                  sizeof( SD_ROUTINE ),
                                  ( uint8_t* )sdQueueBuffer,
                                  &xSDqueue );
    sdHandle = osThreadNew( vSDtask, NULL, &fatsdTask_attributes );
  #endif
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vSDsendRoutine ( SD_ROUTINE* routine )
{
  SD_ROUTINE sdRoutine = *routine;
  if ( pSDqueue != NULL )
  {
    xQueueSend( pSDqueue, &sdRoutine, portMAX_DELAY );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
FRESULT eSDsaveConfig ( void )
{
  uint32_t length = 0U;
  FRESULT  res    = FR_OK;
  res = eFILEerase( FATSD_FILE_CONFIG );
  if ( res == FR_OK )
  {
    length = uSYSputChar( cFATSDgetBuffer(), length, '[' );
    length = uSYSendString( cFATSDgetBuffer(), length );
    res    = eFILEaddLine( FATSD_FILE_CONFIG, cFATSDgetBuffer(), length );
    if ( res == FR_OK )
    {
      for ( uint32_t i=0U; i<SETTING_REGISTER_NUMBER; i++ )
      {
        length = uRESTmakeShortCongig( configReg[i], cFATSDgetBuffer() );
        if ( i < ( SETTING_REGISTER_NUMBER - 1U ) )
        {
          length = uSYSputChar( cFATSDgetBuffer(), length, ',' );
        }
        length = uSYSendString( cFATSDgetBuffer(), length );
        res    = eFILEaddLine( FATSD_FILE_CONFIG, cFATSDgetBuffer(), length );
        if ( res != FR_OK )
        {
          break;
        }
      }
      if ( res == FR_OK )
      {
        length = 0U;
        length = uSYSputChar( cFATSDgetBuffer(), length, ']' );
        length = uSYSendString( cFATSDgetBuffer(), length );
        res    = eFILEaddLine( FATSD_FILE_CONFIG, cFATSDgetBuffer(), length );
      }
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
FRESULT eSDloadConfig ( void )
{
  FRESULT  res = FR_OK;
  status       = SD_CONFIG_STATUS_OK;
  configCheker = 0U;
  res          = eFILEreadLineByLine( FATSD_FILE_CONFIG, cSDcheckConfigCallback );
  if ( res == FR_OK )
  {
    if ( configCheker != SETTING_REGISTER_NUMBER )
    {
      status = SD_CONFIG_STATUS_FILE_ERROR;
      res    = FR_FILE_CHECK_ERROR;
    }
    if ( status == SD_CONFIG_STATUS_OK )
    {
      res = eFILEreadLineByLine( FATSD_FILE_CONFIG, cSDreadConfigCallback );
      if ( res == FR_OK )
      {
        if ( configCheker == 0U )
        {
          res = FR_FILE_CHECK_ERROR;
        }
      }
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

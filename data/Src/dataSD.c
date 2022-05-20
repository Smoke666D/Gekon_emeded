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
#include "string.h"
#include "stdio.h"
#include "controllerTypes.h"
/*------------------------- Define ------------------------------------------------------------------*/
/*----------------------- Structures ----------------------------------------------------------------*/
static osThreadId_t  sdHandle                       = NULL;
static QueueHandle_t pSDqueue                       = NULL;
static StaticQueue_t xSDqueue                       = { 0U };
static SD_ROUTINE    sdQueueBuffer[SD_QUEUE_LENGTH] = { 0U };
/*----------------------- Constant ------------------------------------------------------------------*/
/*----------------------- Variables -----------------------------------------------------------------*/
static SD_CONFIG_STATUS status       = SD_CONFIG_STATUS_FAT_ERROR;
static uint8_t          configCheker = 0U;
/*----------------------- Functions -----------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PRIVATE -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
char* cSDcheckConfigCallback ( uint16_t length )
{
  char* buf = cFATSDgetBuffer();
  if ( length > SD_CONFIG_MIN_LENGTH )
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
  if ( length > SD_CONFIG_MIN_LENGTH )
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
    length = 0U;
    if ( res == FR_OK )
    {
      for ( uint32_t i=0U; ( ( i < SETTING_REGISTER_NUMBER ) && ( res != FR_OK ) ); i++ )
      {
        length = uRESTmakeShortCongig( configReg[i], cFATSDgetBuffer() );
        if ( i < ( SETTING_REGISTER_NUMBER - 1U ) )
        {
          length = uSYSputChar( cFATSDgetBuffer(), length, ',' );
        }
        length = uSYSendString( cFATSDgetBuffer(), length );
        res    = eFILEaddLine( FATSD_FILE_CONFIG, cFATSDgetBuffer(), length );
        length = 0U;
      }
      if ( res == FR_OK )
      {
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
  status       = SD_CONFIG_STATUS_OK;
  configCheker = 0U;
  FRESULT res  = eFILEreadLineByLine( FATSD_FILE_CONFIG, cSDcheckConfigCallback );
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
uint32_t uSDcopyDigsToLine ( uint16_t* data, uint32_t length, char* output, char seporator )
{
  uint32_t res = 0U;
  char     buffer[6U] = { 0U };
  for ( uint8_t i=0U; i<length; i++ )
  {
    output[res] = 0U;
    res += sprintf( buffer, "%d", data[i] );
    strcat( output, buffer );
    if ( i < ( length - 1U ) )
    {
      output[res] = seporator;
      res++;
    }
  }
  return uSYSendString( output, res );
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uSDmakeMeasurement ( void* input, uint8_t length, char* output )
{
  uint16_t* data = ( uint16_t* )input;
  uint32_t  res  = 0U;
  switch ( data[0U] )
  {
    case MEASUREMENT_RECORD_TYPE_RECORD:
      res += uSDcopyDigsToLine( &data[1U], ( length - 1U ), &output[res], ' ' );
      break;
    case MEASUREMENT_RECORD_TYPE_LEGEND:
      output[0U] = 0U;
      ( void )strcat( output, "// " );
      res = 3U;
      res += uSDcopyDigsToLine( &data[1U], ( length - 1U ), &output[res], ' ' );
      break;
    case MEASUREMENT_RECORD_TYPE_PREAMBOLA:
      output[0U] = 0U;
      ( void )strcat( output, "// " );
      res = 3U;
      res += uSDcopyDigsToLine( &data[1U], ( length - 1U ), &output[res], '.' );
      break;
    default:
      break;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
void vSDtask ( void* argument )
{
  SD_ROUTINE input  = { 0U };
  FRESULT    res    = FR_OK;
  uint32_t   length = 0U;
  for (;;)
  {
    if ( pSDqueue != NULL )
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
                length = uSDmakeMeasurement( input.data, input.length, cFATSDgetBuffer() );
                res    = eFILEaddLine( FATSD_FILE_MEASUREMENT, cFATSDgetBuffer(), length );
              #endif
              break;
            case FATSD_FILE_LOG:
              #if defined( WRITE_LOG_TO_SD )
                length = uRESTmakeLog( ( LOG_RECORD_TYPE* )input.data, cFATSDgetBuffer() );
                length = uSYSendString( cFATSDgetBuffer(), length );
                res    = eFILEaddLine( FATSD_FILE_LOG, cFATSDgetBuffer(), length );
                osDelay( 1000U );
              #endif
              break;
            default:
              break;
          }
        }
      }
    }
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
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

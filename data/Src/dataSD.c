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
#include "system.h"
/*------------------------- Define ------------------------------------------------------------------*/
/*----------------------- Structures ----------------------------------------------------------------*/
static osThreadId_t sdHandle = NULL;
/*----------------------- Constant ------------------------------------------------------------------*/
/*----------------------- Variables -----------------------------------------------------------------*/
static SD_CONFIG_STATUS status = SD_CONFIG_STATUS_FAT_ERROR;
static uint8_t          flag   = 0U;
static FRESULT          confRes;
/*----------------------- Functions -----------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PRIVATE -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
char* cSDcheckConfigCallback ( uint16_t length )
{
  char* buf = NULL;
  if ( length == 0U )
  {

  }
  else
  {

  }
  return buf;
}
/*---------------------------------------------------------------------------------------------------*/
char* cSDreadConfigCallback ( uint16_t length )
{
  char* buf = NULL;
  if ( length == 0U )
  {

  }
  else
  {

  }
  return buf;
}
/*---------------------------------------------------------------------------------------------------*/
void vSDtask ( void* argument )
{
  for (;;)
  {
    if ( ( eFATSDgetStatus() == SD_STATUS_MOUNTED ) && ( flag == 0U ) )
    {
      confRes = eSDsaveConfig();
      flag = 1U;
    }
    else if ( ( eFATSDgetStatus() != SD_STATUS_MOUNTED ) && ( flag == 1U ) )
    {
      flag = 0U;
    }
    osDelay( 100U );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLIC --------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
void vSDinit ( void )
{
  const osThreadAttr_t fatsdTask_attributes = {
    .name       = "fatsdTask",
    .priority   = ( osPriority_t ) FATSD_TASK_PRIORITY,
    .stack_size = FATSD_TASK_STACK_SIZE
  };
  sdHandle = osThreadNew( vSDtask, NULL, &fatsdTask_attributes );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
FRESULT eSDsaveConfig ( void )
{
  uint32_t i      = 0U;
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
      for ( i=0U; i<SETTING_REGISTER_NUMBER; i++ )
      {
        length = uRESTmakeShortCongig( configReg[i], cFATSDgetBuffer() );
        length = uSYSputChar( cFATSDgetBuffer(), length, ',' );
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
  status = SD_CONFIG_STATUS_FAT_ERROR;
  res    = eFILEreadLineByLine( FATSD_FILE_CONFIG, cSDcheckConfigCallback );
  if ( res == FR_OK )
  {
    if ( status == SD_CONFIG_STATUS_OK )
    {
      res = eFILEreadLineByLine( FATSD_FILE_CONFIG, cSDreadConfigCallback );
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

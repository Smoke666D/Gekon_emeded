/*
 * log.c
 *
 *  Created on: Jun 26, 2020
 *      Author: 79110
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "journal.h"
#include "dataAPI.h"
#include "dataProces.h"
#include "system.h"
//#include "fatsd.h"
#include "rest.h"
#include "common.h"
#include "dataSD.h"
/*----------------------- Structures ----------------------------------------------------------------*/
/*----------------------- Constant ------------------------------------------------------------------*/
/*----------------------- Variables -----------------------------------------------------------------*/
/*----------------------- Functions -----------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PRIVATE -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLICK -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
uint32_t uLOGgetTime ( void )
{
  RTC_TIME time = { 0U };
  uint32_t res  = 0U;
  if ( eRTCgetTime( &time ) == RTC_OK )
  {
    res = SET_LOG_DATE( time.day, time.month, time.year, time.hour, time.min, time.sec );
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
LOG_STATUS eLOGmakeRecord ( SYSTEM_EVENT event, LOG_RECORD_TYPE* record )
{
  LOG_STATUS res  = LOG_STATUS_ERROR;

  record->time = uLOGgetTime();
  if ( record->time > 0U )
  {
    record->event = event;
    res           = LOG_STATUS_OK;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
LOG_STATUS eLOGaddRecord ( LOG_RECORD_TYPE* record )
{
  LOG_STATUS res  = LOG_STATUS_ERROR;

  #if ( defined( WRITE_LOG_TO_SD ) && defined ( FATSD ) )
    SD_ROUTINE sdRoutine = { 0U };
    sdRoutine.buffer = cFATSDgetBuffer();
    sdRoutine.cmd    = SD_COMMAND_WRITE;
    sdRoutine.file   = FATSD_FILE_LOG;
    sdRoutine.length = uRESTmakeLog( record, sdRoutine.buffer );
    sdRoutine.length = uSYSendString( sdRoutine.buffer, sdRoutine.length );
    vSDsendRoutine( &sdRoutine );
  #endif

  if ( eDATAAPIlog( DATA_API_CMD_ADD, NULL, record ) == DATA_API_STAT_OK )
  {
    vDATAAPIincLogSize();
    res = LOG_STATUS_OK;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

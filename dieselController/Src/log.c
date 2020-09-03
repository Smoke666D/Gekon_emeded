/*
 * log.c
 *
 *  Created on: Jun 26, 2020
 *      Author: 79110
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "log.h"
#include "storage.h"
/*----------------------- Structures ----------------------------------------------------------------*/
LOG_TYPE  log;
/*----------------------- Constant ------------------------------------------------------------------*/
/*----------------------- Variables -----------------------------------------------------------------*/
/*----------------------- Functions -----------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PRIVATE -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLICK -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
LOG_STATUS vLOGclear ( void )
{
  LOG_STATUS res = LOG_ERROR;
  uint16_t   i   = 0U;
  for ( i=0U; i<LOG_SIZE; i++ )
  {
    log.records[i].number       = 0U;
    log.records[i].time         = LOG_TIME_ERROR;
    log.records[i].event.type   = EVENT_NONE;
    log.records[i].event.action = ACTION_NONE;
  }
  log.quant = 0U;
  if ( eSTORAGEwriteLog( &log ) == EEPROM_OK )
  {
    res = LOG_OK;
  }
  return res;
}

LOG_STATUS vLOGwriteRecord ( SYSTEM_EVENT event )
{
  LOG_STATUS res     = LOG_ERROR;
  uint32_t   logTime = LOG_TIME_ERROR;
  RTC_TIME   time;

  if ( eRTCgetTime( &time ) == RTC_OK )
  {
    logTime = SET_LOG_DATE( time.day, time.month, ( time.year - LOG_START_YEAR ), time.hour, time.min, time.sec );
  }
  log.records[log.quant].number = log.quant;
  log.records[log.quant].time   = logTime;
  log.records[log.quant].event  = event;
  log.quant++;

  if ( eSTORAGEwriteLogQuant( log.quant ) == EEPROM_OK )
  {
    if ( eSTORAGEwriteLogRecord( &log.records[log.quant - 1U] ) != EEPROM_OK )
    {
      res = LOG_OK;
    }
  }
  return res;
}

LOG_STATUS vLOGinit ( void )
{
  LOG_STATUS res = LOG_ERROR;
  if ( eSTORAGEreadLog( &log ) == EEPROM_OK )
  {
    res = LOG_OK;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

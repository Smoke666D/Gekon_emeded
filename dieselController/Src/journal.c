/*
 * log.c
 *
 *  Created on: Jun 26, 2020
 *      Author: 79110
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "journal.h"
#include "dataAPI.h"
/*----------------------- Structures ----------------------------------------------------------------*/
/*----------------------- Constant ------------------------------------------------------------------*/
/*----------------------- Variables -----------------------------------------------------------------*/
static JOURNAL_MEASURMENT journal = { 0U };
/*----------------------- Functions -----------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PRIVATE -------------------------------------------------------------------*/
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
/*----------------------- PABLICK -------------------------------------------------------------------*/
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
length_t uJOURNALcalcLength ( JOURNAL_MEASURMENT* journal )
{
  length_t length = 0U;

  if ( journal->option.time == PERMISSION_ENABLE )
  {
    length += 4U;
  }
  if ( journal->option.oil == PERMISSION_ENABLE )
  {
    length += 2U;
  }
  if ( journal->option.coolant == PERMISSION_ENABLE )
  {
    length += 2U;
  }
  if ( journal->option.fuel == PERMISSION_ENABLE )
  {
    length += 2U;
  }
  if ( journal->option.speed == PERMISSION_ENABLE )
  {
    length += 2U;
  }
  if ( journal->option.din == PERMISSION_ENABLE )
  {
    length += 1U;
  }
  if ( journal->option.genV == PERMISSION_ENABLE )
  {
    length += 2U;
  }
  if ( journal->option.genF == PERMISSION_ENABLE )
  {
    length += 2U;
  }
  if ( journal->option.genC == PERMISSION_ENABLE )
  {
    length += 2U;
  }
  if ( journal->option.genP == PERMISSION_ENABLE )
  {
    length += 2U;
  }
  if ( journal->option.mainsV == PERMISSION_ENABLE )
  {
    length += 2U;
  }
  if ( journal->option.mainsF == PERMISSION_ENABLE )
  {
    length += 2U;
  }
  if ( journal->option.battery == PERMISSION_ENABLE )
  {
    length += 2U;
  }

  return length;
}
/*---------------------------------------------------------------------------------------------------*/
void eJOURNALreadRecord ( JOURNAL_MEASURMENT* journal )
{
  uint32_t buffer = 0U;

  journal->data.counter = 0U;
  if ( journal->option.time == PERMISSION_ENABLE )
  {
    buffer = uLOGgetTime();
    journal->data.blob[journal->data.counter     ] = (uint8_t)( buffer        );
    journal->data.blob[journal->data.counter + 1U] = (uint8_t)( buffer >> 8U  );
    journal->data.blob[journal->data.counter + 2U] = (uint8_t)( buffer >> 16U );
    journal->data.blob[journal->data.counter + 3U] = (uint8_t)( buffer >> 24U );
    journal->data.counter += 4U;
  }
  if ( journal->option.oil == PERMISSION_ENABLE )
  {
    buffer = setValue( configReg[OIL_PRESSURE_ALARM_LEVEL_ADR], OIL_SENSOR_SOURCE() );
    journal->data.blob[journal->data.counter     ] = (uint8_t)( buffer        );
    journal->data.blob[journal->data.counter + 1U] = (uint8_t)( buffer >> 8U  );
    journal->data.counter += 2U;
  }
  if ( journal->option.coolant == PERMISSION_ENABLE )
  {
    buffer = setValue( configReg[OIL_PRESSURE_ALARM_LEVEL_ADR], COOLANT_SENSOR_SOURCE() );
    journal->data.blob[journal->data.counter     ] = (uint8_t)( buffer        );
    journal->data.blob[journal->data.counter + 1U] = (uint8_t)( buffer >> 8U  );
    journal->data.counter += 2U;
  }
  if ( journal->option.fuel == PERMISSION_ENABLE )
  {
    buffer = setValue( configReg[OIL_PRESSURE_ALARM_LEVEL_ADR], FUEL_SENSOR_SOURCE() );
    journal->data.blob[journal->data.counter     ] = (uint8_t)( buffer        );
    journal->data.blob[journal->data.counter + 1U] = (uint8_t)( buffer >> 8U  );
    journal->data.counter += 2U;
  }
  if ( journal->option.speed == PERMISSION_ENABLE )
  {

  }
  if ( journal->option.din == PERMISSION_ENABLE )
  {

  }
  if ( journal->option.genV == PERMISSION_ENABLE )
  {

  }
  if ( journal->option.genF == PERMISSION_ENABLE )
  {

  }
  if ( journal->option.genC == PERMISSION_ENABLE )
  {

  }
  if ( journal->option.genP == PERMISSION_ENABLE )
  {

  }
  if ( journal->option.mainsV == PERMISSION_ENABLE )
  {

  }
  if ( journal->option.mainsF == PERMISSION_ENABLE )
  {

  }
  if ( journal->option.battery == PERMISSION_ENABLE )
  {

  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void eJOURNALinit ( void )
{
  uJOURNALcalcLength( &journal );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void eJOURNALprocessing ( void )
{
  if ( journal.enb == PERMISSION_ENABLE )
  {

  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

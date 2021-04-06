/*
 * alarm.c
 *
 *  Created on: 29 окт. 2020 г.
 *      Author: Mike Mihailov
 */
/*--------------------------------- Includes ---------------------------------*/
#include "alarm.h"
#include "config.h"
#include "dataProces.h"
#include "fix16.h"
#include "common.h"
#include "fpo.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "outputData.h"
/*-------------------------------- Structures --------------------------------*/
static ACTIVE_ERROR_LIST  activeErrorList = { 0U };
static SemaphoreHandle_t  xAELsemaphore   = NULL;
/*--------------------------------- Constant ---------------------------------*/
/*-------------------------------- Variables ---------------------------------*/
static fix16_t  hysteresis = 0U;
/*-------------------------------- Functions ---------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/
uint8_t uALARMisWarning ( LOG_RECORD_TYPE record )
{
  uint8_t res = 0U;
  if ( ( record.event.action == ACTION_WARNING   ) ||
       ( record.event.action == ACTION_BAN_START ) )
  {
    res = 1U;
  }
  return res;
}
/*----------------------------------------------------------------------------*/
uint8_t uALARMisError ( LOG_RECORD_TYPE record )
{
  uint8_t res = 0U;
  if ( ( record.event.action == ACTION_EMERGENCY_STOP ) ||
       ( record.event.action == ACTION_SHUTDOWN       ) )
  {
    res = 1U;
  }
  return res;
}
/*----------------------------------------------------------------------------*/
void vALARMsetError ( SYSTEM_EVENT_TYPE event, ERROR_LIST_CMD cmd )
{
  uint8_t adr = 0U;
  uint8_t bit = 0U;
  if ( cmd == ERROR_LIST_CMD_ERASE )
  {
    outputDataReg[ERROR0_ADR]->value = 0U;
    outputDataReg[ERROR1_ADR]->value = 0U;
  }
  else
  {
    switch ( event )
    {
      case EVENT_EXTERN_EMERGENCY_STOP:
        adr = ERROR0_ADR;
        bit = ERROR_EXTERN_EMEGENCY_STOP_ADR;
        break;
      case EVENT_START_FAIL:
        adr = ERROR0_ADR;
        bit = ERROR_START_FAIL_ADR;
        break;
      case EVENT_STOP_FAIL:
        adr = ERROR0_ADR;
        bit = ERROR_STOP_FAIL_ADR;
        break;
      case EVENT_OIL_LOW_PRESSURE:
        adr = ERROR0_ADR;
        bit = ERROR_OIL_LOW_PRESSURE_ADR;
        break;
      case EVENT_OIL_SENSOR_ERROR:
        adr = ERROR0_ADR;
        bit = ERROR_OIL_SENSOR_ADR;
        break;
      case EVENT_ENGINE_HIGHT_TEMP:
        adr = ERROR0_ADR;
        bit = ERROR_TEMP_HIGHT_ADR;
        break;
      case EVENT_ENGINE_TEMP_SENSOR_ERROR:
        adr = ERROR0_ADR;
        bit = ERROR_TEMP_SENSOR_ADR;
        break;
      case EVENT_FUEL_LOW_LEVEL:
        adr = ERROR0_ADR;
        bit = ERROR_FUEL_LOW_LEVEL_ADR;
        break;
      case EVENT_FUEL_HIGHT_LEVEL:
        adr = ERROR0_ADR;
        bit = ERROR_FUEL_HIGHT_LEVEL_ADR;
        break;
      case EVENT_FUEL_LEVEL_SENSOR_ERROR:
        adr = ERROR0_ADR;
        bit = ERROR_FUEL_SENSOR_ADR;
        break;
      case EVENT_SPEED_HIGHT:
        adr = ERROR0_ADR;
        bit = ERROR_SPEED_HIGHT_ADR;
        break;
      case EVENT_SPEED_LOW:
        adr = ERROR0_ADR;
        bit = ERROR_SPEED_LOW_ADR;
        break;
      case EVENT_SPEED_SENSOR_ERROR:
        adr = ERROR0_ADR;
        bit = ERROR_SPEED_SENSOR_ADR;
        break;
      case EVENT_GENERATOR_LOW_VOLTAGE:
        adr = ERROR0_ADR;
        bit = ERROR_GEN_LOW_VOLTAGE_ADR;
        break;
      case EVENT_GENERATOR_HIGHT_VOLTAGE:
        adr = ERROR0_ADR;
        bit = ERROR_GEN_HIGHT_VOLTAGE_ADR;
        break;
      case EVENT_GENERATOR_LOW_FREQUENCY:
        adr = ERROR0_ADR;
        bit = ERROR_GEN_LOW_FREQ_ADR;
        break;
      case EVENT_GENERATOR_HIGHT_FREQUENCY:
        adr = ERROR1_ADR;
        bit = ERROR_GEN_HIGHT_FREQ_ADR;
        break;
      case EVENT_PHASE_IMBALANCE:
        adr = ERROR1_ADR;
        bit = ERROR_PHASE_IMBALANCE_ADR;
        break;
      case EVENT_OVER_CURRENT:
        adr = ERROR1_ADR;
        bit = ERROR_OVER_CURRENT_ADR;
        break;
      case EVENT_OVER_POWER:
        adr = ERROR1_ADR;
        bit = ERROR_OVER_POWER_ADR;
        break;
      case EVENT_SHORT_CIRCUIT:
        adr = ERROR1_ADR;
        bit = ERROR_SHORT_CIRCUIT_ADR;
        break;
      case EVENT_MAINTENANCE_OIL:
        adr = ERROR1_ADR;
        bit = ERROR_MAINTENANCE_OIL_ADR;
        break;
      case EVENT_MAINTENANCE_AIR:
        adr = ERROR1_ADR;
        bit = ERROR_MAINTENANCE_AIR_ADR;
        break;
      case EVENT_MAINTENANCE_FUEL:
        adr = ERROR1_ADR;
        bit = ERROR_MAINTENANCE_FUEL_ADR;
        break;
      case EVENT_INTERRUPTED_START:
        adr = ERROR1_ADR;
        bit = ERROR_ITERRUPTED_START_ADR;
        break;
      case EVENT_INTERRUPTED_STOP:
        adr = ERROR1_ADR;
        bit = ERROR_ITERRUPTED_STOP_ADR;
        break;
      case EVENT_SENSOR_COMMON_ERROR:
        adr = ERROR1_ADR;
        bit = ERROR_SENSOR_COMMON_ADR;
        break;
      case EVENT_USER_FUNCTION_A:
        adr = ERROR1_ADR;
        bit = ERROR_USER_A_ADR;
        break;
      case EVENT_USER_FUNCTION_B:
        adr = ERROR1_ADR;
        bit = ERROR_USER_B_ADR;
        break;
      case EVENT_USER_FUNCTION_C:
        adr = ERROR1_ADR;
        bit = ERROR_USER_C_ADR;
        break;
      case EVENT_USER_FUNCTION_D:
        adr = ERROR2_ADR;
        bit = ERROR_USER_D_ADR;
        break;
      case EVENT_MAINS_PHASE_SEQUENCE:
        adr = ERROR1_ADR;
        bit = ERROR_MAINS_PHASE_SEQ_ADR;
        break;
      case EVENT_GENERATOR_PHASE_SEQUENCE:
        adr = ERROR1_ADR;
        bit = ERROR_GEN_PHASE_SEQ_ADR;
        break;
      default:
        break;
    }
    if ( adr > 0U )
    {
      switch ( cmd )
      {
        case ERROR_LIST_CMD_ADD:
          setBitMap( outputDataReg[adr], bit );
          break;
        case ERROR_LIST_CMD_ACK:
          resetBitMap( outputDataReg[adr], bit );
          break;
        default:
          break;
      }
    }
  }
  return;
}
/*----------------------------------------------------------------------------*/
void vALARMsetWarning ( SYSTEM_EVENT_TYPE event, ERROR_LIST_CMD cmd )
{
  uint8_t adr = 0U;
  uint8_t bit = 0U;
  if ( cmd == ERROR_LIST_CMD_ERASE )
  {
    outputDataReg[WARNING0_ADR]->value = 0U;
  } else {
    switch ( event )
    {
      case EVENT_OIL_LOW_PRESSURE:
        adr = WARNING0_ADR;
        bit = WARNING_OIL_LOW_PRESSURE_ADR;
        break;
      case EVENT_ENGINE_HIGHT_TEMP:
        adr = WARNING0_ADR;
        bit = WARNING_TEMP_HIGHT_ADR;
        break;
      case EVENT_FUEL_LOW_LEVEL:
        adr = WARNING0_ADR;
        bit = WARNING_FUEL_LOW_LEVEL_ADR;
        break;
      case EVENT_FUEL_HIGHT_LEVEL:
        adr = WARNING0_ADR;
        bit = WARNING_FUEL_HIGHT_LEVEL_ADR;
        break;
      case EVENT_CHARGER_FAIL:
        adr = WARNING0_ADR;
        bit = WARNING_CHARGER_FAIL_ADR;
        break;
      case EVENT_BATTERY_LOW:
        adr = WARNING0_ADR;
        bit = WARNING_BATTERY_LOW_ADR;
        break;
      case EVENT_BATTERY_HIGHT:
        adr = WARNING0_ADR;
        bit = WARNING_BATTERY_HIGHT_ADR;
        break;
      case EVENT_GENERATOR_LOW_VOLTAGE:
        adr = WARNING0_ADR;
        bit = WARNING_GEN_LOW_VOLTAGE_ADR;
        break;
      case EVENT_GENERATOR_HIGHT_VOLTAGE:
        adr = WARNING0_ADR;
        bit = WARNING_GEN_HIGHT_VOLTAGE_ADR;
        break;
      case EVENT_GENERATOR_LOW_FREQUENCY:
        adr = WARNING0_ADR;
        bit = WARNING_GEN_LOW_FREQ_ADR;
        break;
      case EVENT_GENERATOR_HIGHT_FREQUENCY:
        adr = WARNING0_ADR;
        bit = WARNING_GEN_HIGHT_FREQ_ADR;
        break;
      case EVENT_OVER_CURRENT:
        adr = WARNING0_ADR;
        bit = WARNING_OVER_CURRENT_ADR;
        break;
      case EVENT_MAINS_LOW_VOLTAGE:
        adr = WARNING0_ADR;
        bit = WARNING_MAINS_LOW_VOLTAGE_ADR;
        break;
      case EVENT_MAINS_HIGHT_VOLTAGE:
        adr = WARNING0_ADR;
        bit = WARNING_MAINS_HIGHT_VOLTAGE_ADR;
        break;
      case EVENT_MAINS_LOW_FREQUENCY:
        adr = WARNING0_ADR;
        bit = WARNING_MAINS_LOW_FREQ_ADR;
        break;
      case EVENT_MAINS_HIGHT_FREQUENCY:
        adr = WARNING0_ADR;
        bit = WARNING_MAINS_HIGHT_FREQ_ADR;
        break;
      case EVENT_MAINTENANCE_OIL:
        adr = WARNING1_ADR;
        bit = WARNING_MAINTENANCE_OIL_ADR;
        break;
      case EVENT_MAINTENANCE_AIR:
        adr = WARNING1_ADR;
        bit = WARNING_MAINTENANCE_AIR_ADR;
        break;
      case EVENT_MAINTENANCE_FUEL:
        adr = WARNING1_ADR;
        bit = WARNING_MAINTENANCE_FUEL_ADR;
        break;
      default:
        break;
    }
    if ( adr > 0U )
    {
      switch ( cmd )
      {
        case ERROR_LIST_CMD_ADD:
          setBitMap( outputDataReg[adr], bit );
          break;
        case ERROR_LIST_CMD_ACK:
          resetBitMap( outputDataReg[adr], bit );
          break;
        default:
          break;
      }
    }
  }
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vALARMinit ( void )
{
  hysteresis    = fix16_div( getValue( &hysteresisLevel ), fix100U );
  xAELsemaphore = xSemaphoreCreateMutex();
  return;
}
/*----------------------------------------------------------------------------*/
void vALARMreInit ( void )
{
  hysteresis = fix16_div( getValue( &hysteresisLevel ), fix100U );
  return;
}
/*----------------------------------------------------------------------------*/
/*
 * API for active error list control
 * input:  cmd    - command for the list
 *            - ERROR_LIST_CMD_ERASE:   erase all records in the list
 *            - ERROR_LIST_CMD_ADD:     add new one record to the list
 *            - ERROR_LIST_CMD_READ:    get pointer to the start of the list
 *            - ERROR_LIST_CMD_ACK:     remove record from the list with specific address
 *            - ERROR_LIST_CMD_COUNTER: get last active address in the list
 *         record - return record with address
 *         adr    - address of the record in the list
 * output: status of the list
 *   - ERROR_LIST_STATUS_EMPTY:     no records in the list
 *   - ERROR_LIST_STATUS_NOT_EMPTY: more than 1 record in the list
 *   - ERROR_LIST_STATUS_OVER:      the list is full of records. No write
 */
ERROR_LIST_STATUS eLOGICERactiveErrorList ( ERROR_LIST_CMD cmd, LOG_RECORD_TYPE* record, uint8_t* adr )
{
  uint8_t i              = 0U;
  uint8_t warningCounter = 0U;
  uint8_t number         = 0U;
  switch ( cmd )
  {
    case ERROR_LIST_CMD_ERASE:
      if ( xSemaphoreTake( xAELsemaphore, SEMAPHORE_AEL_TAKE_DELAY ) == pdTRUE )
      {
        for ( i=0U; i<ACTIV_ERROR_LIST_SIZE; i++ )
        {
          activeErrorList.array[i].event.action = ACTION_NONE;
          activeErrorList.array[i].event.type   = EVENT_NONE;
          activeErrorList.array[i].time         = 0U;
        }
        activeErrorList.counter = 0U;
        activeErrorList.status  = ERROR_LIST_STATUS_EMPTY;
        vFPOsetWarning( RELAY_OFF );
        vALARMsetError( EVENT_NONE, cmd );
        vALARMsetWarning( EVENT_NONE, cmd );
        xSemaphoreGive( xAELsemaphore );
      }
      break;
    case ERROR_LIST_CMD_ACK:
      /*------------------- ACK -------------------*/
      if ( activeErrorList.counter > 0U )
      {
        if ( xSemaphoreTake( xAELsemaphore, SEMAPHORE_AEL_TAKE_DELAY ) == pdTRUE )
        {
          for ( i=0U; i<activeErrorList.counter; i++ )
          {
            if ( ( activeErrorList.array[i].event.type   == record->event.type   ) &&
                 ( activeErrorList.array[i].event.action == record->event.action ) )
            {
              number = i;
              break;
            }
          }
          for ( i=number; i<activeErrorList.counter; i++ )
          {
            activeErrorList.array[i] = activeErrorList.array[i + 1U];
          }
          if ( number < activeErrorList.counter )
          {
            activeErrorList.counter--;
          }
          if ( uALARMisWarning( *record ) > 0U )
          {
            vALARMsetWarning( record->event.type, cmd );
          }
          if ( uALARMisError( *record ) > 0U )
          {
            vALARMsetError( record->event.type, cmd );
          }
          /*------------- Check warnings --------------*/
          warningCounter = 0U;
          for ( i=0U; i<activeErrorList.counter; i++ )
          {
            if ( uALARMisWarning( activeErrorList.array[i] ) > 0U )
            {
              warningCounter++;
            }
          }
          /*------------- Check status ----------------*/
          if ( activeErrorList.counter > 0U )
          {
            activeErrorList.status = ERROR_LIST_STATUS_NOT_EMPTY;
            if ( warningCounter > 0U )
            {
              vFPOsetWarning( RELAY_ON );
            }
          }
          else
          {
            activeErrorList.status = ERROR_LIST_STATUS_EMPTY;
            vFPOsetWarning( RELAY_OFF );
          }
          xSemaphoreGive( xAELsemaphore );
        }
      }
      break;
    case ERROR_LIST_CMD_COUNTER:
      *adr = activeErrorList.counter;
      break;
    case ERROR_LIST_CMD_ADD:
      if ( xSemaphoreTake( xAELsemaphore, SEMAPHORE_AEL_TAKE_DELAY ) == pdTRUE )
      {
        if ( activeErrorList.counter <= ACTIV_ERROR_LIST_SIZE )
        {
          if ( uALARMisWarning( *record ) > 0U )
          {
            vFPOsetWarning( RELAY_ON );
          }
          activeErrorList.array[activeErrorList.counter] = *record;
          activeErrorList.counter++;
          activeErrorList.status = ERROR_LIST_STATUS_NOT_EMPTY;
        }
        else
        {
          activeErrorList.status = ERROR_LIST_STATUS_OVER;
        }
        if ( uALARMisWarning( *record ) > 0U )
        {
          vALARMsetWarning( record->event.type, cmd );
        }
        if ( uALARMisError( *record ) > 0U )
        {
          vALARMsetError( record->event.type, cmd );
        }
        xSemaphoreGive( xAELsemaphore );
      }
      break;
    case ERROR_LIST_CMD_READ:
      *record = activeErrorList.array[*adr];
      break;
    default:
      break;
  }
  return activeErrorList.status;
}
/*----------------------------------------------------------------------------*/
void vCONTROLLERprintActiveErrorList ( void )
{
  uint8_t         i       = 0;
  uint8_t         counter = 0U;
  LOG_RECORD_TYPE record  = { 0U };

  vSYSSerial( "------------------Active Error List------------------\r\n" );
  eLOGICERactiveErrorList( ERROR_LIST_CMD_COUNTER, &record, &counter );
  for ( i=0U; i<counter; i++ )
  {
    eLOGICERactiveErrorList( ERROR_LIST_CMD_READ, &record, &i );
    vLOGICprintLogRecord( record );
  }
  vSYSSerial( "-----------------------------------------------------\r\n" );
  return;
}
/*-----------------------------------------------------------------------------------------*/
uint8_t uALARMisForList ( SYSTEM_EVENT* event )
{
  uint8_t res = 0U;
  if ( ( event->action == ACTION_BAN_START      ) ||
       ( event->action == ACTION_WARNING        ) ||
       ( event->action == ACTION_SHUTDOWN       ) ||
       ( event->action == ACTION_EMERGENCY_STOP ) )
  {
    res = 1U;
  }
  return res;
}
/*-----------------------------------------------------------------------------------------*/
void vERRORreset ( ERROR_TYPE* error )
{
  error->trig   = TRIGGER_IDLE;
  error->status = ALARM_STATUS_IDLE;
  return;
}
/*-----------------------------------------------------------------------------------------*/
void vERRORrelax ( ERROR_TYPE* error )
{
  LOG_RECORD_TYPE rec = { 0U };
  if ( error->trig != TRIGGER_IDLE )
  {
    rec.event = error->event;
    if ( ( error->ack == PERMISSION_ENABLE ) && ( uALARMisForList( &error->event ) > 0U ) )
    {
      eLOGICERactiveErrorList( ERROR_LIST_CMD_ACK, &rec, NULL );
    }
    vERRORreset( error );
  }
  return;
}
/*-----------------------------------------------------------------------------------------*/
void vERRORtriggering ( ERROR_TYPE* error )
{
  LOG_RECORD_TYPE record   = { 0U };
  if ( error->ignor == PERMISSION_DISABLE )
  {
    vSYSeventSend( error->event, &record );
    if ( uALARMisForList( &error->event ) > 0U )
    {
      eLOGICERactiveErrorList( ERROR_LIST_CMD_ADD, &record, NULL );
    }
  }
  error->trig = TRIGGER_SET;
  return;
}
/*-----------------------------------------------------------------------------------------*/
void vERRORholding ( ERROR_TYPE* error )
{
  if ( error->trig == TRIGGER_IDLE )
  {
    error->status = ALARM_STATUS_IDLE;
  }
  return;
}
/*-----------------------------------------------------------------------------------------*/
void vALARMreset ( ALARM_TYPE* alarm )
{
  vLOGICresetTimer( &alarm->timer );
  vERRORrelax( &alarm->error );
  return;
}
/*-----------------------------------------------------------------------------------------*/
void vERRORcheck ( ERROR_TYPE* error, uint8_t flag )
{
  if ( error->enb == PERMISSION_ENABLE )
  {
    if ( error->active == PERMISSION_ENABLE )
    {
      switch ( error->status )
      {
        case ALARM_STATUS_IDLE:
          if ( flag > 0U )
          {
            vERRORtriggering( error );
            if ( error->ack == PERMISSION_ENABLE )
            {
              error->status = ALARM_STATUS_RELAX;
            }
            else
            {
              error->status = ALARM_STATUS_HOLD;
            }
          }
          break;
        case ALARM_STATUS_RELAX:
          if ( flag == 0U )
          {
            vERRORrelax( error );
          }
          break;
        case ALARM_STATUS_HOLD:
          vERRORholding( error );
          break;
        default:
          error->status = ALARM_STATUS_IDLE;
          break;
      }
    }
    else if ( error->status != ALARM_STATUS_IDLE )
    {
      error->status = ALARM_STATUS_IDLE;
    }
    else
    {

    }
  }
  return;
}
/*-----------------------------------------------------------------------------------------*/
TRIGGER_STATE eERRORisActive ( ERROR_TYPE* error )
{
  TRIGGER_STATE res = TRIGGER_IDLE;
  if ( ( error->status != ALARM_STATUS_IDLE ) &&
       ( error->status != ALARM_STATUS_WAIT_DELAY ) )
  {
    res = TRIGGER_SET;
  }
  return res;
}
/*-----------------------------------------------------------------------------------------*/
void vALARMcheck ( ALARM_TYPE* alarm, fix16_t val )
{
  fix16_t levelOff = 0U;

  if ( ( alarm->error.enb == PERMISSION_ENABLE ) && ( alarm->error.active == PERMISSION_ENABLE ) )
  {
    switch ( alarm->error.status )
    {
      /*-----------------------------------------------------------------------------------*/
      /*---------------------------------- Start condition --------------------------------*/
      /*-----------------------------------------------------------------------------------*/
      case ALARM_STATUS_IDLE:
        if ( ( ( alarm->type == ALARM_LEVEL_LOW   ) && ( val <= alarm->level ) ) ||
             ( ( alarm->type == ALARM_LEVEL_HIGHT ) && ( val >= alarm->level ) ) )
        {
          alarm->error.status = ALARM_STATUS_WAIT_DELAY;
          if ( alarm->timer.delay == 0U )
          {
            if ( alarm->error.trig == TRIGGER_IDLE )
            {
              alarm->error.status = ALARM_STATUS_TRIG;
            }
            else
            {
              alarm->error.status = ALARM_STATUS_RELAX;
            }
          }
          else
          {
            vLOGICstartTimer( &alarm->timer, "Alarm timer         " );
          }
        }
        break;
      /*-----------------------------------------------------------------------------------*/
      /*--------------------------------- Delay of trigger --------------------------------*/
      /*-----------------------------------------------------------------------------------*/
      case ALARM_STATUS_WAIT_DELAY:
        if ( uLOGICisTimer( &alarm->timer ) > 0U )
        {
          if ( alarm->error.trig == TRIGGER_IDLE )
          {
            alarm->error.status = ALARM_STATUS_TRIG;
          }
          else
          {
            alarm->error.status = ALARM_STATUS_RELAX;
          }
        }
        else if ( ( ( alarm->type == ALARM_LEVEL_LOW   ) && ( val > alarm->level ) ) ||
                  ( ( alarm->type == ALARM_LEVEL_HIGHT ) && ( val < alarm->level ) ) )
        {
          alarm->error.status = ALARM_STATUS_IDLE;
          vLOGICresetTimer( &alarm->timer );
        }
        else
        {

        }
        break;
      /*-----------------------------------------------------------------------------------*/
      /*---------------------------------- Alarm trigger ----------------------------------*/
      /*-----------------------------------------------------------------------------------*/
      case ALARM_STATUS_TRIG:
        vERRORtriggering( &alarm->error );
        if ( alarm->error.ack == PERMISSION_ENABLE )
        {
          alarm->error.status = ALARM_STATUS_RELAX;
        }
        else
        {
          alarm->error.status = ALARM_STATUS_HOLD;
        }
        break;
      /*-----------------------------------------------------------------------------------*/
      /*------------------------------- Holding alarm state -------------------------------*/
      /*-----------------------------------------------------------------------------------*/
      case ALARM_STATUS_HOLD:
        vERRORholding( &alarm->error );
        break;
      /*-----------------------------------------------------------------------------------*/
      /*----------------------------- Alarm trigger relaxation ----------------------------*/
      /*-----------------------------------------------------------------------------------*/
      case ALARM_STATUS_RELAX:
        if ( alarm->type == ALARM_LEVEL_LOW )
        {
          levelOff = fix16_mul( alarm->level, fix16_add( F16( 1U ), hysteresis ) );

          if ( val > levelOff )
          {
            vERRORrelax( &alarm->error );
          }
        }
        else
        {
          levelOff = fix16_mul( alarm->level, fix16_sub( F16( 1U ), hysteresis ) );
          if ( val < levelOff )
          {
            vERRORrelax( &alarm->error );
          }
        }
        break;
      /*-----------------------------------------------------------------------------------*/
      /*-----------------------------------------------------------------------------------*/
      /*-----------------------------------------------------------------------------------*/
      default:
        alarm->error.status = ALARM_STATUS_IDLE;
        break;
    }
  }
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

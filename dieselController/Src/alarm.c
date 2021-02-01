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
/*-------------------------------- Structures --------------------------------*/
static ACTIVE_ERROR_LIST  activeErrorList = { 0U };
static SemaphoreHandle_t  xAELsemaphore   = NULL;
/*--------------------------------- Constant ---------------------------------*/
/*-------------------------------- Variables ---------------------------------*/
static fix16_t   hysteresis = 0U;
/*-------------------------------- Functions ---------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/
uint8_t vALARMisWarning ( LOG_RECORD_TYPE record )
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
        xSemaphoreGive( xAELsemaphore );
      }
      break;
    case ERROR_LIST_CMD_ACK:
      /*------------------- ACK -------------------*/
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
        activeErrorList.counter--;
        /*------------- Check warnings --------------*/
        warningCounter = 0U;
        for ( i=0U; i<activeErrorList.counter; i++ )
        {
          if ( vALARMisWarning( activeErrorList.array[i] ) > 0U )
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
      break;
    case ERROR_LIST_CMD_COUNTER:
      *adr = activeErrorList.counter;
      break;
    case ERROR_LIST_CMD_ADD:
      if ( xSemaphoreTake( xAELsemaphore, SEMAPHORE_AEL_TAKE_DELAY ) == pdTRUE )
      {
        if ( activeErrorList.counter <= ACTIV_ERROR_LIST_SIZE )
        {
          if ( vALARMisWarning( *record ) > 0U )
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
  if ( ( event->action == ACTION_PLAN_STOP_AND_BAN_START ) ||
       ( event->action  == ACTION_BAN_START              ) ||
       ( event->action == ACTION_WARNING                 ) ||
       ( event->action == ACTION_EMERGENCY_STOP          ) )
  {
    res = 1U;
  }
  return res;
}
/*-----------------------------------------------------------------------------------------*/
void vERRORrelax ( ERROR_TYPE* error )
{
  LOG_RECORD_TYPE rec = { 0U };
  rec.event = error->event;
  if ( ( error->ack == PERMISSION_ENABLE ) && ( uALARMisForList( &error->event ) > 0U ) )
  {
    eLOGICERactiveErrorList( ERROR_LIST_CMD_ACK, &rec, NULL );
  }
  error->status = ALARM_STATUS_IDLE;
  error->trig   = TRIGGER_IDLE;
  return;
}
/*-----------------------------------------------------------------------------------------*/
void vERRORtriggering ( ERROR_TYPE* error )
{
  LOG_RECORD_TYPE record   = { 0U };
  vSYSeventSend( error->event, &record );
  if ( uALARMisForList( &error->event ) > 0U )
  {
    eLOGICERactiveErrorList( ERROR_LIST_CMD_ADD, &record, NULL );
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
void vERRORreset ( ERROR_TYPE* error )
{
  error->trig   = TRIGGER_IDLE;
  error->status = ALARM_STATUS_IDLE;
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
            vLOGICstartTimer( &alarm->timer );
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

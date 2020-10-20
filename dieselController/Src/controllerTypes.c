/*
 * controllerTypes.c
 *
 *  Created on: 4 сент. 2020 г.
 *      Author: 79110
 */
/*--------------------------------- Includes ---------------------------------*/
#include "controllerTypes.h"
#include "dataProces.h"
#include "config.h"
#include "stdio.h"
#include "journal.h"
#include "common.h"
/*-------------------------------- Structures --------------------------------*/
static StaticQueue_t      xEventQueue;
static QueueHandle_t      pEventQueue;
static SemaphoreHandle_t  xSYSTIMERsemaphore = NULL;
static ACTIVE_ERROR_LIST  activeErrorList    = { 0U };
static SemaphoreHandle_t  xAELsemaphore      = NULL;
/*--------------------------------- Constant ---------------------------------*/
const char* logActionsDictionary[LOG_ACTION_SIZE] = {
    "Нет",
    "Предупреждение",
    "Аварийная остановка",
    "Переключение на генератор",
    "Переключение на сеть",
    "Плановая остановка",
    "Отключение нагрузки"
};
const char* logTypesDictionary[LOG_TYPES_SIZE] = {
    "Нет",
    "Внешная аварийная остановка",
    "Ошибка пуска двигателя",
    "Ошибка остановки двигателя",
    "Низкое давление масла",
    "Ошибка датчика давления масла",
    "Высокая температура ОЖ",
    "Ошибка датчика температуры ОЖ",
    "Низкий уровень топлива",
    "Высокий уровень топлива",
    "Ошибка датчика топлива",
    "Высокие обороты",
    "Низкие обороты",
    "Ошибка датчика оборотов",
    "Ошибка зарядного устройства",
    "Низкое напряжение АКБ",
    "Высокое напряжение АКБ",
    "Низкое напряжение генератора",
    "Высокое напряжение генератора",
    "Низкая частота генератора",
    "Высокая частота генератора",
    "Перекос фаз",
    "Перегрузка по току",
    "Перегрузка по мощности",
    "Короткое замыкание",
    "Низкое напряжение сети",
    "Высокое напряжение сети",
    "Низкая частота сети",
    "Высокая частота сети",
    "ТО масло",
    "ТО воздух",
    "ТО топливо",
    "???",
    "???"
};
#if ( DEBUG_SERIAL_ALARM > 0U )
  const char* eventTypesStr[] =
  {
    "NONE",                       /* NONE */
    "EXTERN_EMERGENCY_STOP",      /* EMERGENCY_STOP */
    "START_FAIL",                 /* EMERGENCY_STOP */
    "STOP_FAIL",                  /* EMERGENCY_STOP */
    "OIL_LOW_PRESSURE",           /* WARNING & EMERGENCY_STOP */
    "OIL_SENSOR_ERROR",           /* EMERGENCY_STOP */
    "ENGINE_HIGHT_TEMP",          /* WARNING & EMERGENCY_STOP */
    "ENGINE_TEMP_SENSOR_ERROR",   /* EMERGENCY_STOP */
    "FUEL_LOW_LEVEL",             /* WARNING & EMERGENCY_STOP */
    "FUEL_HIGHT_LEVEL",           /* WARNING & EMERGENCY_STOP */
    "FUEL_LEVEL_SENSOR_ERROR",    /* EMERGENCY_STOP */
    "SPEED_HIGHT",                /* EMERGENCY_STOP */
    "SPEED_LOW",                  /* EMERGENCY_STOP */
    "SPEED_SENSOR_ERROR",         /* EMERGENCY_STOP */
    "CHARGER_FAIL",               /* WARNING & EMERGENCY_STOP */
    "BATTERY_LOW",                /* WARNING */
    "BATTERY_HIGHT",              /* WARNING */
    "GENERATOR_LOW_VOLTAGE",      /* WARNING & EMERGENCY_STOP */
    "GENERATOR_HIGHT_VOLTAGE",    /* WARNING & EMERGENCY_STOP */
    "GENERATOR_LOW_FREQUENCY",    /* WARNING & EMERGENCY_STOP */
    "GENERATOR_HIGHT_FREQUENCY",  /* WARNING & EMERGENCY_STOP */
    "PHASE_IMBALANCE",            /* EMERGENCY_STOP */
    "OVER_CURRENT",               /* EMERGENCY_STOP */
    "OVER_POWER",                 /* EMERGENCY_STOP */
    "SHORT_CIRCUIT",              /* EMERGENCY_STOP */
    "MAINS_LOW_VOLTAGE",          /* WARNING */
    "MAINS_HIGHT_VOLTAGE",        /* WARNING */
    "MAINS_LOW_FREQUENCY",        /* WARNING */
    "MAINS_HIGHT_FREQUENCY",      /* WARNING */
    "MAINTENANCE_OIL",            /* WARNING */
    "MAINTENANCE_AIR",            /* WARNING */
    "MAINTENANCE_FUEL",           /* WARNING */
    "MAINS_VOLTAGE_RELAX",
    "MAINS_FREQUENCY_RELAX",
  };
  const char* alarmActionStr[] =
  {
    "NONE",
    "WARNING",
    "EMERGENCY_STOP",
    "LOAD_GENERATOR",
    "LOAD_MAINS",
    "PLAN_STOP",
    "LOAD_SHUTDOWN",
  };
#endif
/*-------------------------------- Variables ---------------------------------*/
static uint8_t   eventBuffer[ EVENT_QUEUE_LENGTH * sizeof( LOG_RECORD_TYPE ) ] = { 0U };
static uint16_t  targetArray[LOGIC_COUNTERS_SIZE]                              = { 0U };
static uint16_t  counterArray[LOGIC_COUNTERS_SIZE]                             = { 0U };
static timerID_t aciveCounters                                                 = 0U;
static timerID_t activeNumber                                                  = 0U;
static fix16_t   hysteresis                                                    = 0U;
/*-------------------------------- Functions ---------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
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
  uint8_t i = 0U;
  if ( xSemaphoreTake( xAELsemaphore, SEMAPHORE_AEL_TAKE_DELAY ) == pdTRUE )
  {
    switch ( cmd )
    {
      case ERROR_LIST_CMD_ERASE:
        for ( i=0U; i<ACTIV_ERROR_LIST_SIZE; i++ )
        {
          activeErrorList.array[i].event.action = ACTION_NONE;
          activeErrorList.array[i].event.type   = EVENT_NONE;
          activeErrorList.array[i].time         = 0U;
        }
        activeErrorList.counter = 0U;
        activeErrorList.status  = ERROR_LIST_STATUS_EMPTY;
        xSemaphoreGive( xAELsemaphore );
        break;
      case ERROR_LIST_CMD_ACK:
        if ( *adr < activeErrorList.counter )
        {
          for ( i=*adr; i<activeErrorList.counter; i++ )
          {
            activeErrorList.array[i] = activeErrorList.array[i + 1U];
          }
          activeErrorList.counter--;
        }
        if ( activeErrorList.counter > 0U )
        {
          activeErrorList.status = ERROR_LIST_STATUS_NOT_EMPTY;
        }
        else
        {
          activeErrorList.status = ERROR_LIST_STATUS_EMPTY;
        }
        xSemaphoreGive( xAELsemaphore );
        break;
      case ERROR_LIST_CMD_COUNTER:
        *adr = activeErrorList.counter;
        xSemaphoreGive( xAELsemaphore );
        break;
      case ERROR_LIST_CMD_ADD:
        if ( activeErrorList.counter <= ACTIV_ERROR_LIST_SIZE )
        {
          activeErrorList.array[activeErrorList.counter] = *record;
          *adr = activeErrorList.counter;
          activeErrorList.counter++;
          activeErrorList.status = ERROR_LIST_STATUS_NOT_EMPTY;
        }
        else
        {
          activeErrorList.status = ERROR_LIST_STATUS_OVER;
        }
        xSemaphoreGive( xAELsemaphore );
        break;
      case ERROR_LIST_CMD_READ:
        *record = activeErrorList.array[*adr];
        xSemaphoreGive( xAELsemaphore );
        break;
      default:
        break;
    }
  }
  return activeErrorList.status;
}
/*----------------------------------------------------------------------------*/
void vLOGICprintTime ( uint32_t time )
{
  char buffer[21] = { 0U };
  sprintf( &buffer[0U],  "%02d.", (uint16_t)GET_LOG_DAY( time ) );
  sprintf( &buffer[3U],  "%02d.", (uint16_t)GET_LOG_MONTH( time ) );
  sprintf( &buffer[6U],  "%04d/", (uint16_t)GET_LOG_YEAR( time ) );
  sprintf( &buffer[11U], "%02d:", (uint16_t)GET_LOG_HOUR( time ) );
  sprintf( &buffer[14U], "%02d:", (uint16_t)GET_LOG_MIN( time ) );
  sprintf( &buffer[17U], "%02d",  (uint16_t)GET_LOG_SEC( time ) );
  return;
}
/*----------------------------------------------------------------------------*/
void vLOGICprintLogRecord ( LOG_RECORD_TYPE record )
{
  vSYSSerial( "Time: " );
  vLOGICprintTime( record.time );
  vSYSSerial( "Event: " );
  vSYSSerial( eventTypesStr[record.event.type] );
  vSYSSerial( "; Action: " );
  vSYSSerial( alarmActionStr[record.event.action] );
  vSYSSerial( "\r\n" );
  return;
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
/*----------------------------------------------------------------------------*/
void vLOGICprintEvent ( SYSTEM_EVENT event )
{
  #if ( DEBUG_SERIAL_ALARM > 0U )
    vSYSSerial( ">>Event: " );
    vSYSSerial( eventTypesStr[event.type] );
    vSYSSerial( "; Action: " );
    vSYSSerial( alarmActionStr[event.action] );
    vSYSSerial( "\r\n" );
  #endif
  return;
}
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vLOGICinit ( TIM_HandleTypeDef* tim )
{
  hysteresis  = fix16_div( getValue( &hysteresisLevel ), F16( 100U ) );
  pEventQueue = xQueueCreateStatic( EVENT_QUEUE_LENGTH, sizeof( LOG_RECORD_TYPE ), eventBuffer, &xEventQueue );
  HAL_TIM_Base_Start_IT( tim );
  xSYSTIMERsemaphore = xSemaphoreCreateMutex();
  xAELsemaphore      = xSemaphoreCreateMutex();
  return;
}
/*-----------------------------------------------------------------------------------------*/
QueueHandle_t pLOGICgetEventQueue ( void )
{
  return pEventQueue;
}
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
void vLOGICtimerHandler ( void )
{
  uint8_t i = 0U;

  if ( activeNumber > 0U )
  {
    for ( i=0U; i<LOGIC_COUNTERS_SIZE; i++ )
    {
      if ( ( aciveCounters & ( 1U << i ) ) > 0U )
      {
        counterArray[i]++;
      }
    }
  }
  return;
}
/*-----------------------------------------------------------------------------------------*/
TIMER_ERROR vLOGICstartTimer ( SYSTEM_TIMER* timer )
{
  TIMER_ERROR stat = TIMER_OK;
  uint16_t    inc  = ( uint16_t )( fix16_to_int( fix16_mul( timer->delay, F16( 1000U / LOGIC_TIMER_STEP ) ) ) ); /* Delay in units of 0.1 milliseconds */
  uint8_t     i    = 0U;

  if ( activeNumber < LOGIC_COUNTERS_SIZE )
  {
    for ( i=0U; i<LOGIC_COUNTERS_SIZE; i++ )
    {
      if ( ( aciveCounters & ( 1U << i ) ) == 0U )
      {
        break;
      }
    }
    timer->id = i;
    if ( xSemaphoreTake( xSYSTIMERsemaphore, SYS_TIMER_SEMAPHORE_DELAY ) == pdTRUE )
    {
      targetArray[timer->id]  = inc;
      counterArray[timer->id] = 0U;
      aciveCounters          |= 1U << timer->id;
      activeNumber++;
      xSemaphoreGive( xSYSTIMERsemaphore );
    }
    else
    {
      stat = TIMER_ACCESS;
    }
  }
  else
  {
    stat = TIMER_NO_SPACE;
  }
  return stat;
}
/*-----------------------------------------------------------------------------------------*/
TIMER_ERROR vLOGICresetTimer ( SYSTEM_TIMER timer )
{
  TIMER_ERROR stat = TIMER_OK;
  if ( xSemaphoreTake( xSYSTIMERsemaphore, SYS_TIMER_SEMAPHORE_DELAY ) == pdTRUE )
  {
    aciveCounters  &= ~( 1U << timer.id );
    targetArray[timer.id] = 0U;
    if ( activeNumber > 0U )
    {
      activeNumber--;
    }
    else
    {
      stat = TIMER_NO_SPACE;
    }
    xSemaphoreGive( xSYSTIMERsemaphore );
  }
  else
  {
    stat = TIMER_ACCESS;
  }
  return stat;
}
/*-----------------------------------------------------------------------------------------*/
uint8_t uLOGICisTimer ( SYSTEM_TIMER timer )
{
  uint8_t res = 0U;
  if ( ( ( 1U << timer.id ) & aciveCounters ) )
  {
    if ( targetArray[timer.id] <= counterArray[timer.id] )
    {
      if ( vLOGICresetTimer( timer ) == TIMER_OK )
      {
        res = 1U;
      }
    }
  }
  else
  {
    res = 1U;
  }
  return res;
}
/*-----------------------------------------------------------------------------------------*/
void vLOGICtimerCallback ( void )
{
  uint8_t i = 0U;
  for ( i=0U; i<LOGIC_COUNTERS_SIZE; i++ )
  {
    if ( ( aciveCounters & ( 1U << i ) ) > 0U )
    {
      counterArray[i]++;
    }
  }
  return;
}
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
void vSYSeventSend ( SYSTEM_EVENT event, LOG_RECORD_TYPE* record )
{
  LOG_RECORD_TYPE buffer = { 0U };

  eLOGmakeRecord( event, &buffer );
  xQueueSend( pLOGICgetEventQueue(), &buffer, portMAX_DELAY );
  *record = buffer;
  return;
}
/*-----------------------------------------------------------------------------------------*/
uint8_t uALARMisForList ( SYSTEM_EVENT* event )
{
  uint8_t res = 0U;
  if ( ( event->action == ACTION_WARNING ) || ( event->action == ACTION_EMERGENCY_STOP ) )
  {
    res = 1U;
  }
  return res;
}
/*-----------------------------------------------------------------------------------------*/
void vERRORrelax ( ERROR_TYPE* error )
{
  LOG_RECORD_TYPE record = { 0U };

  if ( error->relax.enb == PERMISSION_ENABLE )
  {
    vSYSeventSend( error->event, &record );
  }
  if ( error->ack == PERMISSION_ENABLE )
  {
    eLOGICERactiveErrorList( ERROR_LIST_CMD_ACK, NULL, &error->id );
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
    eLOGICERactiveErrorList( ERROR_LIST_CMD_ADD, &record, &error->id );
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
            if ( ( error->relax.enb == PERMISSION_ENABLE ) || ( error->ack == PERMISSION_ENABLE ) )
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
          vLOGICstartTimer( &alarm->timer );
        }
        break;
      /*-----------------------------------------------------------------------------------*/
      /*--------------------------------- Delay of trigger --------------------------------*/
      /*-----------------------------------------------------------------------------------*/
      case ALARM_STATUS_WAIT_DELAY:
        if ( uLOGICisTimer( alarm->timer ) > 0U )
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
          vLOGICresetTimer( alarm->timer );
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
        if ( ( alarm->error.relax.enb == PERMISSION_ENABLE ) || ( alarm->error.ack == PERMISSION_ENABLE ) )
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
          levelOff = fix16_mul( alarm->level, fix16_sub( F16( 1U ), hysteresis ) );
          if ( val > levelOff )
          {
            vERRORrelax( &alarm->error );
          }
        }
        else
        {
          levelOff = fix16_mul( alarm->level, fix16_add( F16( 1U ), hysteresis ) );
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
/*-----------------------------------------------------------------------------------------*/
void vRELAYautoProces ( RELAY_AUTO_DEVICE* device, fix16_t value )
{
  if ( device->relay.enb > 0U )
  {
    if ( device->onLevel < device->offLevel )
    {
      if ( device->relay.status == RELAY_OFF )
      {
        if ( ( value > device->onLevel ) && ( value < device->offLevel ) )
        {
          device->relay.set( RELAY_ON );
          device->relay.status = RELAY_ON;
        }
      }
      else
      {
        if ( ( value <= device->onLevel ) || ( value >= device->offLevel ) )
        {
          device->relay.set( RELAY_OFF );
          device->relay.status = RELAY_OFF;
        }
      }
    }
    else
    {
      if ( device->relay.status == RELAY_OFF )
      {
        if ( ( value < device->onLevel ) && ( value > device->offLevel ) )
        {
          device->relay.set( RELAY_ON );
          device->relay.status = RELAY_ON;
        }
      }
      else
      {
        if ( ( value >= device->onLevel ) || ( value <= device->offLevel ) )
        {
          device->relay.set( RELAY_OFF );
          device->relay.status = RELAY_OFF;
        }
      }
    }
  }
  return;
}
/*----------------------------------------------------------------------------*/
void vRELAYdelayTrig ( RELAY_DELAY_DEVICE* device )
{
  if ( device->triger == 0U )
  {
    device->triger = 1U;
  }
  return;
}
/*----------------------------------------------------------------------------*/
void vRELAYdelayProcess ( RELAY_DELAY_DEVICE* device )
{
  if ( device->relay.enb > 0U )
  {
    switch ( device->status )
    {
      case RELAY_DELAY_IDLE:
        if ( device->triger > 0U )
        {
          vLOGICstartTimer( &device->timer );
          device->relay.set( RELAY_ON );
          device->relay.status = RELAY_ON;
          device->status       = RELAY_DELAY_WORK;
        }
        break;
      case RELAY_DELAY_WORK:
        if ( uLOGICisTimer( device->timer ) > 0U )
        {
          device->relay.set( RELAY_OFF );
          device->relay.status = RELAY_OFF;
          device->status       = RELAY_DELAY_IDLE;
          device->triger       = 0U;
        }
        break;
      default:
        device->status = RELAY_DELAY_IDLE;
        break;
    }
  }
  return;
}
/*----------------------------------------------------------------------------*/
void vRELAYimpulseReset ( RELAY_IMPULSE_DEVICE* device )
{
  device->relay.set( RELAY_OFF );
  device->status       = RELAY_DELAY_IDLE;
  device->relay.status = RELAY_OFF;
  return;
}
/*----------------------------------------------------------------------------*/
void vRELAYimpulseProcess ( RELAY_IMPULSE_DEVICE* device, fix16_t val )
{
  if ( device->active > 0U )
  {
    if ( ( device->relay.enb > 0U ) && ( device->status != RELAY_IMPULSE_DONE ) )
    {
      if ( ( device->level >= val ) && ( device->relay.status != RELAY_ON ) )
      {
        device->relay.set( RELAY_ON );
        device->relay.status = RELAY_ON;
      }
      if ( ( device->level < val ) && ( device->relay.status != RELAY_OFF ) )
      {
        device->relay.set( RELAY_OFF );
        device->relay.status = RELAY_OFF;
      }
      if ( ( device->relay.status == RELAY_ON ) && ( device->status != RELAY_IMPULSE_START ) )
      {
        device->status = RELAY_IMPULSE_START;
        vLOGICstartTimer( &device->timer );
      }
      if ( ( device->status == RELAY_IMPULSE_START ) && ( uLOGICisTimer( device->timer ) > 0U ) )
      {
        device->status = RELAY_IMPULSE_DONE;
      }
    }
  }
  else if ( device->relay.status != RELAY_OFF )
  {
    vRELAYimpulseReset( device );
  }
  else
  {

  }
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


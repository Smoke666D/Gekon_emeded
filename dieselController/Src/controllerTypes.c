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
/*-------------------------------- Structures --------------------------------*/
static StaticQueue_t     xEventQueue;
static QueueHandle_t     pEventQueue;
static SemaphoreHandle_t xSYSTIMERsemaphore;
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
static uint8_t   eventBuffer[ EVENT_QUEUE_LENGTH * sizeof( SYSTEM_EVENT ) ] = { 0U };
static uint16_t  targetArray[LOGIC_COUNTERS_SIZE]                           = { 0U };
static uint16_t  counterArray[LOGIC_COUNTERS_SIZE]                          = { 0U };
static timerID_t aciveCounters                                              = 0U;
static timerID_t activeNumber                                               = 0U;
static fix16_t   hysteresis                                                 = 0U;
/*-------------------------------- Functions ---------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
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
/*----------------------------------------------------------------------------*/
void vLOGICinit ( TIM_HandleTypeDef* tim )
{
  hysteresis  = fix16_div( getValue( &hysteresisLevel ), F16( 100U ) );
  pEventQueue = xQueueCreateStatic( EVENT_QUEUE_LENGTH, sizeof( SYSTEM_EVENT ), eventBuffer, &xEventQueue );
  HAL_TIM_Base_Start_IT( tim );
  xSYSTIMERsemaphore = xSemaphoreCreateMutex();
  return;
}
/*-----------------------------------------------------------------------------------------*/
QueueHandle_t pLOGICgetEventQueue ( void )
{
  return pEventQueue;
}
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
TIMER_ERROR vLOGICstartTimer ( fix16_t delay, timerID_t* id )
{
  TIMER_ERROR stat = TIMER_OK;
  uint16_t    inc  = ( uint16_t )( fix16_to_int( fix16_mul( delay, F16( 1000U / LOGIC_TIMER_STEP ) ) ) ); /* Delay in units of 0.1 milliseconds */
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
    *id = i;
    if ( xSemaphoreTake( xSYSTIMERsemaphore, SYS_TIMER_SEMAPHORE_DELAY ) == pdTRUE )
    {
      targetArray[*id]  = inc;
      counterArray[*id] = 0U;
      aciveCounters    |= 1U << *id;
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
TIMER_ERROR vLOGICresetTimer ( timerID_t id )
{
  TIMER_ERROR stat = TIMER_OK;
  if ( xSemaphoreTake( xSYSTIMERsemaphore, SYS_TIMER_SEMAPHORE_DELAY ) == pdTRUE )
  {
    aciveCounters  &= ~( 1U << id );
    targetArray[id] = 0U;
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
uint8_t uLOGICisTimer ( timerID_t id )
{
  uint8_t res = 0U;
  if ( targetArray[id] <= counterArray[id] )
  {
    if ( vLOGICresetTimer( id ) == TIMER_OK )
    {
      res = 1U;
    }
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
void vLOGICtoogle ( uint8_t* input )
{
  if ( *input > 0U )
  {
    *input = 0U;
  }
  else
  {
    *input = 1U;
  }
  return;
}
/*-----------------------------------------------------------------------------------------*/
void vALARMcheck ( ALARM_TYPE* alarm, fix16_t val, QueueHandle_t queue )
{
  fix16_t levelOff = 0U;

  if ( ( alarm->enb > 0U ) && ( alarm->active > 0U ) )
  {
    switch ( alarm->status )
    {
      case ALARM_STATUS_IDLE:
        if ( ( ( alarm->type == ALARM_LEVEL_LOW   ) && ( val <= alarm->level ) ) ||
             ( ( alarm->type == ALARM_LEVEL_HIGHT ) && ( val >= alarm->level ) ) )
        {
          alarm->status = ALARM_STATUS_WAIT_DELAY;
          vLOGICstartTimer( alarm->delay, &alarm->timerID );
        }
        break;
      case ALARM_STATUS_WAIT_DELAY:
        if ( ( ( alarm->type == ALARM_LEVEL_LOW   ) && ( val > alarm->level ) ) ||
             ( ( alarm->type == ALARM_LEVEL_HIGHT ) && ( val < alarm->level ) ) )
        {
          alarm->status = ALARM_STATUS_IDLE;
          vLOGICresetTimer( alarm->timerID );
        }
        else if ( uLOGICisTimer( alarm->timerID ) > 0U )
        {
          if ( alarm->trig == 0U )
          {
            alarm->status = ALARM_STATUS_TRIG;
          }
        }
        else
        {

        }
        break;
      case ALARM_STATUS_TRIG:
        if ( queue != NULL )
        {
          xQueueSend( queue, &alarm->event, portMAX_DELAY );
        }
        alarm->trig   = 1U;
        alarm->status = ALARM_STATUS_RELAX;
        break;
      case ALARM_STATUS_RELAX:
        if ( alarm->type == ALARM_LEVEL_LOW )
        {
          levelOff = fix16_mul( alarm->level, fix16_sub( F16( 1U ), hysteresis ) );
          if ( val > levelOff )
          {
            alarm->status = ALARM_STATUS_IDLE;
            if ( alarm->relax.enb > 0U )
            {
              xQueueSend( queue, &alarm->relax.event, portMAX_DELAY );
            }
          }
        }
        else
        {
          levelOff = fix16_mul( alarm->level, fix16_add( F16( 1U ), hysteresis ) );
          if ( val < levelOff )
          {
            alarm->status = ALARM_STATUS_IDLE;
            if ( alarm->relax.enb > 0U )
            {
              xQueueSend( queue, &alarm->relax.event, portMAX_DELAY );
            }
          }
        }
        break;
      default:
        alarm->status = ALARM_STATUS_IDLE;
        break;
    }
  }
  return;
}

void vRELAYproces ( RELAY_AUTO_DEVICE* device, fix16_t value )
{
  float fVal = fix16_to_float( value );
  float fOff = fix16_to_float( device->offLevel );
  float fOn  = fix16_to_float( device->onLevel );
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
    if ( ( device->relay.enb > 0U ) && ( device->status != RELAY_DELAY_DONE ) )
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
      if ( ( device->relay.status == RELAY_ON ) && ( device->status != RELAY_DELAY_START ) )
      {
        device->status = RELAY_DELAY_START;
        vLOGICstartTimer( device->delay, &device->timerID );
      }
      if ( ( device->status == RELAY_DELAY_START ) && ( uLOGICisTimer( device->timerID ) > 0U ) )
      {
        device->status = RELAY_DELAY_DONE;
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


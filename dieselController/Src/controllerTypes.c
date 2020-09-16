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
static StaticQueue_t xEventQueue;
static QueueHandle_t pEventQueue;
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
void vLOGICinit ( void )
{
  hysteresis  = fix16_div( getValue( &hysteresisLevel ), F16( 100U ) );
  pEventQueue = xQueueCreateStatic( EVENT_QUEUE_LENGTH, sizeof( SYSTEM_EVENT ), eventBuffer, &xEventQueue );
}

QueueHandle_t pLOGICgetEventQueue ( void )
{
  return pEventQueue;
}

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
    aciveCounters &= 1U << *id;
    activeNumber++;
    targetArray[*id]  = inc;
    counterArray[*id] = 0U;
  }
  else
  {
    stat = TIMER_NO_SPACE;
  }
  return stat;
}

uint8_t uLOGICisTimer ( timerID_t id )
{
  uint8_t res = 0U;
  if ( targetArray[id] <= counterArray[id] )
  {
    res = 1U;
    aciveCounters |= ~( 1U << id );
    activeNumber--;
  }
  return res;
}

void vLOGICresetTimer ( timerID_t id )
{
  aciveCounters |= ~( 1U << id );
  activeNumber--;
  return;
}

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
          alarm->status = ALARM_STATUS_TRIG;
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

void vRELAYproces ( RELAY_AUTO_DEVICE* device, fix16_t val )
{
  if ( device->relay.enb > 0U )
  {
    if ( ( val < device->offLevel ) && ( device->relay.status != RELAY_OFF ) )
    {
      device->relay.set( RELAY_OFF );
      device->relay.status = RELAY_OFF;
    }
    if ( ( val > device->onLevel ) && ( device->relay.status != RELAY_ON ) )
    {
      device->relay.set( RELAY_ON );
      device->relay.status = RELAY_ON;
    }
  }
  return;
}

void vRELAYimpulseProcess ( RELAY_IMPULSE_DEVICE* device, fix16_t val )
{
  if ( device->active > 0U )
  {
    if ( ( device->relay.enb > 0U ) && ( device->status != RELAY_DELAY_DONE ) )
    {
      if ( ( device->level > val ) && ( device->relay.status != RELAY_ON ) )
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
  return;
}
/*----------------------------------------------------------------------------*/
void vRELAYimpulseReset ( RELAY_IMPULSE_DEVICE* device )
{
  device->status = RELAY_DELAY_IDLE;
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


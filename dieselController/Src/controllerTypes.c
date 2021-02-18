/*
 * controllerTypes.c
 *
 *  Created on: 4 сент. 2020 г.
 *      Author: Mike Mihailov
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
/*--------------------------------- Constant ---------------------------------*/
const fix16_t fix100U = F16( 100U );
const char* logActionsDictionary[LOG_ACTION_SIZE] = {
    "Нет",
    "Предупреждение",
    "Аварийная остановка",
    "Отключение",
    "Плановая остановка",
    "Остановка до устранения ошибки",
    "Запрет следующего старта",
    "Автостарт",
    "Автостоп"
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
    "Двигатель запущен",
    "Двигатель остановлен",
    "Сеть востановлена",
    "Ошибка сети",
    "Прерванный старт",
    "Прерванная остановка",
    "Ошибка общего провода датчиков"
};
#if ( DEBUG_SERIAL_ALARM > 0U )
  const char* eventTypesStr[LOG_TYPES_SIZE] =
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
    "MAINS_ENGINE_START",
    "MAINS_ENGINE_STOP",
    "MAINS_OK",
    "MAINS_FAIL",
    "INTERRUPTED_START",
    "INTERRUPTED_STOP",
    "SENSOR_COMMON_ERROR"
  };
  const char* alarmActionStr[LOG_ACTION_SIZE] =
  {
    "NONE",
    "WARNING",
    "EMERGENCY_STOP",
    "SHUTDOWN",
    "PLAN_STOP",
    "PLAN_STOP_AND_BAN_START",
    "BAN_START",
    "AUTO_START",
    "AUTO_STOP"
  };
#endif
/*-------------------------------- Variables ---------------------------------*/
static uint8_t   eventBuffer[EVENT_QUEUE_LENGTH * sizeof( LOG_RECORD_TYPE )] = { 0U };
static uint16_t  targetArray[LOGIC_COUNTERS_SIZE]                            = { 0U };
static uint16_t  counterArray[LOGIC_COUNTERS_SIZE]                           = { 0U };
static timerID_t aciveCounters                                               = 0U;
static timerID_t activeNumber                                                = 0U;
#ifdef DEBUG
static char timerNames[LOGIC_COUNTERS_SIZE][TIMER_NAME_LENGTH] = { 0U };
#endif
/*-------------------------------- Functions ---------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
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
  pEventQueue = xQueueCreateStatic( EVENT_QUEUE_LENGTH, sizeof( LOG_RECORD_TYPE ), eventBuffer, &xEventQueue );
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
uint8_t uLOGICisTimerActive ( SYSTEM_TIMER timer )
{
  uint8_t res = 0U;
  if ( timer.id < LOGIC_COUNTERS_SIZE )
  {
    res = 1U;
  }
  return res;
}
/*-----------------------------------------------------------------------------------------*/
TIMER_ERROR vLOGICstartTimer ( SYSTEM_TIMER* timer, char* name )
{
  TIMER_ERROR stat         = TIMER_OK;
  uint16_t    inc          = 0U;
  uint8_t     i            = 0U;
  uint8_t     j            = 0U;
  uint8_t     alreadyExist = 0U;

  if ( activeNumber < LOGIC_COUNTERS_SIZE )
  {
    inc = ( uint16_t )( fix16_to_int( fix16_mul( timer->delay, fix16_from_float( 1000U / LOGIC_TIMER_STEP ) ) ) ); /* Delay in units of 0.1 milliseconds */
    /*-----------------------------------------*/
    if ( timer->id > LOGIC_COUNTERS_SIZE )
    {
      for ( i=0U; i<LOGIC_COUNTERS_SIZE; i++ )
      {
        if ( ( aciveCounters & ( 1U << i ) ) == 0U )
        {
          break;
        }
      }
      timer->id = i;
    }
    else
    {
      alreadyExist = 1U;
    }
    /*-----------------------------------------*/
    if ( xSemaphoreTake( xSYSTIMERsemaphore, SYS_TIMER_SEMAPHORE_DELAY ) == pdTRUE )
    {
      #ifdef DEBUG
        for ( j=0U; j<TIMER_NAME_LENGTH; j++ )
        {
          timerNames[i][j] = name[j];
        }
        timerNames[i][TIMER_NAME_LENGTH - 1U] = 0x00U;
      #endif
      targetArray[timer->id]  = inc;
      counterArray[timer->id] = 0U;
      if ( alreadyExist == 0U )
      {
        aciveCounters          |= 1U << timer->id;
        activeNumber++;
      }
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
TIMER_ERROR vLOGICresetTimer ( SYSTEM_TIMER* timer ) /* Проверить стоит ли сделать указатель */
{
  TIMER_ERROR stat = TIMER_OK;
  if ( timer->id <= LOGIC_COUNTERS_SIZE )
  {
    if ( xSemaphoreTake( xSYSTIMERsemaphore, SYS_TIMER_SEMAPHORE_DELAY ) == pdTRUE )
    {
      aciveCounters         &= ~( 1U << timer->id );
      targetArray[timer->id]  = 0U;
      if ( activeNumber > 0U )
      {
        activeNumber--;
      }
      else
      {
        stat = TIMER_NO_SPACE;
      }
      timer->id = LOGIC_DEFAULT_TIMER_ID;
      xSemaphoreGive( xSYSTIMERsemaphore );
    }
    else
    {
      stat = TIMER_ACCESS;
    }
  }
  return stat;
}
/*-----------------------------------------------------------------------------------------*/
void vLOGICresetAllTimers ( void )
{
  uint8_t i = 0U;
  aciveCounters = 0U;
  activeNumber  = 0U;
  for ( i=0U; i<LOGIC_COUNTERS_SIZE; i++ )
  {
    targetArray[i]  = 0U;
    counterArray[i] = 0U;
  }
  return;
}
/*-----------------------------------------------------------------------------------------*/
uint8_t uLOGICisTimer ( SYSTEM_TIMER* timer )
{
  uint8_t res = 0U;
  if ( ( ( 1U << timer->id ) & aciveCounters ) )
  {
    if ( targetArray[timer->id] <= counterArray[timer->id] )
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
void vLOGICprintActiveTimers ( void )
{
  uint8_t i          = 0U;
  char    buffer[3U] = { 0U };
  #ifdef DEBUG
  if ( ( activeNumber == 0U ) && ( aciveCounters == 0U ) )
  {
    vLOGICprintDebug( ">>************************\r\n" );
    vLOGICprintDebug( ">>No active timers \r\n" );
    vLOGICprintDebug( ">>************************\r\n" );
  }
  else
  {
    vLOGICprintDebug( ">>************************\r\n" );
    sprintf( buffer,  "%d", activeNumber );
    vLOGICprintDebug( ">>There are " );
    vLOGICprintDebug( buffer );
    vLOGICprintDebug( " active timers\r\n" );
    for ( i=0U; i<LOGIC_COUNTERS_SIZE; i++ )
    {
      if ( ( aciveCounters & ( 1U << i ) ) > 0U )
      {
        vLOGICprintDebug( ">>" );
        vLOGICprintDebug( timerNames[i] );
        vLOGICprintDebug( "\r\n" );
      }
    }
    vLOGICprintDebug( ">>************************\r\n" );
  }
  #endif
  return;
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
void vRELAYset ( RELAY_DEVICE* relay, RELAY_STATUS status )
{
  if ( relay->enb == PERMISSION_ENABLE )
  {
    relay->set( status );
    relay->status = status;
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
          vLOGICstartTimer( &device->timer, "Relay device        " );
          device->relay.set( RELAY_ON );
          device->relay.status = RELAY_ON;
          device->status       = RELAY_DELAY_WORK;
        }
        break;
      case RELAY_DELAY_WORK:
        if ( uLOGICisTimer( &device->timer ) > 0U )
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
        vLOGICstartTimer( &device->timer, "Relay device        " );
      }
      if ( ( device->status == RELAY_IMPULSE_START ) && ( uLOGICisTimer( &device->timer ) > 0U ) )
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

void vLOGICprintDebug ( const char* str )
{
  #if ( DEBUG_SERIAL_STATUS > 0U )
    vSYSSerial( str );
  #endif
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


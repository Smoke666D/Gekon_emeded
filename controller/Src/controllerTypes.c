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
static DEVICE_INFO        deviceInfo         = { 0U };
/*---------------------------------- Define ----------------------------------*/

/*--------------------------------- Constant ---------------------------------*/
const char* const logActionsDictionary[LOG_ACTION_SIZE] = {
    "Нет",                            /* 0 */
    "Предупреждение",                 /* 1 */
    "Аварийная остановка",            /* 2 */
    "Отключение",                     /* 3 */
    "Плановая остановка",             /* 4 */
    "Остановка до устранения ошибки", /* 5 */
    "Запрет следующего старта",       /* 6 */
    "Автостарт",                      /* 7 */
    "Автостоп"                        /* 8 */
};
const char* const logTypesDictionary[LOG_TYPES_SIZE] = {
    "Нет",                               /* 0  */
    "Внешная аварийная остановка",       /* 1  */
    "Ошибка пуска двигателя",            /* 2  */
    "Ошибка остановки двигателя",        /* 3  */
    "Низкое давление масла",             /* 4  */
    "Ошибка датчика давления масла",     /* 5  */
    "Высокая температура ОЖ",            /* 6  */
    "Ошибка датчика температуры ОЖ",     /* 7  */
    "Низкий уровень топлива",            /* 8  */
    "Высокий уровень топлива",           /* 9  */
    "Ошибка датчика топлива",            /* 10 */
    "Высокие обороты",                   /* 11 */
    "Низкие обороты",                    /* 12 */
    "Ошибка датчика оборотов",           /* 13 */
    "Ошибка зарядного устройства",       /* 14 */
    "Низкое напряжение АКБ",             /* 15 */
    "Высокое напряжение АКБ",            /* 16 */
    "Низкое напряжение генератора",      /* 17 */
    "Высокое напряжение генератора",     /* 18 */
    "Низкая частота генератора",         /* 19 */
    "Высокая частота генератора",        /* 20 */
    "Перекос фаз",                       /* 21 */
    "Перегрузка по току",                /* 22 */
    "Перегрузка по мощности",            /* 23 */
    "Короткое замыкание",                /* 24 */
    "Низкое напряжение сети",            /* 25 */
    "Высокое напряжение сети",           /* 26 */
    "Низкая частота сети",               /* 27 */
    "Высокая частота сети",              /* 28 */
    "ТО масло",                          /* 29 */
    "ТО воздух",                         /* 30 */
    "ТО топливо",                        /* 31 */
    "Двигатель запущен",                 /* 32 */
    "Двигатель остановлен",              /* 33 */
    "Сеть востановлена",                 /* 34 */
    "Ошибка сети",                       /* 35 */
    "Прерванный старт",                  /* 36 */
    "Прерванная остановка",              /* 37 */
    "Ошибка общего провода датчиков",    /* 38 */
    "Пользователькое событие А",         /* 39 */
    "Пользователькое событие B",         /* 40 */
    "Пользователькое событие C",         /* 41 */
    "Пользователькое событие D",         /* 42 */
    "Ошибка чередования фаз сети",       /* 43 */
    "Ошибка чередования фаз генератора", /* 44 */
    "Утечка топлива"                     /* 45 */
};
#if ( DEBUG_SERIAL_ALARM > 0U )
  const char* const eventTypesStr[LOG_TYPES_SIZE] =
  {
    "NONE",                       /* 00 NONE */
    "EXTERN_EMERGENCY_STOP",      /* 01 EMERGENCY_STOP */
    "START_FAIL",                 /* 02 EMERGENCY_STOP */
    "STOP_FAIL",                  /* 03 EMERGENCY_STOP */
    "OIL_LOW_PRESSURE",           /* 04 WARNING & EMERGENCY_STOP */
    "OIL_SENSOR_ERROR",           /* 05 EMERGENCY_STOP */
    "ENGINE_HIGHT_TEMP",          /* 06 WARNING & EMERGENCY_STOP */
    "ENGINE_TEMP_SENSOR_ERROR",   /* 07 EMERGENCY_STOP */
    "FUEL_LOW_LEVEL",             /* 08 WARNING & EMERGENCY_STOP */
    "FUEL_HIGHT_LEVEL",           /* 09 WARNING & EMERGENCY_STOP */
    "FUEL_LEVEL_SENSOR_ERROR",    /* 10 EMERGENCY_STOP */
    "SPEED_HIGHT",                /* 11 EMERGENCY_STOP */
    "SPEED_LOW",                  /* 12 EMERGENCY_STOP */
    "SPEED_SENSOR_ERROR",         /* 13 EMERGENCY_STOP */
    "CHARGER_FAIL",               /* 14 WARNING */
    "BATTERY_LOW",                /* 15 WARNING */
    "BATTERY_HIGHT",              /* 16 WARNING */
    "GENERATOR_LOW_VOLTAGE",      /* 17 WARNING & EMERGENCY_STOP */
    "GENERATOR_HIGHT_VOLTAGE",    /* 18 WARNING & EMERGENCY_STOP */
    "GENERATOR_LOW_FREQUENCY",    /* 19 WARNING & EMERGENCY_STOP */
    "GENERATOR_HIGHT_FREQUENCY",  /* 20 WARNING & EMERGENCY_STOP */
    "PHASE_IMBALANCE",            /* 21 EMERGENCY_STOP */
    "OVER_CURRENT",               /* 22 EMERGENCY_STOP */
    "OVER_POWER",                 /* 23 EMERGENCY_STOP */
    "SHORT_CIRCUIT",              /* 24 EMERGENCY_STOP */
    "MAINS_LOW_VOLTAGE",          /* 25 WARNING */
    "MAINS_HIGHT_VOLTAGE",        /* 26 WARNING */
    "MAINS_LOW_FREQUENCY",        /* 27 WARNING */
    "MAINS_HIGHT_FREQUENCY",      /* 28 WARNING */
    "MAINTENANCE_OIL",            /* 29 WARNING */
    "MAINTENANCE_AIR",            /* 30 WARNING */
    "MAINTENANCE_FUEL",           /* 31 WARNING */
    "MAINS_ENGINE_START",         /* 32 */
    "MAINS_ENGINE_STOP",          /* 33 */
    "MAINS_OK",                   /* 34 */
    "MAINS_FAIL",                 /* 35 */
    "INTERRUPTED_START",          /* 36 */
    "INTERRUPTED_STOP",           /* 37 */
    "SENSOR_COMMON_ERROR",        /* 38 */
    "USER_FUNCTION_A",            /* 39 */
    "USER_FUNCTION_B",            /* 40 */
    "USER_FUNCTION_C",            /* 41 */
    "USER_FUNCTION_D",            /* 42 */
    "MAINS_PHASE_SEQUENCE",       /* 43 */
    "GENERATOR_PHASE_SEQUENCE",   /* 44 */
    "FUEL_LEAK"                   /* 45 */
  };
  const char* const alarmActionStr[LOG_ACTION_SIZE] =
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

static const char* const deviceStatusDic[DEVICE_STATUS_NUMBER] =
{
  "Загрузка...",        /* 00 */
  "Готов к запуску",    /* 01 */
  "Дистанционный пуск", /* 02 */
  "Предпрогрев",        /* 03 */
  "Работа стартера",    /* 04 */
  "Пауза стартера",     /* 05 */
  "Возбуждение",        /* 06 */
  "Прогрев Х.Х.",       /* 07 */
  "Прогрев",            /* 08 */
  "В работе",           /* 09 */
  "Охлаждение",         /* 10 */
  "Охлаждение Х.Х.",    /* 11 */
  "Останов",            /* 12 */
  "Аварийный останов",  /* 13 */
  "Запрет пуска"        /* 14 */
};
/*-------------------------------- Variables ---------------------------------*/
static uint8_t   eventBuffer[EVENT_QUEUE_LENGTH * sizeof( LOG_RECORD_TYPE )] = { 0U };
static uint16_t  targetArray[LOGIC_COUNTERS_SIZE]                            = { 0U };
static uint16_t  counterArray[LOGIC_COUNTERS_SIZE]                           = { 0U };
static timerID_t aciveCounters                                               = 0U;
static timerID_t activeNumber                                                = 0U;
#if defined( DEBUG )
  static char timerNames[LOGIC_COUNTERS_SIZE][TIMER_NAME_LENGTH] = { 0U };
#endif
/*-------------------------------- Functions ---------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/
PERMISSION eSTATUSisTimer ( DEVICE_STATUS status )
{
  PERMISSION res = PERMISSION_ENABLE;
  if ( ( status == DEVICE_STATUS_IDLE           ) ||
       ( status == DEVICE_STATUS_READY_TO_START ) ||
       ( status == DEVICE_STATUS_WORKING        ) ||
       ( status == DEVICE_STATUS_ERROR          ) ||
       ( status == DEVICE_STATUS_BAN_START      ) )
  {
    res = PERMISSION_DISABLE;
  }
  return res;
}
/*----------------------------------------------------------------------------*/
void vSTATUScalcTime ( void )
{
  if ( deviceInfo.timerID < LOGIC_COUNTERS_SIZE )
  {
    deviceInfo.time = ( uint16_t )( ( targetArray[deviceInfo.timerID] - counterArray[deviceInfo.timerID] ) / 10U ); /* sec */
  }
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
const char* cSTATUSgetString ( DEVICE_STATUS status )
{
  return deviceStatusDic[status];
}
/*----------------------------------------------------------------------------*/
DEVICE_STATUS eSTATUSgetStatus ( void )
{
  return deviceInfo.status;
}
/*----------------------------------------------------------------------------*/
void vSTATUSget ( DEVICE_INFO* info )
{
  vSTATUScalcTime();
  *info = deviceInfo;
  return;
}
/*----------------------------------------------------------------------------*/
void vSTATUSsetup ( DEVICE_STATUS status, timerID_t id )
{
  deviceInfo.status  = status;
  deviceInfo.timer   = eSTATUSisTimer( status );
  deviceInfo.timerID = id;
  vSTATUScalcTime();
  return;
}
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*----------------------------------  ------------------------------------------*/
void vLOGICinit ( TIM_HandleTypeDef* tim )
{
  pEventQueue = xQueueCreateStatic( EVENT_QUEUE_LENGTH, sizeof( LOG_RECORD_TYPE ), eventBuffer, &xEventQueue );
  HAL_TIM_Base_Start_IT( tim );
  xSYSTIMERsemaphore = xSemaphoreCreateMutex();
  deviceInfo.timerID = LOGIC_DEFAULT_TIMER_ID;
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
  if ( activeNumber > 0U )
  {
    for ( uint8_t i=0U; i<LOGIC_COUNTERS_SIZE; i++ )
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
  if ( uLOGICisTimerActive( *timer ) > 0U )
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
  aciveCounters = 0U;
  activeNumber  = 0U;
  for ( uint8_t i=0U; i<LOGIC_COUNTERS_SIZE; i++ )
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
  if ( ( ( 1U << timer->id ) & aciveCounters ) > 0U )
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
void vLOGICtimerCallback ( void )
{
  for ( uint8_t i=0U; i<LOGIC_COUNTERS_SIZE; i++ )
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
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


/*
 * fpi.c
 *
 *  Created on: 12 июн. 2020 г.
 *      Author: photo_Mickey
 */
/*--------------------------------- Includes ---------------------------------*/
#include "fpi.h"
#include "common.h"
#include "stm32f2xx_hal_gpio.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
#include "config.h"
#include "dataProces.h"
#include "engine.h"
#include "controllerTypes.h"
#include "dataAPI.h"
#include "alarm.h"
#include "system.h"
/*-------------------------------- Structures --------------------------------*/
static QueueHandle_t     pFPIQueue     = NULL;
static StaticQueue_t     xFPIQueue     = { 0U };
static SemaphoreHandle_t xFPIsemaphore = NULL;
/*--------------------------------- Constant ---------------------------------*/
static const FPI_FUNCTION eFPIfuncList[FPI_FUNCTION_NUM] =
{
  FPI_FUN_NONE,              /* 00 */
  FPI_FUN_USER,              /* 01 */
  FPI_FUN_ALARM_RESET,       /* 02 */
  FPI_FUN_OIL_LOW_PRESSURE,  /* 03 */
  FPI_FUN_HIGHT_ENGINE_TEMP, /* 04 */
  FPI_FUN_LOW_FUEL,          /* 05 */
  FPI_FUN_REMOTE_START,      /* 06 */
  FPI_FUN_IDLING,            /* 07 */
  FPI_FUN_BAN_AUTO_START,    /* 08 */
  FPI_FUN_BAN_GEN_LOAD,      /* 09 */
  FPI_FUN_BAN_AUTO_SHUTDOWN, /* 10 */
  FPI_FUN_EMERGENCY_STOP     /* 11 */
};
static const SYSTEM_EVENT_TYPE eFPIuserEventTypeList[FPI_NUMBER] =
{
  EVENT_USER_FUNCTION_A, /* 0 */
  EVENT_USER_FUNCTION_B, /* 1 */
  EVENT_USER_FUNCTION_C, /* 2 */
  EVENT_USER_FUNCTION_D, /* 3 */
};
static const eConfigReg* const pFPIregConfig[FPI_NUMBER] =
{
  &diaSetup, /* 0 */
  &dibSetup, /* 1 */
  &dicSetup, /* 2 */
  &didSetup  /* 3 */
};
static const eConfigReg* const pFPIregDelay[FPI_NUMBER] =
{
  &diaDelay, /* 0 */
  &dibDelay, /* 1 */
  &dicDelay, /* 2 */
  &didDelay, /* 3 */
};
/*-------------------------------- Variables ---------------------------------*/
static uint8_t  eventBuffer[ 16U * sizeof( FPI_EVENT ) ] = { 0U };
static FPI      fpis[FPI_NUMBER]                         = { 0U };
static uint16_t fpiDataReg                               = 0U;
static uint16_t fpiRawDataReg                            = 0U;
/*-------------------------------- External -----------------------------------*/
osThreadId_t fpiHandle = NULL;
/*-------------------------------- Functions ---------------------------------*/
void vFPITask ( void* argument );
/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/
TRIGGER_STATE uFPIarmingAlways ( void )
{
  return TRIGGER_SET;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE uFPIarmingNever ( void )
{
  return TRIGGER_IDLE;
}
/*----------------------------------------------------------------------------*/
FPI_LEVEL eFPIgetLevel ( const FPI* fpi )
{
  GPIO_PinState pinState = GPIO_PIN_RESET;
  FPI_LEVEL     res      = FPI_LEVEL_LOW;
  pinState = HAL_GPIO_ReadPin ( fpi->port, fpi->pin );
  if ( ( ( fpi->polarity == FPI_POL_NORMAL_OPEN  ) && ( pinState == GPIO_PIN_RESET ) ) ||
       ( ( fpi->polarity == FPI_POL_NORMAL_CLOSE ) && ( pinState == GPIO_PIN_SET   ) ) )
  {
    res = FPI_LEVEL_HIGH;
  }
  return res;
}
/*----------------------------------------------------------------------------*/
/*
 * Get hardware pin level and convert it to logical level
 * input:  fpi - Freely Programmable Input structure
 * output: level of fpi
 */
TRIGGER_STATE eFPIgetTrig ( FPI* fpi )
{
  TRIGGER_STATE res = TRIGGER_IDLE;
  if ( fpi->port != NULL )
  {
    if ( eFPIgetLevel( fpi ) == FPI_LEVEL_HIGH )
    {
      res = TRIGGER_SET;
    }
    if ( ( fpi->level == FPI_LEVEL_LOW ) && ( res == TRIGGER_SET ) )
    {
      fpi->level = FPI_LEVEL_HIGH;
    }
    else if ( ( fpi->level == FPI_LEVEL_HIGH ) && ( res == TRIGGER_SET ) )
    {
      res = TRIGGER_IDLE;
    }
    else if ( ( fpi->level == FPI_LEVEL_HIGH ) && ( res == TRIGGER_IDLE ) )
    {
      fpi->level = FPI_LEVEL_LOW;
      res        = TRIGGER_SET;
    }
    else
    {

    }
  }
  return res;
}
/*----------------------------------------------------------------------------*/
void vFPIcheckReset ( FPI* fpi )
{
  if ( eFPIgetTrig( fpi ) == TRIGGER_IDLE )
  {
    vLOGICresetTimer( &fpi->timer );
    fpi->state = FPI_IDLE;
  }
  return;
}
/*----------------------------------------------------------------------------*/
void vFPIdataInit ( void )
{
  FPI_ACTION input = FPI_ACT_NONE;
  for ( uint8_t i=0U; i<FPI_NUMBER; i++ )
    {
      fpis[i].timer.delay = getValue( pFPIregDelay[i] );
      fpis[i].function    = eFPIfuncList[ getBitMap( pFPIregConfig[i], DIA_FUNCTION_ADR ) ];
      fpis[i].polarity    = getBitMap( pFPIregConfig[i], DIA_POLARITY_ADR );
      fpis[i].arming      = getBitMap( pFPIregConfig[i], DIA_ARMING_ADR );
      input = getBitMap( pFPIregConfig[i], DIA_ACTION_ADR );
      switch ( input )
      {
        case FPI_ACT_EMERGENCY_STOP:
          fpis[i].userError.event.action = ACTION_EMERGENCY_STOP;
          break;
        case FPI_ACT_SHUTDOWN:
          fpis[i].userError.event.action = ACTION_SHUTDOWN;
          break;
        case FPI_ACT_WARNING:
          fpis[i].userError.event.action = ACTION_WARNING;
          break;
        case FPI_ACT_NONE:
          fpis[i].userError.event.action = ACTION_NONE;
          break;
        default:
          fpis[i].userError.event.action = ACTION_NONE;
          break;
      }
      switch ( fpis[i].arming )
      {
      case FRI_ARM_ALWAYS:
        fpis[i].getArming = uFPIarmingAlways;
        break;
      case FPI_ARM_TIMER_STOP:
        fpis[i].getArming = uENGINEisBlockTimerFinish;
        break;
      case FPI_ARM_STARTER_SCROLL_END:
        fpis[i].getArming = uENGINEisStarterScrollFinish;
        break;
      case FRI_ARM_NEVER:
        fpis[i].getArming = uFPIarmingNever;
        break;
      default:
        fpis[i].getArming = uFPIarmingAlways;
        break;
      }
      fpis[i].timer.id = LOGIC_DEFAULT_TIMER_ID; /* Reset timer ID */
      fpis[i].state    = FPI_IDLE;               /* Toogle fpi to the start state */
      fpis[i].level    = FPI_LEVEL_LOW;          /* Reset current level */
      if ( fpis[i].function == FPI_FUN_USER )    /* Reset arming for non user functions */
      {
        fpis[i].userError.enb = PERMISSION_ENABLE;
      }
      else
      {
        fpis[i].userError.enb = PERMISSION_DISABLE;
      }
      fpis[i].userError.active = PERMISSION_ENABLE;
      if ( fpis[i].userError.event.action == ACTION_WARNING )
      {
        fpis[i].userError.ack = PERMISSION_ENABLE;
      }
      else
      {
        fpis[i].userError.ack = PERMISSION_DISABLE;
      }
      fpis[i].userError.event.type = eFPIuserEventTypeList[i];
      fpis[i].userError.trig       = TRIGGER_IDLE;
      fpis[i].userError.status     = ALARM_STATUS_IDLE;
    }
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vFPIinit ( const FPI_INIT* init )
{
  /* Physical part */
  fpis[FPI_A].port = init->portA;
  fpis[FPI_A].pin  = init->pinA;
  fpis[FPI_B].port = init->portB;
  fpis[FPI_B].pin  = init->pinB;
  fpis[FPI_C].port = init->portC;
  fpis[FPI_C].pin  = init->pinC;
  fpis[FPI_D].port = init->portD;
  fpis[FPI_D].pin  = init->pinD;
  HAL_GPIO_WritePin( init->portCS, init->pinCS, GPIO_PIN_SET );
  /* Read parameters form memory */
  vFPIdataInit();
  /* Queue init */
  xFPIsemaphore = xSemaphoreCreateMutex();
  pFPIQueue     = xQueueCreateStatic( 16U, sizeof( FPI_EVENT ), eventBuffer, &xFPIQueue );
  const osThreadAttr_t fpiTask_attributes = {
    .name       = "fpiTask",
    .priority   = ( osPriority_t ) FPI_TASK_PRIORITY,
    .stack_size = FPI_TASK_STACK_SIZE
  };
  fpiHandle = osThreadNew( vFPITask, NULL, &fpiTask_attributes );
  return;
}
/*----------------------------------------------------------------------------*/
uint16_t uFPIgetData ( void )
{
  return fpiDataReg;
}
/*----------------------------------------------------------------------------*/
uint16_t uFPIgetRawData ( void )
{
  return fpiRawDataReg;
}
/*----------------------------------------------------------------------------*/
QueueHandle_t pFPIgetQueue ( void )
{
  return pFPIQueue;
}
/*----------------------------------------------------------------------------*/
FPI_LEVEL eFPIcheckLevel ( FPI_FUNCTION function )
{
  FPI_LEVEL level = FPI_LEVEL_LOW;
  for ( uint8_t i=0U; i<FPI_NUMBER; i++ )
  {
    if ( fpis[i].function == function )
    {
      level = fpis[i].level;
      break;
    }
  }
  return level;
}
/*----------------------------------------------------------------------------*/
void vFPIreset ( void )
{
  if ( xSemaphoreTake( xFPIsemaphore, FPI_TASK_DELAY ) == pdTRUE )
  {
    for ( uint8_t i=0U; i<FPI_NUMBER; i++ )
    {
      if ( ( fpis[i].function == FPI_FUN_USER              ) ||
           ( fpis[i].function == FPI_FUN_OIL_LOW_PRESSURE  ) ||
           ( fpis[i].function == FPI_FUN_HIGHT_ENGINE_TEMP ) ||
           ( fpis[i].function == FPI_FUN_LOW_FUEL          ) )
      {
        fpis[i].level   = FPI_LEVEL_LOW;
        fpis[i].state   = FPI_IDLE;
        fpis[i].trigger = TRIGGER_IDLE;
      }
      vLOGICresetTimer( &fpis[i].timer );
    }
    xSemaphoreGive( xFPIsemaphore );
  }
  return;
}
/*----------------------------------------------------------------------------*/
void vFPIsetBlock ( void )
{
  while ( xSemaphoreTake( xFPIsemaphore, FPI_TASK_DELAY ) != pdTRUE )
  {
    osDelay( 10U );
  }
  for ( uint8_t i=0U; i<FPI_NUMBER; i++ )
  {
    fpis[i].state   = FPI_IDLE;
    fpis[i].trigger = TRIGGER_IDLE;
  }
  xSemaphoreGive( xFPIsemaphore );
  return;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eFPIgetState ( uint8_t n )
{
  TRIGGER_STATE res = TRIGGER_IDLE;
  if ( n < FPI_NUMBER )
  {
    res = fpis[n].trigger;
  }
  return res;
}
/*----------------------------------------------------------------------------*/
uint16_t* uFPIgetMessage ( uint8_t n )
{
  uint16_t* res = NULL;
  if ( n < FPI_NUMBER )
  {
    res = fpis[n].message;
  }
  return res;
}
/*----------------------------------------------------------------------------*/
SYSTEM_EVENT_TYPE eFPIgetUserEventType ( uint8_t n )
{
  return eFPIuserEventTypeList[n];
}
/*----------------------------------------------------------------------------*/
void vFPITask ( void* argument )
{
  FPI_EVENT event = { 0U };
  for (;;)
  {
    /*-------------------- Read system notification --------------------*/
    if ( ( xEventGroupGetBits( xDATAAPIgetEventGroup() ) & DATA_API_FLAG_FPI_TASK_CONFIG_REINIT ) > 0U )
    {
      vFPIdataInit();
      xEventGroupClearBits( xDATAAPIgetEventGroup(), DATA_API_FLAG_FPI_TASK_CONFIG_REINIT );
    }
    if ( xSemaphoreTake( xFPIsemaphore, FPI_TASK_DELAY ) == pdTRUE )
    {
      for ( uint8_t i=0U; i<FPI_NUMBER; i++ )
      {
        if ( HAL_GPIO_ReadPin ( fpis[i].port, fpis[i].pin ) == GPIO_PIN_RESET )
        {
          fpiRawDataReg |= ( uint8_t )( 1U << i );
        }
        else
        {
          fpiRawDataReg &= ~( uint8_t )( 1U << i );
        }


        if ( fpis[i].function != FPI_FUN_NONE )
        {
          switch ( fpis[i].state )
          {
            case FPI_IDLE:
              if ( fpis[i].getArming() == TRIGGER_SET )
              {
                if ( eFPIgetTrig( &fpis[i] ) == TRIGGER_SET )
                {
                  fpis[i].state = FPI_TRIGGERED;
                  if ( fpis[i].level == FPI_LEVEL_LOW )
                  {
                    fpiDataReg     &= ~( uint8_t )( 1U << i );
                    fpis[i].trigger = TRIGGER_IDLE;
                  }
                  else
                  {
                    vLOGICstartTimer( &fpis[i].timer, "FPI timer           " );
                  }
                }
              }
              else
              {
                fpiDataReg      &= ~( uint8_t )( 1U << i );
                fpis[i].trigger  = TRIGGER_IDLE;
              }
              break;
            case FPI_TRIGGERED:
              if ( uLOGICisTimer( &fpis[i].timer ) > 0U )
              {
                event.level    = fpis[i].level;
                event.function = fpis[i].function;
                event.number   = i;
                if ( fpis[i].level == FPI_LEVEL_HIGH )
                {
                  fpiDataReg      |= ( uint8_t )( 1U << i );
                  fpis[i].trigger  = TRIGGER_SET;
                }
                fpis[i].state  = FPI_IDLE;
                xQueueSend( pFPIQueue, &event, portMAX_DELAY );
              }
              else if ( eFPIgetLevel( &fpis[i] ) != fpis[i].level )
              {
                vLOGICresetTimer( &fpis[i].timer );
                fpis[i].state = FPI_IDLE;
              }
              else if ( fpis[i].getArming() == TRIGGER_IDLE )
              {
                fpis[i].state = FPI_IDLE;
              }
              else
              {

              }
              break;
            default:
              fpis[i].state = FPI_IDLE;
              break;
          }
          vERRORcheck( &fpis[i].userError, fpis[i].trigger );
        }
      }
      xSemaphoreGive( xFPIsemaphore );
    }
    osDelay( FPI_TASK_DELAY );
  }
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

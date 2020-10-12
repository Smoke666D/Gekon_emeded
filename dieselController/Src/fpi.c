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
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "config.h"
#include "dataProces.h"
#include "engine.h"
/*-------------------------------- Structures --------------------------------*/
static QueueHandle_t     pFPIQueue     = NULL;
static StaticQueue_t     xFPIQueue     = { 0U };
static SemaphoreHandle_t xFPIsemaphore = NULL;
/*--------------------------------- Constant ---------------------------------*/
const FPI_FUNCTION eFPIfuncList[FPI_FUNCTION_NUM] =
{
  FPI_FUN_NONE,
  FPI_FUN_USER,
  FPI_FUN_ALARM_RESET,
  FPI_FUN_OIL_LOW_PRESSURE,
  FPI_FUN_HIGHT_ENGINE_TEMP,
  FPI_FUN_LOW_FUEL,
  FPI_FUN_REMOTE_START,
  FPI_FUN_IDLING,
  FPI_FUN_BAN_AUTO_START,
  FPI_FUN_BAN_GEN_LOAD,
  FPI_FUN_BAN_AUTO_SHUTDOWN
};
const char* cFPIfunctionNames[FPI_FUNCTION_NUM] =
{
  "NONE",
  "USER",
  "ALARM_RESET",
  "OIL_LOW_PRESSURE",
  "HIGHT_ENGINE_TEMP",
  "LOW_FUEL",
  "REMOTE_START",
  "IDLING",
  "BAN_AUTO_START",
  "BAN_GEN_LOAD",
  "BAN_AUTO_SHUTDOWN"
};
const char* cFPInames[FPI_NUMBER] =
{
  "A",
  "B",
  "C",
  "D"
};
/*-------------------------------- Variables ---------------------------------*/
static uint8_t eventBuffer[ 16U * sizeof( FPI_EVENT ) ] = { 0U };
static FPI     fpis[FPI_NUMBER]                         = { 0U };
/*-------------------------------- External -----------------------------------*/
osThreadId_t fpiHandle = NULL;
/*-------------------------------- Functions ---------------------------------*/
void    vFPITask ( void const* argument );
/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/
uint8_t uFPIarmingAlways ( void )
{
  return 0xFFU;
}
/*----------------------------------------------------------------------------*/
uint8_t uFPIarmingNever ( void )
{
  return 0x00U;
}
/*----------------------------------------------------------------------------*/
/*
 * Get hardware pin level and convert it to logical level
 * input:  fpi - Freely Programmable Input structure
 * output: level of fpi
 */
uint8_t vFPIgetTrig ( FPI* fpi )
{
  GPIO_PinState pinState = GPIO_PIN_RESET;
  uint8_t       res      = 0U;
  if ( fpi->port != NULL )
  {
    pinState = HAL_GPIO_ReadPin ( fpi->port, fpi->pin );
    if ( ( ( fpi->polarity == FPI_POL_NORMAL_OPEN  ) && ( pinState == GPIO_PIN_RESET ) ) ||
         ( ( fpi->polarity == FPI_POL_NORMAL_CLOSE ) && ( pinState == GPIO_PIN_SET   ) ) )
    {
      res = 1U;
    }
    if ( ( fpi->level == FPI_LEVEL_LOW ) && ( res > 0U ) )
    {
      fpi->level = FPI_LEVEL_HIGH;
    }
    else if ( ( fpi->level == FPI_LEVEL_HIGH ) && ( res > 0U ) )
    {
      res = 0U;
    }
    else if ( ( fpi->level == FPI_LEVEL_HIGH ) && ( res == 0U ) )
    {
      fpi->level = FPI_LEVEL_LOW;
      res        = 1U;
    }
    else
    {

    }
  }
  return res;
}
/*----------------------------------------------------------------------------*/
/*
 * Read fpi data from configurations
 * input:  fpi      - Freely Programmable Input structure
 *         setupReg - configuration register of fpi
 *         delayReg - configuration register of fpi delay
 * output: none
 */
void vFPIreadConfigs ( FPI* fpi, const eConfigReg* setupReg, const eConfigReg* delayReg )
{
  fpi->delay    = getUintValue( delayReg );
  fpi->function = eFPIfuncList[ getBitMap( setupReg, 0U ) ];
  fpi->polarity = getBitMap( setupReg, 1U );
  fpi->action   = getBitMap( setupReg, 2U );
  fpi->arming   = getBitMap( setupReg, 3U );
  return;
}
/*----------------------------------------------------------------------------*/
void vFPIcheckReset ( FPI* fpi )
{
  if ( vFPIgetTrig( fpi ) == 0U )
  {
    vLOGICresetTimer( fpi->timerID );
    fpi->state = FPI_IDLE;
  }
  return;
}
/*----------------------------------------------------------------------------*/
void vFPIprintSetup ( void )
{
  uint8_t i = 0U;
  for ( i=0U; i<FPI_NUMBER; i++ )
  {
    vSYSSerial( ">>FPI " );
    vSYSSerial( cFPInames[i] );
    vSYSSerial( "        : " );
    vSYSSerial( cFPIfunctionNames[ ( uint8_t )( fpis[i].function ) ] );
    vSYSSerial( "\n\r" );
  }
  vSYSSerial( "\n\r" );
  return;
}
void vFPIdataInit ( void )
{
  vFPIreadConfigs( &fpis[FPI_A], &diaSetup, &diaDelay );
  vFPIreadConfigs( &fpis[FPI_B], &dibSetup, &dibDelay );
  vFPIreadConfigs( &fpis[FPI_C], &dicSetup, &dicDelay );
  vFPIreadConfigs( &fpis[FPI_D], &didSetup, &didDelay );
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vFPIinit ( const FPI_INIT* init )
{
  uint8_t i = 0U;
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
  /* Logic part */
  for ( i=0U; i<FPI_NUMBER; i++ )
  {
    fpis[i].timerID = 0U;                    /* Reset timer ID */
    fpis[i].state   = FPI_BLOCK;             /* Toogle fpi to the start state */
    fpis[i].level   = FPI_LEVEL_LOW;         /* Reset current level */
    if ( fpis[i].function != FPI_FUN_USER )  /* Reset arming for non user functions */
    {
      fpis[i].arming = FRI_ARM_ALWAYS;
      fpis[i].action = FPI_ACT_NONE;
    }
    /* Setup callbacks */
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
  }
  /* Queue init */
  pFPIQueue  = xQueueCreateStatic( 16U, sizeof( FPI_EVENT ), eventBuffer, &xFPIQueue );
  const osThreadAttr_t fpiTask_attributes = {
    .name       = "fpiTask",
    .priority   = ( osPriority_t ) FPI_TASK_PRIORITY,
    .stack_size = FPI_TASK_STACK_SIZE
  };
  fpiHandle = osThreadNew( vFPITask, NULL, &fpiTask_attributes );

  xFPIsemaphore = xSemaphoreCreateMutex();

  vFPIprintSetup();
  return;
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
  uint8_t   i     = 0U;

  for ( i=0U; i<FPI_NUMBER; i++ )
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
void vFPIsetBlock ( void )
{
  uint8_t i = 0U;
  while ( xSemaphoreTake( xCHARTSemaphore, FPI_TASK_DELAY ) != pdTRUE )
  {
    osDelay( 10U );
  }
  for ( i=0U; i<FPI_NUMBER; i++ )
  {
    fpis[i].state = FPI_BLOCK;
  }
  xSemaphoreGive( xCHARTSemaphore );
  return;
}
/*----------------------------------------------------------------------------*/
void vFPITask ( void const* argument )
{
  FPI_EVENT event       = { FPI_LEVEL_LOW, FPI_FUN_NONE, FPI_ACT_NONE, NULL };
  uint8_t   i           = 0U;
  uint32_t  inputNotifi = 0U;
  for (;;)
  {
    /*-------------------- Read system notification --------------------*/
    if ( xTaskNotifyWait( 0U, 0xFFFFFFFFU, &inputNotifi, TASK_NOTIFY_WAIT_DELAY ) == pdPASS )
    {
      if ( ( inputNotifi & DATA_API_MESSAGE_REINIT ) > 0U )
      {
        vFPIdataInit();
      }
    }
    if ( xSemaphoreTake( xCHARTSemaphore, FPI_TASK_DELAY ) == pdTRUE )
    {
      for ( i=0U; i<FPI_NUMBER; i++ )
      {
        if ( fpis[i].function != FPI_FUN_NONE )
        {
          switch ( fpis[i].state )
          {
            case FPI_BLOCK:
              if ( fpis[i].getArming() > 0U )
              {
                fpis[i].state = FPI_IDLE;
              }
              break;
            case FPI_IDLE:
              if ( vFPIgetTrig( &fpis[i] ) > 0U )
              {
                fpis[i].state = FPI_TRIGGERED;
                vLOGICstartTimer( fpis[i].delay, &fpis[i].timerID );
              }
              break;
            case FPI_TRIGGERED:
              if ( uLOGICisTimer( fpis[i].timerID ) > 0U )
              {
                event.level    = fpis[i].level;
                event.function = fpis[i].function;
                event.action   = fpis[i].action;
                event.message  = fpis[i].message;
                fpis[i].state  = FPI_IDLE;
                xQueueSend( pFPIQueue, &event, portMAX_DELAY );
              }
              break;
            default:
              fpis[i].state = FPI_BLOCK;
              break;
          }
        }
      }
      xSemaphoreGive( xCHARTSemaphore );
    }
    osDelay( FPI_TASK_DELAY );
  }
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

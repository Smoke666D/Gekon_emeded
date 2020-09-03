/*
 * fpi.c
 *
 *  Created on: 12 июн. 2020 г.
 *      Author: photo_Mickey
 */
/*--------------------------------- Includes ---------------------------------*/
#include "fpi.h"
#include "stm32f2xx_hal_gpio.h"
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "config.h"
#include "dataProces.h"
#include "engine.h"
/*-------------------------------- Structures --------------------------------*/
static QueueHandle_t pFPIQueue;
static StaticQueue_t xFPIQueue;
static osThreadId_t  fpiHandle;
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
/*-------------------------------- Variables ---------------------------------*/
static uint8_t eventBuffer[ 16U * sizeof( FPI_EVENT ) ];
static FPI     fpis[FPI_NUMBER];
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
  GPIO_PinState pinState = HAL_GPIO_ReadPin ( fpi->port, fpi->pin );
  uint8_t       res      = 0U;
  if ( ( ( fpi->polarity == FPI_POL_NORMAL_OPEN  ) && ( pinState == GPIO_PIN_SET   ) ) ||
       ( ( fpi->polarity == FPI_POL_NORMAL_CLOSE ) && ( pinState == GPIO_PIN_RESET ) ) )
  {
    res = 1U;
  }
  if ( ( fpi->level == FPI_LEVEL_LOW ) && ( res > 0U ) )
  {
    fpi->level = FPI_LEVEL_HIGH;
  }
  if ( ( fpi->level == FPI_LEVEL_HIGH ) && ( res == 0U ) )
  {
    fpi->level = FPI_LEVEL_LOW;
    res        = 1U;
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
void vFPIreadConfigs ( FPI fpi, const eConfigReg* setupReg, const eConfigReg* delayReg )
{
  fpi.delay    = getValue( delayReg );
  fpi.function = eFPIfuncList[ getBitMap( setupReg, 0U ) ];
  fpi.polarity = getBitMap( setupReg, 1U );
  fpi.action   = getBitMap( setupReg, 2U );
  fpi.arming   = getBitMap( setupReg, 3U );
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vFPIinit ( FPI_INIT* init )
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
  /* Read parameters form memory */
  vFPIreadConfigs( fpis[FPI_A], &diaSetup, &diaDelay );
  vFPIreadConfigs( fpis[FPI_B], &dibSetup, &dibDelay );
  vFPIreadConfigs( fpis[FPI_C], &dicSetup, &dicDelay );
  vFPIreadConfigs( fpis[FPI_D], &didSetup, &didDelay );
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
    .priority   = ( osPriority_t ) osPriorityLow,
    .stack_size = 1024U
  };
  fpiHandle = osThreadNew( vFPITask, NULL, &fpiTask_attributes );
  return;
}

QueueHandle_t pFPIgetQueue ( void )
{
  return pFPIQueue;
}

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

void vFPIsetBlock ( void )
{
  uint8_t i = 0U;
  vTaskSuspend ( &fpiHandle );
  for ( i=0U; i<FPI_NUMBER; i++ )
  {
    fpis[i].state = FPI_BLOCK;
  }
  vTaskResume ( &fpiHandle );
  return;
}

void vFPITask ( void const* argument )
{
  FPI_EVENT event = { FPI_LEVEL_LOW, FPI_FUN_NONE, FPI_ACT_NONE, NULL };
  uint8_t   i     = 0U;
  for (;;)
  {
    for ( i=0U; i<FPI_NUMBER; i++ )
    {
      if ( fpis[i].function != FPI_FUN_NONE)
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
            if ( vFPIgetTrig( &fpis[i] ) > 0U )
            {
              vLOGICresetTimer( fpis[i].timerID );
              fpis[i].state = FPI_IDLE;
            }
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
  }
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

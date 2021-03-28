/*
 * can_timer.c
 *
 *  Created on: 26 мар. 2021 г.
 *      Author: 45
 */

#include "can_timer.h"

/******************************************************************************
* PRIVATE DEFINES
******************************************************************************/

/* TODO: place here your timer register definitions */

/******************************************************************************
* PRIVATE FUNCTIONS
******************************************************************************/

static void     STM32F2TimerInit   (uint32_t freq);
static void     STM32F2TimerStart  (void);
static uint8_t  STM32F2TimerUpdate (void);
static uint32_t STM32F2TimerDelay  (void);
static void     STM32F2TimerReload (uint32_t reload);
static void     STM32F2TimerStop   (void);

/******************************************************************************
* PUBLIC VARIABLE
******************************************************************************/

/* TODO: rename the variable to match the naming convention:
 *   <device>TimerDriver
 */
const CO_IF_TIMER_DRV STM32F2TimerDriver = {
    STM32F2TimerInit,
    STM32F2TimerReload,
    STM32F2TimerDelay,
    STM32F2TimerStop,
    STM32F2TimerStart,
    STM32F2TimerUpdate
};

/******************************************************************************
* PRIVATE VARIABLE
******************************************************************************/

static SemaphoreHandle_t xTimerSemph = NULL;
static StaticSemaphore_t xTimerSemphBuffer;
static uint32_t TimerCounter = 0U;

void vCan_TimerInit()
{
  xTimerSemph = xSemaphoreCreateBinaryStatic( &xTimerSemphBuffer);
}

static void STM32F2TimerInit(uint32_t freq)
{
    /* TODO: initialize timer, clear counter and keep timer stopped */
  HAL_TIM_Base_Stop_IT(ptim);
  ptim->Init.Period = 60000000U / ( freq  );
  ptim->Instance->CNT = 0;
  HAL_TIM_Base_Init(ptim);
  TimerCounter = 0U;
  return;
}

static void STM32F2TimerStart(void)
{
    HAL_TIM_Base_Start_IT(ptim);
    return;
}

static uint8_t STM32F2TimerUpdate(void)
{
    /* TODO: return 1 if timer e vent is elapsed, otherwise 0 */
   uint8_t result = 0u;
   if (TimerCounter > 0u)
   {
     TimerCounter--;
     if (TimerCounter == 0u)
     {
       result = 1u;
     }
   }
   return (result);

}

static uint32_t STM32F2TimerDelay(void)
{
    /* TODO: return current timer counter value */
  return (TimerCounter);
}

static void STM32F2TimerReload(uint32_t reload)
{
    /* TODO: reload timer counter value with given reload value */
    TimerCounter = reload;
}

static void STM32F2TimerStop(void)
{
    TimerCounter = 0U;
    HAL_TIM_Base_Stop_IT(ptim);
    return;
}


/*
 *  CALLBAKC из обработчика прервыания
 *

 */
void vCanTimerCallback()
{

  static portBASE_TYPE xHigherPriorityTaskWoken;
  if (COTmrService(&(xENRG_NODE.Tmr)) >0 )
  {
    xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR( xTimerSemph, &xHigherPriorityTaskWoken );
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
  }
}


void StartCANOPENTimeTask(void *argument)
{

    for (;;)
    {
      xSemaphoreTake( xTimerSemph, portMAX_DELAY );
      COTmrProcess(&(xENRG_NODE.Tmr));
    }
}

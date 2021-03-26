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
* PRIVATE FUNCTIONS
******************************************************************************/



static void STM32F2TimerInit(uint32_t freq)
{
    /* TODO: initialize timer, clear counter and keep timer stopped */
  ptim->Init.Period = 60000000U / ( freq  );
  HAL_TIM_Base_Init(ptim);
  HAL_TIM_Base_Start_IT( ptim );
}

static void STM32F2TimerStart(void)
{
    /* TODO: start hardware timer */
}

static uint8_t STM32F2TimerUpdate(void)
{
    /* TODO: return 1 if timer event is elapsed, otherwise 0 */
    return (0u);
}

static uint32_t STM32F2TimerDelay(void)
{
    /* TODO: return current timer counter value */
    return (0u);
}

static void STM32F2TimerReload(uint32_t reload)
{
    /* TODO: reload timer counter value with given reload value */
}

static void STM32F2TimerStop(void)
{
    /* TODO: stop timer and clear counter value */
}

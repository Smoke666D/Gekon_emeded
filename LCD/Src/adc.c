/*
 * adc.c
 *
 *  Created on: Aug 31, 2020
 *      Author: igor.dymov
 */


#include "adc.h"

extern TIM_HandleTypeDef htim3;

float  fADC3Init(uint16_t freq)
{

  uint16_t Period = 15000000U/ freq;
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = Period;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_TIM_Base_Start_IT( &htim3 );
  return (float)(1/Period);
}


void vADCInit(void)
{
  HAL_GPIO_WritePin( ON_INPOW_GPIO_Port,ON_INPOW_Pin, GPIO_PIN_SET );
}


void StartADCTask(void *argument)
{


   vADCInit();
   for(;;)
   {


   }

}

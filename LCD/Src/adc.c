/*
 * adc.c
 *
 *  Created on: Aug 31, 2020
 *      Author: igor.dymov
 */


#include "adc.h"

static EventGroupHandle_t xADCEvent;
static StaticEventGroup_t xADCCreatedEventGroup;
volatile uint16_t ADC1_IN_Buffer[ADC_FRAME_SIZE*ADC1_CHANNELS];   //ADC1 input data buffer
volatile uint16_t ADC2_IN_Buffer[ADC_FRAME_SIZE*ADC2_CHANNELS];   //ADC2 input data buffer
volatile uint16_t ADC3_IN_Buffer[ADC_FRAME_SIZE*ADC3_CHANNELS];   //ADC3 input data buffer
volatile uint16_t ADC3_ADD_IN_Buffer[ADC_ADD_FRAME_SIZE* ADC3_ADD_CHANNEL];


extern TIM_HandleTypeDef htim3;
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_adc2;
extern DMA_HandleTypeDef hdma_adc3;


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
  //Создаем флаг готовности АПЦ
   xADCEvent = xEventGroupCreateStatic(&xADCCreatedEventGroup );

  HAL_GPIO_WritePin( ON_INPOW_GPIO_Port,ON_INPOW_Pin, GPIO_PIN_SET );
  HAL_ADC_Start_DMA(&hdma_adc3,(uint32_t*)&ADC3_ADD_IN_Buffer,ADC_ADD_FRAME_SIZE);
}


void ADC1_Ready()
{
  static portBASE_TYPE xHigherPriorityTaskWoken;
  xHigherPriorityTaskWoken = pdFALSE;
  xEventGroupSetBitsFromISR(xADCEvent,ADC1_READY,&xHigherPriorityTaskWoken);
  HAL_ADC_Stop_DMA(&hdma_adc1);
  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}
void ADC2_Ready()
{
  static portBASE_TYPE xHigherPriorityTaskWoken;
  xHigherPriorityTaskWoken = pdFALSE;
  xEventGroupSetBitsFromISR(xADCEvent,ADC1_READY,&xHigherPriorityTaskWoken);
  HAL_ADC_Stop_DMA(&hdma_adc2);
  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}
void ADC3_Ready()
{
  static portBASE_TYPE xHigherPriorityTaskWoken;
  xHigherPriorityTaskWoken = pdFALSE;
  xEventGroupSetBitsFromISR(xADCEvent,ADC1_READY,&xHigherPriorityTaskWoken);
  HAL_ADC_Stop_DMA(&hdma_adc3);
  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}


void StartADCTask(void *argument)
{


   vADCInit();
   for(;;)
   {


   }

}

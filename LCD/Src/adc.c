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
static xADCFSMType xADCFSM = DC;

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim8;
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;

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

float  fADC12Init(uint16_t freq)
{

  uint16_t Period = 15000000U/ freq;
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    /* USER CODE BEGIN TIM8_Init 1 */

    /* USER CODE END TIM8_Init 1 */
    htim8.Instance = TIM8;
    htim8.Init.Prescaler = 0;
    htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim8.Init.Period = Period;
    htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
    htim8.Init.RepetitionCounter = 0;
    htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
    {
      Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
    {
      Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
    {
      Error_Handler();
    }
  HAL_TIM_Base_Start_IT( &htim8 );
  return (float)(1/Period);
}


void vADC3DCInit(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

    hadc3.Instance = ADC3;
    hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc3.Init.Resolution = ADC_RESOLUTION_12B;
    hadc3.Init.ScanConvMode = ENABLE;
    hadc3.Init.ContinuousConvMode = ENABLE;
    hadc3.Init.DiscontinuousConvMode = DISABLE;
    hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    hadc3.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;
    hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc3.Init.NbrOfConversion = 5;
    hadc3.Init.DMAContinuousRequests = ENABLE;
    hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;



  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_15;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_14;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_13;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_12;
  sConfig.Rank = 4;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = 5;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}


void vADCInit(void)
{


  switch (xADCFSM)
  {
    case DC:
      HAL_GPIO_WritePin( ON_INPOW_GPIO_Port,ON_INPOW_Pin, GPIO_PIN_SET );
      vADC3DCInit();
      fADC3Init(10000);
      HAL_ADC_Start_DMA(&hadc3,(uint32_t*)&ADC3_ADD_IN_Buffer,ADC_ADD_FRAME_SIZE);
      break;
    case AC:
      HAL_GPIO_WritePin( ON_INPOW_GPIO_Port,ON_INPOW_Pin, GPIO_PIN_RESET );
      HAL_ADC_Start_DMA(&hadc1,(uint32_t*)&ADC1_IN_Buffer,ADC_ADD_FRAME_SIZE);
      HAL_ADC_Start_DMA(&hadc2,(uint32_t*)&ADC2_IN_Buffer,ADC_ADD_FRAME_SIZE);
      HAL_ADC_Start_DMA(&hadc3,(uint32_t*)&ADC3_IN_Buffer,ADC_ADD_FRAME_SIZE);
      break;
    case IDLE:
      HAL_GPIO_WritePin( ON_INPOW_GPIO_Port,ON_INPOW_Pin, GPIO_PIN_RESET );
      HAL_TIM_Base_Stop_IT( &htim3 );
      break;


  }


}


void vADC1_Ready(void)
{
  static portBASE_TYPE xHigherPriorityTaskWoken;
  xHigherPriorityTaskWoken = pdFALSE;
  xEventGroupSetBitsFromISR(xADCEvent,ADC1_READY,&xHigherPriorityTaskWoken);
  HAL_ADC_Stop_DMA(&hadc1);
  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}
void vADC2_Ready(void)
{
  static portBASE_TYPE xHigherPriorityTaskWoken;
  xHigherPriorityTaskWoken = pdFALSE;
  xEventGroupSetBitsFromISR(xADCEvent,ADC1_READY,&xHigherPriorityTaskWoken);
  HAL_ADC_Stop_DMA(&hadc2);
  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}
void vADC3_Ready(void)
{
  static portBASE_TYPE xHigherPriorityTaskWoken;
  xHigherPriorityTaskWoken = pdFALSE;
  xEventGroupSetBitsFromISR(xADCEvent,ADC3_READY,&xHigherPriorityTaskWoken);

  HAL_TIM_Base_Stop_IT( &htim3 );
  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}



static uint16_t  ADCDATA[5]={0,0,0,0,0};

void vGetADCDC( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
   if (cmd == mREAD)
   {
     vUToStr(Data,ADCDATA[ID-1],0);
   }
   return;
}

void vADCCommonInit(void)
{

}

void vADCCommonDeInit(void)
{


}

void StartADCTask(void *argument)
{
  //Создаем флаг готовности АПЦ
   xADCEvent = xEventGroupCreateStatic(&xADCCreatedEventGroup );
   vADCInit();
   for(;;)
   {
    osDelay(500);
     switch (xADCFSM)
     {
       case AC:
         xEventGroupWaitBits(xADCEvent,ADC1_READY | ADC2_READY | ADC3_READY,pdTRUE,pdTRUE,portMAX_DELAY);
         break;
       case DC:
         HAL_TIM_Base_Start_IT( &htim3 );
         xEventGroupWaitBits(xADCEvent,ADC3_READY,pdTRUE,pdTRUE,portMAX_DELAY);
         ADCDATA[0] = (ADC3_ADD_IN_Buffer[0]+ADC3_ADD_IN_Buffer[5]+ADC3_ADD_IN_Buffer[10]+ADC3_ADD_IN_Buffer[15])>>2;
         ADCDATA[1] = (ADC3_ADD_IN_Buffer[1]+ADC3_ADD_IN_Buffer[6]+ADC3_ADD_IN_Buffer[11]+ADC3_ADD_IN_Buffer[16])>>2;
         ADCDATA[2] = (ADC3_ADD_IN_Buffer[2]+ADC3_ADD_IN_Buffer[7]+ADC3_ADD_IN_Buffer[12]+ADC3_ADD_IN_Buffer[17])>>2;
         ADCDATA[3] = (ADC3_ADD_IN_Buffer[3]+ADC3_ADD_IN_Buffer[8]+ADC3_ADD_IN_Buffer[13]+ADC3_ADD_IN_Buffer[18])>>2;
         ADCDATA[4] = (ADC3_ADD_IN_Buffer[4]+ADC3_ADD_IN_Buffer[9]+ADC3_ADD_IN_Buffer[14]+ADC3_ADD_IN_Buffer[19])>>2;
         break;
     }
   }

}

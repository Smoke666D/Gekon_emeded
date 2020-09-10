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
      fADC3Init(15000);


      break;
    case AC:
      fADC12Init(15000);
      fADC3Init(15000);
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

void vADC3_Ready()
{
  static portBASE_TYPE xHigherPriorityTaskWoken;

  xHigherPriorityTaskWoken = pdFALSE;
  xEventGroupSetBitsFromISR(xADCEvent,ADC3_READY,&xHigherPriorityTaskWoken);
  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
  return;

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


static void ADC_DMAConv(DMA_HandleTypeDef *hdma)
{
  /* Retrieve ADC handle corresponding to current DMA handle */
  ADC_HandleTypeDef* hadc = ( ADC_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

  /* Update state machine on conversion status if not in error state */
  if (HAL_IS_BIT_CLR(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL | HAL_ADC_STATE_ERROR_DMA))
  {
    /* Update ADC state machine */
    SET_BIT(hadc->State, HAL_ADC_STATE_REG_EOC);

    /* Determine whether any further conversion upcoming on group regular   */
    /* by external trigger, continuous mode or scan sequence on going.      */
    /* Note: On STM32F2, there is no independent flag of end of sequence.   */
    /*       The test of scan sequence on going is done either with scan    */
    /*       sequence disabled or with end of conversion flag set to        */
    /*       of end of sequence.                                            */
    if(ADC_IS_SOFTWARE_START_REGULAR(hadc)                   &&
       (hadc->Init.ContinuousConvMode == DISABLE)            &&
       (HAL_IS_BIT_CLR(hadc->Instance->SQR1, ADC_SQR1_L) ||
        HAL_IS_BIT_CLR(hadc->Instance->CR2, ADC_CR2_EOCS)  )   )
    {
      /* Disable ADC end of single conversion interrupt on group regular */
      /* Note: Overrun interrupt was enabled with EOC interrupt in          */
      /* HAL_ADC_Start_IT(), but is not disabled here because can be used   */
      /* by overrun IRQ process below.                                      */
      __HAL_ADC_DISABLE_IT(hadc, ADC_IT_EOC);

      /* Set ADC state */
      CLEAR_BIT(hadc->State, HAL_ADC_STATE_REG_BUSY);

      if (HAL_IS_BIT_CLR(hadc->State, HAL_ADC_STATE_INJ_BUSY))
      {
        SET_BIT(hadc->State, HAL_ADC_STATE_READY);
      }
    }

    /* Conversion complete callback */
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
    hadc->ConvCpltCallback(hadc);
#else
    HAL_ADC_ConvCpltCallback(hadc);
#endif /* USE_HAL_ADC_REGISTER_CALLBACKS */
  }
  else /* DMA and-or internal error occurred */
  {
    if ((hadc->State & HAL_ADC_STATE_ERROR_INTERNAL) != 0UL)
    {
      /* Call HAL ADC Error Callback function */
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
      hadc->ErrorCallback(hadc);
#else
      HAL_ADC_ErrorCallback(hadc);
#endif /* USE_HAL_ADC_REGISTER_CALLBACKS */
    }
  else
  {
      /* Call DMA error callback */
      hadc->DMA_Handle->XferErrorCallback(hdma);
    }
  }
}

static void ADC_DMAErro(DMA_HandleTypeDef *hdma)
{
  ADC_HandleTypeDef* hadc = ( ADC_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
  hadc->State= HAL_ADC_STATE_ERROR_DMA;
  /* Set ADC error code to DMA error */
  hadc->ErrorCode |= HAL_ADC_ERROR_DMA;
   /* Error callback */
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
  hadc->ErrorCallback(hadc);
#else
  HAL_ADC_ErrorCallback(hadc);
#endif /* USE_HAL_ADC_REGISTER_CALLBACKS */
}

void InitADCDMA(ADC_HandleTypeDef* hadc)
{
  /* Set the DMA transfer complete callback */
  hadc->DMA_Handle->XferCpltCallback = ADC_DMAConv;
  /* Set the DMA half transfer complete callback */
  hadc->DMA_Handle->XferHalfCpltCallback = NULL;//ADC_DMAHalfConvCplt;
  /* Set the DMA error callback */
  hadc->DMA_Handle->XferErrorCallback = ADC_DMAErro;
}



void StartADCDMA(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length)
{

  __IO uint32_t counter = 0U;


     /* Start the DMA channel */
     HAL_DMA_Start_IT(hadc->DMA_Handle, (uint32_t)&hadc->Instance->DR, (uint32_t)pData, Length);


    /* Process locked */
    __HAL_LOCK(hadc);

    /* Enable the ADC peripheral */
    /* Check if ADC peripheral is disabled in order to enable it and wait during
    Tstab time the ADC's stabilization */
    if((hadc->Instance->CR2 & ADC_CR2_ADON) != ADC_CR2_ADON)
    {
      /* Enable the Peripheral */
      __HAL_ADC_ENABLE(hadc);

      /* Delay for ADC stabilization time */
      /* Compute number of CPU cycles to wait for */
      counter = (ADC_STAB_DELAY_US * (SystemCoreClock / 1000000U));
      while(counter != 0U)
      {
        counter--;
      }
    }

    /* Start conversion if ADC is effectively enabled */
    if(HAL_IS_BIT_SET(hadc->Instance->CR2, ADC_CR2_ADON))
    {
      /* Set ADC state                                                          */
      /* - Clear state bitfield related to regular group conversion results     */
      /* - Set state bitfield related to regular group operation                */
      ADC_STATE_CLR_SET(hadc->State,
                        HAL_ADC_STATE_READY | HAL_ADC_STATE_REG_EOC | HAL_ADC_STATE_REG_OVR,
                        HAL_ADC_STATE_REG_BUSY);

      /* If conversions on group regular are also triggering group injected,    */
      /* update ADC state.                                                      */
      if (READ_BIT(hadc->Instance->CR1, ADC_CR1_JAUTO) != RESET)
      {
        ADC_STATE_CLR_SET(hadc->State, HAL_ADC_STATE_INJ_EOC, HAL_ADC_STATE_INJ_BUSY);
      }

      /* State machine update: Check if an injected conversion is ongoing */
      if (HAL_IS_BIT_SET(hadc->State, HAL_ADC_STATE_INJ_BUSY))
      {
        /* Reset ADC error code fields related to conversions on group regular */
        CLEAR_BIT(hadc->ErrorCode, (HAL_ADC_ERROR_OVR | HAL_ADC_ERROR_DMA));
      }
      else
      {
        /* Reset ADC all error code fields */
        ADC_CLEAR_ERRORCODE(hadc);
      }

      /* Process unlocked */
      /* Unlock before starting ADC conversions: in case of potential           */
      /* interruption, to let the process to ADC IRQ Handler.                   */
      __HAL_UNLOCK(hadc);


      /* Clear regular group conversion flag and overrun flag */
      /* (To ensure of no unknown state from potential previous ADC operations) */
      __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_EOC | ADC_FLAG_OVR);

      /* Enable ADC overrun interrupt */
      __HAL_ADC_ENABLE_IT(hadc, ADC_IT_OVR);

      /* Enable ADC DMA mode */
      hadc->Instance->CR2 |= ADC_CR2_DMA;

        /* if instance of handle correspond to ADC1 and  no external trigger present enable software conversion of regular channels */
        if((hadc->Instance == ADC1) && ((hadc->Instance->CR2 & ADC_CR2_EXTEN) == RESET))
        {
          /* Enable the selected ADC software conversion for regular group */
            hadc->Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
        }
    }


    /* Return function status */
    return HAL_OK;

}

void StartADCTask(void *argument)
{
   uint32_t tt=0;
  //Создаем флаг готовности АПЦ
   xADCEvent = xEventGroupCreateStatic(&xADCCreatedEventGroup );
   vADCInit();
   InitADCDMA(&hadc3);
   HAL_TIM_Base_Start_IT( &htim3 );
   for(;;)
   {
     osDelay(200);
     switch (xADCFSM)
     {
       case AC:
         xEventGroupWaitBits(xADCEvent,ADC1_READY | ADC2_READY | ADC3_READY,pdTRUE,pdTRUE,portMAX_DELAY);
         break;
       case DC:
         StartADCDMA(&hadc3,(uint32_t*)&ADC3_IN_Buffer,ADC_ADD_FRAME_SIZE);
         xEventGroupWaitBits(xADCEvent,ADC3_READY,pdTRUE,pdTRUE,portMAX_DELAY);
         ADCDATA[0] = (ADC3_IN_Buffer[0]+ADC3_IN_Buffer[5]+ADC3_IN_Buffer[10]+ADC3_IN_Buffer[15])>>2;
         ADCDATA[1] = (ADC3_IN_Buffer[1]+ADC3_IN_Buffer[6]+ADC3_IN_Buffer[11]+ADC3_IN_Buffer[16])>>2;
         ADCDATA[2] = (ADC3_IN_Buffer[2]+ADC3_IN_Buffer[7]+ADC3_IN_Buffer[12]+ADC3_IN_Buffer[17])>>2;
         ADCDATA[3] = (ADC3_IN_Buffer[3]+ADC3_IN_Buffer[8]+ADC3_IN_Buffer[13]+ADC3_IN_Buffer[18])>>2;
         ADCDATA[4] = (ADC3_IN_Buffer[4]+ADC3_IN_Buffer[9]+ADC3_IN_Buffer[14]+ADC3_IN_Buffer[19])>>2;

       break;
     }
   }

}

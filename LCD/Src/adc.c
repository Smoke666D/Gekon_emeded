/*
 * adc.c
 *
 *  Created on: Aug 31, 2020
 *      Author: igor.dymov
 */

//#define ARM_MATH_CM3

#include "adc.h"
#include "arm_math.h"
#include "fix16.h"

static   EventGroupHandle_t xADCEvent;
static   StaticEventGroup_t xADCCreatedEventGroup;
volatile int16_t            ADC1_IN_Buffer[ADC_FRAME_SIZE*ADC1_CHANNELS] = { 0U };   //ADC1 input data buffer
volatile int16_t            ADC2_IN_Buffer[ADC_FRAME_SIZE*ADC2_CHANNELS] = { 0U };   //ADC2 input data buffer
volatile int16_t            ADC3_IN_Buffer[ADC_FRAME_SIZE*ADC3_CHANNELS] = { 0U };   //ADC3 input data buffer
static   q15_t              TEMP_BUFFER[ADC_FRAME_SIZE]                  = { 0U };
static   q15_t              TEMP_BUFFER1[ADC_FRAME_SIZE]                 = { 0U };
static   xADCFSMType        xADCFSM                                      = DC;
static   uint16_t           ADCDATA[8U]                                  = { 0U };


uint16_t GetAverVDD(uint8_t channel,uint8_t size)
{

   uint32_t Buffer=0;
   for (uint8_t i=0;i<size;i++)
   {
     Buffer =Buffer+ ADC3_IN_Buffer[i*9+channel];
   }
   Buffer =Buffer/size;
   return  (uint16_t)Buffer;
}


static fix16_t xVDD =0; //Напяжение АЦП канала PowInMCU (измерение напряжения питания)
 //Напряжение АЦП канала ControlSmDin (измерение верхнего опороного напряжения)
static uint16_t uCSD =0;
 //Напряжение АЦП канала Common Analog Sens (общий провод датчиков)
static uint16_t uCAS =0;
static fix16_t xSOP =0; //Напряжение АЦП канала SensOilPressure (напряжение канала давления масла)
static uint16_t uSOP =0;
static fix16_t xSCT =0; //Напряжение АЦП канала SensCoolantTemper (напряжение канала температуры охлаждающей жидкости)
static uint16_t uSCT =0;
static fix16_t xSFL =0; //Напряжение АЦП канала SensFuelLevel (напряжение канала уровня топлива)
static uint16_t uSFL =0;
static uint16_t uCSA =0;

static fix16_t xNET_F1_VDD =0;
static fix16_t xNET_F2_VDD =0;
static fix16_t xNET_F3_VDD =0;
static fix16_t xNET_FREQ =0;
static uint16_t uNetPerodCount = 0;

static fix16_t xGEN_F1_VDD =0;
static fix16_t xGEN_F2_VDD =0;
static fix16_t xGEN_F3_VDD =0;
static fix16_t xGEB_FREQ =0;
static fix16_t xGEN_F1_CUR =0;
static fix16_t xGEN_F2_CUR =0;
static fix16_t xGEN_F3_CUR =0;
static fix16_t xADC_TEMP =0;
static uint32_t ADC3Freq =10000;
static uint32_t ADC2Freq =10000;


/*
 * Сервисная функция для перевода значений АЦП в напряжения
 */
void vADCConvertToVDD ( uint8_t AnalogSwitch )
{
  fix16_t  delta    = 0U;
  uint16_t temp_int = 0U;
  xEventGroupClearBits( xADCEvent, ADC_READY );
  switch ( AnalogSwitch )
  {
    case 1U:
      //Получем среднение заничение АЦП канала питания
      temp_int = GetAverVDD( 4U, DC_SIZE );
      //Пересчитываем его в реальное напяжение.
      xVDD = fix16_mul( fix16_from_int( temp_int ), fix16_from_float( VDD_CF ) );
      //Вычитаем падение на диоде
      xVDD = fix16_sub( xVDD, VT4 );
      //Усредняем сырые значения АЦП
      uCSA = GetAverVDD( 5U, DC_SIZE );
      //Усредняем сырые значения АЦП
      uCAS = GetAverVDD( 6U, DC_SIZE );
      //Если на линии Common analog sens почти равно ControlSmAin, это означает что не у датчиков не подключен общий провод
      if ( ( uCSA - uCAS ) <= DELTA )
      {
        xSOP = fix16_from_int( MAX_RESISTANCE );
        xSCT = xSOP;
        xSFL = xSOP;
      }
      else
      {
        //Усредняем сырые значения АЦП
        uSFL = GetAverVDD( 7U, DC_SIZE );
        if ( ( uCSD - uSCT ) <= DELTA )
        {
          xSCT = 0U;
        }
        else
        {
          temp_int = ( ( uSCT - uCAS ) * R3 ) / ( uCSD - uSCT );
          xSCT = fix16_from_int( temp_int );
        }
        if ( ( uCSD - uSFL ) <= DELTA )
        {
          xSFL = 0U;
        }
        else
        {
          temp_int = ( ( uSFL - uCAS ) * R3 ) / ( uCSD - uSFL );
          xSFL = fix16_from_int( temp_int );
        }
        if ( ( uCSD - uSOP ) <= DELTA )
        {
          xSOP = 0U;
        }
        else
        {
          temp_int = ( ( uSOP - uCAS ) * R3 ) / ( uCSD - uSOP );
          xSOP = fix16_from_int( temp_int );
        }
      }
      break;
    case 0U:
      //Переводим в наряжние на канале АЦП
      uCSD = GetAverVDD( 5U, DC_SIZE );
      //Усредняем сырые значения АЦП

      uSCT= GetAverVDD( 6U, DC_SIZE );
      //Усредняем сырые значения АЦП
      uSOP = GetAverVDD( 7U, DC_SIZE );
      break;
    default:
      break;
  }
  xEventGroupSetBits( xADCEvent, ADC_READY );
  return;
}

fix16_t xADCGetSOP()
{
  return xSOP;
}
fix16_t xADCGetSCT()
{
  return xSCT;

}
fix16_t xADCGetSFL()
{
  return xSFL;
}

void vGetADCDC( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  if (cmd == mREAD)
  {
    xEventGroupWaitBits( xADCEvent, ADC_READY, pdFALSE, pdTRUE, portMAX_DELAY );
    switch ( ID - 1U )
    {
      case VDD_CH:
        fix16_to_str( xVDD, Data, 2U );
        break;
      case CFUEL:
        fix16_to_str( xADCGetSFL(), Data, 0U );
        break;
      case CPRES:
        fix16_to_str( xADCGetSOP(), Data, 0U );
        break;
      case CTEMP:
        fix16_to_str( xADCGetSCT(), Data, 0U );
        break;
      case NET_F1_VDD:
        fix16_to_str( xNET_F1_VDD, Data, 0U );
        break;
      case NET_F2_VDD:
        fix16_to_str( xNET_F2_VDD, Data, 0U );
       break;
      case NET_F3_VDD:
        fix16_to_str( xNET_F3_VDD, Data, 0U );
        break;
      case NET_FREQ:
        fix16_to_str( xNET_FREQ, Data, 1U );
        break;
      case ADC_FREQ:
        fix16_to_str(fix16_from_int( ADC3Freq/10), Data, 0U );
        break;
      case ADC_TEMP:
        fix16_to_str( xADC_TEMP, Data, 0U );
      default:
        break;
    }
  }
  return;
}

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim2;
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;

void vGetChannel(q15_t * dest, int16_t * source, uint8_t off, uint16_t size);

uint16_t  fADC3Init(uint16_t freq)
{

  uint16_t Period = 60000000U / ( freq  );
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
  return Period;
}







float  fADC12Init(uint16_t freq)
{

    uint16_t Period = 60000000U/ (freq*4);
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



float  fADC1Init(uint16_t freq)
{

  uint16_t Period = 60000000U/ (freq*4);
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    /* USER CODE BEGIN TIM8_Init 1 */

    /* USER CODE END TIM8_Init 1 */
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 0;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = Period;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
    htim2.Init.RepetitionCounter = 0;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
      Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
    {
      Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
      Error_Handler();
    }
  HAL_TIM_Base_Start_IT( &htim2 );
  return (float)(1/Period);
}

/*
 *
 *
 */
void vADC3DCInit(xADCFSMType xADCType)
{

  ADC_ChannelConfTypeDef sConfig = {0};

  HAL_ADC_DeInit(&hadc3);

  hadc3.Instance = ADC3;
  hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc3.Init.Resolution = ADC_RESOLUTION_12B;
  hadc3.Init.ScanConvMode = ENABLE;
  hadc3.Init.ContinuousConvMode = DISABLE;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc3.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;
  hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;

  hadc3.Init.DMAContinuousRequests = ENABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (xADCType == DC)
      hadc3.Init.NbrOfConversion = 9;
  else
     hadc3.Init.NbrOfConversion = 4;

  HAL_ADC_Init(&hadc3);

}


void vADCInit(void)
{
  switch (xADCFSM)
  {
    case DC:
      HAL_GPIO_WritePin( ANALOG_SWITCH_GPIO_Port,ANALOG_SWITCH_Pin, GPIO_PIN_SET );
      HAL_GPIO_WritePin( ON_INPOW_GPIO_Port,ON_INPOW_Pin, GPIO_PIN_SET );
      vADC3DCInit(DC);

      fADC3Init(ADC3Freq);
      fADC12Init(ADC2Freq);
      fADC1Init(ADC2Freq);
      break;
    case AC:
      fADC12Init(ADC2Freq);
      fADC3Init(ADC3Freq);
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

void vADC_Ready ( uint8_t adc_number )
{
  static portBASE_TYPE xHigherPriorityTaskWoken;
  /* Process locked */
  EventBits_t t;
  xHigherPriorityTaskWoken = pdFALSE;
  switch ( adc_number )
  {
    case ADC3_READY:
      xEventGroupSetBitsFromISR( xADCEvent, ADC3_READY, &xHigherPriorityTaskWoken );
      break;
    case ADC2_READY:
      xEventGroupSetBitsFromISR( xADCEvent, ADC2_READY, &xHigherPriorityTaskWoken );
      break;
    case ADC1_READY:
      xEventGroupSetBitsFromISR( xADCEvent, ADC1_READY, &xHigherPriorityTaskWoken );
      break;
    default:
      break;
  }
  t= xEventGroupGetBitsFromISR(xADCEvent);
  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
  return;
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

   __HAL_LOCK(hadc);
   __HAL_ADC_DISABLE(hadc);
   __HAL_UNLOCK(hadc);
    if (hadc->Instance == ADC3)
    {
        __HAL_TIM_DISABLE(&htim3);
    }
    if (hadc->Instance == ADC2)
    {
        __HAL_TIM_DISABLE(&htim8);
    }
    if (hadc->Instance == ADC1)
      {
          __HAL_TIM_DISABLE(&htim2);
      }
   /* Start the DMA channel */
   HAL_DMA_Start_IT(hadc->DMA_Handle, (uint32_t)&hadc->Instance->DR, (uint32_t)pData, Length);

   /* Enable the Peripheral */
   __HAL_ADC_ENABLE(hadc);

   osDelay(3);




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
    if (hadc->Instance == ADC3)
    {
      __HAL_TIM_ENABLE(&htim3);
    }
    else
      if (hadc->Instance == ADC2)
      {
        __HAL_TIM_ENABLE(&htim8);
      }
      else
        if (hadc->Instance == ADC1)
        {
          __HAL_TIM_ENABLE(&htim2);
        }
    /* Return function status */
    return HAL_OK;

}

void StartADCTask(void *argument)
{
   static uint8_t OF =0,OF1=0;
   static uint16_t adctime=0;
  //Создаем флаг готовности АПЦ
   xADCEvent = xEventGroupCreateStatic(&xADCCreatedEventGroup );
   vADCInit();
   InitADCDMA(&hadc3);
   InitADCDMA(&hadc2);
   InitADCDMA(&hadc1);


   for(;;)
   {
    osDelay( 100U );
    vADC3DCInit( DC );
    StartADCDMA( &hadc3, ( uint32_t* )&ADC3_IN_Buffer, ( DC_SIZE * 9U ) );         /* Запускаем преобразвоание АЦП */
    xEventGroupWaitBits( xADCEvent, ADC3_READY, pdTRUE, pdTRUE, portMAX_DELAY);   /* Ожидаем флага готовонсти о завершении преобразования */
    ADCDATA[4] = (ADC3_IN_Buffer[8]+ADC3_IN_Buffer[17]+ADC3_IN_Buffer[26]+ADC3_IN_Buffer[35])>>2;
    vADCConvertToVDD(0);
    //Переключаем аналоговый комутатор и ждем пока напряжения за комутатором стабилизируются
    HAL_GPIO_WritePin( ANALOG_SWITCH_GPIO_Port,ANALOG_SWITCH_Pin, GPIO_PIN_SET );
    osDelay(10);
    //Запускаем новоей преобразование
    StartADCDMA(&hadc3,(uint32_t*)&ADC3_IN_Buffer,DC_SIZE*9);
    xEventGroupWaitBits(xADCEvent,ADC3_READY,pdTRUE,pdTRUE,portMAX_DELAY);

    ADCDATA[4] = (ADC3_IN_Buffer[8]+ADC3_IN_Buffer[17]+ADC3_IN_Buffer[26]+ADC3_IN_Buffer[35])>>2;
    vADCConvertToVDD(1);


    //Переключаем обратно аналоговый коммутатор
    HAL_GPIO_WritePin( ANALOG_SWITCH_GPIO_Port,ANALOG_SWITCH_Pin, GPIO_PIN_RESET );

    //Переключем АЦП в режим имзерения AC каналов
    vADC3DCInit(AC);
    osDelay(1);
    //Запускаем преобразвоания по всем каналам АЦП
    StartADCDMA(&hadc2,(uint32_t*)&ADC2_IN_Buffer,ADC_FRAME_SIZE*ADC2_CHANNELS);
    StartADCDMA(&hadc1,(uint32_t*)&ADC1_IN_Buffer,ADC_FRAME_SIZE*ADC1_CHANNELS);
    StartADCDMA(&hadc3,(uint32_t*)&ADC3_IN_Buffer,ADC_FRAME_SIZE*ADC3_CHANNELS);
    xEventGroupWaitBits(xADCEvent,ADC3_READY | ADC2_READY | ADC1_READY,pdTRUE,pdTRUE,portMAX_DELAY);
    adctime = HAL_GetTick();
    //Обработка значений АЦП3.
     switch (vADCGetADC3Data())
     {

       case HIGH_FREQ:
         if (OF>0)
         {
           ADC3Freq = ADC3Freq - ADC3Freq/10;
           if (ADC3Freq <2000)  ADC3Freq = 2000;
           fADC3Init(ADC3Freq);
           OF=0;
         }
         OF++;
         break;
       case LOW_FREQ:
         if (OF>0)
         {
           ADC3Freq = ADC3Freq + ADC3Freq/4;
           if (ADC3Freq >70000)  ADC3Freq = 40000;
           fADC3Init(ADC3Freq);
           OF=0;
         }
         OF++;
         break;
       default:
         break;
     }

    switch (vADCGetADC12Data())
    {

      case LOW_FREQ:
        if (OF1>0)
                 {
                   ADC2Freq = ADC2Freq - ADC2Freq/10;
                   if (ADC2Freq <2000)  ADC2Freq = 2000;
                   fADC1Init(ADC2Freq);
                   fADC12Init(ADC2Freq);

                   OF1=0;
                 }
                 OF1++;
                 break;
           break;
         case HIGH_FREQ:
           if (OF1>0)
                   {
                     ADC2Freq = ADC2Freq + ADC2Freq/4;
                     if (ADC2Freq >70000)  ADC2Freq = 40000;
                     fADC1Init(ADC2Freq);
                     fADC12Init(ADC2Freq);
                     OF1=0;
                   }
                   OF1++;
           break;
         default:
           break;


    }
 //  xADC_TEMP = HAL_GetTick() - adctime;

   }

}
/*
 *  Функция получения из буффера DMA непрерывных данных указанного канала данных
 */
void vGetChannel(q15_t * dest, int16_t * source, uint8_t off, uint16_t size)
{
  for (uint16_t i=0;i<size;i++)
  {
    dest[i] = source[4*i+off];
  }
  return;
}

void vDecNetural(int16_t * data)
{

 for (int16_t index = 0;index<ADC_FRAME_SIZE;index++)
 {
   data[4*index]   = data[4*index]   - data[4*index+4];
   data[4*index+1] = data[4*index+1] - data[4*index+4];
   data[4*index+2] = data[4*index+2] - data[4*index+4];
 }
  return;
}

#define AMP_DELTA 15
#define MAX_ZERO_POINT 20
#define FD  3

uint8_t vADCFindFreq(int16_t * data, uint16_t * count)
{
  uint8_t F1=0,F2=0;
  uint16_t tt=0;
  uint8_t CNT=0,index = 0,res =ADC_ERROR;
  uint16_t PER[MAX_ZERO_POINT];
  for (uint16_t i=FD;i<ADC_FRAME_SIZE-FD;i++)
  {
    //Если значение попадет в корридор окло нуля
    if ((data[i] > -AMP_DELTA) && (data[i] < AMP_DELTA))
    {
        //то прверяем текущую фазу
        if ((data[i] > data[i-FD]) || (data[i] < data[i+FD]))
            F2  = 1U;
        else
            F2 = 0U;
        if (CNT > 0)
        {
          if (((F2==1) && (F1 == 1)) || ((F2==0) &&(F1==0)))
          {
            index = 5;
            break;
          }
        }
        F1 = F2;
        PER[index++] =i;
        i = i+ 20;
        CNT++;
        if (index> MAX_ZERO_POINT) break;
    }
  }

  if ((index > 2) && (index <5))
  {
   tt =0;
   for (uint8_t i =1;i<index;i++)
   {
     tt =tt+ + PER[i]-PER[i-1];
   }
   tt = (tt/(index-1))*2;
   *count = tt;
    res = ADC_OK;
  }
  else
  {
    *count = ADC_FRAME_SIZE;
    if (index < 3) res = HIGH_FREQ;
    if (index > 4) res = LOW_FREQ;
  }
  return res;
}



uint8_t vADCGetADC3Data()
{
  uint8_t result=ADC_ERROR;
  uint16_t uCurPeriod = ADC_FRAME_SIZE-1;
  q15_t  RES =0;
  uint32_t Index =0;
  int16_t maxdata;
  vDecNetural(&ADC3_IN_Buffer);
  vGetChannel(&TEMP_BUFFER,&ADC3_IN_Buffer,2,uCurPeriod); //Разврачиваем данные нужного канала АЦП в неперерывный массив данных

  arm_max_q15(&TEMP_BUFFER,uCurPeriod,&RES,&Index);
  maxdata = xADCMax(  &ADC3_IN_Buffer, 2, uCurPeriod );

  xADC_TEMP = fix16_mul(fix16_from_int(maxdata) ,fix16_from_float( AC_COOF ) );
  xADC_TEMP  =fix16_div(xADC_TEMP, fix16_sqrt(fix16_from_int(2) ) );
 // arm_max_q15(&TEMP_BUFFER,uCurPeriod,&RES,&Index);          //Проверям есть ли на канале напряжение.
  if( RES >= MIN_AMP_VALUE )
  {
        result =  vADCFindFreq(&TEMP_BUFFER, &uCurPeriod);
        if (result==ADC_OK)
        {
          xNET_FREQ = fix16_div(fix16_from_int(ADC3Freq/10),fix16_from_int(uCurPeriod));
          xNET_FREQ= fix16_mul(xNET_FREQ,fix16_from_int(10));
          xNET_F1_VDD = fix16_mul( xADCRMS(&ADC3_IN_Buffer, 2, uCurPeriod ), fix16_from_float( AC_COOF ) );
        }
        else
        {
          return result;
        }
 }
 else
 {
   xNET_F1_VDD =0;
 }
 //Разврачиваем данные нужного канала АЦП в неперерывный массив данных
 vGetChannel(&TEMP_BUFFER,&ADC3_IN_Buffer,1,uCurPeriod);
 //Проверям есть ли на канале напряжение.
 arm_max_q15(&TEMP_BUFFER,uCurPeriod,&RES,&Index);
 if( RES >= MIN_AMP_VALUE )
 {
      if (result!=ADC_OK)
      {
         result = vADCFindFreq(&TEMP_BUFFER, &uCurPeriod);
         if (result==ADC_OK)
         {
            xNET_FREQ = fix16_div(fix16_from_int(ADC3Freq/10),fix16_from_int(uCurPeriod));
            xNET_FREQ= fix16_mul(xNET_FREQ,fix16_from_int(10));
         }
         else
         {
           return result;
         }
      }
      xNET_F2_VDD = fix16_mul( xADCRMS(&ADC3_IN_Buffer, 1, uCurPeriod ), fix16_from_float( AC_COOF ) );
 }
 else
 {
   xNET_F2_VDD = 0;
  }
  //Получаем данные 3-й фазы сети
  vGetChannel(&TEMP_BUFFER,&ADC3_IN_Buffer,0,uCurPeriod);
  //Проверям есть ли на канале напряжение.
  arm_max_q15(&TEMP_BUFFER,uCurPeriod,&RES,&Index);
  if( RES >= MIN_AMP_VALUE )
  {
         if (result!=ADC_OK)
         {
            result = vADCFindFreq(&TEMP_BUFFER, &uCurPeriod);
            if (result==ADC_OK)
            {
               xNET_FREQ = fix16_div(fix16_from_int(ADC3Freq/10),fix16_from_int(uCurPeriod));
               xNET_FREQ= fix16_mul(xNET_FREQ,fix16_from_int(10));
            }
            else
            {
              return result;
            }
         }
        xNET_F3_VDD = fix16_mul( xADCRMS(&ADC3_IN_Buffer, 0, uCurPeriod ), fix16_from_float( AC_COOF ) );
  }
  else
  {
    xNET_F3_VDD = 0;
  }
  return result;
}

uint8_t vADCGetADC12Data()
{
  uint16_t result=ADC_ERROR;
  uint16_t uCurPeriod = ADC_FRAME_SIZE;
   q15_t  RES =0;
   uint32_t Index =0;

   //Вычитаем из фаз, значение на линии нейтрали
     vDecNetural(&ADC2_IN_Buffer);

     vGetChannel(&TEMP_BUFFER,&ADC2_IN_Buffer,0,uCurPeriod);
     arm_max_q15(&TEMP_BUFFER,uCurPeriod,&RES,&Index);          //Проверям есть ли на канале напряжение.
     if( RES >= MIN_AMP_VALUE )
     {
           result =  vADCFindFreq(&TEMP_BUFFER, &uCurPeriod);
           if (result==ADC_OK)
           {
             xNET_FREQ = fix16_div(fix16_from_int(ADC2Freq/10),fix16_from_int(uCurPeriod));
             xNET_FREQ=  fix16_mul(xNET_FREQ,fix16_from_int(10));
             xGEN_F3_VDD= fix16_mul( xADCRMS(&ADC3_IN_Buffer, 0, uCurPeriod ), fix16_from_float( AC_COOF ) );
           }
           else
             goto CUR;
    }
    else
    {
      xGEN_F3_VDD =0;
    }

    vGetChannel(&TEMP_BUFFER,&ADC2_IN_Buffer,1,uCurPeriod);
    arm_max_q15(&TEMP_BUFFER,uCurPeriod,&RES,&Index);          //Проверям есть ли на канале напряжение.
    if( RES >= MIN_AMP_VALUE )
    {
           if (result!=ADC_OK)
           {
              result =  vADCFindFreq(&TEMP_BUFFER, &uCurPeriod);
              if (result==ADC_OK)
              {
                xNET_FREQ = fix16_div(fix16_from_int(ADC2Freq/10),fix16_from_int(uCurPeriod));
                xNET_FREQ= fix16_mul(xNET_FREQ,fix16_from_int(10));
              }
              else
              {
                goto CUR;
              }
           }
           xGEN_F2_VDD= fix16_mul( xADCRMS(&ADC3_IN_Buffer, 1, uCurPeriod ), fix16_from_float( AC_COOF ) );
     }
     else
     {
        xGEN_F2_VDD =0;
     }


      vGetChannel(&TEMP_BUFFER,&ADC2_IN_Buffer,2,uCurPeriod);
      arm_max_q15(&TEMP_BUFFER,uCurPeriod,&RES,&Index);          //Проверям есть ли на канале напряжение.
       if( RES >= MIN_AMP_VALUE )
       {

                  if (result!=ADC_OK)
                  {
                     result =  vADCFindFreq(&TEMP_BUFFER, &uCurPeriod);
                    if (result==ADC_OK)
                    {
                      xNET_FREQ = fix16_div(fix16_from_int(ADC2Freq/10),fix16_from_int(uCurPeriod));
                      xNET_FREQ= fix16_mul(xNET_FREQ,fix16_from_int(10));
                    }
                    else

                      goto CUR;
                  }
                  xGEN_F1_VDD= fix16_mul( xADCRMS(&ADC3_IN_Buffer, 2, uCurPeriod ), fix16_from_float( AC_COOF ) );


      }
      else
      {
         xGEN_F1_VDD =0;
      }
   CUR:
   if (result==ADC_OK)
   {
     xGEN_F3_CUR =  fix16_mul( xADCRMS(&ADC1_IN_Buffer, 0, uCurPeriod ), fix16_from_float( AC_COOF ) );
     xGEN_F2_CUR =  fix16_mul( xADCRMS(&ADC1_IN_Buffer, 1, uCurPeriod ), fix16_from_float( AC_COOF ) );
     xGEN_F1_CUR =  fix16_mul( xADCRMS(&ADC1_IN_Buffer, 2, uCurPeriod ), fix16_from_float( AC_COOF ) );
   }

  return result;

}


fix16_t  xADCRMS(int16_t * source, uint8_t off, uint16_t size )
{
  uint64_t sum =0;
  for (uint16_t i=0;i<size;i++)
  {
    sum =sum+ source[i*4+off]*source[i*4+off];

  }
  sum =sum/size;
  return fix16_from_int (sqrt(sum));

}

int16_t  xADCMax(int16_t * source, uint8_t off, uint16_t size )
{

  int16_t max =0;
  for (uint16_t i=0;i<size;i++)
  {
      if ( source[i*4 + off] >  max)
        max = source[i*4 + off];
  }
  return max;

}

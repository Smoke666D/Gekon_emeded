/*
 * adc.c
 *
 *  Created on: Aug 31, 2020
 *      Author: igor.dymov
 */

//#define ARM_MATH_CM3

#include "adc.h"


static   EventGroupHandle_t xADCEvent;
static   StaticEventGroup_t xADCCreatedEventGroup;
volatile int16_t            ADC1_IN_Buffer[ADC_FRAME_SIZE*ADC1_CHANNELS] = { 0U };   //ADC1 input data buffer
volatile int16_t            ADC2_IN_Buffer[ADC_FRAME_SIZE*ADC2_CHANNELS] = { 0U };   //ADC2 input data buffer
volatile int16_t            ADC3_IN_Buffer[ADC_FRAME_SIZE*ADC3_CHANNELS] = { 0U };   //ADC3 input data buffer
static   uint16_t           ADCDATA[8U]                                  = { 0U };
static   uint8_t            ADC_VALID_DATA                               =  0;

uint8_t  vADCGetADC3Data();
uint8_t  vADCGetADC12Data();
void     vADCInit(void);
uint8_t  vADCFindFreq(int16_t * data, uint16_t * count,uint8_t off, int16_t AMP);
fix16_t  xADCRMS(int16_t * source, uint8_t off, uint16_t size, uint8_t cc  );
int16_t  xADCMax( int16_t * source, uint8_t off, uint16_t size , uint16_t * delay, uint8_t cc );
uint16_t GetAverVDD(uint8_t channel,uint8_t size,uint8_t offset,int16_t * source);
void     vADCSetFreqDetectParam(int16_t AMP,uint8_t * del,uint8_t * fd);
void     vADC3FrInit(uint16_t freq);
void     vADC12FrInit(uint16_t freq);
void     vADCConvertToVDD(uint8_t AnalogSwitch);
void     vDecNetural(int16_t * data);

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim2;
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;




static uint16_t uVDD =0;  //Значение АЦП канала PowInMCU (измерение напряжения питания)

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
static uint16_t uTemp =0;
static fix16_t xNET_F1_VDD =0;
static fix16_t xNET_F2_VDD =0;
static fix16_t xNET_F3_VDD =0;
static fix16_t xNET_FREQ =0;
static fix16_t xGEN_F1_VDD =0;
static fix16_t xGEN_F2_VDD =0;
static fix16_t xGEN_F3_VDD =0;
static fix16_t xGEN_FREQ =0;
static fix16_t xGEN_F1_CUR =0;
static fix16_t xGEN_F2_CUR =0;
static fix16_t xGEN_F3_CUR =0;
static fix16_t xADC_TEMP =0;
static uint32_t ADC3Freq =10000;
static uint32_t ADC2Freq =10000;
static uint8_t uNetFaseRotation = NO_ROTATION;
static uint8_t uGenFaseRotation = NO_ROTATION;
static fix16_t xCosFi =0;
static uint8_t xNetWiring  =STAR;



/*
 *  Константы
 */
//static const fix16_t  xCurTrasforCoof = F16( RCSHUNT * OPTRANSCOOF );  //Коофициент преобазования тока на шунтирующих резиторах токовых входов в напряжение АЦП
//static const fix16_t  xADCVCoof       = F16 ( VRef  /4095);                //Вес одного отсчета АЦП в расчете на пряжение
static const fix16_t  xLCurCoof       = F16 (( VRef /4095)/(RCSHUNT * OPTRANSCOOF) );  //Коофицент пересчета значений токовых АЦП в ток на шунтирующих ризисторах
static const fix16_t  xMinus1         = F16 (-1.0);
static const fix16_t  x3              = F16 (3);
static const fix16_t  xVDD_CF         = F16 (VDD_CF);

/*
 * API функции
 *
 */

/*
 *  Функция возращает наряжения АКБ.
 */
fix16_t xADCGetVDD()
{
  fix16_t xVDD =0;
  xEventGroupWaitBits(xADCEvent,DC_READY,pdTRUE,pdTRUE,5);
  //Пересчитываем его в реальное напяжение.
  xVDD = fix16_mul( fix16_from_int( uVDD ),  xVDD_CF );
 //Вычитаем падение на диоде
  xVDD = fix16_sub( xVDD, VT4 );
  return xVDD;
}

fix16_t xADCGetSOP()
{
  xEventGroupWaitBits(xADCEvent,DC_READY,pdTRUE,pdTRUE,5);
  return xSOP;
}
fix16_t xADCGetSCT()
{
  xEventGroupWaitBits(xADCEvent,DC_READY,pdTRUE,pdTRUE,5);
  return xSCT;
}
fix16_t xADCGetSFL()
{
  xEventGroupWaitBits(xADCEvent,DC_READY,pdTRUE,pdTRUE,5);
  return xSFL;
}

fix16_t xADCGetNETLFreq()
{
  xEventGroupWaitBits(xADCEvent,NET_READY,pdTRUE,pdTRUE,5);
  return xNET_FREQ;
}


fix16_t xADCGetNETL1()
{
  xEventGroupWaitBits(xADCEvent,NET_READY,pdTRUE,pdTRUE,5);
  return xNET_F1_VDD;
}
fix16_t xADCGetNETL2()
{
  xEventGroupWaitBits(xADCEvent,NET_READY,pdTRUE,pdTRUE,5);
  return xNET_F2_VDD;

}
fix16_t xADCGetNETL3()
{
  xEventGroupWaitBits(xADCEvent,NET_READY,pdTRUE,pdTRUE,5);
  return xNET_F3_VDD;
}

fix16_t xADCGetNETL1Lin()
{
  xEventGroupWaitBits(xADCEvent,NET_READY,pdTRUE,pdTRUE,5);
  if (xNetWiring==STAR)
  {
    return fix16_mul(xNET_F1_VDD, fix16_sqrt(x3));
  }
  return xNET_F1_VDD;

}
fix16_t xADCGetNETL2Lin()
{
  xEventGroupWaitBits(xADCEvent,NET_READY,pdTRUE,pdTRUE,5);
  if (xNetWiring==STAR)
   {
    return fix16_mul(xNET_F2_VDD, fix16_sqrt(x3));
   }
  return xNET_F2_VDD;

}
fix16_t xADCGetNETL3Lin()
{
  xEventGroupWaitBits(xADCEvent,NET_READY,pdTRUE,pdTRUE,5);
  if (xNetWiring==STAR)
  {
      return fix16_mul(xNET_F3_VDD, fix16_sqrt(x3));
  }
  return xNET_F3_VDD;
}


fix16_t xADCGetGENL1Cur()
{
  xEventGroupWaitBits(xADCEvent,CUR_READY,pdTRUE,pdTRUE,5);

  return xGEN_F1_CUR;
}
fix16_t xADCGetGENL2Cur()
{
  xEventGroupWaitBits(xADCEvent,CUR_READY,pdTRUE,pdTRUE,5);
  return xGEN_F2_CUR;
}
fix16_t xADCGetGENL3Cur()
{
  xEventGroupWaitBits(xADCEvent,CUR_READY,pdTRUE,pdTRUE,5);
  return xGEN_F3_CUR;
}


fix16_t xADCGetGENL1CurLin()
{

  xEventGroupWaitBits(xADCEvent,CUR_READY,pdTRUE,pdTRUE,5);
  if (xNetWiring== TRIANGLE )
  {
    return fix16_mul(xGEN_F1_CUR, fix16_sqrt(x3));

  }

  return xGEN_F1_CUR;
}
fix16_t xADCGetGENL2CurLin()
{
  xEventGroupWaitBits(xADCEvent,CUR_READY,pdTRUE,pdTRUE,5);
  if (xNetWiring== TRIANGLE )
   {
     return fix16_mul(xGEN_F2_CUR, fix16_sqrt(x3));

   }
  return xGEN_F2_CUR;
}
fix16_t xADCGetGENL3CurLin()
{
  xEventGroupWaitBits(xADCEvent,CUR_READY,pdTRUE,pdTRUE,5);
  if (xNetWiring== TRIANGLE )
   {
     return fix16_mul(xGEN_F3_CUR, fix16_sqrt(x3));

   }
  return xGEN_F3_CUR;
}


fix16_t xADCGetGENL1()
{
  xEventGroupWaitBits(xADCEvent,GEN_READY,pdTRUE,pdTRUE,5);
  return xGEN_F1_VDD;
}
fix16_t xADCGetGENL1Lin()
{
  xEventGroupWaitBits(xADCEvent,GEN_READY,pdTRUE,pdTRUE,5);
  if (xNetWiring==STAR)
  {
      return fix16_mul(xGEN_F1_VDD, fix16_sqrt(x3 ));
  }
  return xGEN_F1_VDD;
}

fix16_t xADCGetGENL2()
{
  xEventGroupWaitBits(xADCEvent,GEN_READY,pdTRUE,pdTRUE,5);
  return xGEN_F2_VDD;

}

fix16_t xADCGetGENL2Lin()
{
  xEventGroupWaitBits(xADCEvent,GEN_READY,pdTRUE,pdTRUE,5);
  if (xNetWiring==STAR)
  {
      return fix16_mul(xGEN_F2_VDD, fix16_sqrt(x3 ));
  }
  return xGEN_F2_VDD;
}

fix16_t xADCGetGENL3()
{
  xEventGroupWaitBits(xADCEvent,GEN_READY,pdTRUE,pdTRUE,5);
  return xGEN_F3_VDD;
}

fix16_t xADCGetGENL3Lin()
{
  xEventGroupWaitBits(xADCEvent,GEN_READY,pdTRUE,pdTRUE,5);
  if (xNetWiring==STAR)
  {
      return fix16_mul(xGEN_F3_VDD, fix16_sqrt(x3 ));
  }
  return xGEN_F3_VDD;
}
fix16_t xADCGetGENLFreq()
{
  xEventGroupWaitBits(xADCEvent,GEN_READY,pdTRUE,pdTRUE,5);
  return xGEN_FREQ;
}

fix16_t xADCGetCOSFi()
{
  xEventGroupWaitBits(xADCEvent,CUR_READY,pdTRUE,pdTRUE,5);
  return xCosFi;

}

/*
 *  Функция возвращает мговенную мощность переменного тока
 */
fix16_t xADCGetPower()
{
  fix16_t temp;
  if ( xNetWiring==ONE_FASE )
  {
     temp = xADCGetGENL1Lin();
  }
  else
  {
    temp = fix16_mul(fix16_sqrt(fix16_from_int(3)),xADCGetGENL1Lin());
  }
  temp = fix16_mul(temp,xADCGetGENL1CurLin());
  temp = fix16_mul(temp, xADCGetCOSFi());
  return temp;

}


uint8_t uADCGetValidDataFlag()
{
  xEventGroupWaitBits(xADCEvent,DC_READY,pdTRUE,pdTRUE,5);
  return ADC_VALID_DATA;
}

uint8_t uADCGetGenFaseRotation()
{
  return uGenFaseRotation;

}

uint8_t uADCGetNetFaseRotation()
{
  return uNetFaseRotation;

}

/*
 * Сервисная функция для перевода значений АЦП в напряжения
 */
void vADCConvertToVDD ( uint8_t AnalogSwitch )
{

  uint16_t temp_int = 0U;

  xEventGroupClearBits( xADCEvent, DC_READY );
  switch ( AnalogSwitch )
  {
    case 1U:
      uVDD = GetAverVDD( 4U, DC_SIZE,9,(int16_t *)&ADC3_IN_Buffer  );   //Получем среднение заничение АЦП канала питания
      uCSA = GetAverVDD( 5U, DC_SIZE,9,(int16_t *)&ADC3_IN_Buffer );    //Усредняем сырые значения АЦП
      uCAS = GetAverVDD( 6U, DC_SIZE, 9,(int16_t *)&ADC3_IN_Buffer );   //Усредняем сырые значения АЦП канала CommonAnalogSensor

      uTemp = GetAverVDD( 3U, DC_SIZE, 4,(int16_t *)&ADC1_IN_Buffer );
      xADC_TEMP = fix16_sub(  fix16_from_float(uTemp*3.3/4095U), fix16_from_float( 0.76 ) );
      xADC_TEMP = fix16_div( xADC_TEMP, fix16_from_float(0.0025) );
      xADC_TEMP = fix16_add(xADC_TEMP, fix16_from_int( 25 ) );
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
        uSCT = GetAverVDD( 7U, DC_SIZE ,9,(int16_t *)&ADC3_IN_Buffer);

        if ( ( ( uCSD - uSCT ) <= DELTA ) || (uSCT <  uCAS) )
        {
          xSCT = 0U;
        }
        else
        {
          temp_int = ( ( uSCT - uCAS ) * R3 ) / ( uCSD - uSCT );
          xSCT = fix16_from_int( temp_int );
        }
        if ( ( ( uCSD - uSFL ) <= DELTA ) || ( uSFL  < uCAS ) )
        {
          xSFL = 0U;
        }
        else
        {
          temp_int = ( ( uSFL - uCAS ) * R3 ) / ( uCSD - uSFL );
          xSFL = fix16_from_int( temp_int );
        }
        if ( ( ( uCSD - uSOP ) <= DELTA ) || ( uSOP < uCAS ) )
        {
          xSOP = 0U;
        }
        else
        {
          temp_int = ( ( uSOP - uCAS ) * R3 ) / ( uCSD - uSOP );
          xSOP = fix16_from_int( temp_int );
        }
      }
      ADC_VALID_DATA =1;
      //Переключаем обратно аналоговый коммутатор
      HAL_GPIO_WritePin( ANALOG_SWITCH_GPIO_Port,ANALOG_SWITCH_Pin, GPIO_PIN_RESET );
      break;
    case 0U:
      //Переводим в наряжние на канале АЦП
      uCSD = GetAverVDD( 5U, DC_SIZE,9,(int16_t *)&ADC3_IN_Buffer );
      //Усредняем сырые значения АЦП

      uSFL = GetAverVDD( 6U, DC_SIZE,9,(int16_t *)&ADC3_IN_Buffer );
      //Усредняем сырые значения АЦП
      uSOP = GetAverVDD( 7U, DC_SIZE ,9,(int16_t *)&ADC3_IN_Buffer);
      //Переключаем аналоговый комутатор и ждем пока напряжения за комутатором стабилизируются
      HAL_GPIO_WritePin( ANALOG_SWITCH_GPIO_Port,ANALOG_SWITCH_Pin, GPIO_PIN_SET );
      osDelay(10);
      break;
    default:
      break;
  }
  xEventGroupSetBits( xADCEvent, DC_READY );
  return;
}




void vGetADCDC( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  if (cmd == mREAD)
  {
    switch ( ID - 1U )
    {
      case ADC_FREQ:
        fix16_to_str(fix16_from_int( ADC3Freq/10), Data, 0U );
        break;
      case ADC_TEMP:
        fix16_to_str( xADC_TEMP, Data, 1U );
        break;
      default:
        break;
    }
  }
  return;
}



void  vADC3FrInit(uint16_t freq)
{
  htim3.Init.Period = 60000000U / ( freq  );
  HAL_TIM_Base_Init(&htim3);
  HAL_TIM_Base_Start_IT( &htim3 );
  return;
}


void  vADC12FrInit(uint16_t freq)
{

    htim8.Init.Period = 120000000U/ (freq);
    HAL_TIM_Base_Init(&htim8);

    htim2.Init.Period = 60000000U / ( freq  );
    HAL_TIM_Base_Init(&htim2);

    HAL_TIM_Base_Start_IT( &htim2 );
    HAL_TIM_Base_Start_IT( &htim8 );
    return;
}



/*
 *
 *
 */
void vADC3DCInit(xADCFSMType xADCType)
{

  HAL_ADC_DeInit(&hadc3);
  HAL_ADC_DeInit(&hadc1);
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


  if (xADCType == DC)
        hadc1.Init.NbrOfConversion = 4;
    else
       hadc1.Init.NbrOfConversion = 3;

    HAL_ADC_Init(&hadc1);

}





void vADC_Ready ( uint8_t adc_number )
{
  static portBASE_TYPE xHigherPriorityTaskWoken;

  xHigherPriorityTaskWoken = pdFALSE;
  switch ( adc_number )
  {
    case ADC3_READY:
      __HAL_TIM_DISABLE(&htim3);
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

    /* Return function status */
    return;

}



void vADCInit(void)
{
      HAL_GPIO_WritePin( ON_INPOW_GPIO_Port,ON_INPOW_Pin, GPIO_PIN_SET );
      HAL_GPIO_WritePin( ANALOG_SWITCH_GPIO_Port,ANALOG_SWITCH_Pin, GPIO_PIN_RESET );
      vADC3DCInit(DC);
      vADC3FrInit(ADC3Freq);
      vADC12FrInit(ADC2Freq);
      hadc3.DMA_Handle->XferCpltCallback =ADC_DMAConv;
      hadc2.DMA_Handle->XferCpltCallback =ADC_DMAConv;
      hadc1.DMA_Handle->XferCpltCallback =ADC_DMAConv;
      hadc3.DMA_Handle->XferHalfCpltCallback = NULL;//ADC_DMAHalfConvCplt;
      hadc2.DMA_Handle->XferHalfCpltCallback = NULL;//ADC_DMAHalfConvCplt;
      hadc1.DMA_Handle->XferHalfCpltCallback = NULL;//ADC_DMAHalfConvCplt;
      hadc3.DMA_Handle->XferErrorCallback = ADC_DMAErro;
      hadc2.DMA_Handle->XferErrorCallback = ADC_DMAErro;
      hadc1.DMA_Handle->XferErrorCallback = ADC_DMAErro;
      ADC_VALID_DATA =0;
      osDelay( 100U );
}


void StartADCTask(void *argument)
{
   static uint8_t OF =0,OF1=0;

  //Создаем флаг готовности АПЦ
   xADCEvent = xEventGroupCreateStatic(&xADCCreatedEventGroup );
   //Иницилиазация АЦП
   vADCInit();

   for(;;)
   {
    osDelay( 100U );
    vADC3DCInit( DC );
    StartADCDMA( &hadc3, ( uint32_t* )&ADC3_IN_Buffer, ( DC_SIZE * 9U ) );         /* Запускаем преобразвоание АЦП */
    __HAL_TIM_ENABLE(&htim3);

    xEventGroupWaitBits( xADCEvent, ADC3_READY, pdTRUE, pdTRUE, portMAX_DELAY);   /* Ожидаем флага готовонсти о завершении преобразования */

    ADCDATA[4] = (ADC3_IN_Buffer[8]+ADC3_IN_Buffer[17]+ADC3_IN_Buffer[26]+ADC3_IN_Buffer[35])>>2;

    vADCConvertToVDD(0);
    //Запускаем новоей преобразование
    StartADCDMA(&hadc3,(uint32_t*)&ADC3_IN_Buffer,DC_SIZE*9);
    StartADCDMA(&hadc1,(uint32_t*)&ADC1_IN_Buffer,DC_SIZE*4);
    __HAL_TIM_ENABLE(&htim3);
    __HAL_TIM_ENABLE(&htim2);

    xEventGroupWaitBits(xADCEvent,ADC3_READY | ADC1_READY ,pdTRUE,pdTRUE,portMAX_DELAY);

    ADCDATA[4] = (ADC3_IN_Buffer[8]+ADC3_IN_Buffer[17]+ADC3_IN_Buffer[26]+ADC3_IN_Buffer[35])>>2;

    vADCConvertToVDD(1);
    //Переключем АЦП в режим имзерения AC каналов
    vADC3DCInit(AC);
    osDelay(1);

    //Запускаем преобразвоания по всем каналам АЦП
    StartADCDMA(&hadc2,(uint32_t*)&ADC2_IN_Buffer,ADC_FRAME_SIZE*ADC2_CHANNELS);
    StartADCDMA(&hadc1,(uint32_t*)&ADC1_IN_Buffer,ADC_FRAME_SIZE*ADC1_CHANNELS);
    StartADCDMA(&hadc3,(uint32_t*)&ADC3_IN_Buffer,ADC_FRAME_SIZE*ADC3_CHANNELS);
    __HAL_TIM_ENABLE(&htim3);
    __HAL_TIM_ENABLE(&htim2);
    __HAL_TIM_ENABLE(&htim8);

    xEventGroupWaitBits(xADCEvent,ADC3_READY | ADC2_READY | ADC1_READY,pdTRUE,pdTRUE,portMAX_DELAY);
  //  adctime = HAL_GetTick();
    //Обработка значений АЦП3.
     switch (vADCGetADC3Data())
     {

       case HIGH_FREQ:
         if (OF>0)
         {
           ADC3Freq = ADC3Freq - ADC3Freq/10;
           if (ADC3Freq <2000)  ADC3Freq = 2000;
           vADC3FrInit(ADC3Freq);
           OF=0;
         }
         OF++;
         break;
       case LOW_FREQ:
         if (OF>0)
         {
           ADC3Freq = ADC3Freq + ADC3Freq/4;
           if (ADC3Freq >70000)  ADC3Freq = 40000;
           vADC3FrInit(ADC3Freq);
           OF=0;
         }
         OF++;
         break;
       default:
         break;
     }

    switch (vADCGetADC12Data())
    {

      case HIGH_FREQ:
        if (OF1>0)
        {
           ADC2Freq = ADC2Freq - ADC2Freq/10;
          if (ADC2Freq <2000)  ADC2Freq = 2000;
          vADC12FrInit(ADC2Freq);
          OF1=0;
        }
        OF1++;
        break;
      case LOW_FREQ:
        if (OF1>0)
        {
           ADC2Freq = ADC2Freq + ADC2Freq/4;
           if (ADC2Freq >70000)  ADC2Freq = 40000;
           vADC12FrInit(ADC2Freq);
           OF1=0;
         }
         OF1++;
         break;
      default:
        break;
    }
 //  xADC_TEMP =fix16_from_int( adctime++);

   }

}



void vCurConvert(int16_t * data, int16_t * ref, int16_t off)
{
 for (int16_t i = 0;i<ADC_FRAME_SIZE*3;i=i+3)
 {
   data[i]   =data[i] - ref[i/3*4+3];
   data[i+1] =data[i+1]-ref[i/3*4+3];
   data[i+2] =data[i+2]-ref[i/3*4+3];
 }
  return;
}


void vDecNetural(int16_t * data)
{
 for (int16_t i = 0;i<ADC_FRAME_SIZE<<2;i=i+4)
 {
   data[i]   = data[i]   - data[i+3];
   data[i+1] = data[i+1] - data[i+3];
   data[i+2] = data[i+2] - data[i+3];
 }
  return;
}

#define MAX_ZERO_POINT 20
#define FASE_DETECT_HISTERESIS  30

uint8_t vADCFindFreq(int16_t * data, uint16_t * count,uint8_t off, int16_t AMP)
{
  uint8_t AMP_DELTA = 15;
  uint8_t FD =  15;//5;
  uint8_t F1=0,F2=0;
  uint16_t tt=0;
  uint8_t CNT=0,index = 0,res =ADC_ERROR;
  uint16_t PER[MAX_ZERO_POINT];

  vADCSetFreqDetectParam(AMP,&AMP_DELTA,&FD);

  for (uint16_t i=FD;i<ADC_FRAME_SIZE-FD;i++)
  {
    //Если значение попадет в корридор окло нуля
    if ((data[i*4+off] > -AMP_DELTA) && (data[i*4+off] < AMP_DELTA))
    {
        //то прверяем текущую фазу
        if ((data[i*4+off] > data[i*4-FD*4+off]) || (data[i*4+off] < data[i*4+off+FD*4]))
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
        i = i+ FD*2;
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
  int16_t iMax =0;
  uint16_t DF1,DF2,DF3;
  vDecNetural((int16_t *)&ADC3_IN_Buffer);
  iMax =xADCMax((int16_t *)&ADC3_IN_Buffer, 2, uCurPeriod,&DF1,4);
  xEventGroupClearBits( xADCEvent, NET_READY );
  if( iMax >= MIN_AMP_VALUE )
  {
        result =  vADCFindFreq((int16_t *)&ADC3_IN_Buffer, &uCurPeriod,2,iMax);
        if (result==ADC_OK)
        {
          iMax =xADCMax((int16_t *)  &ADC3_IN_Buffer, 2, uCurPeriod,&DF1,4);
          xNET_FREQ = fix16_div(fix16_from_int(ADC3Freq/10),fix16_from_int(uCurPeriod));
          xNET_FREQ= fix16_mul(xNET_FREQ,fix16_from_int(10));
        }
        else
        {
          xEventGroupSetBits( xADCEvent, NET_READY );
          return result;
        }

        xNET_F1_VDD = fix16_mul( xADCRMS((int16_t *)&ADC3_IN_Buffer, 2, uCurPeriod,4 ), (fix16_t) 21178 );//умонжить на ( 401U * 3.3 / 4095U )
        xEventGroupSetBits( xADCEvent, NET_READY );
 }
 else
 {
   xNET_F1_VDD =0;
   xNET_F2_VDD =0;
   xNET_F3_VDD =0;
   uNetFaseRotation = NO_ROTATION;
   xEventGroupSetBits( xADCEvent, NET_READY );
   return LOW_AMP;
 }
 //Проверям есть ли на канале напряжение.
 iMax =xADCMax( (int16_t *) &ADC3_IN_Buffer, 1, uCurPeriod, &DF2,4 );
 xEventGroupClearBits( xADCEvent, NET_READY );
 if( iMax  >= MIN_AMP_VALUE )
 {
     xNET_F2_VDD = fix16_mul( xADCRMS((int16_t *)&ADC3_IN_Buffer, 1, uCurPeriod,4),(fix16_t) 21178 );//умонжить на ( 401U * 3.3 / 4095U )
 }
 else
 {
   xNET_F2_VDD = 0;
   uNetFaseRotation=NO_ROTATION;
  }
 xEventGroupSetBits( xADCEvent, NET_READY );
 //Проверям есть ли на канале напряжение.
 iMax =xADCMax((int16_t *)  &ADC3_IN_Buffer, 0, uCurPeriod,&DF3,4 );
 xEventGroupClearBits( xADCEvent, NET_READY );
 if( iMax >= MIN_AMP_VALUE )
 {
      xNET_F3_VDD = fix16_mul( xADCRMS((int16_t *)&ADC3_IN_Buffer, 0, uCurPeriod,4 ), (fix16_t) 21178  );//умонжить на ( 401U * 3.3 / 4095U )
 }
 else
 {
   xNET_F3_VDD = 0;
   uNetFaseRotation=NO_ROTATION;
 }
 xEventGroupSetBits( xADCEvent, NET_READY );
 //Проверяем флаг чередования фаз.Если он сброшен, значит детектировали нулевое напряжение на 1-й фазе и
 //нужно проверить чередование фаз полсе востановления напряжния
 if ( uNetFaseRotation==NO_ROTATION)
 {
   if (    ( (uCurPeriod - DF1 ) > FASE_DETECT_HISTERESIS ) || ((uCurPeriod - DF2 ) > FASE_DETECT_HISTERESIS )
    || ((uCurPeriod - DF3 ) > FASE_DETECT_HISTERESIS ) )
     //Проверяем что максисмумы амплитуды каждой фазы были зафиксировны
     //не в конце перидоа. Это может провести к ошибки, поэтому пропускаем это измерение и ждем удачного.
   {
     if (( abs (DF1 - DF2 )> FASE_DETECT_HISTERESIS ) &&  ( abs(DF2 - DF3 )> FASE_DETECT_HISTERESIS ))
     {
       if (DF2 < DF3)
       {
         uNetFaseRotation = B_C_ROTATION;
       }
       else
       {
         uNetFaseRotation = C_B_ROTATION;
       }
     }
   }
 }
 return ADC_OK;
}

#define COS_DATA_COUNT  10
static int16_t CosBuffer[COS_DATA_COUNT][2];
static uint8_t uCosCount =0;


uint8_t vADCGetADC12Data()
{
  uint16_t result=ADC_ERROR;
  uint16_t uCurPeriod = ADC_FRAME_SIZE-1;
  int16_t iMax =0,iTemp=0;
  fix16_t fix_temp=0;
  uint16_t DF1,DF2,DF3;

  vDecNetural((int16_t *)&ADC2_IN_Buffer);  //Вычитаем из фазы, значение на линии нейтрали
  iMax=xADCMax((int16_t *) &ADC2_IN_Buffer, 2, uCurPeriod, &DF1 ,4); //Вычисляем максимальное амплитудное значение
  xEventGroupClearBits( xADCEvent, GEN_READY );
  if( iMax  >= MIN_AMP_VALUE ) //Если маскимальное омплитудно значение меньше нижнего предела, то фиксируем нулевое напряжение на входе
  {
      result =  vADCFindFreq((int16_t *)&ADC2_IN_Buffer, &uCurPeriod,2,iMax);  //Вычисляем частоту
      if (result==ADC_OK)
      {
        xGEN_FREQ =  fix16_div(fix16_from_int(ADC2Freq/10),fix16_from_int(uCurPeriod));
        xGEN_FREQ =  fix16_mul(xGEN_FREQ,fix16_from_int(10));
        iMax=xADCMax((int16_t *) &ADC2_IN_Buffer, 2, uCurPeriod, &DF1 ,4);
        xGEN_F1_VDD= fix16_mul( xADCRMS((int16_t *)&ADC2_IN_Buffer, 2, uCurPeriod,4 ), (fix16_t) 21178 );  //умонжить на ( 401U * 3.3 / 4095U )
      }
  }
  else
  {
    uGenFaseRotation = NO_ROTATION;
    xGEN_F1_VDD =0;
    xGEN_F2_VDD =0;
    xGEN_F3_VDD =0;
    xCosFi = 0;
    xEventGroupSetBits( xADCEvent, GEN_READY );
    return LOW_AMP;
  }
   xEventGroupSetBits( xADCEvent, GEN_READY );
   if (result==ADC_OK)
   {

       //Проверям есть ли на канале напряжение.
       iMax=xADCMax((int16_t *) &ADC2_IN_Buffer, 1, uCurPeriod, &DF2,4 );
       xEventGroupClearBits( xADCEvent, GEN_READY );
       if( iMax >= MIN_AMP_VALUE )
       {
         xGEN_F2_VDD= fix16_mul(xADCRMS((int16_t *)&ADC2_IN_Buffer, 1, uCurPeriod,4 ), (fix16_t) 21178 ); //умонжить на ( 401U * 3.3 / 4095U )
       }
       else
       {
         xGEN_F2_VDD =0;
       }
       xEventGroupSetBits( xADCEvent, GEN_READY );

       iMax =xADCMax((int16_t *) &ADC2_IN_Buffer, 0, uCurPeriod, &DF3,4 );  //Проверям есть ли на канале напряжение.
       xEventGroupClearBits( xADCEvent, GEN_READY );
       if(iMax >= MIN_AMP_VALUE )
       {
         xGEN_F3_VDD= fix16_mul(xADCRMS((int16_t *)&ADC2_IN_Buffer, 0, uCurPeriod,4 ),(fix16_t) 21178);//умонжить на ( 401U * 3.3 / 4095U )
       }
       else
       {
         xGEN_F3_VDD =0;
       }

       //Проверяем флаг чередования фаз.Если он сброшен, значит детектировали нулевое напряжение на 1-й фазе и
       //нужно проверить чередование фаз полсе востановления напряжния
       if ( uGenFaseRotation==NO_ROTATION)
       {
         if (    ( (uCurPeriod - DF1 ) > FASE_DETECT_HISTERESIS ) || ((uCurPeriod - DF2 ) > FASE_DETECT_HISTERESIS )
             || ((uCurPeriod - DF3 ) > FASE_DETECT_HISTERESIS ) )
           //Проверяем что максисмумы амплитуды каждой фазы были зафиксировны
           //не в конце перидоа. Это может провести к ошибки, поэтому пропускаем это измерение и ждем удачного.
         {
           if (( abs (DF1 - DF2 )> FASE_DETECT_HISTERESIS ) &&  ( abs(DF2 - DF3 )> FASE_DETECT_HISTERESIS ))
           {
             if (DF2 < DF3)
             {
               uGenFaseRotation = B_C_ROTATION;
             }
             else
             {
               uGenFaseRotation = C_B_ROTATION;
             }
           }
         }

       }
   }

   xEventGroupSetBits( xADCEvent, GEN_READY );
  // Расчет значения тока и косинуса фи
   xEventGroupClearBits( xADCEvent, CUR_READY );    //Ставим симофор
   vCurConvert((int16_t *)&ADC1_IN_Buffer,(int16_t *)&ADC2_IN_Buffer,4);   //Вычитаем из значений тока значения нейтрали напряжения генератора
   if (result==ADC_OK)     //Если мы определили значение частоты напряжения генератора, то считаем ток через TrueRMS
   {
     //Вычисяем значение TrueRMS для токовых каналов
     xGEN_F1_CUR =   xADCRMS((int16_t *)&ADC1_IN_Buffer, 0, uCurPeriod,3 );
     xGEN_F2_CUR =   xADCRMS((int16_t *)&ADC1_IN_Buffer, 1, uCurPeriod,3 );
     xGEN_F3_CUR =   xADCRMS((int16_t *)&ADC1_IN_Buffer, 2, uCurPeriod,3 );
     /*
      * расчет косинуса фи.
      */
     xADCMax((int16_t *) &ADC1_IN_Buffer, 0, uCurPeriod, &DF3,3 );//находим сдвиг относительно начал отсчета максимальной амплитуды тока
     //Проверяем, что валидность максимумов тока и напряжения, сравнение FASE_DETECT_HISTERESIS позволяет отсечь систуацию, когда данные зафиксированы
     //со сдвигом фазы окло 90 градусов. В этом случае в пероид пападают 2 мксимума.
     if ( ((uCurPeriod - DF1 ) > FASE_DETECT_HISTERESIS ) && ((uCurPeriod - DF3 ) > FASE_DETECT_HISTERESIS ) && (DF1 <= DF3))
     {
       // Делаем усреднение
       CosBuffer[uCosCount][0] = DF3-DF1;
       CosBuffer[uCosCount][1] = uCurPeriod;
       uCosCount++;
       if (uCosCount>=COS_DATA_COUNT)
       {
         uCosCount=0;
         iMax =0;
         iTemp =0;
         for (uint8_t t=0;t<COS_DATA_COUNT;t++)
         {
           iMax = iMax+  CosBuffer[t][0];
           iTemp = iTemp +CosBuffer[t][1];
         }
         iMax =  iMax/COS_DATA_COUNT;
         iTemp = iTemp/COS_DATA_COUNT;
         xCosFi =fix16_div((fix16_t) 411774U,fix16_from_int(iTemp));  // 2Pi/uCurPeriod
         xCosFi =fix16_mul(fix16_from_int(iMax),xCosFi);
         xCosFi = fix16_mul(fix16_cos(xCosFi),xMinus1);
       }

     }
   }
   else
   {
     //Если по каким-то причинам мы не можем в текущий момент расчитать значение частоты напрежения генератора, то считаем ток арефмечтический исходя из максимального значения
     fix_temp =    fix16_sqrt(fix16_from_int(2));
     xGEN_F1_CUR = fix16_div( fix16_from_int( xADCMax( (int16_t *)&ADC1_IN_Buffer, 0, uCurPeriod, &DF3,3 )), fix_temp );
     xGEN_F2_CUR = fix16_div( fix16_from_int( xADCMax( (int16_t *)&ADC1_IN_Buffer, 1, uCurPeriod, &DF3,3 )), fix_temp );
     xGEN_F3_CUR = fix16_div( fix16_from_int( xADCMax( (int16_t *)&ADC1_IN_Buffer, 2, uCurPeriod, &DF3,3 )), fix_temp );

   }
  //Пересчет данных с АЦП в значения тока на шутнирующих резисторах
   xGEN_F1_CUR = fix16_mul(xGEN_F1_CUR, xLCurCoof  );
   xGEN_F2_CUR = fix16_mul(xGEN_F2_CUR, xLCurCoof  );
   xGEN_F3_CUR = fix16_mul(xGEN_F3_CUR, xLCurCoof  );

   xEventGroupSetBits( xADCEvent, CUR_READY );
  return result;

}

/*
 * Сервисная функция для расчета RMS
 */

fix16_t  xADCRMS(int16_t * source, uint8_t off, uint16_t size, uint8_t cc )
{
  uint64_t sum =0;
  for (uint16_t i=0;i<size*cc;i=i+cc)
  {
    sum =sum+ source[i+off]*source[i+off];

  }
  sum =sum/size;
  return fix16_from_int (sqrt(sum));

}
/*
 * Сервисная функция для вычисления максимального значения
 *  source  -адрес буфера DMA
 *  off     -номер канала, скоторым работает функция
 *  size    -размер буфера для обработки
 */
int16_t  xADCMax( int16_t * source, uint8_t off, uint16_t size, uint16_t * delay, uint8_t cc )
{

  int16_t max =0;
  uint16_t del =0,i;

  for (i=0;i<size;i++)
  {
      if ( source[i*cc + off] >  max)
      {
        max = source[i*cc + off];
        del = i;
      }
  }
  *delay = del;
  return max;

}

uint16_t GetAverVDD(uint8_t channel,uint8_t size,uint8_t offset,int16_t * source)
{

   uint32_t Buffer=0;
   for (uint8_t i=0;i<size;i++)
   {
     Buffer =Buffer+ source[i*offset+channel];
   }
   Buffer =Buffer/size;
   return  (uint16_t)Buffer;
}


/*
 * Процедура настройки параметров алгоритма определния частоты в зависимости от амплитуды входного сигнала
 */
void  vADCSetFreqDetectParam(int16_t AMP,uint8_t * del,uint8_t * fd)
{
  switch (AMP/100)
         {
           case 0:
             *del = 0;
             *fd =  70;
             break;
           case 1:
             *del = 2;
             *fd =  40;
             break;
           case 2:
             *del = 4;
             *fd =  25;
             break;
           case 3:
             *del = 5;
             *fd =  20;
             break;
           case 4:
             *del = 7;
             *fd =  15;
             break;
           case 5:
             *del = 8;
             *fd =  10;
             break;
           case 6:
             *del = 11;
             *fd =  8;
             break;
           case 7:
             *del = 12;
             *fd =  7;
             break;
           case 8:
             *del = 14;
             *fd =  6;
             break;
           case 9:
             *del = 15;
             *fd =  5;
             break;
           case 10:
             *del = 16;
             *fd =  4;
             break;
           case 11:
           case 12:
           case 13:
           case 14:
           case 15:
           case 16:
           case 17:
           case 18:
           case 19:
           case 20:
           default:
             *del = 20;
             *fd =  4;
             break;
         }
}



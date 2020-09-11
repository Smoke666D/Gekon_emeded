/*
 * lcd.c
 *
 *  Created on: Feb 5, 2020
 *      Author: igor.dymov
 *  Драйвер индикатора 128x64 на базе st7920 с использованием библиотеки u8g2, адаптированный для FreeRTOS.
 *  Весь нижний урорвень билиотеки u8g2 "выпелен", для оптимизации скорости работы.
 *  Драйвер использует виртуальную видеопамять LCD_Buffer, который отрисовывется на экране функцией LCD_Redraw
 *  Функция копирует данные экрана из драйвера u8g2 в виртуальную видеопамять, и отрисовывает на экран
 *  строки, которые изменились. Для отрисовки используется ввывод по DMA. Задержки после записи команды в LCD
 *  реализованы на таймере TIM7 и симофорами. Это позволяет "отпускать" ядро для выполнения FreeRTOS в процессе
 *  задержки после записи команды и данных в индикатора, равной 72ums.
 */

#include "lcd.h"
#include "stm32f2xx_hal_spi.h"

/*----------------------- Structures ----------------------------------------------------------------*/
static SemaphoreHandle_t  xSemaphore = NULL;
/*----------------------- Variables -----------------------------------------------------------------*/
static u8g2_t  u8g2;
static uint8_t LCD_Buffer[LCD_DATA_BUFFER_SIZE];
static uint8_t lcd_delay=0;
static uint8_t lcd_brigth_counter =0;
static uint8_t lcd_brigth =0;
/*------------------------ Extern -------------------------------------------------------------------*/
extern TIM_HandleTypeDef htim7;
extern SPI_HandleTypeDef hspi3;


void vLCDWriteCommand( uint8_t data );
void vLCDSend16Data( uint8_t *arg_prt );

/*---------------------------------------------------------------------------------------------------*/
/*
 * Функция инициализации драйвера LCD, передается указатель на симофор, для реализации задержек
 */
void vLCDInit( SemaphoreHandle_t temp )
{
  xSemaphore = temp;
  return;
}

void vLCDBrigthInit()
{
  lcd_brigth = displayBrightnesLevel.value[0U];
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Функция установки яркости подсветки индикатора
 */
void vLCDSetLedBrigth ( uint8_t brigth )
{
  if ( brigth <= displayBrightnesLevel.atrib->max )
  {
    lcd_brigth = brigth;
  }
  else
  {
    lcd_brigth = displayBrightnesLevel.atrib->max;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Функция плучения яркости подсветки индикатора
 */
uint8_t ucLCDGetLedBrigth(void)
{
  return  lcd_brigth;
}

/*---------------------------------------------------------------------------------------------------*/
/*
 * Процедура обновления индикатора
 */
void vLCDRedraw( void )
{
  uint16_t i       = 0U;
  uint8_t  x       = 0U;
  uint8_t  y       = 0U;
  uint16_t y_start = 0U;
  uint16_t y_end   = 0U;
  uint8_t LCD_REDRAW_FLAG = 0U;
  //Сравниваем буфер индикатора и буфер библиотеки u8g2
  //Если находим различия, то устанвливаем флаг LCD_REDRAW_FLAG и фиксируем начальную и конечную строку
  //области индикатора, которую необходимо перерисовывать
  for ( i=0U; i<LCD_DATA_BUFFER_SIZE; i ++ )
  {
    if ( LCD_Buffer[i] != u8g2.tile_buf_ptr[i] )
    {
      LCD_Buffer[i]	= u8g2.tile_buf_ptr[i];
      if ( ! LCD_REDRAW_FLAG )
      {
        LCD_REDRAW_FLAG = 1U;
        y_start         = i;
      }
      else
        y_end = i;
    }
  }
  //Если установлен флаг обновления индикатора переисовываем заданную область
  if ( LCD_REDRAW_FLAG  )
  {
    HAL_GPIO_WritePin( LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET );
    vLCDWriteCommand(  FUNCTION_CMD | BIT8BUS | RE | G_ON );
    y_start = y_start >>4;
    y_end   = ( y_end >>4 ) + 1U;
    for ( i= y_start; i < y_end; i++ )
    {
      if ( i >= 32U )
      {
        x = 8U;
        y = i - 32U;
      }
      else
      {
        x = 0U;
        y = i;
      }
      vLCDWriteCommand( SETDDM_ADR_CMD | y );
      vLCDWriteCommand( SETDDM_ADR_CMD | x );
      vLCDSend16Data( &LCD_Buffer[i <<4 ] );
    }
    HAL_GPIO_WritePin( LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET );
  }
  return;
}


/*---------------------------------------------------------------------------------------------------*/
void vLCDDelay( void )
{
  static portBASE_TYPE xHigherPriorityTaskWoken;

  //Управление подсветкой
  lcd_brigth_counter++;
  if (lcd_brigth_counter == lcd_brigth)
      HAL_GPIO_WritePin( LCD_LED_GPIO_Port, LCD_LED_Pin, GPIO_PIN_RESET );
  else
    if (lcd_brigth_counter >= 100U)
    {
      lcd_brigth_counter = 0U;
      HAL_GPIO_WritePin( LCD_LED_GPIO_Port, LCD_LED_Pin, GPIO_PIN_SET );
    }
  //Задержка после команды lcd
  if (lcd_delay)
    if ( hspi3.State != HAL_SPI_STATE_BUSY_TX )
    {
        lcd_delay = 0;
        xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR( xSemaphore, &xHigherPriorityTaskWoken );
        portEND_SWITCHING_ISR( xHigherPriorityTaskWoken )
    }
  return;
}

#define SPI_DEFAULT_TIMEOUT 100U


static HAL_StatusTypeDef SPI_WaitFlagStateUntilTimeout1(SPI_HandleTypeDef *hspi, uint32_t Flag, FlagStatus State,
                                                       uint32_t Timeout, uint32_t Tickstart)
{
  while ((__HAL_SPI_GET_FLAG(hspi, Flag) ? SET : RESET) != State)
  {
    if (Timeout != HAL_MAX_DELAY)
    {
      if (((HAL_GetTick() - Tickstart) >= Timeout) || (Timeout == 0U))
      {
        /* Disable the SPI and reset the CRC: the CRC value should be cleared
        on both master and slave sides in order to resynchronize the master
        and slave for their respective CRC calculation */

        /* Disable TXE, RXNE and ERR interrupts for the interrupt process */
        __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_TXE | SPI_IT_RXNE | SPI_IT_ERR));

        if ((hspi->Init.Mode == SPI_MODE_MASTER) && ((hspi->Init.Direction == SPI_DIRECTION_1LINE)
                                                     || (hspi->Init.Direction == SPI_DIRECTION_2LINES_RXONLY)))
        {
          /* Disable SPI peripheral */
          __HAL_SPI_DISABLE(hspi);
        }

        /* Reset CRC Calculation */
        if (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
        {
          SPI_RESET_CRC(hspi);
        }

        hspi->State = HAL_SPI_STATE_READY;

        /* Process Unlocked */
        __HAL_UNLOCK(hspi);

        return HAL_TIMEOUT;
      }
    }
  }

}

static void SPI_DMATransmit(DMA_HandleTypeDef *hdma)
{
  SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)(((DMA_HandleTypeDef *)hdma)->Parent); /* Derogation MISRAC2012-Rule-11.5 */
  uint32_t tickstart;

  /* Init tickstart for timeout management*/
  tickstart = HAL_GetTick();

  /* Disable ERR interrupt */
  __HAL_SPI_DISABLE_IT(hspi, SPI_IT_ERR);

  /* Disable Tx DMA Request */
  CLEAR_BIT(hspi->Instance->CR2, SPI_CR2_TXDMAEN);

  /* Check the end of the transaction */

  if (SPI_WaitFlagStateUntilTimeout1(hspi, SPI_FLAG_BSY, RESET, SPI_DEFAULT_TIMEOUT, tickstart) != HAL_OK)
   {
     SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
   }

  hspi->TxXferCount = 0U;
  hspi->State = HAL_SPI_STATE_READY;

}

void vLCD_HAL_SPI_DMA_Init()
{
  /* Set the SPI TxDMA Half transfer complete callback */
  hspi3.hdmatx->XferHalfCpltCallback = NULL;

   /* Set the SPI TxDMA transfer complete callback */
   hspi3.hdmatx->XferCpltCallback = SPI_DMATransmit;

   /* Set the DMA error callback */
   hspi3.hdmatx->XferErrorCallback = NULL;

   /* Set the DMA AbortCpltCallback */
   hspi3.hdmatx->XferAbortCallback = NULL;

}


HAL_StatusTypeDef SPI_Transmit_DMA(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef errorcode = HAL_OK;


  /* Process Locked */
  __HAL_LOCK(hspi);

  if (hspi->State != HAL_SPI_STATE_READY)
  {
    errorcode = HAL_BUSY;
    goto error;
  }

  /* Set the transaction information */
  hspi->State       = HAL_SPI_STATE_BUSY_TX;
  hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
  hspi->pTxBuffPtr  = (uint8_t *)pData;
  hspi->TxXferSize  = Size;
  hspi->TxXferCount = Size;

  /* Init field not used in handle to zero */
  hspi->pRxBuffPtr  = (uint8_t *)NULL;
  hspi->TxISR       = NULL;
  hspi->RxISR       = NULL;
  hspi->RxXferSize  = 0U;
  hspi->RxXferCount = 0U;

  /* Configure communication direction : 1Line */
  if (hspi->Init.Direction == SPI_DIRECTION_1LINE)
  {
    SPI_1LINE_TX(hspi);
  }

#if (USE_SPI_CRC != 0U)
  /* Reset CRC Calculation */
  if (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
    SPI_RESET_CRC(hspi);
  }
#endif /* USE_SPI_CRC */



  /* Enable the Tx DMA Stream/Channel */
  if (HAL_OK != HAL_DMA_Start_IT(hspi->hdmatx, (uint32_t)hspi->pTxBuffPtr, (uint32_t)&hspi->Instance->DR,
                                 hspi->TxXferCount))
  {
    /* Update SPI error code */
    SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_DMA);
    errorcode = HAL_ERROR;

    hspi->State = HAL_SPI_STATE_READY;
    goto error;
  }
  /* Check if the SPI is already enabled */
  if ((hspi->Instance->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE)
  {
    /* Enable SPI peripheral */
    __HAL_SPI_ENABLE(hspi);
  }

  /* Enable the SPI Error Interrupt Bit */
  __HAL_SPI_ENABLE_IT(hspi, (SPI_IT_ERR));

  /* Enable Tx DMA Request */
  SET_BIT(hspi->Instance->CR2, SPI_CR2_TXDMAEN);

error :
  /* Process Unlocked */
  __HAL_UNLOCK(hspi);
  return errorcode;
}



/*---------------------------------------------------------------------------------------------------*/
static uint8_t Data[33U] ={0xFA,};
inline void vLCDSend16Data( uint8_t *arg_prt )
{
  uint8_t *data;
  uint8_t i = 0U;

  data = ( uint8_t* )arg_prt;
  for ( i=0U; i<16U; i++ )
  {
    Data[( i << 1 ) + 1U]= data[i] & 0x0f0U;
    Data[( i << 1 ) + 2U]= data[i] << 4U;
  }
  SPI_Transmit_DMA( &hspi3, Data, 33U );
  lcd_delay = 1;
  xSemaphoreTake( xSemaphore, portMAX_DELAY );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * LCD Write Command
 */
static uint8_t DataC[3U]={0xF8};

inline void vLCDWriteCommand( uint8_t com )
{
  DataC[1U] = 0xF0U & com;
  DataC[2U] = 0xF0U & ( com << 4U );
  SPI_Transmit_DMA( &hspi3, DataC, 3U );
  lcd_delay =1;
  xSemaphoreTake( xSemaphore, portMAX_DELAY );
  return;
}

void vST7920init(void)
{

  HAL_TIM_Base_Start_IT( &htim7 );
  vLCDBrigthInit();
  vLCD_HAL_SPI_DMA_Init();

  HAL_GPIO_WritePin( LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET );
  osDelay(40);
  vLCDWriteCommand( 0x38U );
  osDelay(1);
  vLCDWriteCommand( 0x38U );
  osDelay(1);
  vLCDWriteCommand( 0x08U );
  osDelay(1);
  vLCDWriteCommand( 0x01U );
  osDelay(11);
  vLCDWriteCommand( 0x06U );
  osDelay(1);
  vLCDWriteCommand( 0x02U );
  osDelay(1);

  HAL_GPIO_WritePin( LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET );
}


/*---------------------------------------------------------------------------------------------------*/
void vLCD_Init()
{
  uint16_t i = 0U;
  //инициализация библиотеки u8g2
  //вся работа непосредственно с индикатором ведется напрямую, менуя драйвер нижнего уровня u8g2
  //поэтому последние 2 парамтера инициализируются 0
  u8g2_Setup_st7920_s_128x64_f( &u8g2, U8G2_R0, 0,0);
  vST7920init();
  vMenuInit( &u8g2 );
  u8g2_ClearBuffer( &u8g2 );
  for ( i=0U; i<LCD_DATA_BUFFER_SIZE; i++ )
  {
    LCD_Buffer[i] = 0xFFU;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/










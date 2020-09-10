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
  HAL_SPI_Transmit_DMA( &hspi3, Data, 33U );
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
  HAL_SPI_Transmit_DMA( &hspi3, DataC, 3U );
  lcd_delay =1;
  xSemaphoreTake( xSemaphore, portMAX_DELAY );
  return;
}

void vST7920init(void)
{

  HAL_TIM_Base_Start_IT( &htim7 );
  vLCDBrigthInit();
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
void StartLcdTask(void *argument)
{
  vLCD_Init();
  for(;;)
  {
    vMenuTask();
  }
}









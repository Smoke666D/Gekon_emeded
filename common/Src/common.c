/*
 * common.c
 *
 *  Created on: 19 мар. 2020 г.
 *      Author: photo_Mickey
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "common.h"
#include "string.h"
#include "stdio.h"
#include "system.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "test.h"
/*----------------------- Structures ----------------------------------------------------------------*/
static UART_HandleTypeDef* debug_huart         = NULL;
static QueueHandle_t       pSERIALqueue        = NULL;
static StaticQueue_t       xSERIALqueue        = { 0U };
static osThreadId_t        serialHandle        = NULL;
static osThreadId_t        serialOutHandle     = NULL;
static osThreadId_t        serialProtectHandle = NULL;
static SERIAL_TYPE         serial              = { 0U };
/*------------------------ Variables ----------------------------------------------------------------*/
static char* outputBuffer[SERIAL_QUEUE_SIZE] = { 0U };
/*---------------------------------------------------------------------------------------------------*/
/*
 * Send debug message thru serial
 * input: msg - string with message
 * output:  none
 */
void vSYSserial ( const char* msg )
{
  if ( serial.uart != NULL )
  {
    xQueueSend( pSERIALqueue, msg, SERIAL_OUTPUT_TIMEOUT );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vSERIALtask ( void* argument )
{
  for (;;)
  {
    /* End of operation from ISR */
    if ( ulTaskNotifyTake( pdTRUE, SERIAL_OUTPUT_TIMEOUT ) )
    {
      switch ( serial.state )
      {
        case SERIAL_STATE_READING: /* End of input message */
          if ( serial.error == 0U )
          {
            ( void )vTESTprocess( ( const char* )serial.input );
          }
          serial.counter = 0U;
          serial.state   = SERIAL_STATE_IDLE;
          __HAL_UART_ENABLE_IT( serial.uart,  UART_IT_RXNE ); /* Enable the UART Data Register not empty Interrupt */
          if ( serial.error == 0U )
          {
            vSYSserial( ( const char* )cTESTgetOutput() );
          }
          else
          {
            serial.error = 0U;
          }
          break;
        case SERIAL_STATE_WRITING: /* End of writing message */
          serial.counter = 0U;
          serial.length  = 0U;
          serial.state   = SERIAL_STATE_IDLE;
          __HAL_UART_DISABLE_IT( serial.uart, UART_IT_TC   ); /* Disable the UART Transmit Complete Interrupt */
          __HAL_UART_ENABLE_IT(  serial.uart, UART_IT_RXNE ); /* Enable the UART Data Register not empty Interrupt */
          break;
        default:
          serial.counter = 0U;
          serial.length  = 0U;
          serial.state   = SERIAL_STATE_IDLE;
          __HAL_UART_DISABLE_IT( serial.uart, UART_IT_TC   ); /* Disable the UART Transmit Complete Interrupt */
          __HAL_UART_ENABLE_IT(  serial.uart, UART_IT_RXNE ); /* Enable the UART Data Register not empty Interrupt */
          break;
      }
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vSERIALoutputTask ( void* argument )
{
  for (;;)
  {
    if ( xQueueReceive( pSERIALqueue, serial.output, SERIAL_OUTPUT_TIMEOUT ) == pdPASS )
    {
      serial.length = strlen( ( const char* )serial.output );
      if ( serial.length > 0U )
      {
        while ( serial.state != SERIAL_STATE_IDLE )
        {
          osDelay( SERIAL_OUTPUT_TIMEOUT );
        }
        serial.state   = SERIAL_STATE_WRITING;
        serial.counter = 1U;
        __HAL_UART_ENABLE_IT(  serial.uart, UART_IT_TC   ); /* Enable the UART Transmit Complete Interrupt */
        __HAL_UART_DISABLE_IT( serial.uart, UART_IT_RXNE ); /* Disable the UART Data Register not empty Interrupt */
        serial.uart->Instance->DR = ( uint16_t )( serial.output[0U] );
      }
    }
  }
}
/*---------------------------------------------------------------------------------------------------*/
void vSERIALprotectTask ( void* argument )
{
  BaseType_t   yield = pdFALSE;
  TaskHandle_t hTask = (TaskHandle_t)serialHandle;
  for (;;)
  {
    if ( ulTaskNotifyTake( pdTRUE, SERIAL_OUTPUT_TIMEOUT ) )
    {
      osDelay( SERIAL_PROTECT_TIMEOUT );
      if ( serial.state == SERIAL_STATE_READING )
      {
        serial.error = 1U;
        vTaskNotifyGiveFromISR( hTask, &yield );
        portYIELD_FROM_ISR ( yield );
      }
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/* Tx Transfer completed callbacks */
void HAL_UART_TxCpltCallback( UART_HandleTypeDef *huart )
{
  BaseType_t   yield = pdFALSE;
  TaskHandle_t hTask = (TaskHandle_t)serialHandle;
  if ( huart == serial.uart )
  {
    __HAL_UART_CLEAR_FLAG( debug_huart, UART_FLAG_TC );
    switch ( serial.state )
    {
      case SERIAL_STATE_WRITING:
        if ( serial.counter < serial.length )
        {
          serial.uart->Instance->DR = serial.output[serial.counter++];
        }
        else
        {
          __HAL_UART_DISABLE_IT( serial.uart, UART_IT_TC   ); /* Disable the UART Transmit Complete Interrupt */
          vTaskNotifyGiveFromISR( hTask, &yield );
          portYIELD_FROM_ISR ( yield );
        }
        break;
      default:
        vTaskNotifyGiveFromISR( hTask, &yield );
        portYIELD_FROM_ISR ( yield );
        break;
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/* Rx Transfer completed callbacks */
void HAL_UART_RxCpltCallback( UART_HandleTypeDef *huart )
{
  BaseType_t   yield    = pdFALSE;
  TaskHandle_t hTask    = (TaskHandle_t)serialHandle;
  TaskHandle_t hProtect = (TaskHandle_t)serialProtectHandle;
  if ( huart == serial.uart )
  {
    __HAL_UART_CLEAR_FLAG( debug_huart, UART_FLAG_RXNE );
    switch ( serial.state )
    {
      case SERIAL_STATE_IDLE:
        serial.counter   = 1U;
        serial.input[0U] = ( uint8_t )__HAL_UART_FLUSH_DRREGISTER( serial.uart );
        serial.state     = SERIAL_STATE_READING;
        vTaskNotifyGiveFromISR( hProtect, &yield );
        portYIELD_FROM_ISR ( yield );
        break;
      case SERIAL_STATE_READING:
        serial.input[serial.counter++] = ( uint8_t )__HAL_UART_FLUSH_DRREGISTER( serial.uart );
        if ( serial.input[serial.counter - 1U] == SERIAL_END_CHAR )
        {
          serial.input[serial.counter] = 0U;
          __HAL_UART_DISABLE_IT( serial.uart, UART_IT_RXNE ); /* Enable the UART Data Register not empty Interrupt */
          vTaskNotifyGiveFromISR( hTask, &yield );
          portYIELD_FROM_ISR ( yield );
        }
        break;
      default:
        vTaskNotifyGiveFromISR( hTask, &yield );
        portYIELD_FROM_ISR ( yield );
        break;
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vSERIALhandler ( void )
{
  uint32_t     isrflags = READ_REG( serial.uart->Instance->SR  );
  uint32_t     cr1its   = READ_REG( serial.uart->Instance->CR1 );
  BaseType_t   yield    = pdFALSE;
  TaskHandle_t hTask    = (TaskHandle_t)serialHandle;
  TaskHandle_t hProtect = (TaskHandle_t)serialProtectHandle;
  /* Read mode */
  if ( ( ( isrflags & USART_SR_RXNE ) != RESET ) && ( ( cr1its & USART_CR1_RXNEIE ) != RESET ) )
  {
    __HAL_UART_CLEAR_FLAG( debug_huart, UART_FLAG_RXNE );
    switch ( serial.state )
    {
      case SERIAL_STATE_IDLE:
        serial.counter   = 1U;
        serial.input[0U] = ( uint8_t )__HAL_UART_FLUSH_DRREGISTER( serial.uart );
        serial.state     = SERIAL_STATE_READING;
        vTaskNotifyGiveFromISR( hProtect, &yield );
        portYIELD_FROM_ISR ( yield );
        break;
      case SERIAL_STATE_READING:
        serial.input[serial.counter++] = ( uint8_t )__HAL_UART_FLUSH_DRREGISTER( serial.uart );
        if ( serial.input[serial.counter - 1U] == SERIAL_END_CHAR )
        {
          serial.input[serial.counter] = 0U;
          __HAL_UART_DISABLE_IT( serial.uart, UART_IT_RXNE ); /* Enable the UART Data Register not empty Interrupt */
          vTaskNotifyGiveFromISR( hTask, &yield );
          portYIELD_FROM_ISR ( yield );
        }
        break;
      default:
        vTaskNotifyGiveFromISR( hTask, &yield );
        portYIELD_FROM_ISR ( yield );
        break;
    }
  }
  /* Write mode */
  if ( ( ( isrflags & USART_SR_TC ) != RESET ) && ( ( cr1its & USART_CR1_TCIE ) != RESET ) )
  {
    __HAL_UART_CLEAR_FLAG( debug_huart, UART_FLAG_TC );
    switch ( serial.state )
    {
      case SERIAL_STATE_WRITING:
        if ( serial.counter < serial.length )
        {
          serial.uart->Instance->DR = serial.output[serial.counter++];
        }
        else
        {
          __HAL_UART_DISABLE_IT( serial.uart, UART_IT_TC   ); /* Disable the UART Transmit Complete Interrupt */
          vTaskNotifyGiveFromISR( hTask, &yield );
          portYIELD_FROM_ISR ( yield );
        }
        break;
      default:
        vTaskNotifyGiveFromISR( hTask, &yield );
         portYIELD_FROM_ISR ( yield );
        break;
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Declare UART for debug serial port
 * input:	uart - UART structure
 * output:	none
 */
void vSERIALinit ( UART_HandleTypeDef* uart )
{
  osThreadAttr_t task_attributes = { 0U };
  serial.uart   = uart;
  serial.state  = SERIAL_STATE_IDLE;
  pSERIALqueue  = xQueueCreateStatic( SERIAL_QUEUE_SIZE, sizeof( char* ), outputBuffer, &xSERIALqueue );
  task_attributes.name       = "serialTask";
  task_attributes.priority   = ( osPriority_t ) SERIAL_TASK_PRIORITY;
  task_attributes.stack_size = SERIAL_TSAK_STACK_SIZE;
  serialHandle     = osThreadNew( vSERIALtask,       NULL, &task_attributes );
  task_attributes.name       = "serialOutTask";
  task_attributes.priority   = ( osPriority_t ) SERIAL_TASK_PRIORITY;
  task_attributes.stack_size = SERIAL_TSAK_STACK_SIZE;
  serialOutHandle = osThreadNew( vSERIALoutputTask, NULL, &task_attributes );
  task_attributes.name       = "serialProtectTask";
  task_attributes.priority   = ( osPriority_t ) SERIAL_TASK_PRIORITY;
  task_attributes.stack_size = SERIAL_TSAK_STACK_SIZE;
  serialProtectHandle = osThreadNew( vSERIALprotectTask, NULL, &task_attributes );
  __HAL_UART_DISABLE_IT( serial.uart, UART_IT_TC );  /* Disable the UART Transmit Complete Interrupt */
  __HAL_UART_ENABLE_IT( serial.uart, UART_IT_RXNE ); /* Enable the UART Data Register not empty Interrupt */
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vSYSprintFix16 ( fix16_t value )
{
  char buffer[10U] = { 0U };
  fix16_to_str( value, buffer, 2U );
  vSYSserial( "$" );
  vSYSserial( buffer );
  vSYSserial( ";\r\n" );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vSYSgetUniqueID32 ( uint32_t* id )
{
  id[0U] = GET_UNIQUE_ID0;
  id[1U] = GET_UNIQUE_ID1;
  id[2U] = GET_UNIQUE_ID2;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vSYSgetUniqueID16 ( uint16_t* id )
{
  id[0U] = ( uint16_t )( GET_UNIQUE_ID0 & 0xFFFF );
  id[1U] = ( uint16_t )( ( GET_UNIQUE_ID0 >> 16U ) & 0xFFFF );
  id[2U] = ( uint16_t )( GET_UNIQUE_ID1 & 0xFFFF );
  id[3U] = ( uint16_t )( ( GET_UNIQUE_ID1 >> 16U ) & 0xFFFF );
  id[4U] = ( uint16_t )( GET_UNIQUE_ID2 & 0xFFFF );
  id[5U] = ( uint16_t )( ( GET_UNIQUE_ID2 >> 16U ) & 0xFFFF );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uEncodeURI ( const uint16_t* input, uint8_t length, char* output )
{
  uint8_t shift = 0U;
  uint8_t i     = 0U;
  for ( i=0U; i<length; i++ )
  {
    if ( input[i] > 0x00FFU )
    {
      ( void )sprintf( &output[shift], " %02X %02X", ( uint8_t )( ( input[i] & 0xFF00U ) >> 8U ), ( uint8_t )( input[i] & 0x00FFU ) );
      output[shift]      = '%';
      output[shift + 3U] = '%';
      shift += 6U;
    }
    else
    {
      ( void )sprintf( &output[shift], " %02X", ( uint8_t )( input[i] & 0x00FFU ) );
      output[shift] = '%';
      shift += 3U;
    }
  }
  return shift;
}
/*---------------------------------------------------------------------------------------------------*/
void vDecodeURI( const char* input, uint16_t* output, uint8_t length )
{
  uint8_t i          = 0U;
  uint8_t j          = 0U;
  char    cBuf[2U]   = { 0U };
  uint8_t hexBuf[8U] = { 0U };

  for( i=0; i<8U; i++ )
  {
    if ( input[j] == '%' )
    {
      cBuf[0U] = input[j + 1U];
      cBuf[1U] = input[j + 2U];
      hexBuf[i] = strtol( cBuf, NULL, 16U );
      j += 3U;
    }
    else
    {
      hexBuf[i] = input[j];
      j++;
    }
  }
  j = 0U;
  for( i=0; i<length; i++ )
  {
    if ( ( hexBuf[j] & 0x80U ) > 0U )
    {
      output[i] = ( ( ( uint16_t )hexBuf[j] ) << 8U ) | ( uint16_t )hexBuf[j + 1U];
      j += 2U;
    }
    else
    {
      output[i] = ( uint16_t )hexBuf[j];
      j++;
    }
  }

  return;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uSYSisConst ( void* ptr )
{
  uint8_t res = 1U;
  if ( ptr > 0x20000000U )
  {
    res = 0U;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uSYSputChar ( char* str, uint32_t length, char ch )
{
  str[length] = ch;
  return ( length + 1U );
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uSYSendString ( char* str, uint32_t length )
{
  str[length] = '\n';
  return ( length + 1U );
}
/*---------------------------------------------------------------------------------------------------*/
fix16_t fSYSconstrain ( fix16_t in, fix16_t min, fix16_t max )
{
  fix16_t out = in;
  if ( in < min )
  {
    out = min;
  }
  else if ( in > max )
  {
    out = max;
  }
  else
  {

  }
  return out;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/



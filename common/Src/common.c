/*
 * common.c
 *
 *  Created on: 19 мар. 2020 г.
 *      Author: photo_Mickey
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "cli.h"
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
#include "stdlib.h"
/*----------------------- Structures ----------------------------------------------------------------*/
static QueueHandle_t pSERIALqueue        = NULL;
static StaticQueue_t xSERIALqueue        = { 0U };
static osThreadId_t  serialHandle        = NULL;
static osThreadId_t  serialOutHandle     = NULL;
static osThreadId_t  serialProtectHandle = NULL;
static SERIAL_TYPE   serial              = { 0U };
/*------------------------ Variables ----------------------------------------------------------------*/
static UART_MESSAGE outputBuffer[SERIAL_QUEUE_SIZE] = { 0U };

#if defined ( UNIT_TEST )
  static char     unitOutput[UNIT_TEST_BUFFER_SIZE] = { 0U };
  static uint16_t unitCounter                       = 0U;
  static uint8_t  unitSenderDone                    = 0U;
#endif
/*---------------------------------------------------------------------------------------------------*/
void vSERIALstartReading ( void )
{
  HAL_UART_Receive_IT( serial.uart, &serial.buffer, 1U );
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t iSERIALisEndChar ( uint8_t input )
{
  uint8_t res = 0U;
  if ( ( input == SERIAL_END_CHAR_0 ) || ( input == SERIAL_END_CHAR_1 ) )
  {
    res = 1U;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Send debug message thru serial
 * input: msg - string with message
 * output:  none
 */
void vSYSserial ( const char* data, uint16_t length )
{
  if ( ( serial.uart != NULL ) && ( pSERIALqueue != NULL ) && ( length > 0U ) )
  {
    const UART_MESSAGE message = {
      .data   = ( char* )data,
      .length = length
    };
    xQueueSend( pSERIALqueue, &message, SERIAL_OUTPUT_TIMEOUT );
  }
  return;
}
#if defined ( UNIT_TEST )
void vUNITputChar ( int data )
{
  if ( unitCounter < UNIT_TEST_BUFFER_SIZE )
  {
    unitOutput[unitCounter++] = ( char )( data );
  }
  return;
}
void vUNITresetOutput ( void )
{
  unitCounter = 0U;
  return;
}
uint8_t eSERIALgetSerialState ( void )
{
  return unitSenderDone;
}
void vUNITwriteOutput ( void )
{
  vSYSserial( unitOutput, unitCounter );
  unitSenderDone = 0U;
  unitCounter    = 0U;
  return;
}
#endif
/*---------------------------------------------------------------------------------------------------*/
void vSERIALtask ( void* argument )
{
  for (;;)
  {
    if ( ulTaskNotifyTake( pdTRUE, SERIAL_OUTPUT_TIMEOUT ) )
    {
      if ( ( serial.state == SERIAL_STATE_READING ) && ( serial.error == 0U ) )
      {
        ( void )vCLIprocess( ( const char* )serial.input, serial.length );
        vSYSserial( ( const char* )cCLIgetOutput(), uCLIgetOutLength() );
      }
      #if defined ( UNIT_TEST )
        unitSenderDone = 1U;
      #endif
      serial.length  = 0U;
      serial.state   = SERIAL_STATE_IDLE;
      serial.error   = 0U;
      vSERIALstartReading();
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vSERIALoutputTask ( void* argument )
{
  for (;;)
  {
    if ( xQueueReceive( pSERIALqueue, &serial.output, SERIAL_OUTPUT_TIMEOUT ) == pdPASS )
    {
      if ( serial.output.length > 0U )
      {
        while ( serial.state != SERIAL_STATE_IDLE )
        {
          osDelay( SERIAL_OUTPUT_TIMEOUT );
        }
        serial.state = SERIAL_STATE_WRITING;
        HAL_UART_AbortReceive_IT( serial.uart );
        HAL_UART_Transmit_IT( serial.uart, ( uint8_t* )serial.output.data, serial.output.length );
      }
    }
  }
  return;
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
void HAL_UART_TxCpltCallback ( UART_HandleTypeDef *huart )
{
  BaseType_t yield = pdFALSE;
  if ( huart == serial.uart )
  {
    vTaskNotifyGiveFromISR( (TaskHandle_t)serialHandle, &yield );
    portYIELD_FROM_ISR ( yield );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/* Rx Transfer completed callbacks */
void HAL_UART_RxCpltCallback( UART_HandleTypeDef *huart )
{
  BaseType_t yield = pdFALSE;
  if ( huart == serial.uart )
  {
    switch ( serial.state )
    {
      case SERIAL_STATE_IDLE:
        serial.length    = 1U;
        serial.input[0U] = serial.buffer;
        serial.state     = SERIAL_STATE_READING;
        vSERIALstartReading();
        vTaskNotifyGiveFromISR( (TaskHandle_t)serialProtectHandle, &yield );
        portYIELD_FROM_ISR ( yield );
        break;
      case SERIAL_STATE_READING:
        serial.input[serial.length++] = serial.buffer;
        if ( iSERIALisEndChar( serial.buffer ) > 0U )
        {
          serial.input[serial.length] = 0U;
          vTaskNotifyGiveFromISR( (TaskHandle_t)serialHandle, &yield );
          portYIELD_FROM_ISR ( yield );
        }
        else
        {
          vSERIALstartReading();
        }
        break;
      default:
        vTaskNotifyGiveFromISR( (TaskHandle_t)serialHandle, &yield );
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
  pSERIALqueue  = xQueueCreateStatic( SERIAL_QUEUE_SIZE, sizeof( UART_MESSAGE ), outputBuffer, &xSERIALqueue );
  task_attributes.name       = "serialTask";
  task_attributes.priority   = ( osPriority_t ) SERIAL_TASK_PRIORITY;
  task_attributes.stack_size = SERIAL_TSAK_STACK_SIZE;
  serialHandle     = osThreadNew( vSERIALtask,       NULL, &task_attributes );
  task_attributes.name       = "serialOutTask";
  task_attributes.priority   = ( osPriority_t ) SERIAL_TASK_PRIORITY;
  task_attributes.stack_size = SERIAL_TSAK_STACK_SIZE;
  serialOutHandle            = osThreadNew( vSERIALoutputTask, NULL, &task_attributes );
  task_attributes.name       = "serialProtectTask";
  task_attributes.priority   = ( osPriority_t ) SERIAL_TASK_PRIORITY;
  task_attributes.stack_size = SERIAL_TSAK_STACK_SIZE;
  serialProtectHandle = osThreadNew( vSERIALprotectTask, NULL, &task_attributes );
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
  for ( uint8_t i=0U; i<length; i++ )
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
  uint8_t j          = 0U;
  char    cBuf[2U]   = { 0U };
  uint8_t hexBuf[8U] = { 0U };
  for ( uint8_t i=0; i<8U; i++ )
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
  for( uint8_t i=0; i<length; i++ )
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
uint32_t uSYSputChar ( char* str, uint32_t length, char ch )
{
  str[length] = ch;
  return ( length + 1U );
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uSYSendString ( char* str, uint32_t length )
{
  str[length]      = '\n';
  str[length + 1U] = 0U;
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



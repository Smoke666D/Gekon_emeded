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
/*----------------------- Structures ----------------------------------------------------------------*/
static UART_HandleTypeDef*	debug_huart;
/*---------------------------------------------------------------------------------------------------*/
/*
 * Declare UART for debug serial port
 * input:	uart - UART structure
 * output:	none
 */
void vSYSInitSerial( UART_HandleTypeDef* uart )
{
	debug_huart = uart;
	return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Send debug message thru serial
 * input:	msg - string with message
 * output:	none
 */
void vSYSSerial( char* msg )
{
	HAL_UART_Transmit(debug_huart, (uint8_t*)msg, strlen(msg), 0xFFFF);
	return;
}
/*---------------------------------------------------------------------------------------------------*/
void vSYSgetUniqueID32( uint32_t* id )
{
  id[0U] = GET_UNIQUE_ID0;
  id[1U] = GET_UNIQUE_ID1;
  id[2U] = GET_UNIQUE_ID2;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vSYSgetUniqueID16( uint16_t* id )
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
uint8_t uEncodeURI( char* output, uint16_t* input, uint8_t length )
{
  uint8_t shift = 0U;
  uint8_t i     = 0U;
  for( i=0U; i<length; i++ )
  {
    if ( input[i] > 0x00FF )
    {
      sprintf( &output[shift], " %02X %02X", (uint8_t)((input[i]&0xFF00)>>8), (uint8_t)(input[i]&0x00FF));
      output[shift]      = '%';
      output[shift + 3U] = '%';
      shift += 6U;
    }
    else
    {
      sprintf( &output[shift], " %02X", (uint8_t)(input[i]&0x00FF) );
      output[shift] = '%';
      shift += 3U;
    }
  }
  return shift;
}
/*---------------------------------------------------------------------------------------------------*/

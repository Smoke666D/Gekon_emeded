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
void vSYSInitSerial ( UART_HandleTypeDef* uart )
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
void vSYSserial ( const char* msg )
{
  HAL_UART_Transmit( debug_huart, ( const uint8_t* )msg, strlen(msg), 0xFFFFU );
  return;
}

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



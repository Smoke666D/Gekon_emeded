/*
 * common.c
 *
 *  Created on: 19 мар. 2020 г.
 *      Author: photo_Mickey
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "common.h"
#include "string.h"
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
void vSYSgetUniqueID( uint32_t* id )
{
  id[0U] = GET_UNIQUE_ID0;
  id[1U] = GET_UNIQUE_ID1;
  id[2U] = GET_UNIQUE_ID2;
	return;
}
/*---------------------------------------------------------------------------------------------------*/


/*
 * sys.c
 *
 *  Created on: Feb 6, 2020
 *      Author: mikhail.mikhailov
 */
#include "sys.h"
#include "string.h"


UART_HandleTypeDef debug_huart;

void vSYSInitSerial( void )
{
	debug_huart.Instance = USART3;
	debug_huart.Init.BaudRate = 115200;
	debug_huart.Init.WordLength = UART_WORDLENGTH_8B;
	debug_huart.Init.StopBits = UART_STOPBITS_1;
	debug_huart.Init.Parity = UART_PARITY_NONE;
	debug_huart.Init.Mode = UART_MODE_TX_RX;
	debug_huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	debug_huart.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&debug_huart) != HAL_OK)
	{
		Error_Handler();
	}
	return;
}

void vSYSSerial( char* msg )
{
	HAL_UART_Transmit(&debug_huart, (uint8_t*)msg, strlen(msg), 0xFFFF);
	return;
}

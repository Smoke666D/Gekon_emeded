/*
 * tcpip.h
 *
 *  Created on: 10 февр. 2020 г.
 *      Author: mikhail.mikhailov
 */

#ifndef INC_SERVER_H_
#define INC_SERVER_H_
/*----------------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "http.h"
#include "freeData.h"
/*----------------------------- Define ---------------------------------------*/
#define MAC_ADR_LENGTH 6U
/*------------------------------ Enum ----------------------------------------*/
typedef enum
{
  SERVER_STATE_UNPLUG,
  SERVER_STATE_PLUG,
  SERVER_STATE_DOWN,
  SERVER_STATE_UP,
} SERVER_STATE;

typedef enum
{
  SERVER_OK,
  SERVER_NEW_CONNECT_ERROR,
  SERVER_BIND_ERROR,
  SERVER_LISTEN_ERROR,
  SERVER_CLOSE_ERROR,
  SERVER_ACCEPT_ERROR,
  SERVER_DNS_ERROR,
  SERVER_REMOTE_CONNECT_ERROR,
  SERVER_WRITE_ERROR,
  SERVER_RECEIVE_ERROR,
} SERVER_ERROR;

typedef enum
{
  RECEIVE_MESSAGE_COMPLETE,
  RECEIVE_MESSAGE_CONTINUES,
  RECEIVE_MESSAGE_ERROR,
} RECEIVE_MESSAGE;
/*---------------------------- Structures ------------------------------------*/

/*------------------------------ Default -------------------------------------*/
#define  HTTP_INPUT_BUFFER_SIZE     1024U
#define  HTTP_OUTPUT_BUFFER_SIZE    256U
/*----------------------------- Functions ------------------------------------*/
void     vSERVERinit ( void );               /* Waiting the end of server initialization */
void     vSERVERgetStrIP ( char* ipStr );    /* Read local IP address of device in char array format */
uint8_t* pSERVERgetMAC ( void );
/*----------------------------------------------------------------------------*/
#endif /* INC_SERVER_H_ */

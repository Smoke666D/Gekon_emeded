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
/*------------------------------ Enum ----------------------------------------*/
typedef enum
{
	SERVER_OK,
	SERVER_NEW_CONNECT_ERROR,
	SERVER_BIND_ERROR,
	SERVER_LISTEN_ERROR,
	SERVER_CLOSE_ERROR,
	SERVER_ACCEPT_ERROR,
} SERVER_ERROR;
/*------------------------------ Default -------------------------------------*/
#define	HTTP_INPUT_BUFFER_SIZE		512U
#define	HTTP_OUTPUT_BUFFER_SIZE		256U
/*----------------------------- Functions ------------------------------------*/

void 					cETHgetStrIP( char* ipStr );

void 					vSERVERinit( void );
SERVER_ERROR 	eSERVERstart( void );
SERVER_ERROR 	eSERVERstop( void );
SERVER_ERROR 	eSERVERlistenRoutine( void );
/*----------------------------------------------------------------------------*/
#endif /* INC_SERVER_H_ */

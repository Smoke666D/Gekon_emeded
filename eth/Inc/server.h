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
/*------------------------------ Enum ----------------------------------------*/
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
/*------------------------------ Default -------------------------------------*/
#define	HTTP_INPUT_BUFFER_SIZE		512U
#define	HTTP_OUTPUT_BUFFER_SIZE		256U
/*----------------------------- Functions ------------------------------------*/
void 					cSERVERgetStrIP( char* ipStr );		// Read local IP address of device in char array format
void 					vSERVERinit( void );							// Waiting the end of server initialization
SERVER_ERROR 	eSERVERstart( void );							// Start server. Open 80 port and start listen it
SERVER_ERROR 	eSERVERstop( void );							// Stop server connection
SERVER_ERROR 	eSERVERlistenRoutine( void );			// Routine handler of incoming packages.

SERVER_ERROR 	eHTTPsendRequest( char* httpStr, char* hostName );

HTTP_STATUS eHTTPrequest( HTTP_REQUEST* request, HTTP_RESPONSE* response, char* output );								/* Send request, get and parsing response */
void  			eHTTPresponse( char* input, HTTP_REQUEST* request, HTTP_RESPONSE* response, char* output );	/* Make response for input request */
/*----------------------------------------------------------------------------*/
#endif /* INC_SERVER_H_ */

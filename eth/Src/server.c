/*
 * tcpip.c
 *
 *  Created on: 10 февр. 2020 г.
 *      Author: mikhail.mikhailov
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "server.h"
#include "http.h"
#include "sys.h"

#include "cmsis_os.h"

#include "lwip.h"
#include "api.h"
#include "string.h"
/*----------------------- Structures ----------------------------------------------------------------*/
static 		struct 		netconn * nc;
static 		struct 		netconn * in_nc;

static 		osThreadId_t 	netClientHandle;
/*----------------------- Variables -----------------------------------------------------------------*/
volatile 	err_t 			res 			= ERR_OK;
/*----------------------- Functions -----------------------------------------------------------------*/
void startNetClientTask(void const * argument);
/*---------------------------------------------------------------------------------------------------*/
/**
 * Read local IP address of device in char array format
 * input: buffer of char for IP address
 * 				length of buffer is IP4ADDR_STRLEN_MAX = 16
 * 				from ip4_addr.h
 * 				if ipStr[0] = 0x00, initialization havn't finished
 */
void cETHgetStrIP( char* ipStr )
{
	uint8_t i = 0U;
	char*   pointer = ip4addr_ntoa( &gnetif.ip_addr );

	for( i=0U; i<IP4ADDR_STRLEN_MAX; i++ )
	{
		ipStr[i] = pointer[i];
	}

	return;
}
/*---------------------------------------------------------------------------------------------------*/
void vSERVERinit( void )
{
	while ( gnetif.ip_addr.addr == 0U ) osDelay(1);		// Wait the ip to reach the structure
	return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
 * Open 80 port and start listen it
 */
SERVER_ERROR eSERVERstart( void )
{
	SERVER_ERROR 	servRes 	= SERVER_OK;
	err_t 				netconRes = ERR_OK;

	nc = netconn_new( NETCONN_TCP );										// Create new network connection TCP TYPE
	if ( nc != NULL )
	{
		netconRes = netconn_bind (nc, IP_ADDR_ANY, 80 );	// Bind connection to well known port number 80
		if ( netconRes == ERR_OK )
		{
			netconRes = netconn_listen( nc );								// Tell connection to go into listening mode
			if ( netconRes != ERR_OK )
			{
				servRes = SERVER_LISTEN_ERROR;
			}
		}
		else
		{
			servRes = SERVER_BIND_ERROR;
		}
	}
	else
	{
		servRes = SERVER_NEW_CONNECT_ERROR;
	}

	return servRes;
}
/*---------------------------------------------------------------------------------------------------*/
SERVER_ERROR eSERVERstop( void )
{
	SERVER_ERROR 	servRes 	= SERVER_OK;
	err_t 				netconRes = ERR_OK;

	netconRes = netconn_close( nc );
	if ( netconRes != ERR_OK )
	{
		servRes = SERVER_CLOSE_ERROR;
	}
	return servRes;
}
/*---------------------------------------------------------------------------------------------------*/
/**
 * Listen open port
 */
SERVER_ERROR eSERVERlistenRoutine( void )
{
	SERVER_ERROR 	servRes 	= SERVER_OK;

	res = netconn_accept( nc, &in_nc );			// Grab new connection
	if ( res != ERR_OK )										// Block until we get an incoming connection
	{
		servRes = SERVER_ACCEPT_ERROR;
	}
	else
	{
		const osThreadAttr_t netClientTask_attributes = {
				.name       = "netClientTask",
				.priority   = ( osPriority_t ) osPriorityLow,
				.stack_size = 1024U
		};
		netClientHandle = osThreadNew( startNetClientTask, (void*)in_nc, &netClientTask_attributes );
	}
	return servRes;
}
/*---------------------------------------------------------------------------------------------------*/
void startNetClientTask( void const * argument )
{
	struct 				netconn * netcon = ( struct netconn * )argument;
	struct 				netbuf  * nb;
	char*					input       = pvPortMalloc( HTTP_INPUT_BUFFER_SIZE );
	char*					output			= pvPortMalloc( HTTP_OUTPUT_BUFFER_SIZE );
	HTTP_RESPONSE response;
	HTTP_REQUEST	request;
	uint32_t 			len         = 0U;

	uint32_t			mesNum			= 0U;
	char*					pchSt;
	char*					pchEn;
	uint32_t			i 					= 0U;

	uint16_t 			outlen  		= 0;

  for(;;)
  {
  	if( netconn_recv( netcon, &nb ) == ERR_OK )
  	{
  		len = netbuf_len( nb );
  		netbuf_copy( nb, input, len );
  		netbuf_delete( nb );
  		input[len] = 0U;

  		eHTTPparsingRequest( input, &request );
  		eHTTPbuildResponse( request, &response );
  		eHTTPmakeResponse( output, response );


  		if ( response.method != HTTP_METHOD_NO )
  		{
  			netconn_write( netcon, output, strlen(output), NETCONN_COPY );
  			if ( response.contentLength != 0U )
  			{
  				mesNum = ( uint32_t )( response.contentLength / HTTP_OUTPUT_BUFFER_SIZE ) + 1U;
  				pchSt  = response.data;
  				pchEn  = pchSt + HTTP_OUTPUT_BUFFER_SIZE;
  				for( i=0U; i<mesNum; i++ )
  				{
  					strncpy( output, ( pchSt ), ( pchEn - pchSt ) );
  				  pchSt  = pchEn;
  				  pchEn  = pchSt + HTTP_OUTPUT_BUFFER_SIZE;
  				  outlen = strlen(output);
  				  if ( outlen > HTTP_OUTPUT_BUFFER_SIZE)
  				  {
  				  	outlen = HTTP_OUTPUT_BUFFER_SIZE;
  				  }
  				  netconn_write( netcon, output, outlen, NETCONN_COPY );
  				}
  			}
  		}
  	}
  	else
  	{
  		netconn_close( netcon );
  		netconn_delete( netcon );
  		vPortFree( input );
  		vPortFree( output );
  		osThreadExit();
  	}
  }
}
/*---------------------------------------------------------------------------------------------------*/


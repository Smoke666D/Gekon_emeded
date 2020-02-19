/*
 * tcpip.c
 *
 *  Created on: 10 февр. 2020 г.
 *      Author: mikhail.mikhailov
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "server.h"
#include "sys.h"
#include "cmsis_os.h"
#include "lwip.h"
#include "api.h"
#include "string.h"
/*----------------------- Structures ----------------------------------------------------------------*/
static 		struct 		netconn * nc;									// Connection to the port 80
static 		struct 		netconn * in_nc;							// New user connection
/*------------------------- Task --------------------------------------------------------------------*/
static 		osThreadId_t 	netClientHandle;					// Network task handle
/*----------------------- Variables -----------------------------------------------------------------*/

/*----------------------- Functions -----------------------------------------------------------------*/
void 					startNetClientTask(void const * argument);					// Network task function
SERVER_ERROR 	eHTTPsendRequest( char* httpStr, char* hostName );
/*---------------------------------------------------------------------------------------------------*/
/**
 * Read local IP address of device in char array format
 * Input: 	buffer of char for IP address
 * 					length of buffer is IP4ADDR_STRLEN_MAX = 16
 * 					from ip4_addr.h
 * 					if ipStr[0] = 0x00, initialization havn't finished
 * Output:	none
 */
void cSERVERgetStrIP( char* ipStr )
{
	uint8_t i 			= 0U;
	char* 	pointer = ip4addr_ntoa( &gnetif.ip_addr );

	for( i=0U; i<IP4ADDR_STRLEN_MAX; i++ )
	{
		ipStr[i] = pointer[i];
	}
	return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Waiting the end of server initialization.
 * IP address available after the end of this function.
 * Run it one time after MX_LWIP_Init().
 * Input: 	none
 * Output:	none
 */
void vSERVERinit( void )
{
	while ( gnetif.ip_addr.addr == 0U )
	{
		osDelay(1);		// Wait the ip to reach the structure
	}
	return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
 * Start server. Open 80 port and start listen it.
 * Run it one time to start.
 * Input:		none
 * Output:	server error code
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
		else servRes = SERVER_BIND_ERROR;
	}
	else servRes = SERVER_NEW_CONNECT_ERROR;

	return servRes;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Stop server connection
 * Input: 	none
 * Output:	server error code
 */
SERVER_ERROR eSERVERstop( void )
{
	SERVER_ERROR 	servRes 	= SERVER_CLOSE_ERROR;
	err_t 				netconRes = ERR_OK;

	netconRes = netconn_close( nc );
	if ( netconRes == ERR_OK )
	{
		netconRes = netconn_delete( nc );
	}
	return servRes;
}
/*---------------------------------------------------------------------------------------------------*/
/**
 * Routine handler of incoming packages.
 * Listen 80 port. For all new connection
 * create new task. Put it to the loop.
 * Input:		none
 * Output:	server error code
 */
SERVER_ERROR eSERVERlistenRoutine( void )
{
	SERVER_ERROR 	servRes 	= SERVER_OK;
	err_t 				netconRes = ERR_OK;

	netconRes = netconn_accept( nc, &in_nc );			// Grab new connection
	if ( netconRes != ERR_OK )										// Block until we get an incoming connection
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
  		eHTTPresponse( input, &request, &response, output );
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
  					if ( strncpy( output, ( pchSt ), ( pchEn - pchSt ) ) != NULL )
  					{
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
SERVER_ERROR eHTTPsendRequest( char* httpStr, char* hostName )
{
	SERVER_ERROR 	res     = SERVER_OK;
	struct 				netconn * ncr;
	struct 				netbuf  * nbr;
	char* 				IPstr;
	ip_addr_t 		remote_ip;
	uint16_t 			len;

	if ( netconn_gethostbyname( hostName, &remote_ip ) == ERR_OK )
	{
		IPstr = ip4addr_ntoa( &remote_ip );
		ip4addr_aton( IPstr, &remote_ip );
		ncr = netconn_new( NETCONN_TCP );
		if ( ncr != NULL )
		{
			if ( netconn_bind( ncr, &gnetif.ip_addr, 0U ) == ERR_OK )
			{
				if ( netconn_connect( ncr, &remote_ip, 80U ) == ERR_OK )
				{
					if ( netconn_write( ncr, httpStr, strlen( httpStr ), NETCONN_COPY ) == ERR_OK )
					{
						if ( netconn_recv( ncr, &nbr ) == ERR_OK )
						{
							len = netbuf_len( nbr );
							netbuf_copy( nbr, httpStr, len );
							netbuf_delete( nbr );
							httpStr[len] = 0;
						}
					}
					else res = SERVER_WRITE_ERROR;
				}
				else res = SERVER_REMOTE_CONNECT_ERROR;
			}
			else res = SERVER_BIND_ERROR;
		}
		else res = SERVER_NEW_CONNECT_ERROR;
	}
	else res = SERVER_DNS_ERROR;
/*
	if ( netconn_close( ncr ) != ERR_OK )
	{
		res = SERVER_CLOSE_ERROR;
	}
	netconn_delete( ncr );
*/
	//vSYSSerial( httpStr );
	return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*
 * Send request, get and parsing response
 * input:		request 	- input request structure
 * 					response 	- output response structure
 * 					output 		- buffer for parsed data of response
 * output:	status of operation
 */
HTTP_STATUS eHTTPrequest( HTTP_REQUEST* request, HTTP_RESPONSE* response, char* output )
{
	HTTP_STATUS res    = HTTP_STATUS_BAD_REQUEST;
	char				buffer[HTTP_BUFER_SIZE];

	if ( eHTTPmakeRequest( buffer, request ) == HTTP_STATUS_OK )
	{
		if ( eHTTPsendRequest( buffer, request->host ) == SERVER_OK )
		{
			res = eHTTPparsingResponse( buffer, output, response );
		}
	}

	return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Make response for input request
 * input:		input    - input http string
 * 					request  - output structure after parsing
 * 					response - output structure
 * 					output   - response string
 * output:	none
 */
void eHTTPresponse( char* input, HTTP_REQUEST* request, HTTP_RESPONSE* response, char* output )
{
	eHTTPparsingRequest( input, request );
	eHTTPbuildResponse( request, response );
	eHTTPmakeResponse( output, response );
	return;
}
/*---------------------------------------------------------------------------------------------------*/











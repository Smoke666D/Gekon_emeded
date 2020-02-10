/*
 * eth_common.c
 *
 *  Created on: Jan 30, 2020
 *      Author: mikhail.mikhailov
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "eth_common.h"
#include "http.h"
#include "sys.h"

#include "cmsis_os.h"

#include "lwip.h"
#include "api.h"
#include "string.h"

#include "../Site/index.h"
/*----------------------- Structures ----------------------------------------------------------------*/
static 		struct 		netconn * nc;
static 		struct 		netconn * in_nc;

static 		osThreadId_t 	netClientHandle;
/*----------------------- Variables -----------------------------------------------------------------*/
volatile 	err_t 			res 			= ERR_OK;
static 		ip_addr_t		local_ip;
static		char 				localIpStr[IP4ADDR_STRLEN_MAX];
/*
static		ip_addr_t 	remote_ip;
*/
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
/**
 * Open 80 port and start listen it
 */
void vETHinitLwip( void )
{
	while ( gnetif.ip_addr.addr == 0U )
	{
		osDelay(1);
	}
	cETHgetStrIP( localIpStr );
	local_ip  = gnetif.ip_addr;
	nc = netconn_new( NETCONN_TCP );
	if ( nc == NULL )
	{
		while( 1 ) osDelay( 1 );
	}
	res = netconn_bind(nc, IP_ADDR_ANY, 80);
	if ( res != ERR_OK )
	{
		while( 1 ) osDelay( 1 );
	}
	res = netconn_listen( nc );
	if ( res != ERR_OK )
	{
		while( 1 ) osDelay( 1 );
	}
}
/*---------------------------------------------------------------------------------------------------*/
/**
 * Listen open port
 */
uint8_t vETHlistenRoutine( void )
{
	uint8_t client = 0U;

	res = netconn_accept( nc, &in_nc );
	if ( res != ERR_OK )
	{
		client = 0U;
	}
	else
	{
		const osThreadAttr_t netClientTask_attributes = {
				.name       = "netClientTask",
				.priority   = ( osPriority_t ) osPriorityLow,
				.stack_size = 1024U
		};
		osDelay( 1 );
		netClientHandle = osThreadNew( startNetClientTask, (void*)in_nc, &netClientTask_attributes );
		client = 1U;
	}
	return client;
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
	err_t					res         = ERR_OK;

	uint32_t			mesNum			= 0U;
	char*					pchSt;
	char*					pchEn;
	uint32_t			i 					= 0U;

	uint32_t			control 		= 0U;
	uint16_t 			outlen  		= 0;

  for(;;)
  {
  	res = netconn_recv( netcon, &nb );
  	if( res == ERR_OK )
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
  			control = 0U;
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
  				  control += outlen;
  				}

  				char buffer[6];
  				sprintf( buffer, "%lu", control );
  				vSYSSerial(buffer);

  				vSYSSerial("\r\n");
  			}
  		}
  	}
  	vPortFree( input );
  	vPortFree( output );
  	osThreadExit();
  }
}
/*---------------------------------------------------------------------------------------------------*/









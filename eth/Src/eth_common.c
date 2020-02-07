/*
 * eth_common.c
 *
 *  Created on: Jan 30, 2020
 *      Author: mikhail.mikhailov
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "eth_common.h"
#include "http.h"
#include "index.h"

#include "sys.h"

#include "cmsis_os.h"

#include "lwip.h"
#include "api.h"
#include "string.h"
/*----------------------- Structures ----------------------------------------------------------------*/
static 		struct 		netconn * nc;
static 		struct 		netconn * in_nc;

static 		osThreadId_t 	netClientHandle;
static 		HTTP_RESPONSE response;
static		HTTP_REQUEST	request;
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
		netClientHandle = osThreadNew( startNetClientTask, (void*)in_nc, &netClientTask_attributes );
		client = 1U;
	}
	return client;
}
/*---------------------------------------------------------------------------------------------------*/
static char buf[500];

#define	httpBufSize		256
static char	output[httpBufSize];

void startNetClientTask( void const * argument )
{
	struct 		netconn * netcon = ( struct netconn * )argument;
	struct 		netbuf  * nb;
	//char *		buffer       = pvPortMalloc( 2048U );
	uint32_t 	len          = 0U;
	err_t			res          = ERR_OK;

	uint32_t	mesNum			= 0U;
	char*			pchSt;
	char*			pchEn;
	uint32_t	i = 0U;

	uint32_t	control = 0U;

  for(;;)
  {
  	res = netconn_recv( netcon, &nb );
  	if( res == ERR_OK )
  	{
  		len = netbuf_len( nb );
  		netbuf_copy( nb, buf, len );
  		netbuf_delete( nb );
  		buf[len] = 0U;

  		eHTTPparsingRequest( buf, &request );
  		eHTTPbuildResponse( request, &response );
  		eHTTPmakeResponse( output, response );


  		vSYSSerial("******************************************************\n\r");
  		vSYSSerial( buf );
  		vSYSSerial("******************************************************\n\r");
  		vSYSSerial(output);
  		vSYSSerial("******************************************************\n\r");

  		if ( response.contentLength != HTTP_METHOD_NO )
  		{
  			vSYSSerial("[");
  			netconn_write( netcon, output, strlen(output), NETCONN_COPY );

  			if ( response.contentLength != 0U )
  			{
  				vSYSSerial("[");
  				mesNum = ( uint32_t )( HTML_LENGTH / httpBufSize ) + 2U;
  				pchSt  = response.data;
  				pchEn  = pchSt + httpBufSize;
  				for( i=0U; i<mesNum; i++ )
  				{
  					strncpy( output, ( pchSt ), ( pchEn - pchSt ) );
  				  pchSt  = pchEn;
  				  pchEn  = pchSt + httpBufSize;
  				  vSYSSerial("|");
  				  netconn_write( netcon, output, strlen(output), NETCONN_COPY );
  				  control += strlen(output);
  				}
  				vSYSSerial("]");
  				char buffer[6];
  				sprintf( buffer, "%lu", control );
  				vSYSSerial("\r\n");
  				vSYSSerial(buffer);
  			}
  		}
  	}
  	osDelay( 1 );
  }
}
/*---------------------------------------------------------------------------------------------------*/









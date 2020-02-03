/*
 * eth_common.c
 *
 *  Created on: Jan 30, 2020
 *      Author: mikhail.mikhailov
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "eth_common.h"
#include "lwip.h"
#include "ip4_addr.h"
#include "cmsis_os.h"
#include "api.h"
#include "string.h"
/*----------------------- Structures ----------------------------------------------------------------*/
static 		struct 		netconn * nc;
//static 		struct 		netconn * in_nc;
//static 		struct 		netbuf 	* nb;
/*----------------------- Variables -----------------------------------------------------------------*/
static			uint8_t		start = 1;
volatile 	err_t 			nc_res;
//static		uint16_t 		len;
static 		ip_addr_t		local_ip;
static		ip_addr_t 	remote_ip;
static		char localIpStr[IP4ADDR_STRLEN_MAX];
/*----------------------- Functions -----------------------------------------------------------------*/
void cETHgetStrIP ( char* ipStr );
/*---------------------------------------------------------------------------------------------------*/
void cETHgetStrIP ( char* ipStr )
{
	uint8_t i = 0;
	char*   pointer = ip4addr_ntoa( &gnetif.ip_addr );;

	for( i=0; i<IP4ADDR_STRLEN_MAX; i++)
	{
		ipStr[i] = pointer[i];
	}

	return;
}
/*---------------------------------------------------------------------------------------------------*/
void vETHinitLwip( void )
{
	if ( gnetif.ip_addr.addr == 0U )
	{
		osDelay(1);
	}
	else
	{
		if (start)
		{
			cETHgetStrIP( localIpStr );
			local_ip  = gnetif.ip_addr;
			ip4addr_aton( "172.217.22.4", &remote_ip );
			nc = netconn_new( NETCONN_TCP );
			if ( nc == NULL )
			{
				// error
			}
			nc_res = netconn_bind( nc, &gnetif.ip_addr. 0U );
			if ( nc_res != ERR_OK )
			{
				// error
			}
			nc_res = netconn_connect( nc, &remote_ip, 80U );
			start = 0;
		}
	}
	/*
	local_ip  = uETHgetIP();
	ip4addr_aton( "5.255.255.77", &remote_ip );


	nc = netconn_new( NETCONN_TCP );
	if ( nc == NULL )
	{
		// error
	}
	nc_res = netconn_bind( nc, &local_ip. 0U );
	if ( nc_res != ERR_OK )
	{
		// error
	}
	nc_res = netconn_connect( nx, &remote_ip, 80U );
	*/
}
/*---------------------------------------------------------------------------------------------------*/

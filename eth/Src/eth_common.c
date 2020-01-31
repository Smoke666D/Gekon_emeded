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
static 		struct 		netconn * in_nc;
static 		struct 		netbuf 	* nb;
/*----------------------- Variables -----------------------------------------------------------------*/
volatile 	err_t 			nc_res;
static		uint16_t 		len;
static 		ip_addr_t		local_ip;
static		ip_addr_t 	remote_ip;
/*----------------------- Functions -----------------------------------------------------------------*/
uint32_t uETHgetIP ( void );
void cETHgetStrIP ( char* ipStr );
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	getIP
* @param 	none
* @retval IP address
*/
uint32_t uETHgetIP ( void )
{
	return gnetif.ip_addr.addr;
}
/*---------------------------------------------------------------------------------------------------*/
void cETHgetStrIP ( char* ipStr )
{
	return ip4addr_ntoa( uETHgetIP() );
}
/*---------------------------------------------------------------------------------------------------*/
void vETHinitLwip( void )
{
	MX_LWIP_Init();
	while ( uETHgetIP == 0U )
	{
		osDelay(1);
	}
	local_ip  = uETHgetIP();
	ip4addr_aton( "5.255.255.77", &remote_ip );


	nc = netconn_new( NETCONN_TCP );
	if ( nc == NULL )
	{
		/* error */
	}
	nc_res = netconn_bind( nc, &local_ip. 0U );
	if ( nc_res != ERR_OK )
	{
		/* error */
	}
	nc_res = netconn_connect( nx, &remote_ip, 80U );
}
/*---------------------------------------------------------------------------------------------------*/

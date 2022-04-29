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
#include "common.h"
#include "ip_addr.h"
#include "opt.h"
#include "ethernetif.h"
#include "system.h"
/*----------------------- Structures ----------------------------------------------------------------*/
static struct netconn* nc            = NULL;   // Connection to the port 80
static struct netconn* in_nc         = NULL;   // New user connection
static osThreadId_t    netTaskHandle = NULL;
/*----------------------- Constant ------------------------------------------------------------------*/
static const char defaultIp[IP4ADDR_STRLEN_MAX] = { '0', '0', '.', '0', '0', '.', '0', '0', '.', '0', '0', 0U, 0U, 0U, 0U, 0U };
/*----------------------- Variables -----------------------------------------------------------------*/
static char         serverInput[HTTP_INPUT_BUFFER_SIZE]   = { 0U };
static char         serverOutput[HTTP_OUTPUT_BUFFER_SIZE] = { 0U };
static SERVER_STATE serverState                           = SERVER_STATE_UNPLUG;
static osThreadId_t linkHandle                            = NULL;
/*----------------------- Functions -----------------------------------------------------------------*/
SERVER_ERROR    eHTTPsendRequest ( const char* hostName, char* httpStr );
RECEIVE_MESSAGE eSERVERanalizMessage ( const char* message, uint32_t length );
void            vLINKTask ( void* argument );
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/**
 * Start server. Open 80 port and start listen it.
 * Run it one time to start.
 * Input:  none
 * Output: server error code
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
SERVER_ERROR eSERVERstart ( void )
{
  SERVER_ERROR 	servRes   = SERVER_OK;
  err_t         netconRes = ERR_OK;

  nc = netconn_new( NETCONN_TCP );                   // Create new network connection TCP TYPE
  if ( nc != NULL )
  {
    netconRes = netconn_bind ( nc, IP_ADDR_ANY, 80U ); // Bind connection to well known port number 80
    if ( netconRes == ERR_OK )
    {
      netconRes = netconn_listen( nc );	             // Tell connection to go into listening mode
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
/*
 * Stop server connection
 * Input:  none
 * Output: server error code
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
SERVER_ERROR eSERVERstop ( void )
{
  SERVER_ERROR 	servRes   = SERVER_CLOSE_ERROR;
  err_t         netconRes = ERR_OK;

  netconRes = netconn_close( nc );
  if ( netconRes == ERR_OK )
  {
    netconRes = netconn_delete( nc );
  }
  return servRes;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
RECEIVE_MESSAGE eSERVERanalizMessage ( const char* message, uint32_t length )
{
  RECEIVE_MESSAGE res                 = RECEIVE_MESSAGE_ERROR;
  char*           pchSt               = NULL;
  char*           pchEn               = NULL;
  char            buffer[5U]          = { 0U };
  uint32_t        contentLengthHeader = 0U;

  pchSt = strstr( message, "PUT" );
  if ( ( pchSt != NULL ) && ( pchSt[0U] < 0x7FU ) )
  {
    pchSt = strstr( message, HTTP_LENGTH_LINE );
    if ( ( pchSt != NULL) && ( pchSt[0U] < 0x7FU ) )
    {
      pchSt = &pchSt[strlen( HTTP_LENGTH_LINE )];
      pchEn = strchr( pchSt, LF_HEX );
      if ( pchEn != NULL )
      {
        pchEn -= 1U;
        ( void )strncpy( buffer, pchSt, ( pchEn - pchSt ) );
        contentLengthHeader = atoi( buffer );
        if ( contentLengthHeader > 0U )
        {
          pchSt = strstr( message, HTTP_END_HEADER );
          if ( pchSt != NULL )
          {
            pchSt = &pchSt[strlen( HTTP_END_HEADER )];
            if ( strlen( pchSt ) >= contentLengthHeader )
            {
              res = RECEIVE_MESSAGE_COMPLETE;
            }
            else
            {
              res = RECEIVE_MESSAGE_CONTINUES;
            }
          }
        }
        else if ( message[length - 1U] == CR_HEX )
        {
          res = RECEIVE_MESSAGE_COMPLETE;
        }
        else
        {

        }
      }
    }
  }
  else
  {
    res = RECEIVE_MESSAGE_COMPLETE;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vSERVERclientClose ( struct netconn* netcon )
{
  netconn_close( netcon );
  netconn_delete( netcon );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Make response for input request
 * input:  input    - input http string
 *         request  - output structure after parsing
 *         response - output structure
 *         output   - response string
 * output: none
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void eHTTPresponse ( char* input, HTTP_REQUEST* request, HTTP_RESPONSE* response, char* output, uint32_t remoteIP )
{
  eHTTPparsingRequest( input, request );
  vHTTPbuildResponse( request, response, remoteIP );
  vHTTPmakeResponse( output, response );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Send request, get and parsing response
 * input:  request  - input request structure
 *         response - output response structure
 *         output   - buffer for parsed data of response
 * output: status of operation
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
HTTP_STATUS eHTTPrequest ( HTTP_REQUEST* request, HTTP_RESPONSE* response, char* output )
{
  HTTP_STATUS res                     = HTTP_STATUS_BAD_REQUEST;
  char        buffer[HTTP_BUFER_SIZE] = { 0U };
  vHTTPmakeRequest( request, buffer );
  if ( eHTTPsendRequest( request->host, buffer ) == SERVER_OK )
  {
    res = eHTTPparsingResponse( buffer, output, response );
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/**
 * Routine handler of incoming packages.
 * Listen 80 port. For all new connection
 * create new task. Put it to the loop.
 * Input:  none
 * Output: server error code
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
SERVER_ERROR eSERVERlistenRoutine ( void )
{
  SERVER_ERROR     servRes    = SERVER_OK;
  struct netbuf*   nb         = NULL;
  char*            endInput   = serverInput;
  RECEIVE_MESSAGE  endMessage = RECEIVE_MESSAGE_CONTINUES;
  HTTP_RESPONSE    response   = { 0U };
  HTTP_REQUEST     request    = { 0U };
  uint32_t         len        = 0U;
  STREAM_STATUS    status     = STREAM_CONTINUES;
  uint32_t         remoteIP   = 0U;

  if ( netconn_accept( nc, &in_nc ) != ERR_OK )                 // Grab new connection && Block until we get an incoming connection
  {
    servRes = SERVER_ACCEPT_ERROR;
  }
  else
  {
    if ( netconn_recv( in_nc, &nb ) == ERR_OK )
    {
      /*-------------------- Input message --------------------*/
      len = netbuf_len( nb );                                /* Get length of input message */
      netbuf_copy( nb, endInput, len );                      /* Copy message from net buffer to local buffer */
      remoteIP = in_nc->pcb.ip->remote_ip.addr;              /* Get remote IP address */
      netbuf_delete( nb );                                   /* Delete net buffer */
      endInput[len] = 0x00U;                                 /* Mark end of string */
      endMessage = eSERVERanalizMessage( serverInput, len ); /* Analysis is the message have been ended */
      /*------------------- Analysis Message -------------------*/
      if ( endMessage == RECEIVE_MESSAGE_COMPLETE )
      {
        endInput = serverInput;                                                    /* Return pointer to the start of the local buffer */
        eHTTPresponse( serverInput, &request, &response, serverOutput, remoteIP ); /* Parsing request and prepare the response        */
        /*-------------------- Send response ---------------------*/
        if ( response.status != HTTP_STATUS_ERROR )
        {
           ( void )netconn_write( in_nc, serverOutput, strlen( serverOutput ), NETCONN_COPY );  /* Send header of the response */
          /*-------------------- Send content ----------------------*/
          if ( response.content.length > 0U )                                           /* There is content */
          {
            while ( status == STREAM_CONTINUES )
            {
              status = response.callBack( &response.stream );
              if ( status != STREAM_ERROR )
              {
                ( void )netconn_write( in_nc, response.stream.content, response.stream.length, response.stream.flag );	/* Send content */
              }
              else
              {
                break;
              }
            }
            status = STREAM_CONTINUES;
          }
        }
        vSERVERclientClose( in_nc );
      }
      /*-------------------- Continue message --------------------*/
      else if ( endMessage == RECEIVE_MESSAGE_CONTINUES )
      {
        endInput = &endInput[len];
      }
      else
      {
        vSERVERclientClose( in_nc );
      }
    }
    else
    {
      /*--------------------- Close connection ---------------------*/
      vSERVERclientClose( in_nc );
    }
  }
  return servRes;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vStartNetTask ( void *argument )
{
  if ( linkHandle == NULL )
  {
    const osThreadAttr_t linkTask_attributes = {
      .name       = "linkTask",
      .priority   = ( osPriority_t ) ETHERNET_LINK_TASK_PRIORITY,
      .stack_size = ETHERNET_LINK_TASK_STACK_SIZE
    };
    linkHandle = osThreadNew( vLINKTask, NULL, &linkTask_attributes );
  }
  for(;;)
  {
    if ( serverState == SERVER_STATE_UP )
    {
      eSERVERlistenRoutine();
    }
    osDelay( 10U );
  }
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
SERVER_ERROR eHTTPsendRequest ( const char* hostName, char* httpStr )
{
  SERVER_ERROR 	  res       = SERVER_OK;
  #if ( LWIP_DNS > 0U )
  struct netconn* ncr       = NULL;
  struct netbuf*  nbr       = NULL;
  char*           IPstr     = NULL;
  ip_addr_t       remote_ip = { 0U };
  uint16_t        len       = 0U;

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
              httpStr[len] = 0U;
            }
          }
          else
          {
            res = SERVER_WRITE_ERROR;
          }
        }
        else
        {
          res = SERVER_REMOTE_CONNECT_ERROR;
        }
      }
      else
      {
        res = SERVER_BIND_ERROR;
      }
    }
    else
    {
      res = SERVER_NEW_CONNECT_ERROR;
    }
  }
  else
  {
    res = SERVER_DNS_ERROR;
  }
  #else
    res = SERVER_DNS_ERROR;
  #endif
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vSERVERaddConnection ( void )
{
  ip4_addr_t ipaddr  = { 0U };
  ip4_addr_t netmask = { 0U };
  ip4_addr_t gw      = { 0U };
  ipaddr.addr  = 0U;
  netmask.addr = 0U;
  gw.addr      = 0U;
  // Here we can write new MAC address!!!
  netif_add( &gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vSERVERinitConnection ( void )
{
  char ipaddrStr[16U] = { 0U };
  vHTTPinit();
  while ( gnetif.ip_addr.addr == 0U )
  {
    osDelay( 1U );    // Wait the ip to reach the structure
  }
  ( void )uSERVERgetStrIP( ipaddrStr );
  if ( eSERVERstart() != SERVER_OK )
  {
    while( 1U )
    {
      osDelay( 1U );
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vSERVERcheckPlug ( void )
{
  uint32_t phyreg = 0U;
  switch ( serverState )
  {
    case SERVER_STATE_UNPLUG:
      tcpip_init( NULL, NULL );
      vSERVERaddConnection();
      serverState = SERVER_STATE_PLUG;
      break;
    case SERVER_STATE_PLUG:
      if ( HAL_ETH_ReadPHYRegister( &heth, PHY_BSR, &phyreg ) == HAL_OK )
      {
	      if ( ( phyreg & PHY_LINKED_STATUS ) > 0U )
	      {
	        netif_set_default( &gnetif );
	        netif_set_up( &gnetif );
	        dhcp_start( &gnetif );
	        vSERVERinitConnection();
	        serverState = SERVER_STATE_UP;
	      }
      }
      break;
    case SERVER_STATE_DOWN:
      if ( HAL_ETH_ReadPHYRegister( &heth, PHY_BSR, &phyreg ) == HAL_OK )
      {
	      if ( ( phyreg & PHY_LINKED_STATUS ) > 0U )
	      {
	        netif_set_up( &gnetif );
	        serverState = SERVER_STATE_UP;
	      }
      }
      break;
    case SERVER_STATE_UP:
      if ( HAL_ETH_ReadPHYRegister( &heth, PHY_BSR, &phyreg ) == HAL_OK )
      {
	if ( ( phyreg & PHY_LINKED_STATUS ) == 0U )
	{
	  netif_set_down( &gnetif );
	  serverState = SERVER_STATE_DOWN;
	}
      }
      break;
    default:
      break;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vLINKTask ( void* argument )
{
  for(;;)
  {
    vSERVERcheckPlug();
    osDelay( 1000U );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLIC --------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*
 * Waiting the end of server initialization.
 * IP address available after the end of this function.
 * Run it one time after MX_LWIP_Init().
 * Input:  none
 * Output: none
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vSERVERinit ( void )
{
  #if defined( ETHERNET )
    const osThreadAttr_t netTask_attributes = {
      .name = "netTask",
      .stack_size = NET_TASK_STACK_SIZE,
      .priority = (osPriority_t) NET_TASK_PRIORITY
    };
    netTaskHandle = osThreadNew( vStartNetTask, NULL, &netTask_attributes );
  #endif
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Read local IP address of device in char array format
 * Input:  buffer of char for IP address
 *         length of buffer is IP4ADDR_STRLEN_MAX = 16
 *         from ip4_addr.h
 *         if ipStr[0] = 0x00, initialization havn't finished
 * Output: none
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
uint8_t uSERVERgetStrIP ( char* ipStr )
{
  uint8_t i       = 0U;
  char*   pointer = ip4addr_ntoa( &gnetif.ip_addr );
  if ( serverState == SERVER_STATE_UP )
  {
    for( i=0U; i<IP4ADDR_STRLEN_MAX; i++ )
    {
      ipStr[i] = pointer[i];
    }
  }
  else
  {
    for ( i=0U; i<IP4ADDR_STRLEN_MAX; i++ )
    {
      ipStr[i] = defaultIp[i];
    }
  }
  return strlen( ipStr );
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t* pSERVERgetMAC ( void )
{
  return heth.Init.MACAddr;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/


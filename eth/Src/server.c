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
/*----------------------- Structures ----------------------------------------------------------------*/
static struct netconn* nc;             // Connection to the port 80
static struct netconn* in_nc;          // New user connection
/*------------------------- Task --------------------------------------------------------------------*/
static osThreadId_t  netClientHandle;  // Network task handle
/*----------------------- Variables -----------------------------------------------------------------*/

/*----------------------- Functions -----------------------------------------------------------------*/
void            startNetClientTask (void const * argument);              // Network task function
SERVER_ERROR    eHTTPsendRequest ( const char* hostName, char* httpStr );
RECEIVE_MESSAGE eSERVERanalizMessage ( const char* message, uint32_t length );
/*---------------------------------------------------------------------------------------------------*/
/**
 * Read local IP address of device in char array format
 * Input:  buffer of char for IP address
 *         length of buffer is IP4ADDR_STRLEN_MAX = 16
 *         from ip4_addr.h
 *         if ipStr[0] = 0x00, initialization havn't finished
 * Output: none
 */
void cSERVERgetStrIP ( char* ipStr )
{
  uint8_t i       = 0U;
  char*   pointer = ip4addr_ntoa( &gnetif.ip_addr );

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
 * Input:  none
 * Output: none
 */
void vSERVERinit ( void )
{
  vHTTPinit();
  while ( gnetif.ip_addr.addr == 0U )
  {
    osDelay( 1U );		// Wait the ip to reach the structure
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
 * Start server. Open 80 port and start listen it.
 * Run it one time to start.
 * Input:  none
 * Output: server error code
 */
SERVER_ERROR eSERVERstart ( void )
{
  SERVER_ERROR 	servRes   = SERVER_OK;
  err_t         netconRes = ERR_OK;

  nc = netconn_new( NETCONN_TCP );                   // Create new network connection TCP TYPE
  if ( nc != NULL )
  {
    netconRes = netconn_bind ( nc, IP_ADDR_ANY, 80 ); // Bind connection to well known port number 80
    if ( netconRes == ERR_OK )
    {
      netconRes = netconn_listen( nc );	             // Tell connection to go into listening mode
      if ( netconRes != ERR_OK )
      {
        servRes = SERVER_LISTEN_ERROR;
      }
    } else servRes = SERVER_BIND_ERROR;
  } else servRes = SERVER_NEW_CONNECT_ERROR;
  return servRes;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Stop server connection
 * Input:  none
 * Output: server error code
 */
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
/**
 * Routine handler of incoming packages.
 * Listen 80 port. For all new connection
 * create new task. Put it to the loop.
 * Input:  none
 * Output: server error code
 */
SERVER_ERROR eSERVERlistenRoutine ( void )
{
  SERVER_ERROR servRes   = SERVER_OK;
  err_t        netconRes = ERR_OK;

  netconRes = netconn_accept( nc, &in_nc );  // Grab new connection
  if ( netconRes != ERR_OK )                 // Block until we get an incoming connection
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
RECEIVE_MESSAGE eSERVERanalizMessage ( const char* message, uint32_t length )
{
  RECEIVE_MESSAGE res                 = RECEIVE_MESSAGE_ERROR;
  char*           pchSt               = NULL;
  char*           pchEn               = NULL;
  char            buffer[5U]          = { 0U, 0U, 0U, 0U, 0U };
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
        strncpy( buffer, pchSt, ( pchEn - pchSt ) );
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
void startNetClientTask ( void const * argument )
{
  struct netconn*  netcon     = ( struct netconn * )argument;
  struct netbuf*   nb         = NULL;
  char*            input      = pvPortMalloc( HTTP_INPUT_BUFFER_SIZE );
  char*            endInput   = input;
  RECEIVE_MESSAGE  endMessage = RECEIVE_MESSAGE_CONTINUES;
  char*            output     = pvPortMalloc( HTTP_OUTPUT_BUFFER_SIZE );
  HTTP_RESPONSE    response;
  HTTP_REQUEST     request;
  uint32_t         len        = 0U;
  STREAM_STATUS    status     = STREAM_CONTINUES;
  uint32_t         remoteIP   = 0U;
  for (;;)
  {
    if ( netconn_recv( netcon, &nb ) == ERR_OK )
    {
      /*-------------------- Input message --------------------*/
      len = netbuf_len( nb );                            /* Get length of input message */
      netbuf_copy( nb, endInput, len );                  /* Copy message from net buffer to local buffer */
      remoteIP = netcon->pcb.ip->remote_ip.addr;         /* Get remote IP address */
      netbuf_delete( nb );                               /* Delete net buffer */
      endInput[len] = 0x00U;                             /* Mark end of string */
      endMessage = eSERVERanalizMessage( input, len );   /* Analysis is the message have been ended */
      /*------------------- Analysis Message -------------------*/
      if ( endMessage == RECEIVE_MESSAGE_COMPLETE )
      {
        endInput = input;                                              /* Return pointer to the start of the local buffer */
        eHTTPresponse( input, &request, &response, output, remoteIP ); /* Parsing request and prepare the response */
  	  /*-------------------- Send response ---------------------*/
        if ( response.status != HTTP_STATUS_ERROR )
        {
          netconn_write( netcon, output, strlen( output ), NETCONN_COPY );  /* Send header of the response */
  	  /*-------------------- Send content ----------------------*/
          if ( response.contentLength > 0U )															  /* There is content */
          {
            while ( status == STREAM_CONTINUES )
            {
              status = response.callBack( &response.stream );
              if ( status != STREAM_ERROR )
              {
                netconn_write( netcon, response.stream.content, response.stream.length, NETCONN_COPY );	/* Send content */
              }
              else
              {
                break;
              }
            }
            status = STREAM_CONTINUES;
          }
        }
      }
      /*-------------------- Continue message --------------------*/
      else if ( endMessage == RECEIVE_MESSAGE_CONTINUES )
      {
        endInput = &endInput[len];
      }
    }
    /*--------------------- Close connection ---------------------*/
    netconn_close( netcon );
    netconn_delete( netcon );
    vPortFree( input );
    vPortFree( output );
    osThreadExit();
  }
}
/*---------------------------------------------------------------------------------------------------*/
SERVER_ERROR eHTTPsendRequest ( const char* hostName, char* httpStr )
{
  SERVER_ERROR 	   res     = SERVER_OK;
  struct netconn * ncr;
  struct netbuf  * nbr;
  char*            IPstr;
  ip_addr_t        remote_ip;
  uint16_t         len;

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
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Send request, get and parsing response
 * input:  request  - input request structure
 *         response - output response structure
 *         output   - buffer for parsed data of response
 * output: status of operation
 */
HTTP_STATUS eHTTPrequest ( HTTP_REQUEST* request, HTTP_RESPONSE* response, char* output )
{
  HTTP_STATUS res = HTTP_STATUS_BAD_REQUEST;
  char        buffer[HTTP_BUFER_SIZE];

  if ( eHTTPmakeRequest( request, buffer ) == HTTP_STATUS_OK )
  {
    if ( eHTTPsendRequest( request->host, buffer ) == SERVER_OK )
    {
      res = eHTTPparsingResponse( buffer, output, response );
    }
  }
  return res;
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
void eHTTPresponse ( char* input, HTTP_REQUEST* request, HTTP_RESPONSE* response, char* output, uint32_t remoteIP )
{
  eHTTPparsingRequest( input, request );
  vHTTPbuildResponse( request, response, remoteIP );
  eHTTPmakeResponse( output, response );
  return;
}
/*---------------------------------------------------------------------------------------------------*/

void vStartNetTask(void *argument)
{
  char ipaddr[16];
  vSYSSerial( ">>DHCP: ");
  vSERVERinit();
  vSYSSerial( "done!\n\r");
  cSERVERgetStrIP( ipaddr );
  vSYSSerial( ">>IP address: ");
  vSYSSerial( ipaddr );
  vSYSSerial("\n\r");
  vSYSSerial( ">>TCP: " );
  if ( eSERVERstart() != SERVER_OK )
  {
    vSYSSerial( "fail!\n\r" );
    while( 1U ) osDelay( 1U );
  }
  vSYSSerial( "done!\n\r" );
  vSYSSerial( ">>Server ready and listen port 80!\n\r" );
  HAL_GPIO_WritePin( GPIOB, LD3_Pin, GPIO_PIN_SET );
  for(;;)
  {
    eSERVERlistenRoutine();
    osDelay( 10U );
  }
}









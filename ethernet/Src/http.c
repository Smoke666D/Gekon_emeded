/*
 * http.c
 *
 *  Created on: Feb 6, 2020
 *      Author: mikhail.mikhailov
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "freeData.h"
#include "http.h"
#include "web.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "cmsis_os.h"
#include "rest.h"
#include "config.h"
#include "chart.h"
#include "EEPROM.h"
#include "storage.h"
#include "RTC.h"
#include "measurement.h"
#include "dataAPI.h"
#include "system.h"
/*----------------------- Structures ----------------------------------------------------------------*/
static char     restBuffer[REST_BUFFER_SIZE] = { 0U };
static RTC_TIME time;
/*----------------------- Constant ------------------------------------------------------------------*/
const char *httpMethodsStr[HTTP_METHOD_NUM] = { HTTP_METHOD_STR_GET, HTTP_METHOD_STR_POST, HTTP_METHOD_STR_PUT, HTTP_METHOD_STR_HEAD, HTTP_METHOD_STR_OPTION};
/*----------------------- Variables -----------------------------------------------------------------*/
static AUTH_IP_TYPE ethAuthList[CONNECT_STACK_SIZE] = { 0U }; /* List of authorization IPs*/
static uint8_t      ethAuthListPointer              = 0U;
/*----------------------- Functions -----------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
AUTH_STATUS eHTTPisAuth ( uint32_t ip )
{
  PASSWORD_TYPE password = { .data = 0U, .status = PASSWORD_RESET };
  AUTH_STATUS   res      = AUTH_VOID;
  uint8_t       i        = 0U;
  uint8_t       new      = 1U;
  if ( eDATAAPIpassword( DATA_API_CMD_READ, &password ) == DATA_API_STAT_OK )
  {
    if ( password.status == PASSWORD_SET )
    {
      for ( i=0U; i<CONNECT_STACK_SIZE; i++ )
      {
        if ( ip == ethAuthList[i].ip )
        {
          res = ethAuthList[i].status;
          new = 0U;
          break;
        }
      }
      if ( new > 0U )
      {
        ethAuthList[ethAuthListPointer].ip     = ip;
        ethAuthList[ethAuthListPointer].status = AUTH_VOID;
        res = AUTH_VOID;
        if ( ethAuthListPointer < CONNECT_STACK_SIZE )
        {
          ethAuthListPointer++;
        }
        else
        {
          ethAuthListPointer = 0U;
        }
      }
    }
    else
    {
      res = AUTH_DONE;
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vHHTPsetAuth ( uint32_t ip, AUTH_STATUS status )
{
  uint8_t i = 0U;
  if ( eHTTPisAuth( ip ) != status )
  {
    for ( i=0U; i<CONNECT_STACK_SIZE; i++ )
    {
      if ( ip == ethAuthList[i].ip )
      {
        ethAuthList[i].status = status;
      }
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Clean request structure
 * Input:  request structure
 * Output: none
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vHTTPcleanRequest ( HTTP_REQUEST *request )
{
  uint8_t i = 0U;
  request->method = HTTP_METHOD_NO;
  for ( i=0U; i<HTTP_PATH_LENGTH; i++)
  {
    request->path[i] = 0x00U;
  }
  for ( i=0U; i<HTTP_PATH_LENGTH; i++)
  {
    request->host[i] = 0x00U;
  }
  request->content.type = HTTP_CONTENT_HTML;
  request->connect      = HTTP_CONNECT_CLOSED;
  request->cache        = HTTP_CACHE_NO_CACHE;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Add type of the content to a string
 * input:  httpStr - target string
 *         type    - type of content
 * output: none
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vHTTPaddContentType ( char* httpStr, HTTP_CONTENT_TYPE type )
{
  ( void )strcat( httpStr, HTTP_CONTENT_LINE );
  switch ( type )
  {
    case HTTP_CONTENT_HTML:
      ( void )strcat( httpStr, HTTP_CONTENT_STR_HTML );
      break;
    case HTTP_CONTENT_CSS:
      ( void )strcat( httpStr, HHTP_CONTENT_STR_CSS );
      break;
    case HTTP_CONTENT_JS:
      ( void )strcat( httpStr, HTTP_CONTENT_STR_JS );
      break;
    case HTTP_CONTENT_JSON:
      ( void )strcat( httpStr, HTTP_CONTENT_STR_JSON );
      break;
    case HTTP_CONTENT_XML:
      ( void )strcat( httpStr, HTTP_CONTENT_STR_XML );
      break;
    default:
      ( void )strcat( httpStr, "Error" );
      break;
  }
  ( void )strcat( httpStr, HTTP_END_LINE );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vHTTPaddContentEncoding ( char* httpStr, HTTP_ENCODING encoding )
{
  if ( encoding != HTTP_ENCODING_NO )
  {
    ( void )strcat( httpStr, HTTP_ENCODING_LINE );
    switch ( encoding )
    {
      case HTTP_ENCODING_NO:
        break;
      case HTTP_ENCODING_GZIP:
        ( void )strcat( httpStr, HTTP_CONTENT_ENCODING_GZIP );
        break;
      case HTTP_ENCODING_COMPRESS:
        ( void )strcat( httpStr, HTTP_CONTENT_ENCODING_COMPRESS );
        break;
      case HTTP_ENCODING_DEFLATE:
        ( void )strcat( httpStr, HTTP_CONTENT_ENCODING_DEFLATE );
        break;
      case HTTP_ENCODING_INDENTITY:
        ( void )strcat( httpStr, HTTP_CONTENT_ENCODING_INDENTITY );
        break;
      case HTTP_ENCODING_BR:
        ( void )strcat( httpStr, HTTP_CONTENT_ENCODING_BR );
        break;
      default:
        break;
    }
    ( void )strcat( httpStr, HTTP_END_LINE );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Add cache string to http
 * input:  httpStr - pointer to output http string
 *         cache   - cache type to add
 * output: result of operation
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vHTTPaddCache ( char* httpStr, HTTP_CACHE cache)
{
  ( void )strcat( httpStr, HTTP_CACHE_CONTROL );
  switch ( cache )
  {
    case HTTP_CACHE_NO_CACHE:
      ( void )strcat( httpStr, HTTP_CACHE_STR_NO_CACHE );
      break;
    case HTTP_CACHE_NO_STORE:
      ( void )strcat( httpStr, HTTP_CACHE_STR_NO_STORE );
      break;
    case HTTP_CACHE_NO_CACHE_STORE:
      ( void )strcat( httpStr, HTTP_CACHE_STR_NO_CACHE );
      ( void )strcat( httpStr, "," );
      ( void )strcat( httpStr, HTTP_CACHE_STR_NO_STORE );
      break;
    default:
      ( void )strcat( httpStr, HTTP_CACHE_STR_NO_CACHE );
      ( void )strcat( httpStr, "," );
      ( void )strcat( httpStr, HTTP_CACHE_STR_NO_STORE );
      break;
  }
  ( void )strcat( httpStr, HTTP_END_LINE );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Stream call back for file transfer from flash
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
STREAM_STATUS cHTTPstreamFile ( HTTP_STREAM* stream )
{
  stream->status = STREAM_END;
  stream->index++;
  return stream->status;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Stream callback for file trasfer from EEPROM
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
STREAM_STATUS cHTTPstreamFileWebApp ( HTTP_STREAM* stream )
{
  stream->status = STREAM_CONTINUES;
  if ( stream->length > ( stream->size - stream->index ) )
  {
    stream->length = stream->size - stream->index;
    stream->status = STREAM_END;
  }
  stream->content = ( char* )( &data__web_html[stream->index] );
  stream->index += stream->length;
  return stream->status;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Stream call back for configuration data transfer
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
STREAM_STATUS cHTTPstreamConfigs ( HTTP_STREAM* stream )
{
  uint32_t length = stream->size - stream->start;
  if ( ( stream->index == 0U ) && ( length > 1U ) )
  {
    restBuffer[0U] = '[';
    stream->length = uRESTmakeConfig( configReg[stream->start + stream->index], &restBuffer[1U] ) + 1U;
  }
  else
  {
    stream->length = uRESTmakeConfig( configReg[stream->start + stream->index], restBuffer );
  }
  if ( stream->length == 0U )
  {
    stream->status = STREAM_ERROR;
  }
  else
  {
    stream->index++;
    if ( ( stream->start + stream->index ) >= stream->size )
    {
      if ( length > 1U )
      {
        restBuffer[stream->length] = ']';
      }
      stream->length++;
      stream->status = STREAM_END;
    }
    else
    {
      if ( length > 1U )
      {
        restBuffer[stream->length] = ',';
      }
      stream->length++;
      stream->status = STREAM_CONTINUES;
    }
    restBuffer[stream->length] = 0U;
    stream->content = restBuffer;
  }
  return stream->status;
}
/*---------------------------------------------------------------------------------------------------*/
STREAM_STATUS cHTTPstreamMeasurement ( HTTP_STREAM* stream )
{
  return stream->status;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Stream call back for charts data transfer
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
STREAM_STATUS cHTTPstreamCharts ( HTTP_STREAM* stream )
{
  uint32_t length = stream->size - stream->start;
  if ( ( stream->index == 0U ) && ( length > 1U ) )
  {
    restBuffer[0U] = '[';
    stream->length = uRESTmakeChart(  charts[stream->start + stream->index], &restBuffer[1U] ) + 1U;
  }
  else
  {
    stream->length = uRESTmakeChart( charts[stream->start + stream->index], restBuffer );
  }
  if ( stream->length == 0U )
  {
    stream->status = STREAM_ERROR;
  }
  else
  {
    stream->index++;
    if ( ( stream->start + stream->index ) >= stream->size )
    {
      if ( length > 1U )
      {
        restBuffer[stream->length] = ']';
      }
      stream->length++;
      stream->status = STREAM_END;
    }
    else
    {
      if ( length > 1U )
      {
        restBuffer[stream->length] = ',';
      }
      stream->length++;
      stream->status  = STREAM_CONTINUES;
    }
    restBuffer[stream->length] = 0U;
    stream->content = restBuffer;
  }
  return stream->status;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Stream callback for time transfer
 */

#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
STREAM_STATUS cHTTPstreamTime ( HTTP_STREAM* stream )
{
  restBuffer[stream->length] = 0U;
  stream->status             = STREAM_END;
  stream->length             = uRESTmakeTime( &time, restBuffer );
  stream->content            = restBuffer;
  stream->flag               = STREAM_FLAG_COPY;
  return stream->status;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
STREAM_STATUS cHTTPstreamData ( HTTP_STREAM* stream )
{
  stream->status             = STREAM_END;
  stream->length             = uRESTmakeData( *( freeDataArray[stream->start] ), restBuffer );
  restBuffer[stream->length] = 0U;
  stream->content            = restBuffer;
  stream->flag               = STREAM_FLAG_COPY;
  return stream->status;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
STREAM_STATUS cHTTPstreamString ( HTTP_STREAM* stream )
{
  stream->status             = STREAM_END;
  restBuffer[stream->length] = 0U;
  stream->content            = restBuffer;
  return stream->status;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
STREAM_STATUS cHTTPstreamLog ( HTTP_STREAM* stream )
{
  LOG_RECORD_TYPE record = { 0U };
  uint8_t         shift  = 0U;
  uint16_t        adr    = 0U;
  if ( stream->index >= ( stream->size - 1U ) )
  {
    stream->status = STREAM_END;
  }
  else
  {
    stream->status = STREAM_CONTINUES;
    if ( stream->index == 0 ) {
      shift = 1U;
      restBuffer[0U] = '[';
    }
  }
  adr = ( uint16_t )stream->index;
  if ( eDATAAPIlog( DATA_API_CMD_LOAD, &adr, &record ) != DATA_API_STAT_OK )
  {
    stream->status = STREAM_ERROR;
  }
  stream->length = shift + uRESTmakeLog( &record, &restBuffer[shift] );
  if ( stream->status == STREAM_CONTINUES )
  {
    restBuffer[stream->length] = ',';
    stream->length++;
  }
  if ( stream->status == STREAM_END )
  {
    restBuffer[stream->length] = ']';
    stream->length++;
  }
  stream->index++;
  restBuffer[stream->length] = 0U;
  stream->content            = restBuffer;
  stream->flag               = STREAM_FLAG_COPY;
  return stream->status;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Clean response structure
 * Input:  request structure
 * Output: none
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vHTTPCleanResponse ( HTTP_RESPONSE *response )
{
  uint32_t i = 0U;

  response->status = HTTP_STATUS_BAD_REQUEST;
  response->method = HTTP_METHOD_NO;
  for ( i=0U; i<HEADER_LENGTH; i++ )
  {
    response->header[i] = 0U;
  }
  response->content.length = 0U;
  response->content.type   = HTTP_CONTENT_HTML;
  response->connect        = HTTP_CONNECT_CLOSED;
  response->cache          = HTTP_CACHE_NO_CACHE;
  response->encoding       = HTTP_ENCODING_NO;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Get the string of line from multiline text.
 * The end of line is LF simbol (Line Feed)
 * Input:  input - pointer to char array of multiline text
 *         num   - number of line of interest
 *         line       - pointer to char array of output buffer for line
 * Output: 1     - There is valid line in output buffer
 *         0     - There is no valid line in output buffer
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
uint8_t uHTTPgetLine ( const char* input, uint16_t num, char* line )
{
  uint8_t     res   = 0U;
  uint16_t    i     = 0U;
  const char* start = input;
  const char* end   = strchr( input, LF_HEX );
  for ( i=0U; i<num; i++ )
  {
    start = strchr( end, LF_HEX ) + 1U;
    end   = strchr( start, LF_HEX );
  }
  if ( start && end)
  {
    res = 1U;
    if ( strncpy( line, start, ( strlen( start ) - strlen( end ) ) ) == NULL )
    {
      res = 0U;
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vHTTPinit ( void )
{
  uint8_t i = 0U;
  for ( i=0U; i<CONNECT_STACK_SIZE; i++ )
  {
    ethAuthList[i].ip     = 0U;
    ethAuthList[i].status = AUTH_VOID;
  }
  ethAuthListPointer = 0U;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Parsing data from request text
 * Input:  req     - pointer to a char array with input request in text form
 *         request - pointer to a the output request structure
 * Output: HTTP status
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
HTTP_STATUS eHTTPparsingRequest ( const char* req, HTTP_REQUEST* request )
{
  HTTP_STATUS  res       = HTTP_STATUS_BAD_REQUEST;
  uint8_t      i         = 0U;
  char*        pchSt     = NULL;
  char*        pchEnd    = NULL;
  char         line[50U] = { 0U };

  if ( uHTTPgetLine( req, 0U, line ) > 0U )
  {
    res        = HTTP_STATUS_OK;
    vHTTPcleanRequest( request );
    /*--------------------------- Parsing HTTP methods ---------------------------*/
    for( i=0U; i<HTTP_METHOD_NUM; i++)
    {
      pchSt = strstr( line, httpMethodsStr[i] );
      if ( pchSt != NULL)
      {
        request->method = i + 1U;
        break;
      }
    }
    if ( request->method == HTTP_METHOD_NO )
    {
      res = HTTP_STATUS_BAD_REQUEST;
    }
    /*-----------------------------------------------------------------------------*/
    if ( res == HTTP_STATUS_OK )
    {
      /*--------------------------- Parsing HTTP path  ----------------------------*/
      pchSt  = strchr( line, '/' );
      pchEnd = strchr( pchSt, ' ' );
      if ( ( strlen( pchSt ) - strlen( pchEnd ) ) > 2U )
      {
        if ( strncpy( request->path, ( pchSt ), ( strlen( pchSt ) - strlen( pchEnd ) ) ) == NULL )
        {
          res = HTTP_STATUS_BAD_REQUEST;
        }
      }
      /*----------------------------- Parsing Content -----------------------------*/
      pchSt  = strstr( req, HTTP_END_HEADER );
      if ( pchSt != NULL )
      {
        request->content.data = &pchSt[strlen( HTTP_END_HEADER )];
      }
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Parsing data from response text
 * Input:  req     - pointer to a char array with input request in text form
 *         request - pointer to a output response structure
 * Output: HTTP status
 */
HTTP_STATUS eHTTPparsingResponse ( const char* input, char* data, HTTP_RESPONSE* response  )
{
  HTTP_STATUS res    = HTTP_STATUS_BAD_REQUEST;
  char*       pchSt  = NULL;
  char*       pchEn  = NULL;
  pchSt = strchr( input, ' ');
  pchSt++;
  if ( pchSt != NULL )
  {
    pchEn = strchr( pchSt, ' ' );
    if ( pchEn != NULL )
    {
      response->status = strtol( pchSt, &pchEn, 10U );
      if ( response->status == HTTP_STATUS_OK )
      {
        res = HTTP_STATUS_OK;
        pchSt = strstr( input, "\r\n\r\n" );
        pchSt = &pchSt[4U];
        if ( pchSt != NULL )
        {
          pchEn = strcpy( data, pchSt );
          if ( pchEn == NULL )
          {
            res = HTTP_STATUS_BAD_REQUEST;
          }
        }
        else
        {
          res = HTTP_STATUS_BAD_REQUEST;
        }
      }
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Build put response in to response structure
 * Input:  path     - url to the file from the request
 *         response - structure of the response
 *         content  - content of the request
 * Output: none
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vHTTPbuildPutResponse ( char* path, HTTP_RESPONSE *response, char* content, uint32_t remoteIP )
{
  uint16_t      adr        = 0xFFFFU;
  REST_REQUEST  request    = 0U;
  REST_ADDRESS  adrFlag    = REST_NO_ADR;
  uint16_t      data       = 0U;
  PASSWORD_TYPE password   = { .status = PASSWORD_RESET,.data = 0U };
  PASSWORD_TYPE passwordSt = { .status = PASSWORD_RESET,.data = 0U };
  response->header[strlen("Thu, 06 Feb 2020 15:11:53 GMT")] = 0U;
  response->cache          = HTTP_CACHE_NO_CACHE_STORE;
  response->connect        = HTTP_CONNECT_CLOSED;
  response->status         = HTTP_STATUS_BAD_REQUEST;
  response->content.length = 0U;
  if ( path[0U] > 0U )
  {
    adrFlag = eRESTgetRequest( path, &request, &adr );
    switch ( request )
    {
      case REST_CONFIGS:
        if ( eHTTPisAuth( remoteIP ) == AUTH_DONE )
        {
          if ( ( adr != 0xFFFFU ) && ( adr < SETTING_REGISTER_NUMBER ) && ( adrFlag != REST_NO_ADR ) )
          {
            if ( eRESTparsingConfig( content, adr ) == REST_OK )
            {
              response->content.type   = HTTP_CONTENT_JSON;
              response->status         = HTTP_STATUS_OK;
              response->content.length = 0U;
            }
          }
        }
        else
        {
          response->content.type   = HTTP_CONTENT_JSON;
          response->status         = HTTP_STATUS_UNAUTHORIZED;
          response->content.length = 0U;
        }
        break;
      case REST_CHARTS:
        if ( eHTTPisAuth( remoteIP ) == AUTH_DONE )
        {
          if ( ( adr != 0xFFFFU ) && ( adr < SETTING_REGISTER_NUMBER ) && ( adrFlag != REST_NO_ADR ) )
          {
            if ( eRESTparsingChart( content, adr ) == REST_OK )
            {
              response->content.type   = HTTP_CONTENT_JSON;
              response->status         = HTTP_STATUS_OK;
              response->content.length = 0U;
            }
          }
        }
        else
        {
          response->content.type   = HTTP_CONTENT_JSON;
          response->status         = HTTP_STATUS_UNAUTHORIZED;
          response->content.length = 0U;
        }
        break;
      case REST_SAVE_CONFIGS:
        if ( eHTTPisAuth( remoteIP ) == AUTH_DONE )
        {
          if ( eDATAAPIconfigValue( DATA_API_CMD_SAVE, 0U, NULL ) == DATA_API_STAT_OK )
          {
            response->content.type   = HTTP_CONTENT_JSON;
            response->status         = HTTP_STATUS_OK;
            response->content.length = 0U;
          }
        }
        else
        {
          response->content.type   = HTTP_CONTENT_JSON;
          response->status         = HTTP_STATUS_UNAUTHORIZED;
          response->content.length = 0U;
        }
        break;
      case REST_SAVE_CHARTS:
        if ( eHTTPisAuth( remoteIP ) == AUTH_DONE )
        {
          if ( eDATAAPIchart( DATA_API_CMD_SAVE, 0U, NULL ) == DATA_API_STAT_OK )
          {
            response->content.type   = HTTP_CONTENT_JSON;
            response->status         = HTTP_STATUS_OK;
            response->content.length = 0U;
          }
        }
        else
        {
          response->content.type   = HTTP_CONTENT_JSON;
          response->status         = HTTP_STATUS_UNAUTHORIZED;
          response->content.length = 0U;
        }
        break;
      case REST_TIME:
        if ( eHTTPisAuth( remoteIP ) == AUTH_DONE )
        {
          if ( eRESTparsingTime( content, &time ) == REST_OK )
          {
            if ( eRTCsetTime( &time ) == RTC_OK )
            {
              response->content.type   = HTTP_CONTENT_JSON;
              response->status         = HTTP_STATUS_OK;
              response->content.length = 0U;
            }
          }
        }
        else
        {
          response->content.type   = HTTP_CONTENT_JSON;
          response->status         = HTTP_STATUS_UNAUTHORIZED;
          response->content.length = 0U;
        }
        break;
      case REST_FREE_DATA:
        if ( eHTTPisAuth( remoteIP ) == AUTH_DONE )
        {
          if ( adr < FREE_DATA_SIZE )
          {
            if ( eRESTparsingFreeData( content, &data ) == REST_OK )
            {
              if ( eDATAAPIfreeData( DATA_API_CMD_WRITE, adr, &data ) == DATA_API_STAT_OK )
              {
                if ( eDATAAPIfreeData( DATA_API_CMD_SAVE, adr, &data ) == DATA_API_STAT_OK )
                {
                  response->content.type   = HTTP_CONTENT_JSON;
                  response->status         = HTTP_STATUS_OK;
                  response->content.length = 0U;
                }
              }
            }
          }
        }
        else
        {
          response->content.type  = HTTP_CONTENT_JSON;
          response->status        = HTTP_STATUS_UNAUTHORIZED;
          response->content.length = 0U;
        }
        break;
      case REST_LOG_ERASE:
        if ( eHTTPisAuth( remoteIP ) == AUTH_DONE )
        {
          if ( eDATAAPIlog( DATA_API_CMD_ERASE, NULL, NULL ) == DATA_API_STAT_OK )
          {
            response->content.type  = HTTP_CONTENT_JSON;
            response->status        = HTTP_STATUS_OK;
            response->content.length = 0U;
          }
        }
        else
        {
          response->content.type  = HTTP_CONTENT_JSON;
          response->status        = HTTP_STATUS_UNAUTHORIZED;
          response->content.length = 0U;
        }
        break;
      case REST_PASSWORD:
        if ( eHTTPisAuth( remoteIP ) == AUTH_DONE )
        {
          if ( eRESTparsingPassword( content, &password ) == REST_OK )
          {
            if ( eDATAAPIpassword( DATA_API_CMD_WRITE, &password ) == DATA_API_STAT_OK )
            {
              if ( eDATAAPIpassword( DATA_API_CMD_SAVE, NULL ) == DATA_API_STAT_OK )
              {
                response->content.type  = HTTP_CONTENT_JSON;
                response->status        = HTTP_STATUS_OK;
                response->content.length = 0U;
              }
            }
          }
        }
        else
        {
          response->content.type  = HTTP_CONTENT_JSON;
          response->status        = HTTP_STATUS_UNAUTHORIZED;
          response->content.length = 0U;
        }
        break;
      case REST_AUTH:
        if ( eDATAAPIpassword( DATA_API_CMD_READ, &passwordSt ) == DATA_API_STAT_OK )
        {
          if ( passwordSt.status == PASSWORD_SET )
          {
            if ( eRESTparsingAuth( content, &password ) == REST_OK )
            {
              if ( password.data == passwordSt.data )
              {
                vHHTPsetAuth( remoteIP, AUTH_DONE );
                response->content.type  = HTTP_CONTENT_JSON;
                response->status        = HTTP_STATUS_OK;
                response->content.length = 0U;
              }
              else
              {
                response->content.type  = HTTP_CONTENT_JSON;
                response->status        = HTTP_STATUS_UNAUTHORIZED;
                response->content.length = 0U;
              }
            }
          }
          else
          {
            vHHTPsetAuth( remoteIP, AUTH_DONE );
            response->content.type  = HTTP_CONTENT_JSON;
            response->status        = HTTP_STATUS_OK;
            response->content.length = 0U;
          }
        }
        break;
      case REST_ERASE_PASSWORD:
        if ( eHTTPisAuth( remoteIP ) == AUTH_VOID )
        {
          response->content.type  = HTTP_CONTENT_JSON;
          response->status        = HTTP_STATUS_UNAUTHORIZED;
          response->content.length = 0U;
        }
        break;
      case REST_MEASUREMENT_ERASE:
        break;
      case REST_OUTPUT:
        break;
      case REST_REQUEST_ERROR:
        break;
      default:
        break;
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Build get response in response structure
 * Input:   path     - url to the file from request
 *          response - structure of response
 * Output:  none
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vHTTPbuildGetResponse ( char* path, HTTP_RESPONSE *response, uint32_t remoteIP )
{
  char*           strStr  = NULL;
  uint16_t        adr     = 0xFFFFU;
  REST_REQUEST    request = REST_REQUEST_ERROR;
  REST_ADDRESS    adrFlag = REST_NO_ADR;
  HTTP_STREAM*    stream  = NULL;
  uint32_t        i       = 0U;
  uint16_t        logAdr  = 0U;
  LOG_RECORD_TYPE record  = { 0U };
  DATA_API_STATUS status  = DATA_API_STAT_OK;
  /*----------------- Common header -----------------*/
  strStr = strcpy( response->header, "Thu, 06 Feb 2020 15:11:53 GMT" );
  response->header[strlen("Thu, 06 Feb 2020 15:11:53 GMT")] = 0U;
  response->cache         = HTTP_CACHE_NO_CACHE_STORE;
  response->connect       = HTTP_CONNECT_CLOSED;
  response->status        = HTTP_STATUS_BAD_REQUEST;
  response->content.length = 0U;
  response->encoding      = HTTP_ENCODING_NO;
  /*----------------- Parsing path -----------------*/
  /*------------------ INDEX.HTML ------------------*/
  strStr = strstr(path, "index" );
  if ( ( path[0U] == 0x00U ) || ( strStr != NULL) )
  {
    stream                  = &response->stream;
    stream->index           = 0U;
    stream->start           = 0U;
    stream->content         = ( char* )( data__web_html );
    stream->length          = WEB_LENGTH;
    stream->size            = WEB_LENGTH;
    stream->flag            = STREAM_FLAG_NO_COPY;
    response->callBack      = cHTTPstreamFileWebApp;
    response->content.type  = HTTP_CONTENT_HTML;
    response->status        = HTTP_STATUS_OK;
    response->content.length = stream->size;
    response->encoding      = HTTP_ENCODING_GZIP;
  }
  /*--------------------- REST ---------------------*/
  else if ( path[0U] > 0U )
  {
    if ( eHTTPisAuth( remoteIP ) == AUTH_DONE )
    {
      if ( path[2U] == 'h')
      {
        i = 0U;
      }
      adrFlag = eRESTgetRequest( path, &request, &adr );
      switch ( request )
      {
        case REST_CONFIGS:
          /*------------------ Broadcast -------------------*/
          if ( adrFlag == REST_NO_ADR )
          {
            stream             = &response->stream;
            stream->size       = SETTING_REGISTER_NUMBER;
            stream->index      = 0U;
            stream->start      = 0U;
            stream->flag       = STREAM_FLAG_COPY;
            response->callBack = cHTTPstreamConfigs;
            for( i=0U; i<stream->size; i++ )
            {
              response->content.length += uRESTmakeConfig( configReg[i], restBuffer );
            }
            response->content.length += 1U + stream->size;            /* '[' + ']' + ',' */
            response->content.type   = HTTP_CONTENT_JSON;
            response->status         = HTTP_STATUS_OK;
            response->content.data   = restBuffer;
          }
          /*------------- Specific address ------------------*/
          else
          {
            if ( ( adr != 0xFFFFU ) && ( adr < SETTING_REGISTER_NUMBER ) )
            {
              response->content.length = uRESTmakeConfig( configReg[adr], restBuffer );
            }
            stream                 = &response->stream;
            stream->size           = adr + 1U;
            stream->start          = adr;
            stream->index          = 0U;
            stream->flag           = STREAM_FLAG_COPY;
            response->callBack     = cHTTPstreamConfigs;
            response->content.type = HTTP_CONTENT_JSON;
            response->status       = HTTP_STATUS_OK;
            response->content.data = restBuffer;
          }
          break;
        case REST_CHARTS:
          /*------------------ Broadcast -------------------*/
          if ( adrFlag == REST_NO_ADR )
          {
            stream             = &response->stream;
            stream->size       = CHART_NUMBER;
            stream->index      = 0U;
            stream->start      = 0U;
            stream->flag       = STREAM_FLAG_COPY;
            response->callBack = cHTTPstreamCharts;
            for ( i=0U; i<stream->size; i++ )
            {
              response->content.length += uRESTmakeChart( charts[i], restBuffer );
            }
            response->content.length += 1U + stream->size;            /* '[' + ']' + ',' */
            response->content.type   = HTTP_CONTENT_JSON;
            response->status         = HTTP_STATUS_OK;
            response->content.data   = restBuffer;
          }
          break;
        case REST_TIME:
          if ( eRTCgetTime( &time ) == RTC_OK )
          {
            stream                   = &response->stream;
            stream->size             = 1U;
            stream->start            = 0U;
            stream->index            = 0U;
            stream->flag             = STREAM_FLAG_COPY;
            response->content.length = uRESTmakeTime( &time, restBuffer );;
            response->callBack       = cHTTPstreamTime ;
            response->content.type   = HTTP_CONTENT_JSON;
            response->status         = HTTP_STATUS_OK;
            response->content.data   = restBuffer;
          }
          break;
        case REST_FREE_DATA:
          if ( adrFlag != REST_NO_ADR )
          {
            if ( adr < FREE_DATA_SIZE )
            {
              stream                   = &response->stream;
              stream->size             = 1U;
              stream->start            = adr;
              stream->index            = 0U;
              stream->flag             = STREAM_FLAG_COPY;
              response->content.length = uRESTmakeData( *freeDataArray[stream->start], restBuffer );;
              response->callBack       = cHTTPstreamData;
              response->content.type   = HTTP_CONTENT_JSON;
              response->status         = HTTP_STATUS_OK;
              response->content.data   = restBuffer;
            }
          }
          break;
        case REST_LOG:
          if ( adrFlag == REST_NO_ADR )
          {
            stream             = &response->stream;
            stream->size       = LOG_SIZE;
            stream->index      = 0U;
            stream->start      = 0U;
            stream->flag       = STREAM_FLAG_COPY;
            response->callBack = cHTTPstreamLog;
            for ( i=0U; i<stream->size; i++ )
            {
              logAdr = ( uint16_t )i;
              status = eDATAAPIlog( DATA_API_CMD_LOAD, &logAdr, &record );
              if ( status != DATA_API_STAT_OK )
              {
                response->status = HTTP_STATUS_BAD_REQUEST;
              }
              response->content.length += uRESTmakeLog( &record, restBuffer );
            }
            response->content.length += 1U + stream->size;            /* '[' + ']' + ',' */
            response->content.type   = HTTP_CONTENT_JSON;
            response->status         = HTTP_STATUS_OK;
            response->content.data   = restBuffer;
          }
          break;
        case REST_MEMORY:
          #if defined( FATSD )
            stream                   = &response->stream;
            stream->size             = 1U;
            stream->start            = 0U;
            stream->index            = 0U;
            stream->flag             = STREAM_FLAG_COPY;
            stream->length           = uRESTmakeMemorySize( restBuffer );
            response->content.length = stream->length;
            response->callBack       = cHTTPstreamString;
            response->content.type   = HTTP_CONTENT_JSON;
            response->status         = HTTP_STATUS_OK;
            response->content.data   = restBuffer;
          #endif
          break;
        case REST_MEASUREMENT_LENGTH:
          #if defined( FATSD )
            stream                   = &response->stream;
            stream->size             = 1U;
            stream->start            = 0U;
            stream->index            = 0U;
            stream->flag             = STREAM_FLAG_COPY;
            stream->length           = uRESTmakeMeasurementLength( restBuffer );
            response->content.length = stream->length;
            response->callBack       = cHTTPstreamString;
            response->content.type   = HTTP_CONTENT_JSON;
            response->status         = HTTP_STATUS_OK;
            response->content.data   = restBuffer;
          #endif
          break;
        case REST_MEASUREMENT:
          #if defined( FATSD )
            stream = &response->stream;
            if ( adrFlag == REST_NO_ADR )
            {
              stream->size  = uMEASUREMENTgetSize();
              stream->start = 0U;
              for ( i=0U; i<stream->size; i++ )
              {
                response->content.length += uRESTmakeMeasurement( i, restBuffer );
              }
              response->content.length += 1U + stream->size;            /* '[' + ']' + ',' */
            }
            else
            {
              if ( adr < uMEASUREMENTgetSize() )
              {
                response->content.length += uRESTmakeMeasurement( adr, restBuffer );
              }
              stream->size  = adr + 1U;
              stream->start = adr;
            }
            stream->index          = 0U;
            stream->flag           = STREAM_FLAG_COPY;
            response->callBack     = cHTTPstreamMeasurement;
            response->content.type = HTTP_CONTENT_JSON;
            response->status       = HTTP_STATUS_OK;
            response->content.data = restBuffer;
          #endif
          break;
        case REST_OUTPUT:
          if ( ( adrFlag == REST_YES_ADR ) && ( adr < OUTPUT_DATA_REGISTER_NUMBER ) )
          {
            vOUTPUTupdate( adr );
            stream                   = &response->stream;
            stream->size             = 1U;
            stream->start            = 0U;
            stream->index            = 0U;
            stream->flag             = STREAM_FLAG_COPY;
            stream->length           = uRESTmakeOutput( outputDataReg[adr], outputDataDictionary[adr], restBuffer );
            response->content.length = stream->length;
            response->callBack       = cHTTPstreamString;
            response->content.type   = HTTP_CONTENT_JSON;
            response->status         = HTTP_STATUS_OK;
            response->content.data   = restBuffer;
          }
          else
          {
            response->status = HTTP_STATUS_BAD_REQUEST;
          }
          break;
        case REST_SYSTEM:
          stream                   = &response->stream;
          stream->size             = 1U;
          stream->start            = 0U;
          stream->index            = 0U;
          stream->flag             = STREAM_FLAG_COPY;
          stream->length           = uRESTmakeSystem( restBuffer );
          response->content.length = stream->length;
          response->callBack       = cHTTPstreamString;
          response->content.type   = HTTP_CONTENT_JSON;
          response->status         = HTTP_STATUS_OK;
          response->content.data   = restBuffer;
          break;
        case REST_REQUEST_ERROR:
          break;
        default:
          response->status         = HTTP_STATUS_BAD_REQUEST;
          response->content.length = 0U;
          break;
      }
    }
    else
    {
      response->status         = HTTP_STATUS_UNAUTHORIZED;
      response->content.length = 0U;
    }
  }
  else
  {
    response->status         = HTTP_STATUS_NON_CONNECT;
    response->content.length = 0U;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Build response in response structure
 * Input:  request  - structure of request
 *         response - structure of response
 * Output: HTTP status
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vHTTPbuildResponse ( HTTP_REQUEST* request, HTTP_RESPONSE* response, uint32_t remoteIP )
{
  vHTTPCleanResponse( response );
  response->status = HTTP_STATUS_BAD_REQUEST;
  switch ( request->method )
  {
    case HTTP_METHOD_NO:
      break;
    case HTTP_METHOD_GET:
      response->method = HTTP_METHOD_GET;
      vHTTPbuildGetResponse( request->path, response, remoteIP );
      break;
    case HTTP_METHOD_POST:
      response->method = HTTP_METHOD_POST;
      break;
    case HTTP_METHOD_PUT:
      response->method = HTTP_METHOD_PUT;
      vHTTPbuildPutResponse( request->path, response, request->content.data, remoteIP );
      break;
    case HTTP_METHOD_HEAD:
      break;
    case HTTP_METHOD_OPTION:
      break;
    default:
      break;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Build request structure
 * Input:  request - structure of request
 * Output: HTTP status
 */
HTTP_STATUS eHTTPbuildRequest ( HTTP_REQUEST* request )
{
  HTTP_STATUS res = HTTP_STATUS_OK;
  switch ( request->method )
  {
    case HTTP_METHOD_NO:
      res = HTTP_STATUS_BAD_REQUEST;
      break;
    case HTTP_METHOD_GET:
      res = HTTP_STATUS_OK;
      break;
    case HTTP_METHOD_POST:
      res = HTTP_STATUS_BAD_REQUEST;
      break;
    case HTTP_METHOD_HEAD:
      res = HTTP_STATUS_BAD_REQUEST;
      break;
    case HTTP_METHOD_OPTION:
      res = HTTP_STATUS_BAD_REQUEST;
      break;
    default:
      res = HTTP_STATUS_BAD_REQUEST;
      break;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Make string request from request structure
 * Input:  httpStr  - pointer to char array of output buffer for response string
 *         response - response structure
 * Output: HTTP_STATUS
 */
void vHTTPmakeRequest ( const HTTP_REQUEST* request, char* httpStr )
{
  ( void )strcpy( httpStr, "GET " );
  ( void )strcat( httpStr, request->path );
  ( void )strcat( httpStr, " HTTP/1.1" );
  ( void )strcat( httpStr, HTTP_END_LINE );
  ( void )strcat( httpStr, HTTP_HOST_LINE );
  ( void )strcat( httpStr, request->host );
  ( void )strcat( httpStr, HTTP_END_LINE );
  ( void )strcat( httpStr, HTTP_CONN_LINE );
  ( void )strcat( httpStr, HTTP_END_LINE );
  vHTTPaddCache( httpStr, request->cache );
  ( void )strcat( httpStr, HTTP_END_LINE );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Make string response from response structure
 * Input:  httpStr  - pointer to char array of output buffer for response string
 *         response - response structure
 * Output: HTTP_STATUS
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vHTTPmakeResponse ( char* httpStr, HTTP_RESPONSE* response )
{
  char         buffer[30U] = { 0U };
  // STATUS
  switch( response->status )
  {
    case HTTP_STATUS_OK:
      ( void )strcpy( httpStr, HTTP_OK_STATUS_LINE );
      break;
    case HTTP_STATUS_BAD_REQUEST:
      ( void )strcpy( httpStr, HTTP_NOT_FOUND_STATUS_LINE );
      break;
    case HTTP_STATUS_UNAUTHORIZED:
      ( void )strcpy( httpStr, HTTP_UNAUTHORIZED_LINE );
      break;
    default:
      ( void )strcpy( httpStr, HTTP_NOT_FOUND_STATUS_LINE );
      break;
  }
  ( void )strcat( httpStr, HTTP_END_LINE );
  ( void )strcat( httpStr, HTTP_DATE_LINE );
  ( void )strcat( httpStr, response->header );
  ( void )strcat( httpStr, HTTP_END_LINE );
  ( void )strcat( httpStr, HTTP_LENGTH_LINE);
  ( void )itoa( response->content.length, buffer, 10U );
  ( void )strcat( httpStr, buffer );
  ( void )strcat( httpStr, HTTP_END_LINE );
  vHTTPaddContentType( httpStr, response->content.type );
  vHTTPaddContentEncoding ( httpStr, response->encoding );
  vHTTPaddCache( httpStr, response->cache );
  ( void )strcat( httpStr, HTTP_CONN_LINE );
  ( void )strcat( httpStr, HTTP_END_LINE );
  ( void )strcat( httpStr, HTTP_END_LINE );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

/*
 * http.c
 *
 *  Created on: Feb 6, 2020
 *      Author: mikhail.mikhailov
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "freeData.h"
#include "http.h"
#include "index.h"
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
#include "dataAPI.h"
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
  request->contetntType = HTTP_CONTENT_HTML;
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
char* vHTTPaddContetntType ( char* httpStr, HTTP_CONTENT type )
{
  char* strRes = NULL;

  strRes = strcat( httpStr, HTTP_CONTENT_LINE );
  switch ( type )
  {
    case HTTP_CONTENT_HTML:
      strRes = strcat( httpStr, HTTP_CONTENT_STR_HTML );
      break;
    case HTTP_CONTENT_CSS:
      strRes = strcat( httpStr, HHTP_CONTENT_STR_CSS );
      break;
    case HTTP_CONTENT_JS:
      strRes = strcat( httpStr, HTTP_CONTENT_STR_JS );
      break;
    case HTTP_CONTENT_JSON:
      strRes = strcat( httpStr, HTTP_CONTENT_STR_JSON );
      break;
    case HTTP_CONTENT_XML:
      strRes = strcat( httpStr, HTTP_CONTENT_STR_XML );
      break;
    default:
      strRes = strcat( httpStr, "Error" );
      break;
  }
  strRes = strcat( httpStr, HTTP_END_LINE );

  return strRes;
}
/*---------------------------------------------------------------------------------------------------*/
char* vHTTPaddContentEncoding ( char* httpStr, HTTP_ENCODING encoding )
{
  char* strRes = NULL;

  if ( encoding != HTTP_ENCODING_NO )
  {
    strRes = strcat( httpStr, HTTP_ENCODING_LINE );
    switch ( encoding )
    {
      case HTTP_ENCODING_NO:
        break;
      case HTTP_ENCODING_GZIP:
        strRes = strcat( httpStr, HTTP_CONTENT_ENCODING_GZIP );
        break;
      case HTTP_ENCODING_COMPRESS:
        strRes = strcat( httpStr, HTTP_CONTENT_ENCODING_COMPRESS );
        break;
      case HTTP_ENCODING_DEFLATE:
        strRes = strcat( httpStr, HTTP_CONTENT_ENCODING_DEFLATE );
        break;
      case HTTP_ENCODING_INDENTITY:
        strRes = strcat( httpStr, HTTP_CONTENT_ENCODING_INDENTITY );
        break;
      case HTTP_ENCODING_BR:
        strRes = strcat( httpStr, HTTP_CONTENT_ENCODING_BR );
        break;
      default:
        break;
    }
    strRes = strcat( httpStr, HTTP_END_LINE );
  }
  else
  {
    strRes = httpStr;
  }
  return strRes;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Add cache string to http
 * input:  httpStr - pointer to output http string
 *         cache   - cache type to add
 * output: result of operation
 */
char* vHTTPaddCache ( char* httpStr, HTTP_CACHE cache)
{
  char* strRes = NULL;

  strRes = strcat( httpStr, HTTP_CACHE_CONTROL );
  switch ( cache )
  {
    case HTTP_CACHE_NO_CACHE:
      strRes = strcat( httpStr, HTTP_CACHE_STR_NO_CACHE );
      break;
    case HTTP_CACHE_NO_STORE:
      strRes = strcat( httpStr, HTTP_CACHE_STR_NO_STORE );
      break;
    case HTTP_CACHE_NO_CACHE_STORE:
      strRes = strcat( httpStr, HTTP_CACHE_STR_NO_CACHE );
      strRes = strcat( httpStr, "," );
      strRes = strcat( httpStr, HTTP_CACHE_STR_NO_STORE );
      break;
    default:
      strRes = strcat( httpStr, HTTP_CACHE_STR_NO_CACHE );
      strRes = strcat( httpStr, "," );
      strRes = strcat( httpStr, HTTP_CACHE_STR_NO_STORE );
      break;
  }
  strRes = strcat( httpStr, HTTP_END_LINE );
  return strRes;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Stream call back for file transfer from flash
 */
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
STREAM_STATUS cHTTPstreamFileEEPROM ( HTTP_STREAM* stream )
{
  uint16_t        length = HTTP_EWA_TRANSFER_SIZE;
  DATA_API_STATUS status = DATA_API_STAT_ERROR;

  stream->status = STREAM_CONTINUES;
  if ( length > ( stream->size - stream->index ) )
  {
    length = stream->size - stream->index;
    stream->status = STREAM_END;
  }
  while ( status != DATA_API_STAT_OK )
  {
    status = eDATAAPIewa( DATA_API_CMD_LOAD, ( STORAGE_EWA_DATA_ADR + stream->index ), ( uint8_t* )restBuffer, length );
    if ( status != DATA_API_STAT_OK )
    {
      osDelay( 10U );
    }
  }
  stream->index += length;
  stream->length = length;
  restBuffer[stream->length] = 0U;
  stream->content = restBuffer;
  return stream->status;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Stream call back for configuration data transfer
 */
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
/*
 * Stream call back for charts data transfer
 */
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
STREAM_STATUS cHTTPstreamTime ( HTTP_STREAM* stream )
{
  stream->status = STREAM_END;
  stream->length = uRESTmakeTime( &time, restBuffer );
  restBuffer[stream->length] = 0U;
  stream->content = restBuffer;
  return stream->status;
}
/*---------------------------------------------------------------------------------------------------*/
STREAM_STATUS cHTTPstreamData ( HTTP_STREAM* stream )
{
  stream->status = STREAM_END;
  stream->length = uRESTmakeData( *( freeDataArray[stream->start] ), restBuffer );
  restBuffer[stream->length] = 0U;
  stream->content = restBuffer;
  return stream->status;
}
/*---------------------------------------------------------------------------------------------------*/
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
  return stream->status;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Clean response structure
 * Input:  request structure
 * Output: none
 */
void vHTTPCleanResponse ( HTTP_RESPONSE *response )
{
  uint32_t i = 0U;

  response->status = HTTP_STATUS_BAD_REQUEST;
  response->method = HTTP_METHOD_NO;
  for ( i=0U; i<HEADER_LENGTH; i++ )
  {
    response->header[i] = 0U;
  }
  response->contentLength = 0U;
  response->contetntType  = HTTP_CONTENT_HTML;
  response->connect       = HTTP_CONNECT_CLOSED;
  response->cache         = HTTP_CACHE_NO_CACHE;
  response->encoding      = HTTP_ENCODING_NO;
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
uint8_t uHTTPgetLine ( const char* input, uint16_t num, char* line )
{
  uint8_t     res   = 0U;
  uint16_t    i     = 0U;
  const char* start = input;
  const char* end   = strchr( input, LF_HEX );

  for ( i=0U; i<num; i++ )
  {
    start = strchr( end, LF_HEX );
    start++;
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
        if( strncpy( request->path, ( pchSt ), ( strlen( pchSt ) - strlen( pchEnd ) ) ) == NULL )
        {
          res = HTTP_STATUS_BAD_REQUEST;
        }
      }
      /*----------------------------- Parsing Content -----------------------------*/
      pchSt  = strstr( req, HTTP_END_HEADER );
      if ( pchSt != NULL )
      {
        request->content = &pchSt[strlen( HTTP_END_HEADER )];
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
void eHTTPbuildPutResponse ( char* path, HTTP_RESPONSE *response, char* content, uint32_t remoteIP )
{
  uint16_t      adr        = 0xFFFFU;
  REST_REQUEST  request    = 0U;
  REST_ADDRESS  adrFlag    = REST_NO_ADR;
  uint16_t      data       = 0U;
  PASSWORD_TYPE password   = { .status = PASSWORD_RESET,.data = 0U };
  PASSWORD_TYPE passwordSt = { .status = PASSWORD_RESET,.data = 0U };

  response->header[strlen("Thu, 06 Feb 2020 15:11:53 GMT")] = 0U;
  response->cache         = HTTP_CACHE_NO_CACHE_STORE;
  response->connect       = HTTP_CONNECT_CLOSED;
  response->status        = HTTP_STATUS_BAD_REQUEST;
  response->contentLength = 0U;
  if ( path[0U] > 0U )
  {
    adrFlag = eRESTgetRequest( path, &request, &adr );
    switch ( request )
    {
      case REST_CONFIGS:
        if ( eHTTPisAuth( remoteIP ) )
        {
          if ( ( adr != 0xFFFFU ) && ( adr < SETTING_REGISTER_NUMBER ) && ( adrFlag != REST_NO_ADR ) )
          {
            if ( adr == 3U )
            {
        	    adr = 3U;
            }
            if ( eRESTparsingConfig( content, adr ) == REST_OK )
            {
              response->contetntType  = HTTP_CONTENT_JSON;
              response->status        = HTTP_STATUS_OK;
              response->contentLength = 0U;
            }
          }
        }
        else
        {
          response->contetntType  = HTTP_CONTENT_JSON;
          response->status        = HTTP_STATUS_UNAUTHORIZED;
          response->contentLength = 0U;
        }
        break;
      case REST_CHARTS:
        if ( eHTTPisAuth( remoteIP ) )
        {
          if ( ( adr != 0xFFFFU ) && ( adr < SETTING_REGISTER_NUMBER ) && ( adrFlag != REST_NO_ADR ) )
          {
            if ( eRESTparsingChart( content, adr ) == REST_OK )
            {
              response->contetntType  = HTTP_CONTENT_JSON;
              response->status        = HTTP_STATUS_OK;
              response->contentLength = 0U;
            }
          }
        }
        else
        {
          response->contetntType  = HTTP_CONTENT_JSON;
          response->status        = HTTP_STATUS_UNAUTHORIZED;
          response->contentLength = 0U;
        }
        break;
      case REST_SAVE_CONFIGS:
        if ( eHTTPisAuth( remoteIP ) )
        {
          if ( eDATAAPIconfigValue( DATA_API_CMD_SAVE, 0U, NULL ) == DATA_API_STAT_OK )
          {
            response->contetntType  = HTTP_CONTENT_JSON;
            response->status        = HTTP_STATUS_OK;
            response->contentLength = 0U;
          }
        }
        else
        {
          response->contetntType  = HTTP_CONTENT_JSON;
          response->status        = HTTP_STATUS_UNAUTHORIZED;
          response->contentLength = 0U;
        }
        break;
      case REST_SAVE_CHARTS:
        if ( eHTTPisAuth( remoteIP ) )
        {
	        if ( eDATAAPIchart( DATA_API_CMD_SAVE, 0U, NULL ) == DATA_API_STAT_OK )
          {
            response->contetntType  = HTTP_CONTENT_JSON;
            response->status        = HTTP_STATUS_OK;
            response->contentLength = 0U;
          }
        }
        else
        {
          response->contetntType  = HTTP_CONTENT_JSON;
          response->status        = HTTP_STATUS_UNAUTHORIZED;
          response->contentLength = 0U;
        }
        break;
      case REST_TIME:
        if ( eHTTPisAuth( remoteIP ) )
        {
	        if ( eRESTparsingTime( content, &time ) == REST_OK )
	        {
	          if ( vRTCsetTime( &time ) == RTC_OK )
	          {
              response->contetntType  = HTTP_CONTENT_JSON;
              response->status        = HTTP_STATUS_OK;
              response->contentLength = 0U;
	          }
	        }
	      }
        else
        {
          response->contetntType  = HTTP_CONTENT_JSON;
          response->status        = HTTP_STATUS_UNAUTHORIZED;
          response->contentLength = 0U;
        }
        break;
      case REST_FREE_DATA:
        if ( eHTTPisAuth( remoteIP ) )
        {
	       if ( adr < FREE_DATA_SIZE )
	        {
	          if ( eRESTparsingFreeData( content, &data ) == REST_OK )
	          {
	            if ( eDATAAPIfreeData( DATA_API_CMD_WRITE, adr, &data ) == DATA_API_STAT_OK )
	            {
	              if ( eDATAAPIfreeData( DATA_API_CMD_SAVE, adr, &data ) == DATA_API_STAT_OK )
	              {
                  response->contetntType  = HTTP_CONTENT_JSON;
                  response->status        = HTTP_STATUS_OK;
                  response->contentLength = 0U;
	              }
	            }
	          }
	        }
	      }
        else
        {
          response->contetntType  = HTTP_CONTENT_JSON;
          response->status        = HTTP_STATUS_UNAUTHORIZED;
          response->contentLength = 0U;
        }
        break;
      case REST_LOG_ERASE:
        if ( eHTTPisAuth( remoteIP ) )
        {
          if ( eDATAAPIlog( DATA_API_CMD_ERASE, NULL, NULL ) == DATA_API_STAT_OK )
          {
            response->contetntType  = HTTP_CONTENT_JSON;
            response->status        = HTTP_STATUS_OK;
            response->contentLength = 0U;
          }
        }
        else
        {
          response->contetntType  = HTTP_CONTENT_JSON;
          response->status        = HTTP_STATUS_UNAUTHORIZED;
          response->contentLength = 0U;
        }
        break;
      case REST_PASSWORD:
        if ( eHTTPisAuth( remoteIP ) )
        {
          if ( eRESTparsingPassword( content, &password ) == REST_OK )
          {
            if ( eDATAAPIpassword( DATA_API_CMD_WRITE, &password ) == DATA_API_STAT_OK )
            {
              if ( eDATAAPIpassword( DATA_API_CMD_SAVE, NULL ) == DATA_API_STAT_OK )
              {
                response->contetntType  = HTTP_CONTENT_JSON;
                response->status        = HTTP_STATUS_OK;
                response->contentLength = 0U;
              }
            }
          }
        }
        else
        {
          response->contetntType  = HTTP_CONTENT_JSON;
          response->status        = HTTP_STATUS_UNAUTHORIZED;
          response->contentLength = 0U;
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
                response->contetntType  = HTTP_CONTENT_JSON;
                response->status        = HTTP_STATUS_OK;
                response->contentLength = 0U;
              }
              else
              {
                response->contetntType  = HTTP_CONTENT_JSON;
                response->status        = HTTP_STATUS_UNAUTHORIZED;
                response->contentLength = 0U;
              }
            }
          }
          else
          {
            vHHTPsetAuth( remoteIP, AUTH_DONE );
            response->contetntType  = HTTP_CONTENT_JSON;
            response->status        = HTTP_STATUS_OK;
            response->contentLength = 0U;
          }
        }
        break;
      case REST_ERASE_PASSWORD:
        if ( eHTTPisAuth( remoteIP ) )
        {

        }
        else
        {
          response->contetntType  = HTTP_CONTENT_JSON;
          response->status        = HTTP_STATUS_UNAUTHORIZED;
          response->contentLength = 0U;
        }
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
void eHTTPbuildGetResponse ( char* path, HTTP_RESPONSE *response, uint32_t remoteIP )
{
  char*           strStr                     = NULL;
  uint16_t        adr                        = 0xFFFFU;
  REST_REQUEST    request                    = REST_REQUEST_ERROR;
  REST_ADDRESS    adrFlag                    = REST_NO_ADR;
  HTTP_STREAM*    stream                     = NULL;
  uint32_t        i                          = 0U;
  uint32_t        ewaLen                     = 0U;
  uint16_t        logAdr                     = 0U;
  uint8_t         buffer[EEPROM_LENGTH_SIZE] = { 0U };
  DATA_API_STATUS status                     = DATA_API_STAT_OK;
  LOG_RECORD_TYPE record                     = { 0U };
  /*----------------- Common header -----------------*/
  strStr = strcpy( response->header, "Thu, 06 Feb 2020 15:11:53 GMT" );
  response->header[strlen("Thu, 06 Feb 2020 15:11:53 GMT")] = 0U;
  response->cache         = HTTP_CACHE_NO_CACHE_STORE;
  response->connect       = HTTP_CONNECT_CLOSED;
  response->status        = HTTP_STATUS_BAD_REQUEST;
  response->contentLength = 0U;
  response->encoding      = HTTP_ENCODING_NO;
  /*----------------- Parsing path -----------------*/
  /*------------------ INDEX.HTML ------------------*/
  strStr = strstr(path, "index" );
  if ( ( path[0U] == 0x00U ) || ( strStr != NULL) )
  {
    //do
    //{
      status = eDATAAPIewa( DATA_API_CMD_LOAD, STORAGE_EWA_ADR, buffer, EEPROM_LENGTH_SIZE );
    //} while ( status != DATA_API_STAT_OK );
    //eEEPROMreadMemory( STORAGE_EWA_ADR, buffer, EEPROM_LENGTH_SIZE );
    ewaLen = ( ( ( uint32_t )( buffer[0U] ) ) << 16U ) |
             ( ( ( uint32_t )( buffer[1U] ) ) <<  8U ) |
               ( ( uint32_t )( buffer[2U] ) );
    status = eDATAAPIewa( DATA_API_CMD_LOAD, ( STORAGE_EWA_DATA_ADR + ewaLen - 2U ), buffer, EEPROM_LENGTH_SIZE );
    //eEEPROMreadMemory( ( STORAGE_EWA_DATA_ADR + ewaLen - 2U ), buffer, EEPROM_LENGTH_SIZE );
    if ( ( buffer[0U] != 0x00U ) && ( buffer[0U] != 0xFFU ) && ( buffer[1U] == 0x00U ) )
    {
      stream                  = &(response->stream);
      stream->index           = 0U;
      stream->start           = 0U;
      stream->length          = 0U;
      stream->size            = ewaLen;
      response->callBack      = cHTTPstreamFileEEPROM;
      response->contetntType  = HTTP_CONTENT_HTML;
      response->status        = HTTP_STATUS_OK;
      response->contentLength = stream->size;
      response->encoding      = HTTP_ENCODING_GZIP;
    }
    else
    {
      stream                  = &(response->stream);
      stream->index           = 0U;
      stream->start           = 0U;
      stream->content         = data__index_html;
      stream->length          = HTML_LENGTH;
      stream->size            = 1U;
      response->callBack      = cHTTPstreamFile;
      response->contetntType  = HTTP_CONTENT_HTML;
      response->status        = HTTP_STATUS_OK;
      response->contentLength = HTML_LENGTH;
      response->encoding      = HTTP_ENCODING_GZIP;
    }
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
            stream = &(response->stream);
            stream->size       = SETTING_REGISTER_NUMBER;
            stream->index      = 0U;
            stream->start      = 0U;
            response->callBack = cHTTPstreamConfigs;
            for( i=0U; i<stream->size; i++ )
            {
              response->contentLength += uRESTmakeConfig( configReg[i], restBuffer );
            }
            response->contentLength += 1U + stream->size;            /* '[' + ']' + ',' */
            response->contetntType   = HTTP_CONTENT_JSON;
            response->status         = HTTP_STATUS_OK;
            response->data           = restBuffer;
          }
          /*------------- Specific address ------------------*/
          else
          {
            if ( ( adr != 0xFFFFU ) && ( adr < SETTING_REGISTER_NUMBER ) )
            {
              response->contentLength = uRESTmakeConfig( configReg[adr], restBuffer );
            }
            stream = &(response->stream);
            stream->size            = adr + 1U;
            stream->start           = adr;
            stream->index           = 0U;
            response->callBack      = cHTTPstreamConfigs;
            response->contetntType  = HTTP_CONTENT_JSON;
            response->status        = HTTP_STATUS_OK;
          }
          break;
        case REST_CHARTS:
          /*------------------ Broadcast -------------------*/
          if ( adrFlag == REST_NO_ADR )
          {
            stream = &(response->stream);
            stream->size  = CHART_NUMBER;
            stream->index = 0U;
            stream->start = 0U;
            response->callBack = cHTTPstreamCharts;
            for ( i=0U; i<stream->size; i++ )
            {
              response->contentLength += uRESTmakeChart( charts[i], restBuffer );
            }
            response->contentLength += 1U + stream->size;            /* '[' + ']' + ',' */
            response->contetntType   = HTTP_CONTENT_JSON;
            response->status         = HTTP_STATUS_OK;
            response->data           = restBuffer;
          }
          break;
        case REST_TIME:
          if ( eRTCgetTime( &time ) == RTC_OK )
          {
            stream = &(response->stream);
            stream->size            = 1U;
            stream->start           = 0U;
            stream->index           = 0U;
            response->contentLength = uRESTmakeTime( &time, restBuffer );;
            response->callBack      = cHTTPstreamTime ;
            response->contetntType  = HTTP_CONTENT_JSON;
            response->status        = HTTP_STATUS_OK;
          }
          break;
        case REST_FREE_DATA:
	        if ( adrFlag != REST_NO_ADR )
	        {
	          if ( adr < FREE_DATA_SIZE )
	          {
	            stream = &(response->stream);
	            stream->size            = 1U;
	            stream->start           = adr;
	            stream->index           = 0U;
	            response->contentLength = uRESTmakeData( *freeDataArray[stream->start], restBuffer );;
	            response->callBack      = cHTTPstreamData;
	            response->contetntType  = HTTP_CONTENT_JSON;
	            response->status        = HTTP_STATUS_OK;
	          }
	        }
          break;
        case REST_LOG:
          if ( adrFlag == REST_NO_ADR )
          {
            stream = &(response->stream);
            stream->size       = LOG_SIZE;
            stream->index      = 0U;
            stream->start      = 0U;
            response->callBack = cHTTPstreamLog;
            for ( i=0U; i<stream->size; i++ )
            {
              logAdr = ( uint16_t )i;
              status = eDATAAPIlog( DATA_API_CMD_LOAD, &logAdr, &record );
              response->contentLength += uRESTmakeLog( &record, restBuffer );
            }
            response->contentLength += 1U + stream->size;            /* '[' + ']' + ',' */
            response->contetntType   = HTTP_CONTENT_JSON;
            response->status         = HTTP_STATUS_OK;
            response->data           = restBuffer;
          }
          break;
        case REST_REQUEST_ERROR:
          break;
        default:
          response->status        = HTTP_STATUS_BAD_REQUEST;
          response->contentLength = 0U;
          break;
      }
    }
    else
    {
      response->status        = HTTP_STATUS_UNAUTHORIZED;
      response->contentLength = 0U;
    }
  }
  else
  {
    response->status        = HTTP_STATUS_NON_CONNECT;
    response->contentLength = 0U;
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
      eHTTPbuildGetResponse( request->path, response, remoteIP );
      break;
    case HTTP_METHOD_POST:
      response->method = HTTP_METHOD_POST;
      break;
    case HTTP_METHOD_PUT:
      response->method = HTTP_METHOD_PUT;
      eHTTPbuildPutResponse( request->path, response, request->content, remoteIP );
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
HTTP_STATUS eHTTPmakeRequest ( const HTTP_REQUEST* request, char* httpStr )
{
  HTTP_STATUS res = HTTP_STATUS_BAD_REQUEST;

  if ( strcpy( httpStr, "GET " ) != NULL )
  {
    if ( strcat( httpStr, request->path ) != NULL )
    {
      if( strcat( httpStr, " HTTP/1.1" ) != NULL )
      {
        if( strcat( httpStr, HTTP_END_LINE ) != NULL )
        {
          if( strcat( httpStr, HTTP_HOST_LINE ) != NULL )
          {
            if( strcat( httpStr, request->host ) != NULL )
            {
              if( strcat( httpStr, HTTP_END_LINE ) != NULL )
              {
                if( strcat( httpStr, HTTP_CONN_LINE ) != NULL )
                {
                  if( strcat( httpStr, HTTP_END_LINE ) != NULL )
                  {
                    if( vHTTPaddCache( httpStr, request->cache ) != NULL )
                    {
                      if( strcat( httpStr, HTTP_END_LINE ) != NULL )
                      {
                        res = HTTP_STATUS_OK;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Make string response from response structure
 * Input:  httpStr  - pointer to char array of output buffer for response string
 *         response - response structure
 * Output: HTTP_STATUS
 */
HTTP_STATUS eHTTPmakeResponse ( char* httpStr, HTTP_RESPONSE* response )
{
  HTTP_STATUS  res         = HTTP_STATUS_ERROR;
  char         buffer[30U] = { 0U };
  char*        strRes      = NULL;

  // STATUS
  switch( response->status )
  {
    case HTTP_STATUS_OK:
      strRes = strcpy( httpStr, HTTP_OK_STATUS_LINE );
      break;
    case HTTP_STATUS_BAD_REQUEST:
      strRes = strcpy( httpStr, HTTP_NOT_FOUND_STATUS_LINE );
      break;
    case HTTP_STATUS_UNAUTHORIZED:
      strRes = strcpy( httpStr, HTTP_UNAUTHORIZED_LINE );
      break;
    default:
      strRes = strcpy( httpStr, HTTP_NOT_FOUND_STATUS_LINE );
      break;
  }
  if ( strRes != NULL )
  {
    if ( strcat( httpStr, HTTP_END_LINE ) != NULL )
    {
      // DATE
      if ( strcat( httpStr, HTTP_DATE_LINE ) != NULL )
      {
        if ( strcat( httpStr, response->header ) != NULL )
        {
          if ( strcat( httpStr, HTTP_END_LINE ) != NULL )
          {
            // LENGTH
            if ( strcat( httpStr, HTTP_LENGTH_LINE ) != NULL )
            {
              if ( itoa( response->contentLength, buffer, 10U ) != NULL )
              {
                if ( strcat( httpStr, buffer ) != NULL )
                {
                  if ( strcat( httpStr, HTTP_END_LINE ) != NULL )
                  {
                    // CONTENT TYPE
                    if ( vHTTPaddContetntType( httpStr, response->contetntType ) != NULL )
                    {
                      // ENCODING
                      if ( vHTTPaddContentEncoding ( httpStr, response->encoding ) != NULL )
                      {
                        // CACHE
                        if ( vHTTPaddCache( httpStr, response->cache ) != NULL )
                        {
                          // CONNECTION
                          if ( strcat( httpStr, HTTP_CONN_LINE ) != NULL )
                          {
                            if ( strcat( httpStr, HTTP_END_LINE ) != NULL )
                            {
                              if ( strcat( httpStr, HTTP_END_LINE ) != NULL )
                              {
                                res = HTTP_STATUS_OK;
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

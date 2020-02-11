/*
 * http.c
 *
 *  Created on: Feb 6, 2020
 *      Author: mikhail.mikhailov
 */

/*----------------------- Includes ------------------------------------------------------------------*/
#include "http.h"
// Site
#include "index.h"
// Common
#include "string.h"
#include "stdio.h"
/*----------------------- Structures ----------------------------------------------------------------*/

/*----------------------- Constant ------------------------------------------------------------------*/
const char *httpMethodsStr[HTTP_METHOD_NUM] = { HTTP_METHOD_STR_GET, HTTP_METHOD_STR_POST, HTTP_METHOD_STR_HEAD, HTTP_METHOD_STR_OPTION};
/*----------------------- Variables -----------------------------------------------------------------*/

/*----------------------- Functions -----------------------------------------------------------------*/
void 						vHTTPcleanRequest( HTTP_REQUEST *httpRequest );
void 						vHTTPCleanResponse( HTTP_RESPONSE *response );
void 						eHTTPbuildGet( char* path, HTTP_RESPONSE *response );
uint8_t 				uHTTPgetLine( char* input, uint16_t num, char* line );
//HTTP_SITE_PATH 	eHTTPgetPath( char* path );
/*---------------------------------------------------------------------------------------------------*/
void vHTTPcleanRequest( HTTP_REQUEST *httpRequest )
{
	uint8_t i = 0U;

	httpRequest->method = HTTP_METHOD_NO;
	for( i=0U; i<HTTP_PATH_LENGTH; i++)
	{
		httpRequest->path[i] = 0x00U;
	}
	return;
}
/*---------------------------------------------------------------------------------------------------*/
void vHTTPCleanResponse( HTTP_RESPONSE *response )
{
	uint32_t i = 0U;

	response->status = HTTP_STATUS_BAD_REQUEST;
	response->method = HTTP_METHOD_NO;
	for( i=0U; i<DATE_LENGTH; i++ )
	{
		response->date[i] = 0x00U;
	}
	for( i=0U; i<SERVER_LENGTH; i++)
	{
		response->server[i] = 0x00U;
	}
	for( i=0U; i<DATE_LENGTH; i++)
	{
		response->modified[i] = 0x00U;
	}
	response->contentLength = 0U;
	response->contetntType  = HTTP_CONTENT_HTML;
	response->connect 			= HTTP_CONNECT_CLOSED;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uHTTPgetLine( char* input, uint16_t num, char* line )
{
	uint8_t   res   = 0U;
	uint16_t  i     = 0U;
	char*			start = input;
	char*			end   = strchr( input, LF_HEX );

	for( i=0U; i<num; i++ )
	{
		start = strchr( ( end ), LF_HEX ) + 1;
		end   = strchr( start, LF_HEX );
	}

	if ( start && end)
	{
		res = 1U;
		strncpy( line, start, ( end - start ) );
	}

	return res;
}
/*---------------------------------------------------------------------------------------------------*/
HTTP_STATUS eHTTPparsingRequest( char* req, HTTP_REQUEST *request )
{
	HTTP_STATUS 	res 	 = HTTP_STATUS_OK;
	uint8_t 			i      = 0U;
	char* 				pchSt  = NULL;
	char* 				pchEnd = NULL;
	char					line[50];

	uHTTPgetLine( req, 0, line );
	vHTTPcleanRequest( request );
	/* Parsing HTTP methods */
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
	/* Parsing HTTP path  */
	if ( res == HTTP_STATUS_OK )
	{
		pchSt  = strchr( line, '/' );
		pchEnd = strchr( pchSt, ' ' );
		if ( ( pchEnd - pchSt ) > 2U )
		{
			strncpy( request->path, ( pchSt ), ( pchEnd - pchSt ) );
		}
	}

	return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
HTTP_SITE_PATH eHTTPgetPath( char* path )
{
	HTTP_SITE_PATH res = NO_PATH;

	if ( strstr( path, STYLE_CSS_PATH ) )
	{
		res = STYLE_CSS;
	}
	else if ( strstr( path, MAIN_JS_PATH ) )
	{
		res = MAIN_JS;
	}

	return res;
}
*/
/*---------------------------------------------------------------------------------------------------*/
void eHTTPbuildGet( char* path, HTTP_RESPONSE *response)
{
	//HTTP_SITE_PATH file = NO_PATH;

	strcpy( response->date, "Thu, 06 Feb 2020 15:11:53 GMT" );
	response->cache = HTTP_CACHE_NO_CACHE_STORE;

	response->connect = HTTP_CONNECT_CLOSED;
	if( path[0] == 0x00 )
	{
		response->contetntType 	= HTTP_CONTENT_HTML;
		response->status 				= HTTP_STATUS_OK;
		response->contentLength = HTML_LENGTH;
		response->data 					= data__index_html;
	}
	else if ( path[0] )
	{
		/*
		file = eHTTPgetPath( path );
		switch ( file )
		{

		}
		*/
	}
	else
	{
		response->status 				= HTTP_STATUS_NON_CONNECT;
		response->contentLength = 0U;
	}
	return;
}
/*---------------------------------------------------------------------------------------------------*/
HTTP_STATUS eHTTPbuildResponse( HTTP_REQUEST request, HTTP_RESPONSE *response)
{
	HTTP_STATUS res = HTTP_STATUS_OK;

	vHTTPCleanResponse( response );
	response->method = HTTP_METHOD_NO;

	switch ( request.method )
	{
		case HTTP_METHOD_NO:
			res = HTTP_STATUS_BAD_REQUEST;
			break;
		case HTTP_METHOD_GET:
			response->method = HTTP_METHOD_GET;
			eHTTPbuildGet( request.path, response );
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
HTTP_STATUS eHTTPmakeResponse( char* httpStr, HTTP_RESPONSE response )
{
	HTTP_STATUS 	res = HTTP_STATUS_OK;
	char					buffer[30];

	// STATUS
	switch( response.status )
	{
		case HTTP_STATUS_OK:
			strcpy( httpStr, HTTP_OK_STATUS_LINE );
			break;
		case HTTP_STATUS_BAD_REQUEST:
			strcpy( httpStr, HTTP_NOT_FOUND_STATUS_LINE );
			break;
		default:
			strcpy( httpStr, HTTP_NOT_FOUND_STATUS_LINE );
			break;
	}
	strcat( httpStr, HTTP_END_LINE );
	// DATE
	strcat( httpStr, HTTP_DATE_LINE );
	strcat( httpStr, response.date );
	strcat( httpStr, HTTP_END_LINE );
	// LENGTH
	strcat( httpStr, HTTP_LENGTH_LINE );
	sprintf( buffer, "%lu", response.contentLength );
	strcat( httpStr, buffer );
	strcat( httpStr, HTTP_END_LINE );
	// CONTENT TYPE
	strcat( httpStr, HTTP_CONTENT_LINE );
	switch ( response.contetntType )
	{
		case HTTP_CONTENT_HTML:
			strcat( httpStr, HTTP_CONTENT_STR_HTML );
			break;
		case HTTP_CONTENT_CSS:
			strcat( httpStr, HHTP_CONTENT_STR_CSS );
			break;
		case HTTP_CONTENT_JS:
			strcat( httpStr, HTTP_CONTENT_STR_JS );
			break;
		default:
			strcat( httpStr, "Error" );
			break;
	}
	strcat( httpStr, HTTP_END_LINE );
	// CACHE
	strcat( httpStr, HTTP_CACHE_CONTROL );
	switch ( response.cache )
	{
		case HTTP_CACHE_NO_CACHE:
			strcat( httpStr, HTTP_CACHE_STR_NO_CACHE );
			break;
		case HTTP_CACHE_NO_STORE:
			strcat( httpStr, HTTP_CACHE_STR_NO_STORE );
			break;
		case HTTP_CACHE_NO_CACHE_STORE:
			strcat( httpStr, HTTP_CACHE_STR_NO_CACHE );
			strcat( httpStr, ',' );
			strcat( httpStr, HTTP_CACHE_STR_NO_STORE );
			break;
		default:
			strcat( httpStr, HTTP_CACHE_STR_NO_CACHE );
			strcat( httpStr, ',' );
			strcat( httpStr, HTTP_CACHE_STR_NO_STORE );
			break;
	}
	strcat( httpStr, HTTP_END_LINE );
	// CONNECTION
	strcat( httpStr, HTTP_CONN_LINE );
	strcat( httpStr, HTTP_END_LINE );

	strcat( httpStr, HTTP_END_LINE );

	return res;
}
/*---------------------------------------------------------------------------------------------------*/






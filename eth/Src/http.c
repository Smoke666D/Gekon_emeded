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
void 		vHTTPcleanRequest( HTTP_REQUEST *httpRequest );					// Clean request structure
void 		vHTTPCleanResponse( HTTP_RESPONSE *response );					// Clean response structure
uint8_t	uHTTPgetLine( char* input, uint16_t num, char* line );	// Get the string of line from multiline text.
void 		eHTTPbuildGet( char* path, HTTP_RESPONSE *response );		// Build get response in response structure
/*---------------------------------------------------------------------------------------------------*/
/*
 * Clean request structure
 * Input:		request structure
 * Output:	none
 */
void vHTTPcleanRequest( HTTP_REQUEST *request )
{
	uint8_t i = 0U;

	request->method = HTTP_METHOD_NO;
	for( i=0U; i<HTTP_PATH_LENGTH; i++)
	{
		request->path[i] = 0x00U;
	}
	return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Clean response structure
 * Input:		request structure
 * Output:	none
 */
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
/*
 * Get the string of line from multiline text.
 * The end of line is LF simbol (Line Feed)
 * Input:		input - pointer to char array of multiline text
 * 					num		- number of line of interest
 * 					line	- pointer to char array of output buffer for line
 * Output:	1 		- There is valid line in output buffer
 * 					0 		- There is no valid line in output buffer
 */
uint8_t uHTTPgetLine( char* input, uint16_t num, char* line )
{
	uint8_t   res   = 0U;
	uint16_t  i     = 0U;
	char*			start = input;
	char*			end   = strchr( input, LF_HEX );

	for( i=0U; i<num; i++ )
	{
		start = strchr( ( end ), LF_HEX ) + 1U;
		end   = strchr( start, LF_HEX );
	}

	if ( start && end)
	{
		res = 1U;
		if ( strncpy( line, start, ( end - start ) ) == NULL )
		{
			res = 0U;
		}
	}

	return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Parsing data from request text
 * Input:		req 		- pointer to char array with input request in text form
 * 					request	- pointer to the ouyput request structure
 * Output:	HTTP status
 */
HTTP_STATUS eHTTPparsingRequest( char* req, HTTP_REQUEST *request )
{
	HTTP_STATUS 	res 	 = HTTP_STATUS_BAD_REQUEST;
	uint8_t 			i      = 0U;
	char* 				pchSt  = NULL;
	char* 				pchEnd = NULL;
	char					line[50];

	if ( uHTTPgetLine( req, 0, line ) > 0U)
	{
		res 	 = HTTP_STATUS_OK;
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
				if( strncpy( request->path, ( pchSt ), ( pchEnd - pchSt ) ) == NULL )
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
 * Build get response in response structure
 * Input:		path 			- url to the file from request
 * 					response	- structure of response
 * Output:	none
 */
void eHTTPbuildGet( char* path, HTTP_RESPONSE *response)
{
	char *res;

	res = strcpy( response->date, "Thu, 06 Feb 2020 15:11:53 GMT" );
	response->cache = HTTP_CACHE_NO_CACHE_STORE;

	response->connect = HTTP_CONNECT_CLOSED;
	if( path[0U] == 0x00U )
	{
		response->contetntType 	= HTTP_CONTENT_HTML;
		response->status 				= HTTP_STATUS_OK;
		response->contentLength = HTML_LENGTH;
		response->data 					= data__index_html;
	}
	else if ( path[0U] > 0U )
	{

	}
	else
	{
		response->status 				= HTTP_STATUS_NON_CONNECT;
		response->contentLength = 0U;
	}
	return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Build response in response structure
 * Input:		request		- structure of request
 * 					response	- structure of response
 * Output:	HTTP status
 */
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
/*
 * Make string response from response structure
 * Input:		httpStr  - pointer to char array of output buffer for response string
 *  				response - response structure
 * Output:	HTTP_STATUS
 */
HTTP_STATUS eHTTPmakeResponse( char* httpStr, HTTP_RESPONSE response )
{
	HTTP_STATUS 	res = HTTP_STATUS_OK;
	char					buffer[30];
	char					*strRes;

	// STATUS
	switch( response.status )
	{
		case HTTP_STATUS_OK:
			strRes = strcpy( httpStr, HTTP_OK_STATUS_LINE );
			break;
		case HTTP_STATUS_BAD_REQUEST:
			strRes = strcpy( httpStr, HTTP_NOT_FOUND_STATUS_LINE );
			break;
		default:
			strRes = strcpy( httpStr, HTTP_NOT_FOUND_STATUS_LINE );
			break;
	}
	strRes = strcat( httpStr, HTTP_END_LINE );
	// DATE
	strRes = strcat( httpStr, HTTP_DATE_LINE );
	strRes = strcat( httpStr, response.date );
	strRes = strcat( httpStr, HTTP_END_LINE );
	// LENGTH
	strRes = strcat( httpStr, HTTP_LENGTH_LINE );
	strRes = sprintf( buffer, "%lu", response.contentLength );
	strRes = strcat( httpStr, buffer );
	strRes = strcat( httpStr, HTTP_END_LINE );
	// CONTENT TYPE
	strRes = strcat( httpStr, HTTP_CONTENT_LINE );
	switch ( response.contetntType )
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
		default:
			strRes = strcat( httpStr, "Error" );
			break;
	}
	strRes = strcat( httpStr, HTTP_END_LINE );
	// CACHE
	strRes = strcat( httpStr, HTTP_CACHE_CONTROL );
	switch ( response.cache )
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
	// CONNECTION
	strRes = strcat( httpStr, HTTP_CONN_LINE );
	strRes = strcat( httpStr, HTTP_END_LINE );

	strRes = strcat( httpStr, HTTP_END_LINE );

	return res;
}
/*---------------------------------------------------------------------------------------------------*/






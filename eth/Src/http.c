/*
 * http.c
 *
 *  Created on: Feb 6, 2020
 *      Author: mikhail.mikhailov
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "http.h"
#include "index.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "cmsis_os.h"
/*----------------------- Structures ----------------------------------------------------------------*/

/*----------------------- Constant ------------------------------------------------------------------*/
const char *httpMethodsStr[HTTP_METHOD_NUM] = { HTTP_METHOD_STR_GET, HTTP_METHOD_STR_POST, HTTP_METHOD_STR_HEAD, HTTP_METHOD_STR_OPTION};
/*----------------------- Variables -----------------------------------------------------------------*/

/*----------------------- Functions -----------------------------------------------------------------*/
void 		vHTTPcleanRequest( HTTP_REQUEST *httpRequest );									/* Clean request structure */
void 		vHTTPCleanResponse( HTTP_RESPONSE *response );									/* Clean response structure */
uint8_t	uHTTPgetLine( char* input, uint16_t num, char* line );					/* Get the string of line from multiline text */
void 		eHTTPbuildGetResponse( char* path, HTTP_RESPONSE *response );		/* Build get response in response structure */
char*		vHTTPaddCache( char* httpStr, HTTP_CACHE cache);								/* Add cache string to http */
void 		vHTTPaddContetntType( char* httpStr, HTTP_CONTENT type );
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
	for ( i=0U; i<HTTP_PATH_LENGTH; i++)
	{
		request->path[i] = 0x00U;
	}
	for ( i=0U; i<HTTP_PATH_LENGTH; i++)
	{
		request->host[i] = 0x00U;
	}
	request->contetntType = HTTP_CONTENT_HTML;
	request->connect			= HTTP_CONNECT_CLOSED;
	request->cache				= HTTP_CACHE_NO_CACHE;

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
	for ( i=0U; i<DATE_LENGTH; i++ )
	{
		response->date[i] = 0x00U;
	}
	for ( i=0U; i<SERVER_LENGTH; i++)
	{
		response->server[i] = 0x00U;
	}
	for ( i=0U; i<DATE_LENGTH; i++)
	{
		response->modified[i] = 0x00U;
	}
	response->contentLength = 0U;
	response->contetntType  = HTTP_CONTENT_HTML;
	response->connect 			= HTTP_CONNECT_CLOSED;
	response->cache					= HTTP_CACHE_NO_CACHE;

	return;
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

	for ( i=0U; i<num; i++ )
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
 * 					request	- pointer to the output request structure
 * Output:	HTTP status
 */
HTTP_STATUS eHTTPparsingRequest( char* req, HTTP_REQUEST* request )
{
	HTTP_STATUS 	res 	 = HTTP_STATUS_BAD_REQUEST;
	uint8_t 			i      = 0U;
	char* 				pchSt  = NULL;
	char* 				pchEnd = NULL;
	char					line[50];

	if ( uHTTPgetLine( req, 0, line ) > 0U )
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
 * Parsing data from response text
 * Input:		req 		- pointer to char array with input request in text form
 * 					request	- pointer to the output response structure
 * Output:	HTTP status
 */
HTTP_STATUS eHTTPparsingResponse( char* input, char* data, HTTP_RESPONSE* response  )
{
	HTTP_STATUS 	res 	 = HTTP_STATUS_BAD_REQUEST;
	char* 				pchSt  = NULL;
	char* 				pchEn  = NULL;
	char					buffer[5];


	pchSt = strchr( input, ' ') + 1U;
	if ( pchSt != NULL )
	{
		pchEn = strchr( pchSt, ' ' );
		if ( pchEn != NULL )
		{
			if ( strncpy( buffer, pchSt, ( pchEn - pchSt ) ) != NULL )
			{
				response->status = atoi( buffer );
				if ( response->status == HTTP_STATUS_OK )
				{
					res = HTTP_STATUS_OK;
					pchSt = strstr( input, "\r\n\r\n" ) + 4U;
					if ( pchSt != NULL )
					{
						pchEn = strcpy( data, pchSt );
						if ( pchEn == NULL )
						{
							res = HTTP_STATUS_BAD_REQUEST;
						}
					}
					else res = HTTP_STATUS_BAD_REQUEST;
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
void eHTTPbuildGetResponse( char* path, HTTP_RESPONSE *response)
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
HTTP_STATUS eHTTPbuildResponse( HTTP_REQUEST* request, HTTP_RESPONSE* response)
{
	HTTP_STATUS res = HTTP_STATUS_OK;

	vHTTPCleanResponse( response );
	response->method = HTTP_METHOD_NO;

	switch ( request->method )
	{
		case HTTP_METHOD_NO:
			res = HTTP_STATUS_BAD_REQUEST;
			break;
		case HTTP_METHOD_GET:
			response->method = HTTP_METHOD_GET;
			eHTTPbuildGetResponse( request->path, response );
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
 * Build request structure
 * Input:		request		- structure of request
 * Output:	HTTP status
 */
HTTP_STATUS eHTTPbuildRequest( HTTP_REQUEST* request )
{
	HTTP_STATUS res = HTTP_STATUS_OK;
	switch ( request->method )
	{
		case HTTP_METHOD_NO:
			res = HTTP_STATUS_BAD_REQUEST;
			break;
		case HTTP_METHOD_GET:
			//eHTTPbuildGetRequest( request );
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
 * Input:		httpStr  - pointer to char array of output buffer for response string
 *  				response - response structure
 * Output:	HTTP_STATUS
 */
HTTP_STATUS eHTTPmakeRequest ( char* httpStr, HTTP_REQUEST* request )
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
 * Input:		httpStr  - pointer to char array of output buffer for response string
 *  				response - response structure
 * Output:	HTTP_STATUS
 */
HTTP_STATUS eHTTPmakeResponse( char* httpStr, HTTP_RESPONSE* response )
{
	HTTP_STATUS 	res = HTTP_STATUS_OK;
	char					buffer[30];
	char					*strRes;

	// STATUS
	switch( response->status )
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
	strRes = strcat( httpStr, response->date );
	strRes = strcat( httpStr, HTTP_END_LINE );
	// LENGTH
	strRes = strcat( httpStr, HTTP_LENGTH_LINE );
	strRes = sprintf( buffer, "%lu", response->contentLength );
	strRes = strcat( httpStr, buffer );
	strRes = strcat( httpStr, HTTP_END_LINE );
	// CONTENT TYPE
	vHTTPaddContetntType( httpStr, response->contetntType );
	// CACHE
	strRes = strcat( httpStr, HTTP_CACHE_CONTROL );
	strRes = vHTTPaddCache( httpStr, response->cache );
	// CONNECTION
	strRes = strcat( httpStr, HTTP_CONN_LINE );
	strRes = strcat( httpStr, HTTP_END_LINE );

	strRes = strcat( httpStr, HTTP_END_LINE );

	return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Add cache string to http
 * input:		httpStr - pointer to output http string
 * 					cache		- cache type to add
 * output:	result of operation
 */
char* vHTTPaddCache( char* httpStr, HTTP_CACHE cache)
{
	char *strRes;

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
void vHTTPaddContetntType( char* httpStr, HTTP_CONTENT type )
{
	char* strRes;
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
	return;
}



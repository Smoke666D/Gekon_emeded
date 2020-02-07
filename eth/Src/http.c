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
/*----------------------- Structures ----------------------------------------------------------------*/

/*----------------------- Constant ------------------------------------------------------------------*/
const char *httpMethodsStr[HTTP_METHOD_NUM] = { HTTP_METHOD_STR_GET, HTTP_METHOD_STR_POST, HTTP_METHOD_STR_HEAD, HTTP_METHOD_STR_OPTION};
//char webPage[] = "<html><body><h1>This is WebServer!</h1></body></html>";
char webPage[53U] = {0x3C, 0x68, 0x74, 0x6D, 0x6C, 0x3E, 0x3C, 0x62, 0x6F, 0x64, 0x79, 0x3E, 0x3C, 0x68, 0x31, 0x3E, 0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x57, 0x65, 0x62, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x21, 0x3C, 0x2F, 0x68, 0x31, 0x3E, 0x3C, 0x2F, 0x62, 0x6F, 0x64, 0x79, 0x3E, 0x3C, 0x2F, 0x68, 0x74, 0x6D, 0x6C, 0x3E };
/*----------------------- Variables -----------------------------------------------------------------*/

/*----------------------- Functions -----------------------------------------------------------------*/
void 		vHTTPcleanRequest( HTTP_REQUEST *httpRequest );
void 		vHTTPCleanResponse( HTTP_RESPONSE *response );
void 		eHTTPbuildGet( char* path, HTTP_RESPONSE *response );
uint8_t vHTTPgetLine( char* input, uint16_t num, char* line );
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
	for( i=0U; i<CONTENT_TYPE_LENGTH; i++ )
	{
		response->contetntType[i] = 0x00U;
	}
	response->connect = HTTP_CONNECT_CLOSED;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t vHTTPgetLine( char* input, uint16_t num, char* line )
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

	vHTTPgetLine( req, 0, line );
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
void eHTTPbuildGet( char* path, HTTP_RESPONSE *response)
{
	strcpy( response->date, "Thu, 06 Feb 2020 15:11:53 GMT" );
	strcpy( response->server, HTTP_SERVER_NAME );
	strcpy(response->modified, "Sat, 20 Nov 2004 07:16:26 GMT");
	strcpy(response->contetntType, "text/html");

	response->connect = HTTP_CONNECT_CLOSED;
	if( path[0] == 0x00 )
	{
		response->status = HTTP_STATUS_OK;
		//response->contentLength = strlen(webPage);
		//response->data = webPage;
		response->contentLength = HTML_LENGTH;
		response->data = data__index_html;
	}
	else
	{
		response->status = HTTP_STATUS_NON_CONNECT;
		response->contentLength = 0U;
	}

	// /css/style.css
	// /js/main.js
	// //js/nouislider.min.js

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

	strcat( httpStr, HTTP_DATE_LINE );
	strcat( httpStr, response.date );
	strcat( httpStr, HTTP_END_LINE );

	strcat( httpStr, HTTP_LENGTH_LINE );
	sprintf( buffer, "%lu", response.contentLength );
	strcat( httpStr, buffer );
	strcat( httpStr, HTTP_END_LINE );

	strcat( httpStr, HTTP_CONTENT_LINE );
	strcat( httpStr, response.contetntType );
	strcat( httpStr, HTTP_END_LINE );

	//Connection: keep-alive

	strcat( httpStr, HTTP_CONN_LINE );
	strcat( httpStr, HTTP_END_LINE );
	strcat( httpStr, HTTP_END_LINE );

	return res;
}
/*---------------------------------------------------------------------------------------------------*/






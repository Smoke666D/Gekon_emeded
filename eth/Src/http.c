/*
 * http.c
 *
 *  Created on: Feb 6, 2020
 *      Author: mikhail.mikhailov
 */

/*----------------------- Includes ------------------------------------------------------------------*/
#include "http.h"
#include "string.h"
/*----------------------- Structures ----------------------------------------------------------------*/

/*----------------------- Variables -----------------------------------------------------------------*/

/*----------------------- Functions -----------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
HTTP_STATUS eHTTPparsingRequest( char* req, uint32_t len, HTTP_REQUEST httpRequest )
{
	HTTP_STATUS 	res = HTTP_STATUS_OK;
	//char					buffer[30];
	uint32_t			i = 0U;

	while( req[i] != ' ' )
	{
		i++;
	}

	switch( req[i+1] )
	{
		case 'G':
			httpRequest.method = HTTP_METHOD_GET;
			break;
		case 'P':
			httpRequest.method = HTTP_METHOD_POST;
			break;
		case 'H':
			httpRequest.method = HTTP_METHOD_HEAD;
			break;
		case 'O':
			httpRequest.method = HTTP_METHOD_OPTION;
			break;
		default:
			httpRequest.method = HTTP_METHOD_ERROR;
			break;
	}

	return HTTP_STATUS_OK;
}
/*---------------------------------------------------------------------------------------------------*/
HTTP_STATUS eHTTPmakeResponse( char* httpStr, uint32_t len, HTTP_RESPONSE response )
{
	HTTP_STATUS 	res   = HTTP_STATUS_OK;
	//uint32_t			count = 0U;
	char					buffer[30];

	strcpy(response.server, HTTP_SERVER_NAME);
	strcpy(response.modified, "Sat, 20 Nov 2004 07:16:26 GMT");
	strcpy(response.contetntType, "text/html");



	switch( response.status )
	{
		case HTTP_STATUS_OK:
			strcpy( httpStr, HTTP_OK_STATUS_LINE);
			break;
		case HTTP_STATUS_BAD_REQUEST:
			strcpy( httpStr, HTTP_NOT_FOUND_STATUS_LINE);
			break;
		default:
			break;
	}
	strcat( httpStr, HTTP_END_LINE);

	strcat( httpStr, HTTP_DATE_LINE);
	strcat( httpStr, "Thu, 06 Feb 2020 15:11:53 GMT");
	strcat( httpStr, HTTP_END_LINE);

	strcat( httpStr, HTTP_SERVER_LINE);
	strcat( httpStr, response.server);
	strcat( httpStr, HTTP_END_LINE);

	strcat( httpStr, HTTP_MODIF_LINE);
	strcat( httpStr, response.modified);
	strcat( httpStr, HTTP_END_LINE);

	strcat( httpStr, HTTP_LENGTH_LINE);
	sprintf( buffer, "%d", response.contentLength);
	strcat( httpStr, buffer);
	strcat( httpStr, HTTP_END_LINE);

	strcat( httpStr, HTTP_CONTENT_LINE);
	strcat( httpStr, response.contetntType);
	strcat( httpStr, HTTP_END_LINE);

	strcat( httpStr, HTTP_CONN_LINE);
	strcat( httpStr, HTTP_END_LINE);

	return res;
}
/*---------------------------------------------------------------------------------------------------*/






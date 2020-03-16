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

#include "rest.h"
#include "config.h"
/*----------------------- Structures ----------------------------------------------------------------*/
static char restBuffer[REST_BUFFER_SIZE];
/*----------------------- Constant ------------------------------------------------------------------*/
const char *httpMethodsStr[HTTP_METHOD_NUM] = { HTTP_METHOD_STR_GET, HTTP_METHOD_STR_POST, HTTP_METHOD_STR_PUT, HTTP_METHOD_STR_HEAD, HTTP_METHOD_STR_OPTION};
/*----------------------- Variables -----------------------------------------------------------------*/

/*----------------------- Functions -----------------------------------------------------------------*/
void 		vHTTPcleanRequest( HTTP_REQUEST *httpRequest );										/* Clean request structure */
void 		vHTTPCleanResponse( HTTP_RESPONSE *response );										/* Clean response structure */
uint8_t	uHTTPgetLine( char* input, uint16_t num, char* line );						/* Get the string of line from multiline text */
void 		eHTTPbuildGetResponse( char* path, HTTP_RESPONSE *response );			/* Build get response in response structure */
char*		vHTTPaddCache( char* httpStr, HTTP_CACHE cache);									/* Add cache string to http */
char*		vHTTPaddContetntType( char* httpStr, HTTP_CONTENT type );
char* 	vHTTPaddContentEncoding( char* httpStr, HTTP_ENCODING encoding );	/* Add encoding string to http  */

STREAM_STATUS cHTTPstreamFile( HTTP_STREAM* );							/* Stream call back for file transfer */
STREAM_STATUS cHTTPstreamConfigs( HTTP_STREAM* );						/* Stream call back for configuration data transfer */
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
	for ( i=0U; i<HEADER_LENGTH; i++ )
	{
		response->header[i] = 0x00U;
	}
	response->contentLength = 0U;
	response->contetntType  = HTTP_CONTENT_HTML;
	response->connect 			= HTTP_CONNECT_CLOSED;
	response->cache					= HTTP_CACHE_NO_CACHE;
	response->encoding			= HTTP_ENCODING_NO;

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
		start = strchr( end, LF_HEX ) + 1U;
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
 * Input:		req 		- pointer to a char array with input request in text form
 * 					request	- pointer to a the output request structure
 * Output:	HTTP status
 */
HTTP_STATUS eHTTPparsingRequest( char* req, HTTP_REQUEST* request )
{
	HTTP_STATUS 	res 	 = HTTP_STATUS_BAD_REQUEST;
	uint8_t 			i      = 0U;
	char* 				pchSt  = NULL;
	char* 				pchEnd = NULL;
	char					line[50U];

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
		/*-----------------------------------------------------------------------------*/
		if ( res == HTTP_STATUS_OK )
		{
			/* Parsing HTTP path  */
			pchSt  = strchr( line, '/' );
			pchEnd = strchr( pchSt, ' ' );
			if ( ( pchEnd - pchSt ) > 2U )
			{
				if( strncpy( request->path, ( pchSt ), ( pchEnd - pchSt ) ) == NULL )
				{
					res = HTTP_STATUS_BAD_REQUEST;
				}
			}
			/* Parsing Content */
			pchSt  = strstr( req, HTTP_END_HEADER );
			if ( pchSt != NULL )
			{
				request->content = pchSt + strlen( HTTP_END_HEADER );
			}
		}

	}

	return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Parsing data from response text
 * Input:		req 		- pointer to a char array with input request in text form
 * 					request	- pointer to a output response structure
 * Output:	HTTP status
 */
HTTP_STATUS eHTTPparsingResponse( char* input, char* data, HTTP_RESPONSE* response  )
{
	HTTP_STATUS 	res 	 = HTTP_STATUS_BAD_REQUEST;
	char* 				pchSt  = NULL;
	char* 				pchEn  = NULL;
	char					buffer[5U];

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
 * Build put response in to response structure
 * Input:		path 			- url to the file from the request
 * 					response	- structure of the response
 * 					content   - content of the request
 * Output:	none
 */
void eHTTPbuildPutResponse( char* path, HTTP_RESPONSE *response, char* content )
{
	uint16_t			adr       = 0xFFFFU;
	REST_REQUEST	request   = 0U;
	REST_ADDRESS	adrFlag		= REST_NO_ADR;

	strcpy( response->header, "Thu, 06 Feb 2020 15:11:53 GMT" );
	response->cache         = HTTP_CACHE_NO_CACHE_STORE;
	response->connect       = HTTP_CONNECT_CLOSED;
	response->status 				= HTTP_STATUS_BAD_REQUEST;
	response->contentLength = 0U;
	if ( path[0U] > 0U )
	{
		adrFlag = eRESTgetRequest( path, &request, &adr );
		switch ( request )
		{
			case REST_CONFIGS:
				if ( ( adr != 0xFFFFU ) && ( adr < SETTING_REGISTER_NUMBER ) && ( adrFlag != REST_NO_ADR ) )
				{
					if ( eRESTparsingConfig( content, configReg[adr] ) == REST_OK )
					{
						response->contetntType 	= HTTP_CONTENT_JSON;
						response->status 				= HTTP_STATUS_OK;
						response->contentLength = 0;
					}
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
 * Input:		path 			- url to the file from request
 * 					response	- structure of response
 * Output:	none
 */
void eHTTPbuildGetResponse( char* path, HTTP_RESPONSE *response)
{
	char 					*strStr   = NULL;
	uint16_t			adr       = 0xFFFFU;
	REST_REQUEST	request   = REST_REQUEST_ERROR;
	REST_ADDRESS	adrFlag		= REST_NO_ADR;
	uint32_t			length    = 0U;
	HTTP_STREAM  	*stream   = NULL;
	uint32_t			i         = 0U;
	/*----------------- Common header -----------------*/
	strStr = strcpy( response->header, "Thu, 06 Feb 2020 15:11:53 GMT" );
	response->cache         = HTTP_CACHE_NO_CACHE_STORE;
	response->connect       = HTTP_CONNECT_CLOSED;
	response->status 				= HTTP_STATUS_BAD_REQUEST;
	response->contentLength = 0U;
	response->encoding      = HTTP_ENCODING_NO;
	/*----------------- Parsing path -----------------*/
	/*------------------ INDEX.HTML ------------------*/
	strStr = strstr(path, "index" );
	if ( ( path[0U] == 0x00U ) || ( strStr != NULL) )
	{
		stream = &(response->stream);
		stream->size            = 1U;
		stream->index           = 0U;
		stream->content         = data__index_html;
		stream->length          = HTML_LENGTH;
		response->callBack      = cHTTPstreamFile;
		response->contetntType 	= HTTP_CONTENT_HTML;
		response->status 				= HTTP_STATUS_OK;
		response->contentLength = HTML_LENGTH;
		if ( HTML_ENCODING > 0U )
		{
			response->encoding = HTTP_ENCODING_GZIP;
		}
	}
	/*--------------------- REST ---------------------*/
	else if ( path[0U] > 0U )
	{
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
						response->contentLength += uRESTmakeConfig( restBuffer, configReg[i] );
					}
					response->contentLength += 1U + stream->size;		/* '[' + ']' + ',' */
					response->contetntType 	= HTTP_CONTENT_JSON;
					response->status 				= HTTP_STATUS_OK;
					response->data 					= restBuffer;
				}
				/*------------- Specific address ------------------*/
				else
				{
					if ( ( adr != 0xFFFFU ) && ( adr < SETTING_REGISTER_NUMBER ) )
					{
						response->contentLength = uRESTmakeConfig( restBuffer, configReg[adr] );
					}
					stream = &(response->stream);
					stream->size            = adr + 1U;
					stream->start           = adr;
					stream->index           = 0U;
					response->callBack      = cHTTPstreamConfigs;
					response->contetntType 	= HTTP_CONTENT_JSON;
					response->status 				= HTTP_STATUS_OK;
				}
				break;
			case REST_REQUEST_ERROR:
				break;
			default:
				response->status 				= HTTP_STATUS_BAD_REQUEST;
				response->contentLength = 0U;
				break;
		}
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
void vHTTPbuildResponse( HTTP_REQUEST* request, HTTP_RESPONSE* response)
{
	vHTTPCleanResponse( response );
	response->status = HTTP_STATUS_BAD_REQUEST;

	switch ( request->method )
	{
		case HTTP_METHOD_NO:
			break;
		case HTTP_METHOD_GET:
			response->method = HTTP_METHOD_GET;
			eHTTPbuildGetResponse( request->path, response );
			break;
		case HTTP_METHOD_POST:
			response->method = HTTP_METHOD_POST;
			break;
		case HTTP_METHOD_PUT:
			response->method = HTTP_METHOD_PUT;
			eHTTPbuildPutResponse( request->path, response, request->content );
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
	HTTP_STATUS 	res = HTTP_STATUS_ERROR;
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
							itoa( response->contentLength, buffer, 10U );
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
											if ( strcat( httpStr, HTTP_CACHE_CONTROL ) != NULL )
											{
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
/*
 * Add type of the content to a string
 * input:		httpStr - target string
 * 					type		- type of content
 * output:	none
 */
char* vHTTPaddContetntType( char* httpStr, HTTP_CONTENT type )
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

char* vHTTPaddContentEncoding( char* httpStr, HTTP_ENCODING encoding )
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
 * Stream call back for file transfer
 */
STREAM_STATUS cHTTPstreamFile( HTTP_STREAM* stream )
{
	stream->status = STREAM_END;
	stream->index++;
	return stream->status;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Stream call back for configuration data transfer
 */
STREAM_STATUS cHTTPstreamConfigs( HTTP_STREAM* stream )
{
	uint32_t length = stream->size - stream->start;

	if ( ( stream->index == 0U ) && ( length > 1U ) )
	{
		restBuffer[0U] = '[';
		stream->length = uRESTmakeConfig( &restBuffer[1U], configReg[stream->start + stream->index] ) + 1U;
	}
	else
	{
		stream->length = uRESTmakeConfig( restBuffer, configReg[stream->start + stream->index] );
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










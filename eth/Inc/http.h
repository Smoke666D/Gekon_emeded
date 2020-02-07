/*
 * http.h
 *
 *  Created on: Feb 6, 2020
 *      Author: mikhail.mikhailov
 */

#ifndef INC_HTTP_H_
#define INC_HTTP_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
/*------------------------ Define --------------------------------------*/
#define		CR_HEX								0x0D
#define		LF_HEX								0x0A

#define		DATE_LENGTH						29
#define		SERVER_LENGTH					21
#define		CONTENT_TYPE_LENGTH		9

#define		HTTP_PATH_LENGTH			30
#define		WEB_PAGE_SIZE					60
/*-------------------------- ENUM --------------------------------------*/

typedef enum
{
	//HTTP_STATUS_CONTINUE 						= 100,
	//HTTP_STATUS_SWITCHING_PROTOCOLS = 101,
	HTTP_STATUS_OK									= 200,
	HTTP_STATUS_BAD_REQUEST					= 400,
	HTTP_STATUS_NON_CONNECT					= 404
} HTTP_STATUS;
/*------------------------- Methods -----------------------------------*/
#define		HTTP_METHOD_NUM							4U

#define		HTTP_METHOD_STR_GET					"GET"
#define		HTTP_METHOD_STR_POST				"POST"
#define		HTTP_METHOD_STR_HEAD				"HEAD"
#define		HTTP_METHOD_STR_OPTION			"OPTION"

typedef enum
{
	HTTP_METHOD_NO			= 0x00U,
	HTTP_METHOD_GET			= 0x01U,
	HTTP_METHOD_POST		= 0x02U,
	HTTP_METHOD_HEAD		= 0x03U,
	HTTP_METHOD_OPTION	= 0x04U
} HTTP_METHOD;
/*----------------------------------------------------------------------*/
typedef enum
{
	HTTP_CONNECT_CLOSED
}HTTP_CONNECT;
/*----------------------- Structures -----------------------------------*/
typedef struct
{
		HTTP_METHOD		method;
		char					path[HTTP_PATH_LENGTH];
} HTTP_REQUEST;

typedef struct
{
		HTTP_STATUS		status;
		HTTP_METHOD		method;
		char 					date[DATE_LENGTH];
		char					server[SERVER_LENGTH];
		char					modified[DATE_LENGTH];
		uint32_t			contentLength;
		char					contetntType[CONTENT_TYPE_LENGTH];
		HTTP_CONNECT	connect;
		char* 				data;
} HTTP_RESPONSE;
/* HHTP status-codes ---------------------------------------------------*/
/*
#define		HTTP_STATUS_CONTINUE												"100"
#define		HTTP_STATUS_SWITCHING_PROTOCOLS							"101"
#define		HTTP_STATUS_OK															"200"
#define		HTTP_STATUS_CREATED													"201"
#define		HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION		"203"
#define		HTTP_STATUS_NON_CONNECT											"204"
#define		HTTP_STATUS_RESET_CONTENT										"205"
#define		HTTP_STATUS_PARITAL_CONTENT									"206"
#define		HTTP_STATUS_MULTIPLE_CHOICES								"300"
#define		HTTP_STATUS_MOVED_PERMANENTLY								"301"
#define		HTTP_STATUS_FOUND														"302"
#define		HTTP_STATUS_SEE_PTHER												"303"
#define		HTTP_STATUS_NOT_MODIFIED										"304"
#define		HTTP_STATUS_USE_PROXY												"305"
#define		HTTP_STATUS_TEMPORY_REDIRECT								"307"
#define		HTTP_STATUS_BAD_REQUEST											"400"
#define		HTTP_STATUS_UNAUTHORIZED										"401"
#define		HTTP_STATUS_PAYMENT_REQUIRED								"402"
#define		HTTP_STATUS_FORBIDDEN												"403"
#define		HTTP_STATUS_NOT_FOUND												"404"
#define		HTTP_STATUS_METHOD_NOT_ALLOWED							"405"
#define		HTTP_STATUS_NOT_ACCEPTABLE									"406"
#define		HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED		"407"
#define		HTTP_STATUS_REQUEST_TIMEOUT									"408"
#define		HTTP_STATUS_CONFLICT												"409"
#define		HTTP_STATUS_GONE														"410"
#define		HTTP_STATUS_LENGTH_REQUIRED									"411"
#define		HTTP_STATUS_PRECONDITION_FAILED							"412"
#define		HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE				"413"
#define		HTTP_STATUS_REQUEST_URI_TOO_LARGE						"414"
#define		HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE					"415"
#define		HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE	"416"
#define		HTTP_STATUS_EXPECTATION_FAILED							"417"
#define		HTTP_STATUS_INTERNAL_SERVER_ERROR						"500"
#define		HTTP_STATUS_NOT_IMPLEMENTED									"501"
#define		HTTP_STATUS_BAD_GATEWAY											"502"
#define		HTTP_STATUS_SERVICE_UNAVAILABLE							"503"
#define		HTTP_STATUS_GATEWAY_TIMEOUT									"504"
#define		HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED			"505"
*/
/*------------------------ Tempaltes ----------------------------------*/
#define		HTTP_END_LINE								"\r\n"
#define		HTTP_SERVER_NAME						"EMBmss/0.0.1"

#define		HTTP_OK_STATUS_LINE					"HTTP/1.1 200 OK"
#define		HTTP_NOT_FOUND_STATUS_LINE	"HTTP/1.1 400 Not Found"

#define		HTTP_DATE_LINE							"Date: "
#define		HTTP_SERVER_LINE						"Server: "
#define		HTTP_MODIF_LINE							"Last-Modified: "
#define		HTTP_ETAG_LINE							"ETag: "
#define		HTTP_RANGE_LINE							"Accept-Ranges: bytes"
#define		HTTP_LENGTH_LINE						"Content-Length: "
#define		HTTP_CONN_LINE							"Connection: closed"
#define		HTTP_CONTENT_LINE						"Content-Type: "
/*----------------------- Functions ------------------------------------*/
HTTP_STATUS eHTTPparsingRequest( char* req, HTTP_REQUEST *request );
HTTP_STATUS eHTTPbuildResponse( HTTP_REQUEST request, HTTP_RESPONSE *response );
HTTP_STATUS eHTTPmakeResponse( char* httpStr, HTTP_RESPONSE response );
/*----------------------------------------------------------------------*/
#endif /* INC_HTTP_H_ */


















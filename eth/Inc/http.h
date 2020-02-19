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
#define		CR_HEX								0x0DU
#define		LF_HEX								0x0AU

#define		DATE_LENGTH						29U
#define		SERVER_LENGTH					21U
#define		CONTENT_TYPE_LENGTH		9U

#define		HTTP_PATH_LENGTH			30U
#define		WEB_PAGE_SIZE					60U

#define		HTTP_BUFER_SIZE				600U
/*-------------------------- ENUM --------------------------------------*/
typedef enum
{
	HTTP_STATUS_OK											= 200U,
	HTTP_STATUS_CREATED									= 201U,		/* Код состояния является ответом на POST, который приводит к созданию */
	HTTP_STATUS_NO											= 204U,		/* Нет содержимого. Это ответ на успешный запрос, который не будет возвращать тело */
	HTTP_STATUS_NOT_MDIFIED							= 304U,		/* Используйте этот код состояния, когда заголовки HTTP-кеширования находятся в работе */
	HTTP_STATUS_BAD_REQUEST							= 400U,		/* Этот код состояния указывает, что запрос искажен, например, если тело не может быть проанализировано*/
	HTTP_STATUS_UNAUTHORIZED						= 401U,		/* Если не указаны или недействительны данные аутентификации. Также полезно активировать всплывающее окно auth, если приложение используется из браузера */
	HTTP_STATUS_FORBIDDEN								= 403U,		/* Когда аутентификация прошла успешно, но аутентифицированный пользователь не имеет доступа к ресурсу */
	HTTP_STATUS_NON_CONNECT							= 404U,		/* Если запрашивается несуществующий ресурс */
	HTTP_STATUS_METHOD_NOT_ALLOWED			= 405U,		/* Когда запрашивается HTTP-метод, который не разрешен для аутентифицированного пользователя */
	HTTP_STATUS_GONE										= 410U,		/* Этот код состояния указывает, что ресурс в этой конечной точке больше не доступен. Полезно в качестве защитного ответа для старых версий API */
	HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE	= 415U,		/* Если в качестве части запроса был указан неправильный тип содержимого */
	HTTP_STATUS_UNPROCESSABLE_ENITITY		= 422U,		/* Используется для проверки ошибок */
	HTTP_STATUS_TOO_MANY_REQUESTS				= 429U,		/* Когда запрос отклоняется из-за ограничения скорости */
} HTTP_STATUS;
/*------------------------- Methods -----------------------------------*/
#define		HTTP_METHOD_NUM							4U

#define		HTTP_METHOD_STR_GET					"GET"
#define		HTTP_METHOD_STR_POST				"POST"
#define		HTTP_METHOD_STR_PUT					"PUT"
#define		HTTP_METHOD_STR_HEAD				"HEAD"
#define		HTTP_METHOD_STR_OPTION			"OPTION"

typedef enum
{
	HTTP_METHOD_NO			= 0x00U,
	HTTP_METHOD_GET			= 0x01U,
	HTTP_METHOD_POST		= 0x02U,
	HTTP_METHOD_PUT			= 0x03U,
	HTTP_METHOD_HEAD		= 0x04U,
	HTTP_METHOD_OPTION	= 0x05U
} HTTP_METHOD;
/*----------------------- Content type ---------------------------------*/
#define	HTTP_CONTENT_STR_HTML		"text/html"
#define	HHTP_CONTENT_STR_CSS		"text/css"
#define HTTP_CONTENT_STR_JS			"text/javascript"
#define HTTP_CONTENT_STR_JSON		"application/json"
#define HTTP_CONTENT_STR_XML		"text/xml"

typedef enum
{
	HTTP_CONTENT_HTML,
	HTTP_CONTENT_CSS,
	HTTP_CONTENT_JS,
	HTTP_CONTENT_JSON,
	HTTP_CONTENT_XML,
} HTTP_CONTENT;
/*------------------------ Cache control -------------------------------*/
#define	HTTP_CACHE_STR_NO_CACHE		"no-cache"
#define	HTTP_CACHE_STR_NO_STORE		"no-store"

typedef enum
{
	HTTP_CACHE_NO_CACHE,
	HTTP_CACHE_NO_STORE,
	HTTP_CACHE_NO_CACHE_STORE,
	/*
	HTTP_CACHE_NO_TRANSFORM,
	HTTP_CACHE_ONLY_IF_CACHED,
	HTTP_CACHE_MAX_AGE,
	HTTP_CACHE_MAX_STALE,
	HTTP_CACHE_MIN_FRESH
	*/
} HTTP_CACHE;
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
		char					host[HTTP_PATH_LENGTH];
		HTTP_CONTENT	contetntType;
		HTTP_CONNECT	connect;
		HTTP_CACHE		cache;
} HTTP_REQUEST;

typedef struct
{
		HTTP_STATUS		status;
		HTTP_METHOD		method;
		char 					date[DATE_LENGTH];
		char					server[SERVER_LENGTH];
		char					modified[DATE_LENGTH];
		uint32_t			contentLength;
		HTTP_CONTENT	contetntType;
		HTTP_CONNECT	connect;
		char* 				data;
		HTTP_CACHE		cache;
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
/*------------------------ Templates ----------------------------------*/
#define		HTTP_END_LINE								"\r\n"
#define		HTTP_SERVER_NAME						"EMBmss/0.0.1"

#define		HTTP_OK_STATUS_LINE					"HTTP/1.1 200 OK"
#define		HTTP_NOT_FOUND_STATUS_LINE	"HTTP/1.1 400 Not Found"

#define		HTTP_HOST_LINE							"Host: "
#define		HTTP_DATE_LINE							"Date: "
#define		HTTP_SERVER_LINE						"Server: "
#define		HTTP_MODIF_LINE							"Last-Modified: "
#define		HTTP_ETAG_LINE							"ETag: "
#define		HTTP_RANGE_LINE							"Accept-Ranges: bytes"
#define		HTTP_LENGTH_LINE						"Content-Length: "
#define		HTTP_CONN_LINE							"Connection: closed"
#define		HTTP_CONTENT_LINE						"Content-Type: "
#define		HTTP_CACHE_CONTROL					"Cache-Control: "
/*----------------------- Functions ------------------------------------*/
HTTP_STATUS eHTTPparsingRequest( char* req, HTTP_REQUEST* request );										/* Parsing data from request text */
HTTP_STATUS eHTTPbuildResponse( HTTP_REQUEST* request, HTTP_RESPONSE* response );				/* Build response in response structure */
HTTP_STATUS eHTTPmakeResponse( char* httpStr, HTTP_RESPONSE* response );								/* Make string response from response structure */
HTTP_STATUS eHTTPbuildRequest( HTTP_REQUEST* request );																	/* Build request structure */
HTTP_STATUS eHTTPmakeRequest ( char* httpStr, HTTP_REQUEST* request );										/* Make string request from request structure */
HTTP_STATUS eHTTPparsingResponse( char* input, char* data, HTTP_RESPONSE* response );		/* Parsing data from response text */
/*----------------------------------------------------------------------*/
#endif /* INC_HTTP_H_ */


















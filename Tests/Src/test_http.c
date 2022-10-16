/*
 * test_http.c
 *
 *  Created on: 26 апр. 2022 г.
 *      Author: 79110
 */

#include "tests.h"
#include "unity.h"
#include "http.h"

#include "string.h"
#include "server.h"
#include "controllerTypes.h"

static char buffer[HTTP_OUTPUT_BUFFER_SIZE] = { 0U };

void test_vHTTPcleanRequest ( void )
{
  uint8_t i = 0U;
  HTTP_REQUEST request =
  {
    .method  = HTTP_METHOD_PUT,
    .connect = HTTP_CONNECT_CLOSED,
    .cache   = HTTP_CACHE_NO_CACHE_STORE
  };
  request.content.type = HTTP_CONTENT_JSON;
  for ( i=0U; i<HTTP_PATH_LENGTH; i++ )
  {
    request.path[i] = 0xAA;
    request.host[i] = 0xDD;
  }
  vHTTPcleanRequest( &request );
  TEST_ASSERT_EQUAL( HTTP_METHOD_NO, request.method );
  for ( i=0U; i<HTTP_PATH_LENGTH; i++ )
  {
    TEST_ASSERT_EQUAL( 0U, request.path[i] );
    TEST_ASSERT_EQUAL( 0U, request.host[i] );
  }
  TEST_ASSERT_EQUAL( HTTP_CONTENT_HTML,   request.content.type );
  TEST_ASSERT_EQUAL( HTTP_CONNECT_CLOSED, request.connect );
  TEST_ASSERT_EQUAL( HTTP_CACHE_NO_CACHE, request.cache );
  return;
}
void test_vHTTPaddContentType ( void )
{
  buffer[0U] = 0U;
  vHTTPaddContentType( buffer, HTTP_CONTENT_HTML );
  TEST_ASSERT_EQUAL_STRING( "Content-Type: text/html\r\n", buffer );
  buffer[0U] = 0U;
  vHTTPaddContentType( buffer, HTTP_CONTENT_CSS );
  TEST_ASSERT_EQUAL_STRING( "Content-Type: text/css\r\n", buffer );
  buffer[0U] = 0U;
  vHTTPaddContentType( buffer, HTTP_CONTENT_JS );
  TEST_ASSERT_EQUAL_STRING( "Content-Type: text/javascript\r\n", buffer );
  buffer[0U] = 0U;
  vHTTPaddContentType( buffer, HTTP_CONTENT_JSON );
  TEST_ASSERT_EQUAL_STRING( "Content-Type: application/json; charset=UTF-8\r\n", buffer );
  buffer[0U] = 0U;
  vHTTPaddContentType( buffer, HTTP_CONTENT_XML );
  TEST_ASSERT_EQUAL_STRING( "Content-Type: text/xml\r\n", buffer );
  buffer[0U] = 0U;
  vHTTPaddContentType( buffer, 0xFFU );
  TEST_ASSERT_EQUAL_STRING( "Content-Type: error\r\n", buffer );
  return;
}
void test_vHTTPaddContentEncoding ( void )
{
  buffer[0U] = 0U;
  vHTTPaddContentEncoding( buffer, HTTP_ENCODING_NO );
  TEST_ASSERT_EQUAL_STRING( "", buffer );
  buffer[0U] = 0U;
  vHTTPaddContentEncoding( buffer, HTTP_ENCODING_GZIP );
  TEST_ASSERT_EQUAL_STRING( "Content-Encoding: gzip\r\n", buffer );
  buffer[0U] = 0U;
  vHTTPaddContentEncoding( buffer, HTTP_ENCODING_COMPRESS );
  TEST_ASSERT_EQUAL_STRING( "Content-Encoding: compress\r\n", buffer );
  buffer[0U] = 0U;
  vHTTPaddContentEncoding( buffer, HTTP_ENCODING_DEFLATE );
  TEST_ASSERT_EQUAL_STRING( "Content-Encoding: deflate\r\n", buffer );
  buffer[0U] = 0U;
  vHTTPaddContentEncoding( buffer, HTTP_ENCODING_INDENTITY );
  TEST_ASSERT_EQUAL_STRING( "Content-Encoding: identity\r\n", buffer );
  buffer[0U] = 0U;
  vHTTPaddContentEncoding( buffer, HTTP_ENCODING_BR );
  TEST_ASSERT_EQUAL_STRING( "Content-Encoding: br\r\n", buffer );
  buffer[0U] = 0U;
  vHTTPaddContentEncoding( buffer, 0xFFU );
  TEST_ASSERT_EQUAL_STRING( "Content-Encoding: \r\n", buffer );
  return;
}
void test_vHTTPaddCache ( void )
{
  buffer[0U] = 0U;
  vHTTPaddCache( buffer, HTTP_CACHE_NO_CACHE );
  TEST_ASSERT_EQUAL_STRING( "Cache-Control: no-cache\r\n", buffer );
  buffer[0U] = 0U;
  vHTTPaddCache( buffer, HTTP_CACHE_NO_STORE );
  TEST_ASSERT_EQUAL_STRING( "Cache-Control: no-store\r\n", buffer );
  buffer[0U] = 0U;
  vHTTPaddCache( buffer, HTTP_CACHE_NO_CACHE_STORE );
  TEST_ASSERT_EQUAL_STRING( "Cache-Control: no-cache,no-store\r\n", buffer );
  buffer[0U] = 0U;
  vHTTPaddCache( buffer, 0xFFU );
  TEST_ASSERT_EQUAL_STRING( "Cache-Control: no-cache,no-store\r\n", buffer );
  return;
}
void test_cHTTPstreamConfigs ( void )
{
  HTTP_STREAM stream = { 0U };
  stream.size  = 2U;
  stream.index = 0U;
  stream.start = 0U;
  stream.flag  = STREAM_FLAG_COPY;
  TEST_ASSERT_EQUAL( STREAM_CONTINUES, cHTTPstreamConfigs( &stream ) );
  TEST_ASSERT_EQUAL_STRING( "[{\"adr\":0,\"scale\":0,\"len\":3,\"value\":[2,1,0],\"min\":0,\"max\":0,\"units\":\"%20%20%20%20%20\",\"type\":\"%03%55\",\"bitMapSize\":0,\"bit\":[]},", restBuffer );
  TEST_ASSERT_EQUAL( STREAM_END, cHTTPstreamConfigs( &stream ) );
  TEST_ASSERT_EQUAL_STRING( "{\"adr\":1,\"scale\":0,\"len\":3,\"value\":[1,1,0],\"min\":0,\"max\":0,\"units\":\"%20%20%20%20%20\",\"type\":\"%03%55\",\"bitMapSize\":0,\"bit\":[]}]", restBuffer );
  stream.start = SETTING_REGISTER_NUMBER;
  stream.index = 0U;
  stream.size  = 2U;
  TEST_ASSERT_EQUAL( STREAM_ERROR, cHTTPstreamConfigs( &stream ) );
  return;
}
void test_cHTTPstreamCharts ( void )
{
  HTTP_STREAM stream = { 0U };
  stream.size  = CHART_NUMBER;
  stream.index = 0U;
  stream.start = 0U;
  stream.flag  = STREAM_FLAG_COPY;
  TEST_ASSERT_EQUAL( STREAM_CONTINUES, cHTTPstreamCharts( &stream ) );
  TEST_ASSERT_EQUAL_STRING( "[{\"xType\":0,\"yType\":0,\"size\":2,\"dots\":[{\"x\":0,\"y\":0},{\"x\":15,\"y\":20}]},",  restBuffer );
  TEST_ASSERT_EQUAL( STREAM_CONTINUES, cHTTPstreamCharts( &stream ) );
  TEST_ASSERT_EQUAL_STRING( "{\"xType\":0,\"yType\":1,\"size\":2,\"dots\":[{\"x\":0,\"y\":0},{\"x\":98304000,\"y\":16384000}]},", restBuffer );
  TEST_ASSERT_EQUAL( STREAM_END, cHTTPstreamCharts( &stream ) );
  TEST_ASSERT_EQUAL_STRING( "{\"xType\":0,\"yType\":2,\"size\":2,\"dots\":[{\"x\":0,\"y\":0},{\"x\":98304000,\"y\":6553600}]}]", restBuffer );
  stream.size  = 2U;
  stream.index = 0U;
  stream.start = CHART_NUMBER;
  TEST_ASSERT_EQUAL( STREAM_ERROR, cHTTPstreamCharts( &stream ) );
  return;
}
void test_cHTTPstreamTime ( void )
{
  HTTP_STREAM stream = { 0U };
  stream.size  = 1U;
  stream.index = 0U;
  stream.start = 0U;
  stream.flag  = STREAM_FLAG_COPY;
  TEST_ASSERT_EQUAL( STREAM_END, cHTTPstreamTime( &stream ) );
  TEST_ASSERT_NOT_NULL( strstr( restBuffer, "{\"hour\":" ) );
  TEST_ASSERT_NOT_NULL( strstr( restBuffer, ",\"min\":" ) );
  TEST_ASSERT_NOT_NULL( strstr( restBuffer, ",\"sec\":" ) );
  TEST_ASSERT_NOT_NULL( strstr( restBuffer, ",\"year\":" ) );
  TEST_ASSERT_NOT_NULL( strstr( restBuffer, ",\"month\":" ) );
  TEST_ASSERT_NOT_NULL( strstr( restBuffer, ",\"day\":" ) );
  TEST_ASSERT_NOT_NULL( strstr( restBuffer, ",\"wday\":" ) );
  TEST_ASSERT_NOT_NULL( strstr( restBuffer, "}" ) );
  return;
}
void test_cHTTPstreamData ( void )
{
  HTTP_STREAM stream = { 0U };
  stream.size  = 1U;
  stream.index = 0U;
  stream.start = 0U;
  stream.flag  = STREAM_FLAG_COPY;
  TEST_ASSERT_EQUAL( STREAM_END, cHTTPstreamData( &stream ) );
  TEST_ASSERT_EQUAL_STRING( "{\"value\":0}", restBuffer );
  stream.start = FREE_DATA_SIZE;
  stream.index = 0U;
  TEST_ASSERT_EQUAL( STREAM_ERROR, cHTTPstreamData( &stream ) );
  return;
}
void test_cHTTPstreamJSON ( void )
{
  HTTP_STREAM stream = { 0U };
  TEST_ASSERT_EQUAL( STREAM_END, cHTTPstreamJSON( &stream ) );
  return;
}
void test_cHTTPstreamLog ( void )
{
  HTTP_STREAM stream = { 0U };
  stream.size  = 2U;
  stream.index = 0U;
  stream.start = 0U;
  TEST_ASSERT_EQUAL( STREAM_CONTINUES, cHTTPstreamLog( &stream ) );
  TEST_ASSERT_NOT_NULL( strstr( restBuffer, "[{\"time\":" ) );
  TEST_ASSERT_NOT_NULL( strstr( restBuffer, ",\"type\":38,\"action\":2}," ) );
  TEST_ASSERT_EQUAL( STREAM_END, cHTTPstreamLog( &stream ) );
  TEST_ASSERT_NOT_NULL( strstr( restBuffer, "{\"time\":" ) );
  TEST_ASSERT_NOT_NULL( strstr( restBuffer, ",\"type\":38,\"action\":2}]" ) );
  stream.start = LOG_SIZE;
  stream.index = 0U;
  stream.size  = 2U;
  TEST_ASSERT_EQUAL( STREAM_ERROR, cHTTPstreamLog( &stream ) );
  return;
}
void test_vHTTPCleanResponse ( void )
{
  HTTP_RESPONSE response = { 0U };
  vHTTPCleanResponse( &response );
  TEST_ASSERT_EQUAL( HTTP_STATUS_BAD_REQUEST, response.status );
  TEST_ASSERT_EQUAL( HTTP_METHOD_NO, response.method );
  TEST_ASSERT_EQUAL( 0U, response.content.length );
  TEST_ASSERT_EQUAL( HTTP_CONTENT_HTML, response.content.type );
  TEST_ASSERT_EQUAL( HTTP_CONNECT_CLOSED, response.connect );
  TEST_ASSERT_EQUAL( HTTP_CACHE_NO_CACHE, response.cache );
  TEST_ASSERT_EQUAL( HTTP_ENCODING_NO, response.encoding );
  for  ( uint8_t i=0U; i<HEADER_LENGTH; i++ )
  {
    TEST_ASSERT_EQUAL( 0U, response.header[i] );
  }
  return;
}
void test_uHTTPgetLine ( void )
{
  const char input[] = "First test sting\nSecond test string\nThird test string\n";
  char       output[30U] = { 0U };
  uint8_t    res = 0U;
  res = uHTTPgetLine( input, 1U, output );
  TEST_ASSERT_GREATER_THAN( 0, res );
  TEST_ASSERT_EQUAL_STRING( "Second test string", output );
  return;
}
void test_vHTTPbuildPutResponse ( void )
{
  char path[] = "";
  char content[] = "";
  HTTP_RESPONSE response = { 0U };
  uint32_t      remoteIP = 0U;
  vHTTPbuildPutResponse( path, &response, content, remoteIP );
  return;
}
void test_vHTTPbuildGetResponse ( void )
{
  char path[] = "";
  HTTP_RESPONSE response = { 0U };
  uint32_t      remoteIP = 0U;
  vHTTPbuildGetResponse( path, &response, remoteIP );
  return;
}
void test_eHTTPparsingRequest ( void )
{
  const char   input[] = "GET / HTTP/1.1\r\n";
  HTTP_REQUEST request = { 0U };
  TEST_ASSERT_EQUAL( HTTP_STATUS_OK, eHTTPparsingRequest( input, &request ) );
  TEST_ASSERT_EQUAL( HTTP_METHOD_GET, request.method );
  return;
}
void test_vHTTPbuildResponse ( void )
{
  HTTP_REQUEST  request  = { 0U };
  HTTP_RESPONSE response = { 0U };
  uint32_t      remoteIP = 0U;

  request.method = HTTP_METHOD_NO;
  vHTTPbuildResponse( &request, &response, remoteIP );
  TEST_ASSERT_EQUAL( HTTP_METHOD_NO, response.method );
  request.method = HTTP_METHOD_GET;
  vHTTPbuildResponse( &request, &response, remoteIP );
  TEST_ASSERT_EQUAL( HTTP_METHOD_NO, response.method );
  request.method = HTTP_METHOD_POST;
  vHTTPbuildResponse( &request, &response, remoteIP );
  TEST_ASSERT_EQUAL( HTTP_METHOD_NO, response.method );
  request.method = HTTP_METHOD_PUT;
  vHTTPbuildResponse( &request, &response, remoteIP );
  TEST_ASSERT_EQUAL( HTTP_METHOD_NO, response.method );
  request.method = HTTP_METHOD_HEAD;
  vHTTPbuildResponse( &request, &response, remoteIP );
  TEST_ASSERT_EQUAL( HTTP_METHOD_NO, response.method );
  request.method = HTTP_METHOD_OPTION;
  vHTTPbuildResponse( &request, &response, remoteIP );
  TEST_ASSERT_EQUAL( HTTP_METHOD_NO, response.method );
  request.method = HTTP_METHOD_MAX;
  vHTTPbuildResponse( &request, &response, remoteIP );
  TEST_ASSERT_EQUAL( HTTP_METHOD_NO, response.method );
  return;
}
void test_vHTTPmakeResponse ( void )
{
  char          input[HTTP_OUTPUT_BUFFER_SIZE]  = { 0U };
  HTTP_RESPONSE response = { 0U };
  strcpy( response.header, "HEAD" );
  response.content.length = 10U;
  response.content.type   = HTTP_CONTENT_JSON;
  response.encoding       = HTTP_ENCODING_GZIP;
  response.cache          = HTTP_CACHE_NO_CACHE;

  response.status = HTTP_STATUS_OK;
  vHTTPmakeResponse( input, &response );
  TEST_ASSERT_STRING_EQUAL( "", input );
  response.status = HTTP_STATUS_CREATED;
  vHTTPmakeResponse( input, &response );
  TEST_ASSERT_STRING_EQUAL( "", input );
  response.status = HTTP_STATUS_NO;
  vHTTPmakeResponse( input, &response );
  TEST_ASSERT_STRING_EQUAL( "", input );
  response.status = HTTP_STATUS_NOT_MDIFIED;
  vHTTPmakeResponse( input, &response );
  TEST_ASSERT_STRING_EQUAL( "", input );
  response.status = HTTP_STATUS_BAD_REQUEST;
  vHTTPmakeResponse( input, &response );
  TEST_ASSERT_STRING_EQUAL( "", input );
  response.status = HTTP_STATUS_UNAUTHORIZED;
  vHTTPmakeResponse( input, &response );
  TEST_ASSERT_STRING_EQUAL( "", input );
  response.status = HTTP_STATUS_FORBIDDEN;
  vHTTPmakeResponse( input, &response );
  TEST_ASSERT_STRING_EQUAL( "", input );
  response.status = HTTP_STATUS_NON_CONNECT;
  vHTTPmakeResponse( input, &response );
  TEST_ASSERT_STRING_EQUAL( "", input );
  response.status = HTTP_STATUS_METHOD_NOT_ALLOWED;
  vHTTPmakeResponse( input, &response );
  TEST_ASSERT_STRING_EQUAL( "", input );
  response.status = HTTP_STATUS_GONE;
  vHTTPmakeResponse( input, &response );
  TEST_ASSERT_STRING_EQUAL( "", input );
  response.status = HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE;
  vHTTPmakeResponse( input, &response );
  TEST_ASSERT_STRING_EQUAL( "", input );
  response.status = HTTP_STATUS_UNPROCESSABLE_ENITITY;
  vHTTPmakeResponse( input, &response );
  TEST_ASSERT_STRING_EQUAL( "", input );
  response.status = HTTP_STATUS_TOO_MANY_REQUESTS;
  vHTTPmakeResponse( input, &response );
  TEST_ASSERT_STRING_EQUAL( "", input );
  return;
}
void test_eHTTPbuildRequest ( void )
{
  HTTP_REQUEST request = { 0U };
  request.method = HTTP_METHOD_NO;
  TEST_ASSERT_EQUAL( HTTP_STATUS_BAD_REQUEST, eHTTPbuildRequest( &request ) );
  request.method = HTTP_METHOD_GET;
  TEST_ASSERT_EQUAL( HTTP_STATUS_OK, eHTTPbuildRequest( &request ) );
  request.method = HTTP_METHOD_POST;
  TEST_ASSERT_EQUAL( HTTP_STATUS_BAD_REQUEST, eHTTPbuildRequest( &request ) );
  request.method = HTTP_METHOD_HEAD;
  TEST_ASSERT_EQUAL( HTTP_STATUS_BAD_REQUEST, eHTTPbuildRequest( &request ) );
  request.method = HTTP_METHOD_OPTION;
  TEST_ASSERT_EQUAL( HTTP_STATUS_BAD_REQUEST, eHTTPbuildRequest( &request ) );
  request.method = HTTP_METHOD_MAX;
  TEST_ASSERT_EQUAL( HTTP_STATUS_BAD_REQUEST, eHTTPbuildRequest( &request ) );
  return;
}
void test_vHTTPmakeRequest ( void )
{
  HTTP_REQUEST request     = { 0U };
  char         output[30U] = { 0U };
  strcpy( request.path, "PATH" );
  strcpy( request.host, "HOST" );
  request.cache = HTTP_CACHE_NO_CACHE;
  vHTTPmakeRequest( &request, output );
  TEST_ASSERT_STRING_EQUAL( "GET PATH HTTP/1.1 \r\nHost: HOST\r\nConnection: closed\r\nno-cache\r\n", output );
  return;
}
void test_eHTTPparsingResponse ( void )
{
  const char    input[]   = "iii";
  char          data[30U] = { 0U };
  HTTP_RESPONSE response  = { 0U };
  TEST_ASSERT_EQUAL( HTTP_STATUS_OK, eHTTPparsingResponse( input, data, &response ) );
  return;
}

void runTest_http ( void )
{
  UnitySetTestFile( "http.c" );
  UnityDefaultTestRun( test_vHTTPcleanRequest, "Cleaning request data structure", 0U );
  UnityDefaultTestRun( test_vHTTPaddContentType, "Make content type string", 0U );
  UnityDefaultTestRun( test_vHTTPaddContentEncoding, "Make encoding string", 0U );
  UnityDefaultTestRun( test_vHTTPaddCache, "Make cache string", 0U );
  UnityDefaultTestRun( test_cHTTPstreamConfigs, "Make config string for the stream", 0U );
  UnityDefaultTestRun( test_cHTTPstreamCharts, "Make chart string for the stream", 0U );
  UnityDefaultTestRun( test_cHTTPstreamTime, "Make time string for the stream", 0U );
  UnityDefaultTestRun( test_cHTTPstreamData, "Make free data string for the stream", 0U );
  UnityDefaultTestRun( test_cHTTPstreamJSON, "Make JSON string for the stream", 0U );
  UnityDefaultTestRun( test_cHTTPstreamLog, "Make log record sting for the stram", 0U );
  UnityDefaultTestRun( test_vHTTPCleanResponse, "Cleaning response data structure", 0U );
  UnityDefaultTestRun( test_uHTTPgetLine, "Parsing line from multiline text", 0U );
  /*
  UnityDefaultTestRun( test_vHTTPbuildPutResponse, "Make response for put (write) request", 0U );
  UnityDefaultTestRun( test_vHTTPbuildGetResponse, "Make response for get (read) request", 0U );
  UnityDefaultTestRun( test_vHTTPbuildResponse, "Make response for request", 0U );
  UnityDefaultTestRun( test_vHTTPmakeResponse, "Make response string", 0U );
  UnityDefaultTestRun( test_eHTTPparsingRequest, "Parsing request string", 0U );
  UnityDefaultTestRun( test_eHTTPbuildRequest, "Build request structure", 0U );
  UnityDefaultTestRun( test_vHTTPmakeRequest, "Make string request from request structure", 0U );
  UnityDefaultTestRun( test_eHTTPparsingResponse, "Parsing data from response text", 0U );
  */
  vTESTsendReport();
  return;
}

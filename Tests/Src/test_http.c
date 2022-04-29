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
  TEST_ASSERT_EQUAL_STRING( "Content-Type: \r\n", buffer );
  return;
}
void test_vHTTPaddContentEncoding ( void )
{
  buffer[0U] = 0U;
  vHTTPaddContentEncoding( buffer, HTTP_ENCODING_NO );
  TEST_ASSERT_EQUAL_STRING( "Content-Encoding: \r\n", buffer );
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
  TEST_ASSERT_EQUAL_STRING( "[{\"adr\":0,\"scale\":0,\"len\":3,\"value\":[1,0,0],\"min\":0,\"max\":0,\"units\":\"%20%20%20%20%20\",\"type\":U,\"bitMapSize\":0,\"bit\":[]},", restBuffer );
  TEST_ASSERT_EQUAL( STREAM_END, cHTTPstreamConfigs( &stream ) );
  TEST_ASSERT_EQUAL_STRING( "{\"adr\":1,\"scale\":0,\"len\":3,\"value\":[1,0,0],\"min\":0,\"max\":0,\"units\":\"%20%20%20%20%20\",\"type\":U,\"bitMapSize\":0,\"bit\":[]}]", restBuffer );
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
  TEST_ASSERT_EQUAL_STRING( "[{\"xType\":1,\"yType\":1,\"size\":2,\"dots\":[{\"x\":0,\"y\":0},{\"x\":1500,\"y\":2}]},",  restBuffer );
  TEST_ASSERT_EQUAL( STREAM_CONTINUES, cHTTPstreamCharts( &stream ) );
  TEST_ASSERT_EQUAL_STRING( "{\"xType\":1,\"yType\":2,\"size\":2,\"dots\":[{\"x\":0,\"y\":0},{\"x\":1500,\"y\":250}]},", restBuffer );
  TEST_ASSERT_EQUAL( STREAM_END, cHTTPstreamCharts( &stream ) );
  TEST_ASSERT_EQUAL_STRING( "{\"xType\":1,\"yType\":3,\"size\":2,\"dots\":[{\"x\":0,\"y\":0},{\"x\":1500,\"y\":100}]}]", restBuffer );
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
void test_cHTTPstreamString ( void )
{
  HTTP_STREAM stream = { 0U };
  TEST_ASSERT_EQUAL( STREAM_END, cHTTPstreamString( &stream ) );
  return;
}
void test_cHTTPstreamLog ( void )
{
  HTTP_STREAM stream = { 0U };
  stream.size  = 2U;
  stream.index = 0U;
  stream.start = 0U;
  TEST_ASSERT_EQUAL( STREAM_CONTINUES, cHTTPstreamLog( &stream ) );
  //TEST_ASSERT_EQUAL_STRING( "", restBuffer );
  TEST_ASSERT_EQUAL( STREAM_END, cHTTPstreamLog( &stream ) );
  //TEST_ASSERT_EQUAL_STRING( "", restBuffer );
  stream.start = LOG_SIZE;
  stream.index = 0U;
  stream.size  = 2U;
  TEST_ASSERT_EQUAL( STREAM_ERROR, cHTTPstreamLog( &stream ) );
  return;
}
void test_vHTTPCleanResponse ( void )
{
  return;
}
void test_uHTTPgetLine ( void )
{
  return;
}
void test_vHTTPbuildPutResponse ( void )
{
  return;
}
void test_vHTTPbuildGetResponse ( void )
{
  return;
}
void test_eHTTPparsingRequest ( void )
{
  return;
}
void test_vHTTPbuildResponse ( void )
{
  return;
}
void test_vHTTPmakeResponse ( void )
{
  return;
}
void test_eHTTPbuildRequest ( void )
{
  return;
}
void test_vHTTPmakeRequest ( void )
{
  return;
}
void test_eHTTPparsingResponse ( void )
{
  return;
}




void runTest_http ( void )
{
  UnitySetTestFile( "http.c" );
  UnityDefaultTestRun( test_vHTTPcleanRequest, "", 0U );
  UnityDefaultTestRun( test_vHTTPaddContentType, "", 0U );
  UnityDefaultTestRun( test_vHTTPaddContentEncoding, "", 0U );
  UnityDefaultTestRun( test_vHTTPaddCache, "", 0U );
  UnityDefaultTestRun( test_cHTTPstreamConfigs, "", 0U );
  UnityDefaultTestRun( test_cHTTPstreamCharts, "", 0U );
  UnityDefaultTestRun( test_cHTTPstreamTime, "", 0U );
  UnityDefaultTestRun( test_cHTTPstreamData, "", 0U );
  UnityDefaultTestRun( test_cHTTPstreamString, "", 0U );
  UnityDefaultTestRun( test_cHTTPstreamLog, "", 0U );
  UnityDefaultTestRun( test_vHTTPCleanResponse, "", 0U );
  UnityDefaultTestRun( test_uHTTPgetLine, "", 0U );
  UnityDefaultTestRun( test_vHTTPbuildPutResponse, "", 0U );
  UnityDefaultTestRun( test_vHTTPbuildGetResponse, "", 0U );
  UnityDefaultTestRun( test_eHTTPparsingRequest, "", 0U );
  UnityDefaultTestRun( test_vHTTPbuildResponse, "", 0U );
  UnityDefaultTestRun( test_vHTTPmakeResponse, "", 0U );
  UnityDefaultTestRun( test_eHTTPbuildRequest, "", 0U );
  UnityDefaultTestRun( test_vHTTPmakeRequest, "", 0U );
  UnityDefaultTestRun( test_eHTTPparsingResponse, "", 0U );
  vTESTsendReport();
  return;
}

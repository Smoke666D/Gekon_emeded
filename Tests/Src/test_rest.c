/*
 * test_rest.c
 *
 *  Created on: 26 апр. 2022 г.
 *      Author: 79110
 */

#include "tests.h"
#include "unity.h"
#include "rest.h"
#include "string.h"

static char buffer[REST_BUFFER_SIZE] = { 0U };
static char input[REST_BUFFER_SIZE] = { 0U };

void tets_uRESTmakeStartRecord ( void )
{
  TEST_ASSERT_EQUAL( 9U, uRESTmakeStartRecord( "header", buffer ) );
  TEST_ASSERT_EQUAL_STRING( "\"header\":", buffer );
  return;
}
void tets_uRESTmakeDigRecord ( void )
{
  TEST_ASSERT_EQUAL( 18U, uRESTmakeDigRecord( "record", 0x12345678U, REST_LAST_RECORD, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "\"record\":305419896", buffer );
  TEST_ASSERT_EQUAL( 9U, uRESTmakeDigRecord( "data", 0U, REST_CONT_RECORD, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "\"data\":0,", buffer );
  TEST_ASSERT_EQUAL( 9U, uRESTmakeDigRecord( "", 0U, REST_LAST_RECORD, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "\"\":0,", buffer );
  return;
}
void tets_uRESTmakeValueRecord ( void )
{
  uint16_t data[3U] = { 10U, 20U, 30U };
  TEST_ASSERT_EQUAL( 0U, uRESTmakeValueRecord( "data", data, 0U, ( uint16_t )( 'U' ), REST_LAST_RECORD, buffer ) );
  TEST_ASSERT_EQUAL( 9U, uRESTmakeValueRecord( "data", data, 1U, ( uint16_t )( 'U' ), REST_LAST_RECORD, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "\"data\":10", buffer );
  TEST_ASSERT_EQUAL( 10U, uRESTmakeValueRecord( "data", data, 1U, ( uint16_t )( 'U' ), REST_CONT_RECORD, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "\"data\":10,", buffer );
  TEST_ASSERT_EQUAL( 18U, uRESTmakeValueRecord( "data", data, 3U, ( uint16_t )( 'U' ), REST_CONT_RECORD, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "\"data\":[10,20,30],", buffer );
  TEST_ASSERT_EQUAL( 17U, uRESTmakeValueRecord( "data", data, 3U, ( uint16_t )( 'U' ), REST_LAST_RECORD, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "\"data\":[10,20,30]", buffer );
  data[0U] = ( uint16_t )( 'A' );
  data[0U] = ( uint16_t )( 'B' );
  data[0U] = ( uint16_t )( 'C' );
  TEST_ASSERT_EQUAL( 27U, uRESTmakeValueRecord( "data", data, 3U, ( uint16_t )( 'S' ), REST_CONT_RECORD, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "\"data\":[\"%41\",\"%42\",\"%43\"],", buffer );
  TEST_ASSERT_EQUAL( 26U, uRESTmakeValueRecord( "data", data, 3U, ( uint16_t )( 'S' ), REST_LAST_RECORD, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "\"data\":[\"%41\",\"%42\",\"%43\"]", buffer );
  return;
}
void tets_uRESTmake16FixDigRecord ( void )
{
  TEST_ASSERT_EQUAL( 12U, uRESTmake16FixDigRecord( "fix", fix16_to_float( 10.23f ), REST_CONT_RECORD, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "\"fix\":10.23,", buffer );
  TEST_ASSERT_EQUAL( 11U, uRESTmake16FixDigRecord( "fix", fix16_to_float( 10.23f ), REST_LAST_RECORD, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "\"fix\":10.23", buffer );
  return;
}
void tets_uRESTmakeSignedRecord ( void )
{
  TEST_ASSERT_EQUAL( 10U, uRESTmakeSignedRecord( "sig", -12, REST_CONT_RECORD, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "\"sig\":-10,", buffer );
  TEST_ASSERT_EQUAL( 9U, uRESTmakeSignedRecord( "sig", -12, REST_LAST_RECORD, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "\"sig\":-10", buffer );
  return;
}
void tets_uRESTmakeStrRecord ( void )
{
  char str[] = "Привет";
  TEST_ASSERT_EQUAL( 45U, uRESTmakeStrRecord( "str", ( uint16_t* )str, 6U, REST_CONT_RECORD, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "\"str\":\"%D0%9F%D1%80%D0%B8%D0%B2%D0%B5%D1%82\",", buffer );
  TEST_ASSERT_EQUAL( 44U, uRESTmakeStrRecord( "str", ( uint16_t* )str, 6U, REST_CONT_RECORD, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "\"str\":\"%D0%9F%D1%80%D0%B8%D0%B2%D0%B5%D1%82\"", buffer );
  return;
}
void tets_uRESTmakeSystemRecord ( void )
{
  SYSTEM_DATA data =
  {
    .name  = "name",
    .total = 100U,
    .usage = 56U
  };
  TEST_ASSERT_EQUAL( 31U, uRESTmakeSystemRecord( &data, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "\"name\":{\"total\":100,\"usage\":56}", buffer );
  return;
}
void tets_uRESTmakeBitMap ( void )
{
  eConfigBitMap bitMap =
  {
    .mask  = 100U,
    .shift = 12U
  };
  TEST_ASSERT_EQUAL( 23U, uRESTmakeBitMap( &bitMap, REST_CONT_RECORD, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "{\"mask\":100,\"shift\":12},", buffer );
  TEST_ASSERT_EQUAL( 22U, uRESTmakeBitMap( &bitMap, REST_LAST_RECORD, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "{\"mask\":100,\"shift\":12}", buffer );
  return;
}
void tets_uRESTmakeBitMapArray ( void )
{
  eConfigBitMap bitMap[2U] =
  {
    {
      .mask  = 100U,
      .shift = 12U
    },
    {
      .mask  = 200U,
      .shift = 67U
    }
  };
  TEST_ASSERT_EQUAL( 47U, uRESTmakeBitMapArray( 2U, bitMap, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "[{\"mask\":100,\"shift\":12},{\"mask\":200,\"shift\":67}]", buffer );
  return;
}
void tets_uRESTmakeDot ( void )
{
  eChartDot dot =
  {
    .x = fix16_from_int( 0U ),
    .y = fix16_from_int( 0U )
  };
  TEST_ASSERT_EQUAL( 12U, uRESTmakeDot( &dot, REST_LAST_RECORD, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "{\"x\":0,\"y\":0}", buffer );
  TEST_ASSERT_EQUAL( 13U, uRESTmakeDot( &dot, REST_CONT_RECORD, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "{\"x\":0,\"y\":0},", buffer );
  return;
}
void tets_uRESTmakeDotArray ( void )
{
  eChartDot dots[2U] = { 0U };
  dots[0U].x = fix16_from_int( 0U );
  dots[0U].y = fix16_from_int( 0U );
  dots[1U].x = fix16_from_int( 10U );
  dots[1U].y = fix16_from_int( 20U );
  TEST_ASSERT_EQUAL( 29U, uRESTmakeDotArray( dots, 2U, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "[{\"x\":0,\"y\":0},{\"x\":10,\"y\":20}]", buffer );
  return;
}
void tets_eRESTparsingRecord ( void )
{
  ( void )strcpy( input, "\"data\":{\"field\":12345}" );
  TEST_ASSERT_EQUAL( REST_OK, eRESTparsingRecord( input, "data", buffer ) );
  TEST_ASSERT_EQUAL_STRING( "\"field\":12345", buffer );
  ( void )strcpy( input, "\"data\":{}" );
  TEST_ASSERT_EQUAL( REST_RECORD_NO_DATA_ERROR, eRESTparsingRecord( input, "data", buffer ) );
  ( void )strcpy( input, "\"data\":{\"field\":12345" );
  TEST_ASSERT_EQUAL( REST_RECORD_FORMAT_ERROR, eRESTparsingRecord( input, "data", buffer ) );
  ( void )strcpy( input, "\"error\":{\"field\":12345}" );
  TEST_ASSERT_EQUAL( REST_RECORD_LOST_ERROR, eRESTparsingRecord( input, "data", buffer ) );
  return;
}
void tets_eRESTparsingDig16Record ( void )
{
  uint16_t data = 0U;
  ( void )strcpy( input, "{\"field\":12345}" );
  TEST_ASSERT_EQUAL( REST_OK, eRESTparsingDig16Record( input, "field", &data ) );
  TEST_ASSERT_EQUAL( 12345U, data );
  ( void )strcpy( input, "{\"field\":1.82}" );
  TEST_ASSERT_EQUAL( REST_OK, eRESTparsingDig16Record( input, "field", &data ) );
  TEST_ASSERT_EQUAL( 2U, data );
  return;
}
void tets_uRESTparsing16FixDigRecord ( void )
{
  fix16_t data = 0U;
  ( void )strcpy( input, "\"field\":123" );
  TEST_ASSERT_EQUAL( REST_OK, uRESTparsing16FixDigRecord( input, "field", &data ) );
  TEST_ASSERT_EQUAL( ( uint32_t )( fix16_from_int( 123U ) ), data );
  return;
}
void tets_eRESTparsingDig8Record ( void )
{
  uint8_t data = 0U;
  ( void )strcpy( input, "\"field\":123" );
  TEST_ASSERT_EQUAL( REST_OK, eRESTparsingDig8Record( input, "field", &data ) );
  TEST_ASSERT_EQUAL( 123U, data );
  return;
}
void tets_eRESTparsingSignedRecord ( void )
{
  int8_t data = 0;
  ( void )strcpy( input, "\"field\":-10" );
  TEST_ASSERT_EQUAL( REST_OK, eRESTparsingSignedRecord( input, "field", &data ) );
  TEST_ASSERT_EQUAL( -10, data );
  return;
}
void tets_eRESTparsingStrRecord ( void )
{
  uint16_t data[10U] = { 0U };
  uint8_t  length    = 10U;
  ( void )strcpy( input, "\"field\":\"%D0%BF%D1%80%D0%B8%D0%B2%D0%B5%D1%82%D0%94%D0%B0%D1%88%D0%B0\"" );
  TEST_ASSERT_EQUAL( REST_OK, eRESTparsingStrRecord( input, "field", data, length ) );
  TEST_ASSERT_EQUAL_STRING( "приветДаша", data );
  return;
}
void tets_eRESTparsingValueRecord ( void )
{
  uint16_t data[3U] = { 0U };
  ( void )strcpy( input, "\"data\":123" );
  TEST_ASSERT_EQUAL( REST_OK, eRESTparsingValueRecord( input, "data", ( uint16_t )'U', data, 1U ) );
  TEST_ASSERT_EQUAL( 123U, data[0U] );
  ( void )strcpy( input, "\"data\":[\"%D0%BF\",\"%D1%80\",\"%D0%B8\"]" );
  TEST_ASSERT_EQUAL( REST_OK, eRESTparsingValueRecord( input, "data", ( uint16_t )'C', data, 3U ) );
  TEST_ASSERT_EQUAL( ( uint16_t )( 'П' ), data[0U] );
  TEST_ASSERT_EQUAL( ( uint16_t )( 'р' ), data[1U] );
  TEST_ASSERT_EQUAL( ( uint16_t )( 'и' ), data[2U] );
  ( void )strcpy( input, "\"data\":[123,44,67]" );
  TEST_ASSERT_EQUAL( REST_OK, eRESTparsingValueRecord( input, "data", ( uint16_t )'U', data, 3U ) );
  TEST_ASSERT_EQUAL( 123U, data[0U] );
  TEST_ASSERT_EQUAL( 44U,  data[1U] );
  TEST_ASSERT_EQUAL( 67U,  data[2U] );
  return;
}
void tets_eRESTparsingBitMapArray ( void )
{
  eConfigBitMap bitMap[2U] = { 0U };
  ( void )strcpy( input, "\"data\":[{\"mask\":100,\"shift\":12},{\"mask\":200,\"shift\":67}]" );
  TEST_ASSERT_EQUAL( REST_OK, eRESTparsingBitMapArray( input, "data", bitMap, 2U ) );
  TEST_ASSERT_EQUAL( 100U, bitMap[0U].mask );
  TEST_ASSERT_EQUAL( 12U, bitMap[0U].shift );
  TEST_ASSERT_EQUAL( 200U, bitMap[1U].mask );
  TEST_ASSERT_EQUAL( 67U, bitMap[1U].shift );
  ( void )strcpy( input, "\"data\":[{\"mask\":100,\"shift\":12}]" );
  TEST_ASSERT_EQUAL( REST_RECORD_ARRAY_FORMAT_ERROR, eRESTparsingBitMapArray( input, "data", bitMap, 2U ) );
  ( void )strcpy( input, "\"data\":[{\"mask\":100,\"shift\":12},{\"mask\":200,\"shift\":67}" );
  TEST_ASSERT_EQUAL( REST_RECORD_ARRAY_FORMAT_ERROR, eRESTparsingBitMapArray( input, "data", bitMap, 2U ) );
  ( void )strcpy( input, "\"error\":[{\"mask\":100,\"shift\":12},{\"mask\":200,\"shift\":67}" );
  TEST_ASSERT_EQUAL( REST_RECORD_LOST_ERROR, eRESTparsingBitMapArray( input, "data", bitMap, 2U ) );
  return;
}
void tets_eRESTparsingDotArray ( void )
{
  eChartDot dots[2U] = { 0U };
  ( void )strcpy( input, "\"data\":[{\"x\":0,\"y\":0},{\"x\":10,\"y\":20}]" );
  TEST_ASSERT_EQUAL( REST_OK, eRESTparsingDotArray( input, "data", dots, 2U ) );
  TEST_ASSERT_EQUAL(  0U, dots[0U].x );
  TEST_ASSERT_EQUAL(  0U, dots[0U].y );
  TEST_ASSERT_EQUAL( 10U, dots[1U].x );
  TEST_ASSERT_EQUAL( 20U, dots[1U].y );
  ( void )strcpy( input, "\"data\":[{\"x\":0,\"y\":0}]" );
  TEST_ASSERT_EQUAL( REST_RECORD_ARRAY_FORMAT_ERROR, eRESTparsingDotArray( input, "data", dots, 2U ) );
  ( void )strcpy( input, "\"data\":[{\"x\":0,\"y\":0},{\"x\":10,\"y\":20}" );
  TEST_ASSERT_EQUAL( REST_RECORD_ARRAY_FORMAT_ERROR, eRESTparsingDotArray( input, "data", dots, 2U ) );
  ( void )strcpy( input, "\"error\":[{\"x\":0,\"y\":0},{\"x\":10,\"y\":20}]" );
    TEST_ASSERT_EQUAL( REST_RECORD_LOST_ERROR, eRESTparsingDotArray( input, "data", dots, 2U ) );
  return;
}
void tets_uRESTmakeConfig ( void )
{
  uint16_t      value[2U]  = { 15U, 18U };
  eConfigBitMap bitMap[3U] =
  {
    {.mask = 100U, .shift = 11U },
    {.mask = 200U, .shift = 22U },
    {.mask = 300U, .shift = 33U }
  };
  eConfigAttributes atrib =
  {
    .adr        = 10U,
    .scale      = 1,
    .len        = 2U,
    .bitMapSize = 0U,
    .bitMap     = bitMap,
    .min        = 0U,
    .max        = 400U,
    .type       = ( uint16_t )( 'U' ),
    .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U, 0x0020U }
  };
  eConfigReg reg =
  {
    .atrib = &atrib,
    .value = value
  };
  TEST_ASSERT_EQUAL( 0U, uRESTmakeConfig( &reg, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "{\"adr\":10,\"scale\":1,\"len\":2,\"value\":[15,18],\"min\":0,\"max\":400,\"units\":\"%D1%81%D0%B5%d0%BA%20%20\",\"type\":U,\"bitMapSize\":0,\"bit\":[]}", buffer );
  atrib.len        = 1U;
  atrib.bitMapSize = 3U;
  TEST_ASSERT_EQUAL( 0U, uRESTmakeConfig( &reg, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "{\"adr\":10,\"scale\":1,\"len\":1,\"value\":15,\"min\":0,\"max\":400,\"units\":\"%D1%81%D0%B5%d0%BA%20%20\",\"type\":U,\"bitMapSize\":0,\"bit\":[{\"mask\":100,\"shift\":11},{\"mask\":200,\"shift\":22},{\"mask\":300,\"shift\":33}]}", buffer );
  return;
}
void tets_uRESTmakeShortCongig ( void )
{
  uint16_t      value[2U]  = { 15U, 18U };
  eConfigBitMap bitMap[1U] = { {.mask = 100U, .shift = 11U } };
  eConfigAttributes atrib =
  {
    .adr        = 10U,
    .scale      = 1,
    .len        = 2U,
    .bitMapSize = 0U,
    .bitMap     = bitMap,
    .min        = 0U,
    .max        = 400U,
    .type       = ( uint16_t )( 'U' ),
    .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U, 0x0020U }
  };
  eConfigReg reg =
  {
    .atrib = &atrib,
    .value = value
  };
  TEST_ASSERT_EQUAL( 25U, uRESTmakeShortCongig( &reg, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "{\"adr\":10,\"value\":[15,18]}", buffer );
  atrib.len = 1U;
  TEST_ASSERT_EQUAL( 20U, uRESTmakeShortCongig( &reg, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "{\"adr\":10,\"value\":15}", buffer );
  return;
}
void tets_uRESTmakeChart ( void )
{
  eChartData chart = { 0U };
  chart.size  = 2U;
  chart.xType = X_AXIS_TYPE_CURRENT;
  chart.yType = Y_AXIS_TYPE_FUEL;
  chart.x.min = fix16_from_int( 0U  );
  chart.x.max = fix16_from_int( 10U );
  chart.y.min = fix16_from_int( 0U  );
  chart.y.max = fix16_from_int( 20U );
  chart.dots[0U].x = 0U;
  chart.dots[0U].y = 0U;
  chart.dots[1U].x = 9U;
  chart.dots[1U].y = 15U;
  TEST_ASSERT_EQUAL( 68U, uRESTmakeChart( &chart, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "{\"xType\":1,\"yType\":2,\"size\":2,\"dots\":[{\"x\":0,\"y\":0},{\"x\":9,\"y\":15}]}", buffer );
  return;
}
void tets_uRESTmakeTime ( void )
{
  RTC_TIME time =
  {
    .year  = 22U,
    .wday  = THURSDAY,
    .month = MONTH_FEB,
    .day   = 24U,
    .hour  = 5U,
    .min   = 14U,
    .sec   = 36U
  };
  TEST_ASSERT_EQUAL( 68U, uRESTmakeTime( &time, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "{\"hour\":5,\"min\":14,\"sec\":36,\"year\":22,\"month\":2,\"day\":24,\"wday\":5}", buffer );
  return;
}
void tets_uRESTmakeMemorySize ( void )
{
  TEST_ASSERT_EQUAL( 2U, uRESTmakeMemorySize( buffer ) );
  TEST_ASSERT_EQUAL_STRING( "{}", buffer );
  return;
}
void tets_uRESTmakeMeasurementLength ( void )
{
  //TEST_ASSERT_EQUAL( 0U, uRESTmakeMeasurementLength() );
  return;
}
void tets_uRESTmakeSystem ( void )
{
  TEST_ASSERT_EQUAL( 0U, uRESTmakeSystem( buffer ) );
  //TEST_ASSERT_EQUAL_STRING( "{}", buffer );
  return;
}
void tets_uRESTmakeData ( void )
{
  TEST_ASSERT_EQUAL( 0U, uRESTmakeData( 16U, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "{\"value\":16}", buffer );
  return;
}
void tets_uRESTmakeLog ( void )
{
  LOG_RECORD_TYPE record = { 0U };
  record.event.action = ACTION_EMERGENCY_STOP;
  record.event.type   = EVENT_OVER_POWER;
  record.time         = 0x12345678;
  TEST_ASSERT_EQUAL( 37U, uRESTmakeLog( &record, buffer ) );
  TEST_ASSERT_EQUAL_STRING( "{\"time\":305419896,\"type\":23,\"action\":2}", buffer );
  return;
}
void tets_uRESTmakeOutput ( void )
{
  //eConfigReg data      = { 0U };
  //char       name[10U] = { 0U };
  //TEST_ASSERT_EQUAL( 0U, uRESTmakeOutput( &data, name, buffer ) );
  //TEST_ASSERT_EQUAL_STRING( "{\"adr\":}", buffer );
  return;
}
void tets_uRESTmakeMeasurement ( void )
{
  TEST_ASSERT_EQUAL( 0U, uRESTmakeMeasurement( 0U, buffer ) );
  return;
}
void tets_eRESTgetRequest ( void )
{
  //REST_REQUEST request = { 0U };
  //uint16_t     adr     = 0U;
  //TEST_ASSERT_EQUAL( REST_NO_ADR, eRESTgetRequest() );
  return;
}
void tets_eRESTparsingConfig ( void )
{
  ( void )strcpy( buffer, "{\"value\":800}" );
  TEST_ASSERT_EQUAL( REST_OK, eRESTparsingConfig( buffer, RECORD_INTERVAL_ADR ) );
  TEST_ASSERT_EQUAL( 800U, configReg[RECORD_INTERVAL_ADR]->value[0U] );
  ( void )strcpy( buffer, "\"value\":800" );
  TEST_ASSERT_EQUAL( REST_RECORD_FORMAT_ERROR, eRESTparsingConfig( buffer, RECORD_INTERVAL_ADR ) );
  ( void )strcpy( buffer, "{\"error\":800}" );
  TEST_ASSERT_EQUAL( REST_RECORD_LOST_ERROR, eRESTparsingConfig( buffer, RECORD_INTERVAL_ADR ) );
  ( void )strcpy( buffer, "" );
  TEST_ASSERT_EQUAL( REST_RECORD_LOST_ERROR, eRESTparsingConfig( buffer, RECORD_INTERVAL_ADR ) );
  ( void )strcpy( buffer, "{\"value\":}" );
  TEST_ASSERT_EQUAL( REST_RECORD_NO_DATA_ERROR, eRESTparsingConfig( buffer, RECORD_INTERVAL_ADR ) );
  return;
}
void tets_eRESTparsingShortConfig ( void )
{
  ( void )strcpy( buffer, "{\"adr\":143,\"value\":700}" );
  TEST_ASSERT_EQUAL( REST_OK, eRESTparsingShortConfig( buffer ) );
  TEST_ASSERT_EQUAL( 700U, configReg[RECORD_INTERVAL_ADR]->value[0U] );
  ( void )strcpy( buffer, "{\"error\":143,\"value\":700}" );
  TEST_ASSERT_EQUAL( REST_RECORD_LOST_ERROR, eRESTparsingShortConfig( buffer ) );
  ( void )strcpy( buffer, "{\"adr\":143,\"error\":700}" );
  TEST_ASSERT_EQUAL( REST_RECORD_LOST_ERROR, eRESTparsingShortConfig( buffer ) );
  ( void )strcpy( buffer, "{\"adr\":,\"error\":700}" );
  TEST_ASSERT_EQUAL( REST_RECORD_NO_DATA_ERROR, eRESTparsingShortConfig( buffer ) );
  ( void )strcpy( buffer, "{\"adr\":143,\"value\":}" );
  TEST_ASSERT_EQUAL( REST_RECORD_NO_DATA_ERROR, eRESTparsingShortConfig( buffer ) );
  ( void )strcpy( buffer, "{\"adr\"143,\"value\":700}" );
  TEST_ASSERT_EQUAL( REST_RECORD_FORMAT_ERROR, eRESTparsingShortConfig( buffer ) );
  ( void )strcpy( buffer, "{\"adr\":143,\"value\"700}" );
  TEST_ASSERT_EQUAL( REST_RECORD_FORMAT_ERROR, eRESTparsingShortConfig( buffer ) );
  ( void )strcpy( buffer, "{\"adr\":143,\"value:700}" );
  TEST_ASSERT_EQUAL( REST_RECORD_FORMAT_ERROR, eRESTparsingShortConfig( buffer ) );
  ( void )strcpy( buffer, "{\"adr\":143\"value\":700}" );
  TEST_ASSERT_EQUAL( REST_RECORD_FORMAT_ERROR, eRESTparsingShortConfig( buffer ) );
  ( void )strcpy( buffer, "\"adr\":143,\"value\":700}" );
  TEST_ASSERT_EQUAL( REST_RECORD_FORMAT_ERROR, eRESTparsingShortConfig( buffer ) );
  ( void )strcpy( buffer, "{\"adr\":143,\"value\":700" );
  TEST_ASSERT_EQUAL( REST_RECORD_FORMAT_ERROR, eRESTparsingShortConfig( buffer ) );
  return;
}
void tets_eRESTparsingChart ( void )
{
  ( void )strcpy( buffer, "{\"size\":2,\"dots\":[{\"x\":0,\"y\":0},{\"x\":15,\"y\":20}]}" );
  TEST_ASSERT_EQUAL( REST_OK, eRESTparsingChart( buffer, 0U ) );
  TEST_ASSERT_EQUAL( 2U,  charts[0U]->size );
  TEST_ASSERT_EQUAL( 0U,  charts[0U]->dots[0U].x );
  TEST_ASSERT_EQUAL( 0U,  charts[0U]->dots[0U].y );
  TEST_ASSERT_EQUAL( 15U, charts[0U]->dots[1U].x );
  TEST_ASSERT_EQUAL( 20U, charts[0U]->dots[1U].y );
  ( void )strcpy( buffer, "{\"size\":3,\"dots\":[{\"x\":0,\"y\":0},{\"x\":15,\"y\":20}]}" );
  TEST_ASSERT_EQUAL( REST_RECORD_ARRAY_SIZE_ERROR, eRESTparsingChart( buffer, 0U ) );
  ( void )strcpy( buffer, "{\"size\":1,\"dots\":[{\"x\":0,\"y\":0},{\"x\":15,\"y\":20}]}" );
  TEST_ASSERT_EQUAL( REST_RECORD_ARRAY_SIZE_ERROR, eRESTparsingChart( buffer, 0U ) );
  ( void )strcpy( buffer, "{\"size\":2,\"dots\":[{\"x\":0,\"y\":0}]}" );
  TEST_ASSERT_EQUAL( REST_RECORD_ARRAY_SIZE_ERROR, eRESTparsingChart( buffer, 0U ) );
  ( void )strcpy( buffer, "\"size\":2,\"dots\":[{\"x\":0,\"y\":0},{\"x\":15,\"y\":20}]}" );
  TEST_ASSERT_EQUAL( REST_RECORD_FORMAT_ERROR, eRESTparsingChart( buffer, 0U ) );
  ( void )strcpy( buffer, "{\"error\":2,\"dots\":[{\"x\":0,\"y\":0},{\"x\":15,\"y\":20}]}" );
  TEST_ASSERT_EQUAL( REST_RECORD_LOST_ERROR, eRESTparsingChart( buffer, 0U ) );
  ( void )strcpy( buffer, "{\"size\":2,\"error\":[{\"x\":0,\"y\":0},{\"x\":15,\"y\":20}]}" );
  TEST_ASSERT_EQUAL( REST_RECORD_LOST_ERROR, eRESTparsingChart( buffer, 0U ) );
  ( void )strcpy( buffer, "{\"size\":2,\"dots\":[{\"x\":0,\"y\":0},{\"x\":15,\"y\":20}}" );
  TEST_ASSERT_EQUAL( REST_RECORD_ARRAY_FORMAT_ERROR, eRESTparsingChart( buffer, 0U ) );
  ( void )strcpy( buffer, "{\"size\":2,\"dots\":[{\"x\":0,\"y\":0}{\"x\":15,\"y\":20}]}" );
  TEST_ASSERT_EQUAL( REST_RECORD_ARRAY_FORMAT_ERROR, eRESTparsingChart( buffer, 0U ) );
  return;
}
void tets_eRESTparsingTime ( void )
{
  RTC_TIME time = { 0U };
  ( void )strcpy( buffer, "{\"hour\":5,\"min\":14,\"sec\":36,\"year\":22,\"month\":2,\"day\":24,\"wday\":5}" );
  TEST_ASSERT_EQUAL( REST_OK, eRESTparsingTime( buffer, &time ) );
  TEST_ASSERT_EQUAL( 22U, time.year );
  TEST_ASSERT_EQUAL( THURSDAY, time.wday );
  TEST_ASSERT_EQUAL( MONTH_FEB, time.month );
  TEST_ASSERT_EQUAL( 24U, time.day );
  TEST_ASSERT_EQUAL( 5U, time.hour );
  TEST_ASSERT_EQUAL( 14U, time.min );
  TEST_ASSERT_EQUAL( 36U, time.sec );
  ( void )strcpy( buffer, "{\"hour\":5,\"min\":14,\"sec\":36,\"year\":22,\"day\":24,\"wday\":5}" );
  TEST_ASSERT_EQUAL( REST_RECORD_LOST_ERROR, eRESTparsingTime( buffer, &time ) );
  ( void )strcpy( buffer, "{\"hour\":5,\"min\":14,\"sec\":36,\"year\":22,\"month\":2,\"day\":24,\"wday\":5" );
  TEST_ASSERT_EQUAL( REST_RECORD_FORMAT_ERROR, eRESTparsingTime( buffer, &time ) );
  ( void )strcpy( buffer, "{\"hour\":5,\"min\":14,\"sec\":36,\"error\":22,\"month\":2,\"day\":24,\"wday\":5}" );
  TEST_ASSERT_EQUAL( REST_RECORD_LOST_ERROR, eRESTparsingTime( buffer, &time ) );
  return;
}
void tets_eRESTparsingFreeData ( void )
{
  uint16_t data = 0U;
  ( void )strcpy( buffer, "{\"value\":12}" );
  TEST_ASSERT_EQUAL( REST_OK, eRESTparsingFreeData( buffer, &data ) );
  TEST_ASSERT_EQUAL( 12U, data );
  ( void )strcpy( buffer, "{\"error\":12}" );
  TEST_ASSERT_EQUAL( REST_RECORD_LOST_ERROR, eRESTparsingFreeData( buffer, &data ) );
  ( void )strcpy( buffer, "{\"value\":12" );
  TEST_ASSERT_EQUAL( REST_RECORD_FORMAT_ERROR, eRESTparsingFreeData( buffer, &data ) );
  ( void )strcpy( buffer, "{value\":12}" );
  TEST_ASSERT_EQUAL( REST_RECORD_FORMAT_ERROR, eRESTparsingFreeData( buffer, &data ) );
  ( void )strcpy( buffer, "{\"value\":}" );
  TEST_ASSERT_EQUAL( REST_RECORD_NO_DATA_ERROR, eRESTparsingFreeData( buffer, &data ) );
  return;
}
void tets_eRESTparsingPassword ( void )
{
  PASSWORD_TYPE password = { 0U };
  ( void )strcpy( buffer, "{\"status\":1,\"data\":4660}" );
  TEST_ASSERT_EQUAL( REST_OK, eRESTparsingPassword( buffer, &password ) );
  TEST_ASSERT_EQUAL( 0x1234U, password.data );
  TEST_ASSERT_EQUAL( PASSWORD_SET, password.status );
  ( void )strcpy( buffer, "{\"status\":127,\"data\":4660}" );
  TEST_ASSERT_EQUAL( REST_OK, eRESTparsingPassword( buffer, &password ) );
  TEST_ASSERT_EQUAL( 0x1234U, password.data );
  TEST_ASSERT_EQUAL( PASSWORD_SET, password.status );
  ( void )strcpy( buffer, "{\"status\":0,\"data\":0}" );
  TEST_ASSERT_EQUAL( REST_OK, eRESTparsingPassword( buffer, &password ) );
  TEST_ASSERT_EQUAL( 0U, password.data );
  TEST_ASSERT_EQUAL( PASSWORD_RESET, password.status );
  ( void )strcpy( buffer, "{\"status\":1,\"data\":4660" );
  TEST_ASSERT_EQUAL( REST_RECORD_FORMAT_ERROR, eRESTparsingPassword( buffer, &password ) );
  ( void )strcpy( buffer, "{status\":1,\"data\":4660}" );
  TEST_ASSERT_EQUAL( REST_RECORD_FORMAT_ERROR, eRESTparsingPassword( buffer, &password ) );
  ( void )strcpy( buffer, "\"status\":1,\"data\":4660}" );
  TEST_ASSERT_EQUAL( REST_RECORD_FORMAT_ERROR, eRESTparsingPassword( buffer, &password ) );
  ( void )strcpy( buffer, "{\"status\":1,\"data\":}" );
  TEST_ASSERT_EQUAL( REST_RECORD_NO_DATA_ERROR, eRESTparsingPassword( buffer, &password ) );
  ( void )strcpy( buffer, "{\"status\":,\"data\":4660}" );
  TEST_ASSERT_EQUAL( REST_RECORD_NO_DATA_ERROR, eRESTparsingPassword( buffer, &password ) );
  ( void )strcpy( buffer, "{\"status\":1\"data\":4660}" );
  TEST_ASSERT_EQUAL( REST_RECORD_FORMAT_ERROR, eRESTparsingPassword( buffer, &password ) );
  ( void )strcpy( buffer, "{\"status\":1,\"error\":4660}" );
  TEST_ASSERT_EQUAL( REST_RECORD_LOST_ERROR, eRESTparsingPassword( buffer, &password ) );
  ( void )strcpy( buffer, "{\"error\":1,\"data\":4660}" );
  TEST_ASSERT_EQUAL( REST_RECORD_LOST_ERROR, eRESTparsingPassword( buffer, &password ) );
  return;
}
void tets_eRESTparsingAuth ( void )
{
  PASSWORD_TYPE password = { 0U };
  ( void )strcpy( buffer, "{\"data\":4660}" );
  TEST_ASSERT_EQUAL( REST_OK, eRESTparsingAuth( buffer, &password ) );
  TEST_ASSERT_EQUAL( 0x1234U, password.data );
  ( void )strcpy( buffer, "\"data\":4660" );
  TEST_ASSERT_EQUAL( REST_MESSAGE_FORMAT_ERROR, eRESTparsingAuth( buffer, &password ) );
  ( void )strcpy( buffer, "{\"error\":4660}" );
  TEST_ASSERT_EQUAL( REST_RECORD_LOST_ERROR, eRESTparsingAuth( buffer, &password ) );
  ( void )strcpy( buffer, "{\"data\":}" );
  TEST_ASSERT_EQUAL( REST_RECORD_NO_DATA_ERROR, eRESTparsingAuth( buffer, &password ) );
  return;
}
void runTest_rest ( void )
{
  UnitySetTestFile( "test_rest.c" );
  UnityDefaultTestRun( tets_uRESTmakeStartRecord, "Make start of record", 0U );
  UnityDefaultTestRun( tets_uRESTmakeDigRecord, "Make digital record", 0U );
  UnityDefaultTestRun( tets_uRESTmakeValueRecord, "Make value record", 0U );
  UnityDefaultTestRun( tets_uRESTmake16FixDigRecord, "Make fix16 record", 0U );
  UnityDefaultTestRun( tets_uRESTmakeSignedRecord, "Make signed data record", 0U );
  UnityDefaultTestRun( tets_uRESTmakeStrRecord, "Make string record", 0U );
  UnityDefaultTestRun( tets_uRESTmakeSystemRecord, "Make system record", 0U );
  UnityDefaultTestRun( tets_uRESTmakeBitMap, "Make bitmap record", 0U );
  UnityDefaultTestRun( tets_uRESTmakeBitMapArray, "Make bitmap array record", 0U );
  UnityDefaultTestRun( tets_uRESTmakeDot, "Make chart dot record", 0U );
  UnityDefaultTestRun( tets_uRESTmakeDotArray, "Make chart dots array record", 0U );
  UnityDefaultTestRun( tets_eRESTparsingRecord, "Parsing record", 0U );
  UnityDefaultTestRun( tets_eRESTparsingDig16Record, "Parsing uint16 record", 0U );
  UnityDefaultTestRun( tets_uRESTparsing16FixDigRecord, "Parsing fix16 record", 0U );
  UnityDefaultTestRun( tets_eRESTparsingDig8Record, "Parsing uint8 record", 0U );
  UnityDefaultTestRun( tets_eRESTparsingSignedRecord, "Parsing signed data record", 0U );
  UnityDefaultTestRun( tets_eRESTparsingStrRecord, "Parsing string record", 0U );
  UnityDefaultTestRun( tets_eRESTparsingValueRecord, "Parsing value record", 0U );
  UnityDefaultTestRun( tets_eRESTparsingBitMapArray, "Parsing bitmap array record", 0U );
  UnityDefaultTestRun( tets_eRESTparsingDotArray, "Parsing chart dots array record", 0U );
  UnityDefaultTestRun( tets_uRESTmakeConfig, "Make config record", 0U );
  UnityDefaultTestRun( tets_uRESTmakeShortCongig, "Make short config record", 0U );
  UnityDefaultTestRun( tets_uRESTmakeChart, "Make chart record", 0U );
  UnityDefaultTestRun( tets_uRESTmakeTime, "Make time record", 0U );
  UnityDefaultTestRun( tets_uRESTmakeMemorySize, "Make memory size record", 0U );
  //UnityDefaultTestRun( tets_uRESTmakeMeasurementLength, "Make measurement length record", 0U );
  UnityDefaultTestRun( tets_uRESTmakeSystem, "???", 0U );
  UnityDefaultTestRun( tets_uRESTmakeData, "Make free data record", 0U );
  UnityDefaultTestRun( tets_uRESTmakeLog, "Make log record ", 0U );
  UnityDefaultTestRun( tets_uRESTmakeOutput, "Make output data record", 0U );
  //UnityDefaultTestRun( tets_uRESTmakeMeasurement, "Make measurement record", 0U );
  //UnityDefaultTestRun( tets_eRESTgetRequest, "", 0U );
  UnityDefaultTestRun( tets_eRESTparsingConfig, "Parsing config record", 0U );
  UnityDefaultTestRun( tets_eRESTparsingShortConfig, "Parsing short config record", 0U );
  UnityDefaultTestRun( tets_eRESTparsingChart, "Parsing chart record", 0U );
  UnityDefaultTestRun( tets_eRESTparsingTime, "Parsing time record", 0U );
  UnityDefaultTestRun( tets_eRESTparsingFreeData, "Parsing free data record", 0U );
  UnityDefaultTestRun( tets_eRESTparsingPassword, "Parsing password record", 0U );
  UnityDefaultTestRun( tets_eRESTparsingAuth, "Parsing authentication record", 0U );
  return;
}

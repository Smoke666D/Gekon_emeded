/*
 * testSysTest.c
 *
 *  Created on: 24 мар. 2022 г.
 *      Author: 79110
 */
#include "test.h"
#include "unity.h"
#include "RTC.h"
#include "journal.h"

void setUp ( void )
{
  return;
}

void tearDown ( void )
{
  return;
}

void test_eTESTparseString ( void )
{
  TEST_TYPE message = { 0U };
  eTESTparseString( "", &message );
  TEST_ASSERT_EQUAL( TEST_COMMAND_NO, message.cmd );
  TEST_ASSERT_EQUAL( TEST_TARGET_NO,  message.target );
  TEST_ASSERT_EQUAL( 0U, message.dataFlag );
  eTESTparseString( "get din 0", &message );
  TEST_ASSERT_EQUAL( TEST_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( TEST_TARGET_DIN,  message.target );
  TEST_ASSERT_GREATER_THAN_INT8( 0U,   message.dataFlag );
  TEST_ASSERT_EQUAL( 0U, message.data  );
  eTESTparseString( "get dout 2", &message );
  TEST_ASSERT_EQUAL( TEST_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( TEST_TARGET_DOUT, message.target );
  TEST_ASSERT_GREATER_THAN_INT8( 0U,   message.dataFlag );
  TEST_ASSERT_EQUAL( 2U, message.data  );
  eTESTparseString( "get time", &message );
  TEST_ASSERT_EQUAL( TEST_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( TEST_TARGET_TIME, message.target );
  TEST_ASSERT_EQUAL( 0U, message.dataFlag );
  eTESTparseString( "set time 21.10.2022.11.45.54.", &message );
  TEST_ASSERT_EQUAL( TEST_COMMAND_SET, message.cmd );
  TEST_ASSERT_EQUAL( TEST_TARGET_TIME, message.target );
  TEST_ASSERT_GREATER_THAN_INT8( 0U, message.dataFlag );
  TEST_ASSERT_EQUAL_STRING( "21.10.2022.11.45.54.", message.out );
  eTESTparseString( "get oil", &message );
  TEST_ASSERT_EQUAL( TEST_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( TEST_TARGET_OIL, message.target );
  TEST_ASSERT_EQUAL( 0U, message.dataFlag );
  eTESTparseString( "get cool", &message );
  TEST_ASSERT_EQUAL( TEST_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( TEST_TARGET_COOL, message.target );
  TEST_ASSERT_EQUAL( 0U, message.dataFlag );
  eTESTparseString( "get fuel", &message );
  TEST_ASSERT_EQUAL( TEST_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( TEST_TARGET_FUEL, message.target );
  TEST_ASSERT_EQUAL( 0U, message.dataFlag );
  eTESTparseString( "get bat", &message );
  TEST_ASSERT_EQUAL( TEST_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( TEST_TARGET_BAT, message.target );
  TEST_ASSERT_EQUAL( 0U, message.dataFlag );
  eTESTparseString( "get charg", &message );
  TEST_ASSERT_EQUAL( TEST_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( TEST_TARGET_CHARG, message.target );
  TEST_ASSERT_EQUAL( 0U, message.dataFlag );
  eTESTparseString( "get gen 1", &message );
  TEST_ASSERT_EQUAL( TEST_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( TEST_TARGET_GENERATOR, message.target );
  TEST_ASSERT_GREATER_THAN_INT8( 0U, message.dataFlag );
  TEST_ASSERT_EQUAL( 1U, message.data );
  eTESTparseString( "get mains 2", &message );
  TEST_ASSERT_EQUAL( TEST_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( TEST_TARGET_MAINS, message.target );
  TEST_ASSERT_GREATER_THAN_INT8( 0U, message.dataFlag );
  TEST_ASSERT_EQUAL( 2U, message.data );
  eTESTparseString( "get cur 3", &message );
  TEST_ASSERT_EQUAL( TEST_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( TEST_TARGET_CURRENT, message.target );
  TEST_ASSERT_GREATER_THAN_INT8( 3U, message.dataFlag );
  TEST_ASSERT_EQUAL( 1U, message.data );
  eTESTparseString( "get freq 0", &message );
  TEST_ASSERT_EQUAL( TEST_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( TEST_TARGET_FREQ, message.target );
  TEST_ASSERT_GREATER_THAN_INT8( 0U, message.dataFlag );
  TEST_ASSERT_EQUAL( 0U, message.data );
  eTESTparseString( "get speed", &message );
  TEST_ASSERT_EQUAL( TEST_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( TEST_TARGET_SPEED, message.target );
  TEST_ASSERT_EQUAL( 0U, message.dataFlag );
  eTESTparseString( "get sw 3", &message );
  TEST_ASSERT_EQUAL( TEST_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( TEST_TARGET_SW, message.target );
  TEST_ASSERT_GREATER_THAN_INT8( 0U, message.dataFlag );
  TEST_ASSERT_EQUAL( 3U, message.data );
  eTESTparseString( "set led 1", &message );
  TEST_ASSERT_EQUAL( TEST_COMMAND_SET, message.cmd );
  TEST_ASSERT_EQUAL( TEST_TARGET_LED, message.target );
  TEST_ASSERT_GREATER_THAN_INT8( 0U, message.dataFlag );
  TEST_ASSERT_EQUAL( 1U, message.data );
  eTESTparseString( "reset led 3", &message );
  TEST_ASSERT_EQUAL( TEST_COMMAND_RESET, message.cmd );
  TEST_ASSERT_EQUAL( TEST_TARGET_LED, message.target );
  TEST_ASSERT_GREATER_THAN_INT8( 0U, message.dataFlag );
  TEST_ASSERT_EQUAL( 3U, message.data );
  eTESTparseString( "get storage", &message );
  TEST_ASSERT_EQUAL( TEST_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( TEST_TARGET_STORAGE, message.target );
  TEST_ASSERT_EQUAL( 0U, message.dataFlag );
  return;
}

void test_vTESTtimeToStr ( void )
{
  RTC_TIME time        = { 0U };
  char     buffer[21U] = { 0U };
  time.day   = 21;
  time.month = 10;
  time.year  = 2022 - LOG_START_YEAR;
  time.hour  = 11;
  time.min   = 45;
  time.sec   = 54;
  uTESTtimeToStr( &time, buffer );
  TEST_ASSERT_EQUAL_STRING( "21.10.2022.11.45.54.", buffer );
  return;
}

void test_uTESTparseTimeFild ( void )
{
  char    buffer[21U] = "21.10.2022.11.45.54.";
  char*   pStr        = buffer;
  uint8_t data        = 0U;
  uint8_t res         = 0U;
  res = uTESTparseTimeFild( pStr, &data );
  TEST_ASSERT_GREATER_THAN_INT8( 0U, res );
  TEST_ASSERT_EQUAL( 21U, data );
  res = uTESTparseTimeFild( pStr, &data );
  TEST_ASSERT_GREATER_THAN_INT8( 0U, res );
  TEST_ASSERT_EQUAL( 10U, data );
  res = uTESTparseTimeFild( pStr, &data );
  TEST_ASSERT_GREATER_THAN_INT8( 0U, res );
  TEST_ASSERT_EQUAL( ( 2022U - LOG_START_YEAR ), data );
  res = uTESTparseTimeFild( pStr, &data );
  TEST_ASSERT_GREATER_THAN_INT8( 0U, res );
  TEST_ASSERT_EQUAL( 11U, data );
  res = uTESTparseTimeFild( pStr, &data );
  TEST_ASSERT_GREATER_THAN_INT8( 0U, res );
  TEST_ASSERT_EQUAL( 45U, data );
  res = uTESTparseTimeFild( pStr, &data );
  TEST_ASSERT_GREATER_THAN_INT8( 0U, res );
  TEST_ASSERT_EQUAL( 54U, data );
  return;
}

void test_vTESTstrToTime ( void )
{
  TEST_STATUS res         = TEST_STATUS_OK;
  RTC_TIME    time        = { 0U };
  char        buffer[21U] = "21.10.2022.11.45.54.";
  res = vTESTstrToTime( &time, buffer );
  TEST_ASSERT_EQUAL( TEST_STATUS_OK, res );
  TEST_ASSERT_EQUAL( 21U, time.day );
  TEST_ASSERT_EQUAL( 10U, time.month );
  TEST_ASSERT_EQUAL( ( 2022U - LOG_START_YEAR ), time.year );
  TEST_ASSERT_EQUAL( 11U, time.hour );
  TEST_ASSERT_EQUAL( 45U, time.min );
  TEST_ASSERT_EQUAL( 54U, time.sec );
  return;
}

void test_vTESTstatusToString ( void )
{
  char buffer[21U] = { 0U };
  uTESTstatusToString( TEST_STATUS_OK, buffer );
  TEST_ASSERT_EQUAL_STRING( "Ok", buffer );
  uTESTstatusToString( TEST_STATUS_ERROR_COMMAND, buffer );
  TEST_ASSERT_EQUAL_STRING( "Wrong command", buffer );
  uTESTstatusToString( TEST_STATUS_ERROR_TARGET, buffer );
  TEST_ASSERT_EQUAL_STRING( "Wrong target", buffer );
  uTESTstatusToString( TEST_STATUS_ERROR_DATA, buffer );
  TEST_ASSERT_EQUAL_STRING( "Wrong data", buffer );
  uTESTstatusToString( TEST_STATUS_ERROR_EXECUTING, buffer );
  TEST_ASSERT_EQUAL_STRING( "Executing error", buffer );
  uTESTstatusToString( 0xFFU, buffer );
  TEST_ASSERT_EQUAL_STRING( "Unknown error", buffer );
  return;
}

void test_vTESTdioToStr ( void )
{
  char buffer[21U] = { 0U };
  uTESTdioToStr( 0U, buffer );
  TEST_ASSERT_EQUAL_STRING( "off", buffer );
  uTESTdioToStr( 1U, buffer );
  TEST_ASSERT_EQUAL_STRING( "on", buffer );
  return;
}

void test_vTESTprocess ( void )
{
  return;
}

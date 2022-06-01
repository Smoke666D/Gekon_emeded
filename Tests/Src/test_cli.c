/*
 * testSysTest.c
 *
 *  Created on: 24 мар. 2022 г.
 *      Author: 79110
 */
#include "tests.h"
#include "cli.h"
#include "unity.h"
#include "RTC.h"
#include "journal.h"

void test_vCLIparseString ( void )
{
  TEST_TYPE message = { 0U };
  vCLIparseString( "", &message );
  TEST_ASSERT_EQUAL( CLI_COMMAND_NO, message.cmd );
  TEST_ASSERT_EQUAL( CLI_TARGET_NO,  message.target );
  TEST_ASSERT_EQUAL( 0U, message.dataFlag );
  vCLIparseString( "get din 0", &message );
  TEST_ASSERT_EQUAL( CLI_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( CLI_TARGET_DIN,  message.target );
  TEST_ASSERT_GREATER_THAN_INT8( 0U,   message.dataFlag );
  TEST_ASSERT_EQUAL( 0U, message.data  );
  vCLIparseString( "get dout 2", &message );
  TEST_ASSERT_EQUAL( CLI_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( CLI_TARGET_DOUT, message.target );
  TEST_ASSERT_GREATER_THAN_INT8( 0U,   message.dataFlag );
  TEST_ASSERT_EQUAL( 2U, message.data  );
  vCLIparseString( "get time", &message );
  TEST_ASSERT_EQUAL( CLI_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( CLI_TARGET_TIME, message.target );
  TEST_ASSERT_EQUAL( 0U, message.dataFlag );
  vCLIparseString( "set time 21.10.2022.11.45.54.", &message );
  TEST_ASSERT_EQUAL( CLI_COMMAND_SET, message.cmd );
  TEST_ASSERT_EQUAL( CLI_TARGET_TIME, message.target );
  TEST_ASSERT_GREATER_THAN_INT8( 0U, message.dataFlag );
  TEST_ASSERT_EQUAL_STRING( "21.10.2022.11.45.54.", message.out );
  vCLIparseString( "get oil", &message );
  TEST_ASSERT_EQUAL( CLI_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( CLI_TARGET_OIL, message.target );
  TEST_ASSERT_EQUAL( 0U, message.dataFlag );
  vCLIparseString( "get cool", &message );
  TEST_ASSERT_EQUAL( CLI_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( CLI_TARGET_COOL, message.target );
  TEST_ASSERT_EQUAL( 0U, message.dataFlag );
  vCLIparseString( "get fuel", &message );
  TEST_ASSERT_EQUAL( CLI_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( CLI_TARGET_FUEL, message.target );
  TEST_ASSERT_EQUAL( 0U, message.dataFlag );
  vCLIparseString( "get bat", &message );
  TEST_ASSERT_EQUAL( CLI_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( CLI_TARGET_BAT, message.target );
  TEST_ASSERT_EQUAL( 0U, message.dataFlag );
  vCLIparseString( "get charg", &message );
  TEST_ASSERT_EQUAL( CLI_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( CLI_TARGET_CHARG, message.target );
  TEST_ASSERT_EQUAL( 0U, message.dataFlag );
  vCLIparseString( "get gen 1", &message );
  TEST_ASSERT_EQUAL( CLI_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( CLI_TARGET_GENERATOR, message.target );
  TEST_ASSERT_GREATER_THAN_INT8( 0U, message.dataFlag );
  TEST_ASSERT_EQUAL( 1U, message.data );
  vCLIparseString( "get mains 2", &message );
  TEST_ASSERT_EQUAL( CLI_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( CLI_TARGET_MAINS, message.target );
  TEST_ASSERT_GREATER_THAN_INT8( 0U, message.dataFlag );
  TEST_ASSERT_EQUAL( 2U, message.data );
  vCLIparseString( "get cur 3", &message );
  TEST_ASSERT_EQUAL( CLI_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( CLI_TARGET_CURRENT, message.target );
  TEST_ASSERT_GREATER_THAN_INT8( 0U, message.dataFlag );
  TEST_ASSERT_EQUAL( 3U, message.data );
  vCLIparseString( "get freq 0", &message );
  TEST_ASSERT_EQUAL( CLI_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( CLI_TARGET_FREQ, message.target );
  TEST_ASSERT_GREATER_THAN_INT8( 0U, message.dataFlag );
  TEST_ASSERT_EQUAL( 0U, message.data );
  vCLIparseString( "get speed", &message );
  TEST_ASSERT_EQUAL( CLI_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( CLI_TARGET_SPEED, message.target );
  TEST_ASSERT_EQUAL( 0U, message.dataFlag );
  vCLIparseString( "get sw 3", &message );
  TEST_ASSERT_EQUAL( CLI_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( CLI_TARGET_SW, message.target );
  TEST_ASSERT_GREATER_THAN_INT8( 0U, message.dataFlag );
  TEST_ASSERT_EQUAL( 3U, message.data );
  vCLIparseString( "set led 1", &message );
  TEST_ASSERT_EQUAL( CLI_COMMAND_SET, message.cmd );
  TEST_ASSERT_EQUAL( CLI_TARGET_LED, message.target );
  TEST_ASSERT_GREATER_THAN_INT8( 0U, message.dataFlag );
  TEST_ASSERT_EQUAL( 1U, message.data );
  vCLIparseString( "reset led 3", &message );
  TEST_ASSERT_EQUAL( CLI_COMMAND_RESET, message.cmd );
  TEST_ASSERT_EQUAL( CLI_TARGET_LED, message.target );
  TEST_ASSERT_GREATER_THAN_INT8( 0U, message.dataFlag );
  TEST_ASSERT_EQUAL( 3U, message.data );
  vCLIparseString( "get storage", &message );
  TEST_ASSERT_EQUAL( CLI_COMMAND_GET, message.cmd );
  TEST_ASSERT_EQUAL( CLI_TARGET_STORAGE, message.target );
  TEST_ASSERT_EQUAL( 0U, message.dataFlag );
  return;
}

void test_uCLItimeToStr ( void )
{
  RTC_TIME time        = { 0U };
  char     buffer[21U] = { 0U };
  uint8_t  res         = 0U;
  time.day   = 21;
  time.month = 10;
  time.year  = 2022 - LOG_START_YEAR;
  time.hour  = 11;
  time.min   = 45;
  time.sec   = 54;
  res = uCLItimeToStr( &time, buffer );
  TEST_ASSERT_EQUAL( 19U, res );
  TEST_ASSERT_EQUAL_STRING( "21.10.22.11.45.54.\n", buffer );
  return;
}

void test_cCLIparseTimeFild ( void )
{
  char    buffer[21U] = "21.10.22.11.45.54.\n";
  char*   pStr        = buffer;
  uint8_t data        = 0U;
  pStr = cCLIparseTimeFild( pStr, &data );
  TEST_ASSERT_EQUAL( 21U, data );
  pStr = cCLIparseTimeFild( pStr, &data );
  TEST_ASSERT_EQUAL( 10U, data );
  pStr = cCLIparseTimeFild( pStr, &data );
  TEST_ASSERT_EQUAL( 22U, data );
  pStr = cCLIparseTimeFild( pStr, &data );
  TEST_ASSERT_EQUAL( 11U, data );
  pStr = cCLIparseTimeFild( pStr, &data );
  TEST_ASSERT_EQUAL( 45U, data );
  pStr = cCLIparseTimeFild( pStr, &data );
  TEST_ASSERT_EQUAL( 54U, data );
  return;
}

void test_eCLIstrToTime ( void )
{
  CLI_STATUS res         = CLI_STATUS_OK;
  RTC_TIME   time        = { 0U };
  char       buffer[21U] = "21.10.22.11.45.54.";
  res = eCLIstrToTime( &time, buffer );
  TEST_ASSERT_EQUAL( CLI_STATUS_OK, res );
  TEST_ASSERT_EQUAL( 21U, time.day );
  TEST_ASSERT_EQUAL( 10U, time.month );
  TEST_ASSERT_EQUAL( 22U, time.year );
  TEST_ASSERT_EQUAL( 11U, time.hour );
  TEST_ASSERT_EQUAL( 45U, time.min );
  TEST_ASSERT_EQUAL( 54U, time.sec );
  return;
}

void test_uCLIstatusToString ( void )
{
  char    buffer[21U] = { 0U };
  uint8_t res         = 0U;
  res = uCLIstatusToString( CLI_STATUS_OK, buffer );
  TEST_ASSERT_EQUAL( 3U, res );
  TEST_ASSERT_EQUAL_STRING( "Ok\n", buffer );
  res = uCLIstatusToString( CLI_STATUS_ERROR_COMMAND, buffer );
  TEST_ASSERT_EQUAL( 14U, res );
  TEST_ASSERT_EQUAL_STRING( "Wrong command\n", buffer );
  res = uCLIstatusToString( CLI_STATUS_ERROR_TARGET, buffer );
  TEST_ASSERT_EQUAL( 13U, res );
  TEST_ASSERT_EQUAL_STRING( "Wrong target\n", buffer );
  res = uCLIstatusToString( CLI_STATUS_ERROR_DATA, buffer );
  TEST_ASSERT_EQUAL( 11U, res );
  TEST_ASSERT_EQUAL_STRING( "Wrong data\n", buffer );
  res = uCLIstatusToString( CLI_STATUS_ERROR_EXECUTING, buffer );
  TEST_ASSERT_EQUAL( 16U, res );
  TEST_ASSERT_EQUAL_STRING( "Executing error\n", buffer );
  res = uCLIstatusToString( 0xFFU, buffer );
  TEST_ASSERT_EQUAL( 14U, res );
  TEST_ASSERT_EQUAL_STRING( "Unknown error\n", buffer );
  return;
}

void test_uCLIdioToStr ( void )
{
  char    buffer[21U] = { 0U };
  uint8_t res         = 0U;
  res = uCLIdioToStr( 0U, buffer );
  TEST_ASSERT_EQUAL( 4U, res );
  TEST_ASSERT_EQUAL_STRING( "off\n", buffer );
  res = uCLIdioToStr( 1U, buffer );
  TEST_ASSERT_EQUAL( 3U, res );
  TEST_ASSERT_EQUAL_STRING( "on\n", buffer );
  return;
}

void test_uCLIhexToStr ( void )
{
  uint8_t data[2U]   = { 0x22, 0xA5 };
  char    output[7U] = { 0U };
  uint8_t res = uCLIhexToStr( data, 2U, output );
  TEST_ASSERT_EQUAL( 6U, res );
  TEST_ASSERT_EQUAL_STRING( "22-a5\n", output );
  return;
}

void test_uCLIversionToStr ( void )
{
  uint16_t input[3U]   = { 10, 27, 245 };
  char     output[12U] = { 0U };
  uint8_t res = uCLIversionToStr( input, output );
  TEST_ASSERT_EQUAL( 10U, res );
  TEST_ASSERT_EQUAL_STRING( "10.27.245\n", output );
  return;
}

void test_uCLIparsingFields ( void )
{
  const char* input = "set mac bak lak\n";
  char*       output[CLI_FILDS_NUMBER] = { 0U };
  uint8_t res = uCLIparsingFields( input, output );
  TEST_ASSERT_EQUAL( 4U, res );
  TEST_ASSERT_EQUAL_STRING( "set mac bak lak\n", output[0U] );
  TEST_ASSERT_EQUAL_STRING( "mac bak lak\n", output[1U] );
  TEST_ASSERT_EQUAL_STRING( "bak lak\n", output[2U] );
  TEST_ASSERT_EQUAL_STRING( "lak\n", output[3U] );
  return;
}

void test_uCLIparse ( void )
{
  const char* input          = "foo";
  const char* dictionary[3U] = { "loo", "foo", "goo" };
  uint8_t res = uCLIparse( input, dictionary, 3U );
  TEST_ASSERT_EQUAL( 2U, res );
}

void runTest_cli ( void )
{
  UnitySetTestFile( "test_cli.c" );
  UnityDefaultTestRun( test_uCLIparsingFields, "Fields parsing", 240U );
  UnityDefaultTestRun( test_uCLIparse, "Field parsing", 241U );
  UnityDefaultTestRun( test_vCLIparseString, "String parsing", 242U );
  UnityDefaultTestRun( test_uCLItimeToStr, "Time to string", 243U );
  UnityDefaultTestRun( test_cCLIparseTimeFild, "Parsing time string", 244U );
  UnityDefaultTestRun( test_eCLIstrToTime, "String to time", 245U );
  UnityDefaultTestRun( test_uCLIstatusToString, "Status to string", 246U );
  UnityDefaultTestRun( test_uCLIdioToStr, "DIO to string", 247U );
  UnityDefaultTestRun( test_uCLIhexToStr, "HEX to string", 248U );
  UnityDefaultTestRun( test_uCLIversionToStr, "Version to string", 249U );
  vTESTsendReport();
  return;
}


/*
 * test_dataSD.c
 *
 *  Created on: 30 мая 2022 г.
 *      Author: 79110
 */
#include "tests.h"
#include "unity.h"
#include "fatsd.h"
#include "dataSD.h"
#include "config.h"
#include "rest.h"
#include "dataAPI.h"
#include "measurement.h"

#define TEST_SD_FILE_PROCESS_TIMEOUT  1000U
#define TEST_SD_FILE_PROCESS_DELAY    10U
#define TEST_SD_FILE_START_DELAY      5000U

void test_cSDcheckConfigCallback ( void )
{
  char* res = NULL;
  uint32_t length = uRESTmakeShortCongig( &displayBrightnesLevel, cFATSDgetBuffer() );
  configCheker = 0U;
  res = cSDcheckConfigCallback( length );
  TEST_ASSERT_GREATER_THAN( 0U, configCheker );
  TEST_ASSERT_EQUAL_PTR( cFATSDgetBuffer(), res );

  configCheker = 0U;
  res = cSDcheckConfigCallback( 2U );
  TEST_ASSERT_EQUAL( 0U, configCheker );
  return;
}
void test_cSDreadConfigCallback ( void )
{
  char* res = NULL;
  configCheker = 1U;
  uint32_t length = uRESTmakeShortCongig( &displayBrightnesLevel, cFATSDgetBuffer() );
  res = cSDreadConfigCallback( length );
  TEST_ASSERT_EQUAL_PTR( cFATSDgetBuffer(), res );
  TEST_ASSERT_GREATER_THAN( 0U, configCheker );
  return;
}
void test_uSDcopyDigsToLine ( void )
{
  uint32_t res = 0U;
  uint16_t data[5U] = { 19U, 20U, 33U, 47U, 511U };
  char     output[30U] = { 0U };
  res = uSDcopyDigsToLine( data, 5U, output, ' ', '\n' );
  TEST_ASSERT_EQUAL_STRING( "19 20 33 47 511\n", output );
  TEST_ASSERT_EQUAL_UINT32( 16U, res );
  res = uSDcopyDigsToLine( data, 3U, output, '-', '!' );
  TEST_ASSERT_EQUAL_STRING( "19-20-33!", output );
  TEST_ASSERT_EQUAL_UINT32( 9U, res );
  return;
}
void test_uSDmakeMeasurement ( void )
{
  uint16_t data[10U] = { 0U };
  char     output[30U] = { 0U };
  uint32_t length = 0U;
  /*-----------------------------------------------------------*/
  data[0U] = MEASUREMENT_RECORD_TYPE_PREAMBOLA;
  data[1U] = 22;
  data[2U] = 5;
  data[3U] = 26;
  data[4U] = 14;
  data[5U] = 53;
  data[6U] = 33;
  data[7U] = 47;
  data[8U] = 48;
  data[9U] = 0;
  length = uSDmakeMeasurement( ( void* )( data ), 10U, output );
  TEST_ASSERT_EQUAL_STRING( "// 22.5.26.14.53.33 47 48 0\n", output );
  TEST_ASSERT_EQUAL_UINT32( 28U, length );
  /*-----------------------------------------------------------*/
  data[0U] = MEASUREMENT_RECORD_TYPE_RECORD;
  data[1U] = 53932U;
  data[2U] = 60780U;
  data[3U] = 0U;
  length = uSDmakeMeasurement( ( void* )( data ), 4U, output );
  TEST_ASSERT_EQUAL_STRING( "53932 60780 0\n", output );
  TEST_ASSERT_EQUAL_UINT32( 14U, length );
  /*-----------------------------------------------------------*/
  output[0U] = 0U;
  data[0U]   = 10U;
  length = uSDmakeMeasurement( ( void* )( data ), 5U, output );
  TEST_ASSERT_EQUAL_STRING( "", output );
  TEST_ASSERT_EQUAL_UINT32( 0U, length );
  /*-----------------------------------------------------------*/
  return;
}



void awaitSDstatus ( void )
{
  for ( uint32_t i=0; ( ( i<TEST_SD_FILE_PROCESS_TIMEOUT ) && ( eSDgetStatus() == FR_BUSY ) ); i++ )
  {
    osDelay( TEST_SD_FILE_PROCESS_DELAY );
  }
  return;
}

void test_SDconfigsWrite ( void )
{
  SD_ROUTINE routine = {
    .cmd    = SD_COMMAND_WRITE,
    .file   = FATSD_FILE_CONFIG,
    .data   = { 0U },
    .length = 0U
  };
  vSDsendRoutine( &routine );
  osDelay( TEST_SD_FILE_START_DELAY );
  awaitSDstatus();
  TEST_ASSERT_EQUAL( FR_OK, eSDgetStatus() );
  return;
}
void test_SDconfigsRead ( void )
{
  SD_ROUTINE routine = {
    .cmd    = SD_COMMAND_READ,
    .file   = FATSD_FILE_CONFIG,
    .data   = { 0U },
    .length = 0U
  };
  vSDsendRoutine( &routine );
  osDelay( TEST_SD_FILE_START_DELAY );
  awaitSDstatus();
  TEST_ASSERT_EQUAL( FR_OK, eSDgetStatus() );
  return;
}
void test_SDlog ( void )
{
  LOG_RECORD_TYPE record  = { 0U };
  SD_ROUTINE      routine = {
    .cmd    = SD_COMMAND_WRITE,
    .file   = FATSD_FILE_LOG,
    .data   = { 0U },
    .length = 0U
  };
  eDATAAPIlog( DATA_API_CMD_READ, 0U, &record );
  for ( uint8_t i=0U; i<LOG_RECORD_SIZE; i++ )
  {
    routine.data[i] = ( ( uint8_t* )&record )[i];
  }
  vSDsendRoutine( &routine );
  osDelay( TEST_SD_FILE_PROCESS_DELAY );
  awaitSDstatus();
  TEST_ASSERT_EQUAL( FR_OK, eSDgetStatus() );
  return;
}
void test_SDmeasurement ( void )
{
  SD_ROUTINE routine = {
    .cmd    = SD_COMMAND_WRITE,
    .file   = FATSD_FILE_MEASUREMENT,
    .data   = { 0U },
    .length = 0U
  };
  vMEASUREMENTmakeStartLine( &routine );
  osDelay( TEST_SD_FILE_PROCESS_DELAY );
  awaitSDstatus();
  TEST_ASSERT_EQUAL( FR_OK, eSDgetStatus() );
  return;
}




void runTest_dataSD ( void )
{
  UnitySetTestFile( "dataSD.c" );
  UnityDefaultTestRun( test_cSDcheckConfigCallback, "SD check configuration file", 0U );
  UnityDefaultTestRun( test_cSDreadConfigCallback, "SD reading configuration file", 0U );
  UnityDefaultTestRun( test_uSDcopyDigsToLine, "SD copy digital to the line", 0U );
  UnityDefaultTestRun( test_uSDmakeMeasurement, "SD make measurement record", 0U );
  if ( eFATSDgetStatus() == SD_STATUS_MOUNTED )
  {
    UnityDefaultTestRun( test_SDconfigsWrite, "", 0U );
    UnityDefaultTestRun( test_SDconfigsRead, "", 0U );
    UnityDefaultTestRun( test_SDlog, "", 0U );
    UnityDefaultTestRun( test_SDmeasurement, "", 0U );
  }
  return;
}

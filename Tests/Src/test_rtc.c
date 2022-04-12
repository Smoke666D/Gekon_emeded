/*
 * test_rtc.c
 *
 *  Created on: 11 апр. 2022 г.
 *      Author: 79110
 */
#include "test_rtc.h"
#include "RTC.h"
#include "unity.h"

void test_eRTCwrite ( void )
{
  uint8_t data = 10U;
  TEST_ASSERT_EQUAL( RTC_OK, eRTCwrite( RTC_SECONDS, &data, 1U ) );
  TEST_ASSERT_EQUAL( RTC_OK, eRTCread( RTC_SECONDS, &data, 1U ) );
  TEST_ASSERT_GREATER_OR_EQUAL( 10U, data );
  return;
}

void test_eRTCread ( void )
{
  uint8_t data = 0U;
  TEST_ASSERT_EQUAL( RTC_OK, eRTCread( RTC_SECONDS, &data, 1U ) );
  TEST_ASSERT_GREATER_THAN( 0U, data );
  return;
}

void test_bcdToDec ( void )
{
  TEST_ASSERT_EQUAL( 42U, bcdToDec( 0x42U ) );
  return;
}

void test_decToBcd ( void )
{
  TEST_ASSERT_EQUAL( 0x42U, decToBcd( 42U ) );
  return;
}

void test_eVerifiTime ( void )
{
  RTC_STATUS res  = RTC_OK;
  RTC_TIME   time = {
    .year  = 22U,
    .month = MONTH_JUL,
    .day   = 26U,
    .wday  = THURSDAY,
    .hour  = 13U,
    .min   = 24U,
    .sec   = 57U
  };
  res = eVerifiTime( &time );
  TEST_ASSERT_EQUAL( RTC_OK, res );
  time.sec = 65U;
  res = eVerifiTime( &time );
  vRTCcleanTime( &time );
  TEST_ASSERT_EQUAL( RTC_FORMAT_ERROR, res );
  time.min = 65U;
  res = eVerifiTime( &time );
  vRTCcleanTime( &time );
  TEST_ASSERT_EQUAL( RTC_FORMAT_ERROR, res );
  time.hour = 48U;
  res = eVerifiTime( &time );
  vRTCcleanTime( &time );
  TEST_ASSERT_EQUAL( RTC_FORMAT_ERROR, res );
  time.wday = 10U;
  res = eVerifiTime( &time );
  vRTCcleanTime( &time );
  TEST_ASSERT_EQUAL( RTC_FORMAT_ERROR, res );
  time.day = 42U;
  res = eVerifiTime( &time );
  vRTCcleanTime( &time );
  TEST_ASSERT_EQUAL( RTC_FORMAT_ERROR, res );
  time.month = 13U;
  res = eVerifiTime( &time );
  vRTCcleanTime( &time );
  TEST_ASSERT_EQUAL( RTC_FORMAT_ERROR, res );
  time.year = 100U;
  res = eVerifiTime( &time );
  vRTCcleanTime( &time );
  TEST_ASSERT_EQUAL( RTC_FORMAT_ERROR, res );
  return;
}

void test_vRTCcleanTime ( void )
{
  RTC_TIME time = {
    .year  = 22U,
    .month = MONTH_JUL,
    .day   = 26U,
    .wday  = THURSDAY,
    .hour  = 13U,
    .min   = 24U,
    .sec   = 57U
  };
  vRTCcleanTime( &time );
  TEST_ASSERT_EQUAL( 0U,        time.year  );
  TEST_ASSERT_EQUAL( MONTH_JAN, time.month );
  TEST_ASSERT_EQUAL( 1U,        time.day   );
  TEST_ASSERT_EQUAL( MONDAY,    time.wday  );
  TEST_ASSERT_EQUAL( 0U,        time.hour  );
  TEST_ASSERT_EQUAL( 0U,        time.min   );
  TEST_ASSERT_EQUAL( 0U,        time.sec   );
  return;
}

void test_eRTCgetTime ( void )
{
  RTC_TIME   time = { 0U };
  RTC_STATUS res  = eRTCgetTime( &time );
  TEST_ASSERT_EQUAL( RTC_OK, res );
  TEST_ASSERT_GREATER_THAN( 0U, time.sec );
  return;
}

void test_eRTCsetTime ( void )
{
  RTC_TIME   time = {
    .year  = 22U,
    .month = MONTH_JUL,
    .day   = 26U,
    .wday  = THURSDAY,
    .hour  = 13U,
    .min   = 24U,
    .sec   = 3U
  };
  RTC_STATUS res  = RTC_OK;
  res = eRTCsetTime( &time );
  TEST_ASSERT_EQUAL( RTC_OK, res );
  osDelay( 1000U );
  res = eRTCgetTime( &time );
  TEST_ASSERT_EQUAL( RTC_OK,    res        );
  TEST_ASSERT_EQUAL( 22U,       time.year  );
  TEST_ASSERT_EQUAL( MONTH_JUL, time.month );
  TEST_ASSERT_EQUAL( 26U,       time.day   );
  TEST_ASSERT_EQUAL( THURSDAY,  time.wday  );
  TEST_ASSERT_EQUAL( 13U,       time.hour  );
  TEST_ASSERT_EQUAL( 24U,       time.min   );
  TEST_ASSERT_GREATER_OR_EQUAL( 3U, time.sec );
  return;
}

void test_vRTCgetCashTime ( void )
{
  RTC_TIME time = { 0U };
  vRTCgetCashTime( &time );
  TEST_ASSERT_GREATER_THAN( 0U, time.sec );
  return;
}

void runTest_rtc ( void )
{
  UnitySetTestFile( "../Tests/test/test_rtc.c" );
  UnityDefaultTestRun( test_eRTCread, "Read I2C", 154U );
  UnityDefaultTestRun( test_eRTCwrite, "Write I2C", 155U );
  UnityDefaultTestRun( test_bcdToDec, "BCD to Dec", 156U );
  UnityDefaultTestRun( test_decToBcd, "Dec to BCD", 157U );
  UnityDefaultTestRun( test_vRTCcleanTime, "Clean time", 158U );
  UnityDefaultTestRun( test_eVerifiTime, "Time verification", 159U );
  UnityDefaultTestRun( test_eRTCgetTime, "Get time", 160U );
  UnityDefaultTestRun( test_eRTCsetTime, "Set time", 161U );
  UnityDefaultTestRun( test_vRTCgetCashTime, "Get cash time", 162U );
  return;
}



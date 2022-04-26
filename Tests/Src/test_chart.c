/*
 * test_chart.c
 *
 *  Created on: 26 апр. 2022 г.
 *      Author: 79110
 */

#include "tests.h"
#include "unity.h"
#include "chart.h"

void test_vCHARTupdateLimits ( void )
{
  eChartData chart = { 0U };
  uint8_t    i     = 0U;
  chart.size = 5U;
  for ( i=0U; i<chart.size; i++ )
  {
    chart.dots[i].x = ( fix16_t )( i );
    chart.dots[i].y = ( fix16_t )( i );
  }
  vCHARTupdateLimits( &chart );
  TEST_ASSERT_EQUAL( 0U, chart.x.min );
  TEST_ASSERT_EQUAL( ( chart.size - 1U ), ( uint32_t )chart.x.max );
  TEST_ASSERT_EQUAL( 0U, chart.y.min );
  TEST_ASSERT_EQUAL( ( chart.size - 1U ), ( uint32_t )chart.y.max );
  return;
}
void test_vCHARTcalcFunction ( void )
{
  eChartData     chart = { 0U };
  eChartFunction func  = { 0U };
  chart.size = 2U;
  chart.dots[0U].x = fix16_from_int( 0U );
  chart.dots[0U].y = fix16_from_int( 0U );
  chart.dots[1U].x = fix16_from_int( 10U );
  chart.dots[1U].y = fix16_from_int( 20U );
  vCHARTcalcFunction( ( const eChartData* )( &chart ), 0U, &func );
  TEST_ASSERT_EQUAL( fix16_from_int( 1U ),  func.k );
  TEST_ASSERT_EQUAL( fix16_from_int( 10U ), func.b );
  return;
}
void test_eCHARTfunc ( void )
{
  eChartData chart = { 0U };
  fix16_t    x     = 0U;
  fix16_t    y     = 0U;
  chart.size = 2U;
  chart.dots[0U].x = fix16_from_int( 0U );
  chart.dots[0U].y = fix16_from_int( 0U );
  chart.dots[1U].x = fix16_from_int( 10U );
  chart.dots[1U].y = fix16_from_int( 20U );
  vCHARTupdateLimits( &chart );
  x = fix16_from_int( 0U );
  TEST_ASSERT_EQUAL( FUNC_OK, eCHARTfunc( ( const eChartData* )( &chart ), x, &y ) );
  TEST_ASSERT_EQUAL( fix16_from_int( 10U ), y );
  x = fix16_from_int( 10U );
  TEST_ASSERT_EQUAL( FUNC_OK, eCHARTfunc( ( const eChartData* )( &chart ), x, &y ) );
  TEST_ASSERT_EQUAL( fix16_from_int( 20U ), y );
  x = fix16_from_int( 5U );
  TEST_ASSERT_EQUAL( FUNC_OK, eCHARTfunc( ( const eChartData* )( &chart ), x, &y ) );
  TEST_ASSERT_EQUAL( fix16_from_int( 15U ), y );
  x = fix16_add( chart.x.max, fix16_from_int( 5U ) );
  TEST_ASSERT_EQUAL( FUNC_OVER_MAX_X_ERROR, eCHARTfunc( ( const eChartData* )( &chart ), x, &y ) );
  x = fix16_sub( chart.x.min, fix16_from_int( 5U ) );
  TEST_ASSERT_EQUAL( FUNC_OVER_MIN_X_ERROR, eCHARTfunc( ( const eChartData* )( &chart ), x, &y ) );
  return;
}
void test_xCHARTgetSemophore ( void )
{
  TEST_ASSERT_NOT_EQUAL( NULL, xCHARTgetSemophore() );
  return;
}

void runTest_chart ( void )
{
  UnitySetTestFile( "test_chart.c" );
  UnityDefaultTestRun( test_vCHARTupdateLimits, "Update chart limits", 0U );
  UnityDefaultTestRun( test_vCHARTcalcFunction, "Calculate chart line function", 0U );
  UnityDefaultTestRun( test_eCHARTfunc, "Calculation chart output", 0U );
  UnityDefaultTestRun( test_xCHARTgetSemophore, "Get chart semaphore", 0U );
  return;
}

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
  chart.dots[0U].x = fix16_from_int( 5U );
  chart.dots[0U].y = fix16_from_int( 9U );
  chart.dots[1U].x = fix16_from_int( 13U );
  chart.dots[1U].y = fix16_from_int( 20U );
  vCHARTcalcFunction( ( const eChartData* )( &chart ), 0U, &func );
  TEST_ASSERT_EQUAL( fix16_from_float( 1.375f ), func.k );
  TEST_ASSERT_EQUAL( fix16_from_float( 2.125f ), func.b );
  return;
}
void test_eCHARTfunc ( void )
{
  eChartData chart = { 0U };
  fix16_t    x     = 0U;
  fix16_t    y     = 0U;
  chart.size = 2U;
  chart.dots[0U].x = fix16_from_int( 5U );
  chart.dots[0U].y = fix16_from_int( 9U );
  chart.dots[1U].x = fix16_from_int( 13U );
  chart.dots[1U].y = fix16_from_int( 20U );
  chart.x.max = xAxisAtribs[chart.xType]->max;
  chart.x.min = xAxisAtribs[chart.xType]->min;
  chart.y.max = yAxisAtribs[chart.yType]->max;
  chart.y.min = yAxisAtribs[chart.yType]->min;
  vCHARTupdateLimits( &chart );

  x = chart.dots[0U].x;
  TEST_ASSERT_EQUAL( FUNC_OK, eCHARTfunc( ( const eChartData* )( &chart ), x, &y ) );
  TEST_ASSERT_EQUAL( chart.dots[0U].y, y );

  x = chart.dots[1U].x;
  TEST_ASSERT_EQUAL( FUNC_OK, eCHARTfunc( ( const eChartData* )( &chart ), x, &y ) );
  TEST_ASSERT_EQUAL( chart.dots[1U].y, y );

  x = fix16_from_int( 8U );
  TEST_ASSERT_EQUAL( FUNC_OK, eCHARTfunc( ( const eChartData* )( &chart ), x, &y ) );
  TEST_ASSERT_EQUAL( fix16_from_float( 13.125f ), y );

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
  vTESTsendReport();
  return;
}

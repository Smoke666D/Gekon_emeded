/*
 * test_outputProcessing.c
 *
 *  Created on: 26 апр. 2022 г.
 *      Author: 79110
 */

#include "tests.h"
#include "unity.h"
#include "outputProcessing.h"

void test_uOUTPUTcodeEvent ( void )
{
  SYSTEM_EVENT event = { 0U };
  event.type   = 0x12U;
  event.action = 0x34U;
  TEST_ASSERT_EQUAL( 0x1234U, uOUTPUTcodeEvent( event ) );
  return;
}
void test_eOUTPUTgetType ( void )
{
  uint8_t i = 0U;
  for ( i=0U; i<( OUTPUT_DATA_REGISTER_NUMBER - 1U ); i++ )
  {
    TEST_ASSERT_EQUAL( OUTPUT_REGISTER_TYPE_READ_ONLY, eOUTPUTgetType( i ) );
  }
  TEST_ASSERT_EQUAL( OUTPUT_REGISTER_TYPE_READ_WRITE, eOUTPUTgetType( OUTPUT_DATA_REGISTER_NUMBER - 1U ) );
  return;
}
void test_vOUTPUTwrite ( void )
{
  uint8_t  i    = 0U;
  uint16_t data = 0U;
  uint16_t buf  = 0U;
  for ( i=0U; i<( OUTPUT_DATA_REGISTER_NUMBER - 1U ); i++ )
  {
    buf = uOUTPUTread( i );
    data = 0x1234U;
    TEST_ASSERT_EQUAL( OUTPUT_STATUS_OK, eOUTPUTwrite( i, data ) );
    data = 0U;
    data = uOUTPUTread( i );
    TEST_ASSERT_EQUAL( 0x1234U, data );
    TEST_ASSERT_EQUAL( OUTPUT_STATUS_OK, eOUTPUTwrite( i, buf ) );
  }
  TEST_ASSERT_EQUAL( OUTPUT_STATUS_ACCESS_DENIED, eOUTPUTwrite( ( OUTPUT_DATA_REGISTER_NUMBER - 1U ), 0U ) );
  TEST_ASSERT_EQUAL( OUTPUT_STATUS_ACCESS_DENIED, eOUTPUTwrite( OUTPUT_DATA_REGISTER_NUMBER, 0U ) );
  return;
}
void runTest_outputProcessing ( void )
{
  UnitySetTestFile( "test_outputProcessing.c" );
  UnityDefaultTestRun( test_uOUTPUTcodeEvent, "Coding event", 0U );
  UnityDefaultTestRun( test_eOUTPUTgetType, "Get output register type", 0U );
  UnityDefaultTestRun( test_vOUTPUTwrite, "Read and write output data", 0U );
  return;
}

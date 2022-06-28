/*
 * test_eeprom.c
 *
 *  Created on: 13 апр. 2022 г.
 *      Author: 79110
 */
#include "tests.h"
#include "unity.h"
#include "EEPROM.h"

void test_vEEPROMmakeAdr ( void )
{
  uint32_t input   = 0x12345678U;
  uint8_t  buf[4U] = { 0U };
  vEEPROMmakeAdr( input, buf, 4U );
  TEST_ASSERT_EQUAL( 0x12U, buf[0U] );
  TEST_ASSERT_EQUAL( 0x34U, buf[1U] );
  TEST_ASSERT_EQUAL( 0x56U, buf[2U] );
  TEST_ASSERT_EQUAL( 0x78U, buf[3U] );
  input = 0x123456;
  vEEPROMmakeAdr( input, buf, 3U );
  TEST_ASSERT_EQUAL( 0x12U, buf[0U] );
  TEST_ASSERT_EQUAL( 0x34U, buf[1U] );
  TEST_ASSERT_EQUAL( 0x56U, buf[2U] );
  input = 0x1234;
  vEEPROMmakeAdr( input, buf, 2U );
  TEST_ASSERT_EQUAL( 0x12U, buf[0U] );
  TEST_ASSERT_EQUAL( 0x34U, buf[1U] );
  input = 0x12;
  vEEPROMmakeAdr( input, buf, 1U );
  TEST_ASSERT_EQUAL( 0x12U, buf[0U] );
  return;
}
void runTest_eeprom ( void )
{
  UnitySetTestFile( "test_eeprom.c" );
  UnityDefaultTestRun( test_vEEPROMmakeAdr, "EEPROM address", 0U );
  vTESTsendReport();
  return;
}

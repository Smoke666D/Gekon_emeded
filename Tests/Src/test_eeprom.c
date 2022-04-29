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
  uint32_t input = 0x123456U;
  uint8_t  buf[3U] = { 0U };
  vEEPROMmakeAdr( input, buf );
  TEST_ASSERT_EQUAL( 0x12U, buf[0U] );
  TEST_ASSERT_EQUAL( 0x34U, buf[1U] );
  TEST_ASSERT_EQUAL( 0x56U, buf[2U] );
  return;
}
void runTest_eeprom ( void )
{
  UnitySetTestFile( "test_eeprom.c" );
  UnityDefaultTestRun( test_vEEPROMmakeAdr, "EEPROM address", 0U );
  vTESTsendReport();
  return;
}

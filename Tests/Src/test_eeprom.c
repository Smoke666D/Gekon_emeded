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
  TEST_ASSERT_EQUAL( 0x34U, buf[0U] );
  TEST_ASSERT_EQUAL( 0x56U, buf[0U] );
  return;
}
void test_eEEPROMwrite ( void )
{
  EEPROM_STATUS status = EEPROM_OK;
  status = eEEPROMwrite();
  TEST_ASSERT_EQUAL( EEPROM_OK, status );
  return;
}
void test_eEEPROMread ( void )
{
  EEPROM_STATUS status = EEPROM_OK;
  status = eEEPROMread();
  TEST_ASSERT_EQUAL( EEPROM_OK, status );
  return;
}
void test_eEEPROMwriteEnable ( void )
{
  EEPROM_STATUS status = EEPROM_OK;
  status = eEEPROMwriteEnable();
  TEST_ASSERT_EQUAL( EEPROM_OK, status );
  return;
}
void test_eEEPROMwriteDisable ( void )
{
  EEPROM_STATUS status = EEPROM_OK;
  status = eEEPROMwriteDisable();
  TEST_ASSERT_EQUAL( EEPROM_OK, status );
  return;
}
void test_eEEPROMreadSR ( void )
{
  EEPROM_STATUS status = EEPROM_OK;
  status = eEEPROMreadSR();
  TEST_ASSERT_EQUAL( EEPROM_OK, status );
  return;
}
void test_eEEPROMwriteSR ( void )
{
  EEPROM_STATUS status = EEPROM_OK;
  status = eEEPROMwriteSR();
  TEST_ASSERT_EQUAL( EEPROM_OK, status );
  return;
}
void test_eEEPROMunblock ( void )
{
  EEPROM_STATUS status = EEPROM_OK;
  status = eEEPROMunblock();
  TEST_ASSERT_EQUAL( EEPROM_OK, status );
  return;
}
void test_eEEPROMsetProtect ( void )
{
  EEPROM_STATUS status = EEPROM_OK;
  status = eEEPROMsetProtect();
  TEST_ASSERT_EQUAL( EEPROM_OK, status );
  return;
}
void test_eEEPROMgetProtect ( void )
{
  EEPROM_STATUS status = EEPROM_OK;
  status = eEEPROMgetProtect();
  TEST_ASSERT_EQUAL( EEPROM_OK, status );
  return;
}
void test_eEEPROMpoolUntil ( void )
{
  EEPROM_STATUS status = EEPROM_OK;
  status = eEEPROMpoolUntil();
  TEST_ASSERT_EQUAL( EEPROM_OK, status );
  return;
}
void test_eEEPROMWriteData ( void )
{
  EEPROM_STATUS status = EEPROM_OK;
  status = eEEPROMWriteData();
  TEST_ASSERT_EQUAL( EEPROM_OK, status );
  return;
}
void test_eEEPROMInit ( void )
{
  EEPROM_STATUS status = EEPROM_OK;
  status = eEEPROMInit();
  TEST_ASSERT_EQUAL( EEPROM_OK, status );
  return;
}
void test_eEEPROMreadMemory ( void )
{
  EEPROM_STATUS status = EEPROM_OK;
  status = eEEPROMreadMemory();
  TEST_ASSERT_EQUAL( EEPROM_OK, status );
  return;
}
void test_eEEPROMwriteMemory ( void )
{
  EEPROM_STATUS status = EEPROM_OK;
  status = eEEPROMwriteMemory();
  TEST_ASSERT_EQUAL( EEPROM_OK, status );
  return;
}
void runTest_eeprom ( void )
{
  if ( eEEPROMInit() == EEPROM_OK )
  {
    UnitySetTestFile( "../Tests/test/test_eeprom.c" );
    UnityDefaultTestRun( test_vEEPROMmakeAdr, "", 0U );
    UnityDefaultTestRun( test_eEEPROMwrite, "", 0U );
    UnityDefaultTestRun( test_eEEPROMread, "", 0U );
    UnityDefaultTestRun( test_eEEPROMwriteEnable, "", 0U );
    UnityDefaultTestRun( test_eEEPROMwriteDisable, "", 0U );
    UnityDefaultTestRun( test_eEEPROMreadSR, "", 0U );
    UnityDefaultTestRun( test_eEEPROMwriteSR, "", 0U );
    UnityDefaultTestRun( test_eEEPROMunblock, "", 0U );
    UnityDefaultTestRun( test_eEEPROMsetProtect, "", 0U );
    UnityDefaultTestRun( test_eEEPROMgetProtect, "", 0U );
    UnityDefaultTestRun( test_eEEPROMpoolUntil, "", 0U );
    UnityDefaultTestRun( test_eEEPROMWriteData, "", 0U );
    UnityDefaultTestRun( test_eEEPROMInit, "", 0U );
    UnityDefaultTestRun( test_eEEPROMreadMemory, "", 0U );
    UnityDefaultTestRun( test_eEEPROMwriteMemory, "", 0U );
  }
  else
  {

  }
  return;
}

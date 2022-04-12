/*
 * test_usbhid.c
 *
 *  Created on: 12 апр. 2022 г.
 *      Author: 79110
 */

#include "tests.h"
#include "unity.h"
#include "usbhid.h"
#include "rtc.h"
#include "config.h"

void test_vUint32ToBytes ( void )
{
  uint8_t output[4U] = { 0U };
  vUint32ToBytes( 0x11223344U, output );
  TEST_ASSERT_EQUAL( 0x44, output[0U] );
  TEST_ASSERT_EQUAL( 0x33, output[1U] );
  TEST_ASSERT_EQUAL( 0x22, output[2U] );
  TEST_ASSERT_EQUAL( 0x11, output[3U] );
  return;
}
void test_vUint24ToBytes ( void )
{
  uint8_t output[3U] = { 0U };
  vUint24ToBytes( 0x00112233U, output );
  TEST_ASSERT_EQUAL( 0x33, output[0U] );
  TEST_ASSERT_EQUAL( 0x22, output[1U] );
  TEST_ASSERT_EQUAL( 0x11, output[2U] );
  return;
}
void test_vUint16ToBytes ( void )
{
  uint8_t output[2U] = { 0U };
  vUint16ToBytes( 0x1122U, output );
  TEST_ASSERT_EQUAL( 0x22, output[0U] );
  TEST_ASSERT_EQUAL( 0x11, output[1U] );
  return;
}
void test_uBytesToUnit16 ( void )
{
  const uint8_t input[2U] = { 0x22U, 0x11U };
  TEST_ASSERT_EQUAL( 0x1122, uBytesToUnit16( input ) );
  return;
}
void test_uByteToUint24 ( void )
{
  const uint8_t input[3U] = { 0x33U, 0x22U, 0x11U };
  TEST_ASSERT_EQUAL( 0x00112233, uBytesToUnit24( input ) );
  return;
}
void test_uByteToUint32 ( void )
{
  const uint8_t input[4U] = { 0x44U, 0x33U, 0x22U, 0x11U };
  TEST_ASSERT_EQUAL( 0x11223344, uBytesToUnit32( input ) );
  return;
}
void test_vUSBtimeToReport ( void )
{
  USB_REPORT report = { 0U };
  RTC_TIME   time   = { 0U };
  ( void )eRTCgetTime( &time );
  vUSBtimeToReport( &report );
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_OK, report.stat );
  TEST_ASSERT_EQUAL( sizeof( RTC_TIME ), report.length );
  TEST_ASSERT_EQUAL( time.hour,  report.data[0U] );
  TEST_ASSERT_EQUAL( time.min,   report.data[1U] );
  TEST_ASSERT_EQUAL( time.year,  report.data[3U] );
  TEST_ASSERT_EQUAL( time.month, report.data[4U] );
  TEST_ASSERT_EQUAL( time.day,   report.data[5U] );
  TEST_ASSERT_EQUAL( time.wday,  report.data[6U] );
  TEST_ASSERT_GREATER_OR_EQUAL( time.sec, report.data[2U] );
  return;
}
void test_vUSBfreeDataToReport ( void )
{
  //USB_REPORT report = { 0U };
  //report.adr =
  //vUSBfreeDataToReport( &report );
  //TEST_ASSERT_EQUAL( USB_REPORT_STATE_OK, report.stat );

  return;
}
void test_vUSBlogToReport ( void )
{
  USB_REPORT report = { 0U };
  vUSBlogToReport( &report );
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_OK, report.stat );
  TEST_ASSERT_EQUAL( 6U, report.length );
  TEST_ASSERT_GREATER_OR_EQUAL( 0U, report.data[0U] );
  TEST_ASSERT_GREATER_OR_EQUAL( 0U, report.data[1U] );
  TEST_ASSERT_GREATER_OR_EQUAL( 0U, report.data[2U] );
  TEST_ASSERT_GREATER_OR_EQUAL( 0U, report.data[3U] );
  TEST_ASSERT_GREATER_OR_EQUAL( 0U, report.data[4U] );
  TEST_ASSERT_GREATER_OR_EQUAL( 0U, report.data[5U] );
  report.adr = 0xFFFFU;
  vUSBlogToReport( &report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_ADR, report.stat );
  TEST_ASSERT_EQUAL( 0U, report.length );
  return;
}
void test_vUSBconfigToReport ( void )
{
  USB_REPORT report = { 0U };
  uint8_t    i      = 0U;
  vUSBconfigToReport( &report );
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_OK, report.stat );
  TEST_ASSERT_EQUAL( ( configReg[report.adr].atrib->len * 2U ), report.length );
  for ( i=0U; i<report.length; i++ )
  {
    TEST_ASSERT_GREATER_OR_EQUAL( 0U, report.data[i] );
  }
  report.adr = 0xFFFFU;
  vUSBconfigToReport( &report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_ADR, report.stat );
  TEST_ASSERT_EQUAL( 0U, report.length );
  return;
}
void test_vUSBchartToReport ( void )
{
  USB_REPORT report = { 0U };
  uint8_t    i      = 0U;
  vUSBchartToReport( &report );
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_OK, report.stat );
  TEST_ASSERT_EQUAL( 2U, report.length );
  TEST_ASSERT_GREATER_OR_EQUAL( 0U, report.data[0U] );
  TEST_ASSERT_GREATER_OR_EQUAL( 0U, report.data[1U] );
  report.adr = 1U;
  vUSBchartToReport( &report );
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_OK, report.stat );
  TEST_ASSERT_EQUAL( 8U, report.length );
  for ( i=0U; i<report.length; i++ )
  {
    TEST_ASSERT_GREATER_OR_EQUAL( 0U, report.data[i] );
  }
  report.cmd = 0xFFU;
  vUSBchartToReport( &report );
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_BAD_REQ, report.stat );
  TEST_ASSERT_EQUAL( 0U, report.length );
  report.cmd = USB_REPORT_CMD_GET_CHART_OIL;
  report.adr = 0xFFFFU;
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_BAD_REQ, report.stat );
  TEST_ASSERT_EQUAL( 0U, report.length );
  return;
}
void test_eUSBreportToTime ( void )
{
  const USB_REPORT report = { 0U };
  USB_STATUS       status = USB_STATUS_DONE;
  RTC_TIME         time   = { 0U };
  report.length   = sizeof( RTC_TIME );
  report.data[0U] = 11U;
  report.data[1U] = 22U;
  report.data[2U] = 33U;
  report.data[3U] = 44U;
  report.data[4U] = ( uint8_t )MONTH_MAY;
  report.data[5U] = 24U;
  report.data[6U] = ( uint8_t )THURSDAY;
  status = eUSBreportToTime( &report );
  TEST_ASSERT_EQUAL( RTC_OK, eRTCgetTime( &time ) );
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_OK, status );
  TEST_ASSERT_EQUAL( 11U, time.hour );
  TEST_ASSERT_EQUAL( 22U, time.min );
  TEST_ASSERT_GREATER_OR_EQUAL( 33U, time.sec );
  TEST_ASSERT_EQUAL( 44U, time.year );
  TEST_ASSERT_EQUAL( MONTH_MAY, time.month );
  TEST_ASSERT_EQUAL( 24U, time.day );
  TEST_ASSERT_EQUAL( THURSDAY, time.wday );
  report.data[0U] = 50U;
  status = eUSBreportToTime( &report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_DATA, status );
  report.data[0U] = 11U;
  report.length++;
  status = eUSBreportToTime( &report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_LENGTH, status );
  return;
}
void test_eUSBreportToPassword ( void )
{
  const USB_REPORT report = { 0U };
  USB_STATUS       status = USB_STATUS_DONE;
  report.length   = 3U;
  report.data[0U] = PASSWORD_SET;
  report.data[1U] = 0x34U;
  report.data[2U] = 0x12U;
  status = eUSBreportToPassword( &report );
  TEST_ASSERT_EQUAL( USB_STATUS_DONE, status );
  TEST_ASSERT_GREATER_THAN( 0U, ( configReg[MODULE_SETUP_ADR]->value & 0x01U ) );
  TEST_ASSERT_GREATER_THAN( 0U, systemPassword.status );
  TEST_ASSERT_EQUAL( 0x1234, systemPassword.data );
  report.data[0U] = PASSWORD_RESET;
  status = eUSBreportToPassword( &report );
  TEST_ASSERT_EQUAL( USB_STATUS_DONE, status );
  TEST_ASSERT_EQUAL( 0U, ( configReg[MODULE_SETUP_ADR]->value & 0x01U ) );
  TEST_ASSERT_EQUAL( 0U, systemPassword.status );
  report.length = 0xFFFFU;
  status = eUSBreportToPassword( &report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_LENGTH, status );
  report.length = 1U;
  status = eUSBreportToPassword( &report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_LENGTH, status );
  return;
}
void test_eUSBcheckupPassword ( void )
{
  const USB_REPORT report = { 0U };
  USB_STATUS       status = USB_STATUS_DONE;
  status = eUSBcheckupPassword( &report );
  return;
}
void test_eUSBreportToFreeData ( void )
{
  const USB_REPORT report = { 0U };
  USB_STATUS       status = USB_STATUS_DONE;
  status = eUSBreportToFreeData( &report );
  return;
}
void test_eUSBreportToConfig ( void )
{
  const USB_REPORT report = { 0U };
  USB_STATUS       status = USB_STATUS_DONE;
  status = eUSBreportToConfig( &report );
  return;
}
void test_eUSBreportToChart ( void )
{
  const USB_REPORT report = { 0U };
  USB_STATUS       status = USB_STATUS_DONE;
  status = eUSBreportToChart( &report );
  return;
}
void test_vUSBmakeReport ( void )
{
  USB_REPORT report = { 0U };
  vUSBmakeReport( &report );
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_OK, report.stat );
  return;
}
void test_vUSBparseReport ( void )
{
  USB_REPORT report = { 0U };
  vUSBparseReport( &report );
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_OK, report.stat );
  return;
}
void test_vUSBmeasurementToReport ( void )
{
  USB_REPORT report = { 0U };
  vUSBmeasurementToReport( &report );
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_OK, report.stat );
  return;
}
void test_vUSBmeasurementLengthToReport ( void )
{
  USB_REPORT report = { 0U };
  vUSBmeasurementLengthToReport( &report );
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_OK, report.stat );
  return;
}
void test_vUSBoutputToReport ( void )
{
  USB_REPORT report = { 0U };
  vUSBoutputToReport( &report );
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_OK, report.stat );
  return;
}
void test_eUSBreportToOutput ( void )
{
  const USB_REPORT report = { 0U };
  USB_STATUS       status = USB_STATUS_DONE;
  status = eUSBreportToOutput( &report );
  return;
}
void test_eUSBsaveConfigs ( void )
{
  const USB_REPORT report = { 0U };
  USB_STATUS       status = USB_STATUS_DONE;
  status = eUSBsaveConfigs( &report );
  return;
}
void test_eUSBsaveCharts ( void )
{
  const USB_REPORT report = { 0U };
  USB_STATUS       status = USB_STATUS_DONE;
  status = eUSBsaveCharts( &report );
  return;
}
void test_eUSBeraseLOG ( void )
{
  const USB_REPORT report = { 0U };
  USB_STATUS       status = USB_STATUS_DONE;
  status = eUSBeraseLOG( &report );
  return;
}
void test_eUSBerasePassword ( void )
{
  const USB_REPORT report = { 0U };
  USB_STATUS       status = USB_STATUS_DONE;
  status = eUSBerasePassword( &report );
  return;
}

void runTest_usbhid ( void )
{
  UnitySetTestFile( "../Tests/test/test_usbhid_rtc.c" );
  UnityDefaultTestRun( test_vUint16ToBytes, "Uint 16 to bytes", 0U );
  UnityDefaultTestRun( test_vUint24ToBytes, "Uint 24 to bytes", 0U );
  UnityDefaultTestRun( test_vUint32ToBytes, "Uint 32 to bytes", 0U );
  UnityDefaultTestRun( test_uBytesToUnit16, "Bytes to uint 16", 0U );
  UnityDefaultTestRun( test_uByteToUint24, "Bytes to uint 24", 0U );
  UnityDefaultTestRun( test_uByteToUint32, "Bytes to uint 32", 0U );
  UnityDefaultTestRun( test_vUSBfreeDataToReport, "Free data to report", 0U );
  UnityDefaultTestRun( test_vUSBtimeToReport, "Time data to report", 0U );
  UnityDefaultTestRun( test_vUSBlogToReport, "Log record to report", 0U );
  UnityDefaultTestRun( test_vUSBconfigToReport, "Config data to report", 0U );
  UnityDefaultTestRun( test_vUSBchartToReport, "Chart data to report", 0U );
  UnityDefaultTestRun( test_vUSBoutputToReport, "Output data to report", 0U );
  //UnityDefaultTestRun( test_vUSBmeasurementLengthToReport, "", 0U );
  UnityDefaultTestRun( test_eUSBreportToTime, "Parsing report to time", 0U );
  UnityDefaultTestRun( test_eUSBreportToPassword, "Parsing report to password", 0U );
  UnityDefaultTestRun( test_eUSBcheckupPassword, "Checkup password", 0U );
  UnityDefaultTestRun( test_eUSBreportToFreeData, "Parsing report to free data", 0U );
  UnityDefaultTestRun( test_eUSBreportToConfig, "Parsing report to config data", 0U );
  UnityDefaultTestRun( test_eUSBreportToChart, "Parsing report to chart data", 0U );
  UnityDefaultTestRun( test_eUSBreportToOutput, "Parsing report to output data", 0U );
  UnityDefaultTestRun( test_vUSBmakeReport, "Make USB report", 0U );
  UnityDefaultTestRun( test_vUSBparseReport, "Parsing USB report", 0U );
  UnityDefaultTestRun( test_eUSBsaveConfigs, "Save config data USB command", 0U );
  UnityDefaultTestRun( test_eUSBsaveCharts, "Save chart data USB command", 0U );
  UnityDefaultTestRun( test_eUSBeraseLOG, "Erase log USB command", 0U );
  UnityDefaultTestRun( test_eUSBerasePassword, "Erase password USB command", 0U );
  return;
}



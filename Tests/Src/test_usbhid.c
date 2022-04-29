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
#include "freeData.h"
#include "outputData.h"

static uint8_t    input[USB_REPORT_SIZE]  = { 0U };
static uint8_t    output[USB_REPORT_SIZE] = { 0U };
static USB_REPORT report = { 0U };

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
  TEST_ASSERT_EQUAL( 0x1122, uByteToUnit16( input ) );
  return;
}
void test_uByteToUint24 ( void )
{
  const uint8_t input[3U] = { 0x33U, 0x22U, 0x11U };
  TEST_ASSERT_EQUAL( 0x00112233, uByteToUint24( input ) );
  return;
}
void test_uByteToUint32 ( void )
{
  const uint8_t input[4U] = { 0x44U, 0x33U, 0x22U, 0x11U };
  TEST_ASSERT_EQUAL( 0x11223344, uByteToUint32( input ) );
  return;
}
void test_vUSBtimeToReport ( void )
{
  RTC_TIME time = { 0U };
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
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_INTERNAL, report.stat );
  TEST_ASSERT_EQUAL( 0U, report.length );
  return;
}
void test_vUSBconfigToReport ( void )
{
  uint8_t i = 0U;
  report.adr = 0U;
  vUSBconfigToReport( &report );
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_OK, report.stat );
  TEST_ASSERT_EQUAL( ( configReg[report.adr]->atrib->len * 2U ), report.length );
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
  uint8_t i = 0U;
  /* Read chart data */
  report.cmd = USB_REPORT_CMD_GET_CHART_OIL;
  report.adr = 0U;
  vUSBchartToReport( &report );
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_OK, report.stat );
  TEST_ASSERT_EQUAL( 2U, report.length );
  TEST_ASSERT_GREATER_OR_EQUAL( 0U, report.data[0U] );
  TEST_ASSERT_GREATER_OR_EQUAL( 0U, report.data[1U] );
  /* Read dot data */
  report.adr = 1U;
  vUSBchartToReport( &report );
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_OK, report.stat );
  TEST_ASSERT_EQUAL( 8U, report.length );
  for ( i=0U; i<report.length; i++ )
  {
    TEST_ASSERT_GREATER_OR_EQUAL( 0U, report.data[i] );
  }
  /* Command error */
  report.cmd = 0xFFU;
  vUSBchartToReport( &report );
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_BAD_REQ, report.stat );
  TEST_ASSERT_EQUAL( 0U, report.length );
  /* Address error */
  report.cmd = USB_REPORT_CMD_GET_CHART_OIL;
  report.adr = 0xFFFFU;
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_BAD_REQ, report.stat );
  TEST_ASSERT_EQUAL( 0U, report.length );
  return;
}
void test_eUSBreportToTime ( void )
{
  USB_STATUS status = USB_STATUS_DONE;
  RTC_TIME   time   = { 0U };
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
  TEST_ASSERT_EQUAL( USB_STATUS_DONE, status );
  TEST_ASSERT_EQUAL( 11U, time.hour );
  TEST_ASSERT_EQUAL( 22U, time.min );
  TEST_ASSERT_GREATER_OR_EQUAL( 33U, time.sec );
  TEST_ASSERT_EQUAL( 44U, time.year );
  TEST_ASSERT_EQUAL( MONTH_MAY, time.month );
  TEST_ASSERT_EQUAL( 24U, time.day );
  TEST_ASSERT_EQUAL( THURSDAY, time.wday );
  /* Data error */
  report.data[0U] = 50U;
  status = eUSBreportToTime( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_DATA, status );
  /* Length error */
  report.data[0U] = 11U;
  report.length++;
  status = eUSBreportToTime( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_LENGTH, status );
  return;
}
void test_eUSBreportToPassword ( void )
{
  USB_STATUS status = USB_STATUS_DONE;
  /* Write password */
  report.length   = 3U;
  report.data[0U] = PASSWORD_SET;
  report.data[1U] = 0x34U;
  report.data[2U] = 0x12U;
  status = eUSBreportToPassword( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_DONE, status );
  TEST_ASSERT_GREATER_THAN( 0U, ( configReg[MODULE_SETUP_ADR]->value[0U] & 0x0001U ) );
  TEST_ASSERT_GREATER_THAN( 0U, systemPassword.status );
  TEST_ASSERT_EQUAL( 0x1234, systemPassword.data );
  /* Reset password */
  report.data[0U] = PASSWORD_RESET;
  status = eUSBreportToPassword( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_DONE, status );
  TEST_ASSERT_EQUAL( 0U, ( configReg[MODULE_SETUP_ADR]->value[0U] & 0x0001U ) );
  TEST_ASSERT_EQUAL( 0U, systemPassword.status );
  /* Length errors */
  report.length = 0xFFFFU;
  status = eUSBreportToPassword( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_LENGTH, status );
  report.length = 1U;
  status = eUSBreportToPassword( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_LENGTH, status );
  return;
}
void test_eUSBcheckupPassword ( void )
{
  USB_STATUS status = USB_STATUS_DONE;
  /* Check reseted password  */
  report.length = 2U;
  status = eUSBcheckupPassword( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_DONE, status );
  /* Set password */
  report.length   = 3U;
  report.data[0U] = PASSWORD_SET;
  report.data[1U] = 0x34U;
  report.data[2U] = 0x12U;
  status = eUSBreportToPassword( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_DONE, status );
  /* Check password */
  report.length   = 2U;
  report.data[0U] = 0x34U;
  report.data[1U] = 0x12U;
  status = eUSBcheckupPassword( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_DONE, status );
  /* Wrong password */
  report.data[0U] = 0x00U;
  status = eUSBreportToPassword( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_UNAUTHORIZED_ERROR, status );
  /* Length errors */
  report.length = 1U;
  status = eUSBcheckupPassword( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_LENGTH, status );
  report.length = 0xFFFFU;
  status = eUSBcheckupPassword( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_LENGTH, status );
  /* Reset password */
  report.data[0U] = PASSWORD_RESET;
  report.data[1U] = 0x34U;
  report.data[2U] = 0x12U;
  status = eUSBreportToPassword( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_DONE, status );
  return;
}
void test_eUSBreportToFreeData ( void )
{
  USB_STATUS status = USB_STATUS_DONE;
  report.length   = 2U;
  report.adr      = 0U;
  report.data[0U] = 0x34U;
  report.data[1U] = 0x12U;
  status = eUSBreportToFreeData( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_DONE, status );
  TEST_ASSERT_EQUAL( 0x1234U, *freeDataArray[report.adr] );
  /* Address errors */
  report.adr    = 0xFFFFU;
  status = eUSBreportToFreeData( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_ADR, status );
  /* Length errors  */
  report.adr    = 0U;
  report.length = 1U;
  status = eUSBreportToFreeData( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_LENGTH, status );
  report.length = 0xFFFFU;
  status = eUSBreportToFreeData( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_LENGTH, status );
  /* Cleaning */
  report.length   = 2U;
  report.adr      = 0U;
  report.data[0U] = 0x00U;
  report.data[1U] = 0x00U;
  status = eUSBreportToFreeData( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_DONE, status );
  TEST_ASSERT_EQUAL( 0x0000U, *freeDataArray[report.adr] );
  return;
}
void test_eUSBreportToConfig ( void )
{
  USB_STATUS status = USB_STATUS_DONE;
  uint8_t    i      = 0U;
  uint16_t   buf[MAX_VALUE_LENGTH] = { 0U };
  uint16_t   value = 0U;
  report.adr    = RECORD_INTERVAL_ADR;
  report.length = configReg[report.adr]->atrib->len * 2U;
  for ( i=0U; i<configReg[report.adr]->atrib->len; i++ )
  {
    buf[i] = configReg[report.adr]->value[i];
  }
  for ( i=0U; i<report.length; i++ )
  {
    report.data[i] = 0x11U * i;
  }
  status = eUSBreportToConfig( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_DONE, status );
  for ( i=0U; i<configReg[report.adr]->atrib->len; i++ )
  {
    value = uByteToUnit16( report.data );
    TEST_ASSERT_EQUAL( value, configReg[report.adr]->value[i] );
  }
  /* Length errors  */
  report.length = 0xFFFFU;
  status        = eUSBreportToConfig( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_LENGTH, status );
  /* Address errors */
  report.length = configReg[report.adr]->atrib->len * 2U;
  report.adr    = 0xFFFFU;
  status        = eUSBreportToConfig( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_ADR, status );
  /* Clean */
  report.adr = RECORD_INTERVAL_ADR;
  for ( i=0U; i<configReg[report.adr]->atrib->len; i++ )
  {
    configReg[report.adr]->value[i] = buf[i];
  }
  return;
}
void test_eUSBreportToChart ( void )
{
  USB_STATUS status = USB_STATUS_DONE;
  uint16_t   buf    = charts[OIL_CHART_ADR]->size;
  uint32_t   x      = ( uint32_t )charts[OIL_CHART_ADR]->dots[0U].x;
  uint32_t   y      = ( uint32_t )charts[OIL_CHART_ADR]->dots[0U].y;
  /* Chart length */
  report.cmd      = USB_REPORT_CMD_PUT_CHART_OIL;
  report.adr      = 0U;
  report.length   = 2U;
  report.data[0U] = 0x34U;
  report.data[1U] = 0x12U;
  status          = eUSBreportToChart( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_DONE, status );
  TEST_ASSERT_EQUAL( 0x1234, charts[OIL_CHART_ADR]->size );
  /* Dot data */
  report.adr      = 1U;
  report.length   = 8U;
  report.data[0U] = 0x78U;
  report.data[1U] = 0x56U;
  report.data[2U] = 0x34U;
  report.data[3U] = 0x12U;
  report.data[4U] = 0xF0U;
  report.data[5U] = 0xDEU;
  report.data[6U] = 0xBCU;
  report.data[7U] = 0x9AU;
  status          = eUSBreportToChart( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_DONE, status );
  TEST_ASSERT_EQUAL( 0x12345678, ( uint32_t )charts[OIL_CHART_ADR]->dots[report.adr - 1U].x );
  TEST_ASSERT_EQUAL( 0x9ABCDEF0, ( uint32_t )charts[OIL_CHART_ADR]->dots[report.adr - 1U].y );
  /* Address error*/
  report.adr = 0xFFFFU;
  status     = eUSBreportToChart( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_ADR, status );
  /* Length error */
  report.adr    = 0U;
  report.length = 1U;
  status        = eUSBreportToChart( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_LENGTH, status );
  report.length = 0xFFFFU;
  status        = eUSBreportToChart( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_LENGTH, status );
  report.adr    = 1U;
  report.length = 1U;
  status        = eUSBreportToChart( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_LENGTH, status );
  report.length = 0xFFFFU;
  status        = eUSBreportToChart( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_LENGTH, status );
  /* Command error */
  report.cmd    = 0xFFU;
  report.adr    = 0U;
  report.length = 2U;
  status        = eUSBreportToChart( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_COMMAND, status );
  /* Clean */
  charts[OIL_CHART_ADR]->size       = buf;
  charts[OIL_CHART_ADR]->dots[0U].x = ( fix16_t )x;
  charts[OIL_CHART_ADR]->dots[0U].y = ( fix16_t )y;
  return;
}
void test_vUSBmakeReport ( void )
{
  uint8_t i = 0U;
  report.dir    = 0x01;
  report.cmd    = 0x10;
  report.stat   = 0x01;
  report.adr    = 0x1234U;
  report.length = 9U;
  for ( i=0U; i<report.length; i++ )
  {
    report.data[i] = i;
  }
  vUSBmakeReport( &report );
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_OK, report.stat );
  TEST_ASSERT_EQUAL( 0x01U, report.buf[0U] );
  TEST_ASSERT_EQUAL( 0x10U, report.buf[1U] );
  TEST_ASSERT_EQUAL( 0x01U, report.buf[2U] );
  TEST_ASSERT_EQUAL( 0x12U, report.buf[3U] );
  TEST_ASSERT_EQUAL( 0x34U, report.buf[4U] );
  TEST_ASSERT_EQUAL( 0U,    report.buf[5U] );
  TEST_ASSERT_EQUAL( 0U,    report.buf[6U] );
  TEST_ASSERT_EQUAL( 9U,   report.buf[7U] );
  for ( i=0U; i<report.length; i++ )
  {
    TEST_ASSERT_EQUAL( i, report.buf[8U + i] );
  }
  report.length = 0x123456U;
  vUSBmakeReport( &report );
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_OK, report.stat );
  TEST_ASSERT_EQUAL( 0x12U, report.buf[5U] );
  TEST_ASSERT_EQUAL( 0x34U, report.buf[6U] );
  TEST_ASSERT_EQUAL( 0x56U, report.buf[7U] );
  return;
}
void test_vUSBparseReport ( void )
{
  uint8_t i = 0U;
  report.buf[0U] = 0x01U; /* DIR  */
  report.buf[1U] = 0x10U; /* CMD  */
  report.buf[2U] = 0x01U; /* STAT */
  report.buf[3U] = 0x12U; /* ADR1 */
  report.buf[4U] = 0x34U; /* ADR0 */
  report.buf[5U] = 0U;    /* LEN2 */
  report.buf[6U] = 0U;    /* LEN1 */
  report.buf[7U] = 10U;   /* LEN0 */
  for ( i=0U; i<10U; i++ )
  {
    report.buf[8U + i] = i;
  }
  vUSBparseReport( &report );
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_OK, report.stat );
  TEST_ASSERT_EQUAL( 0x01U,   report.dir    );
  TEST_ASSERT_EQUAL( 0x10U,   report.cmd    );
  TEST_ASSERT_EQUAL( 0x01U,   report.stat   );
  TEST_ASSERT_EQUAL( 0x1234U, report.adr    );
  TEST_ASSERT_EQUAL( 10U,     report.length );
  for ( i=0U; i<10U; i++ )
  {
    TEST_ASSERT_EQUAL( i, report.data[i] );
  }
  report.buf[5U] = 0x12U;
  report.buf[6U] = 0x34U;
  report.buf[7U] = 0x56U;
  vUSBparseReport( &report );
  TEST_ASSERT_EQUAL( 0x123456U, report.length );
  return;
}
void test_vUSBmeasurementToReport ( void )
{
  vUSBmeasurementToReport( &report );
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_BAD_REQ, report.stat );
  return;
}
void test_vUSBmeasurementLengthToReport ( void )
{
  vUSBmeasurementLengthToReport( &report );
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_BAD_REQ, report.stat );
  return;
}
void test_vUSBoutputToReport ( void )
{
  report.adr = BATTERY_VOLTAGE_ADR;
  vUSBoutputToReport( &report );
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_OK, report.stat );
  TEST_ASSERT_EQUAL( 2U, report.length );
  TEST_ASSERT_EQUAL( outputDataReg[report.adr]->value[0U], uByteToUnit16( report.data ) );
  /* Address error */
  report.adr = 0xFFFFU;
  vUSBoutputToReport( &report );
  TEST_ASSERT_EQUAL( USB_REPORT_STATE_BAD_REQ, report.stat );
  TEST_ASSERT_EQUAL( 0U, report.length );
  return;
}
void test_eUSBreportToOutput ( void )
{
  USB_STATUS status = USB_STATUS_DONE;
  uint16_t   buf    = outputDataReg[0U]->value[0U];
  report.adr      = CONTROLL_ADR;
  report.data[0U] = 0x00U;
  report.data[1U] = 0x01U;
  status = eUSBreportToOutput( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_DONE, status );
  TEST_ASSERT_EQUAL( 0x0001U, outputDataReg[CONTROLL_ADR]->value[0U] );
  outputDataReg[CONTROLL_ADR]->value[0U] = buf;
  report.adr = 0U;
  status = eUSBreportToOutput( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_ERROR_ADR, status );
  return;
}
void test_eUSBsaveConfigs ( void )
{
  USB_STATUS status = USB_STATUS_DONE;
  status = eUSBsaveConfigs( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_DONE, status );
  return;
}
void test_eUSBsaveCharts ( void )
{
  USB_STATUS status = USB_STATUS_DONE;
  status = eUSBsaveCharts( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_DONE, status );
  return;
}
void test_eUSBeraseLOG ( void )
{
  USB_STATUS status = USB_STATUS_DONE;
  status = eUSBeraseLOG( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_DONE, status );
  return;
}
void test_eUSBerasePassword ( void )
{
  USB_STATUS status = USB_STATUS_DONE;
  status = eUSBerasePassword( ( const USB_REPORT* )&report );
  TEST_ASSERT_EQUAL( USB_STATUS_DONE, status );
  TEST_ASSERT_EQUAL( PASSWORD_RESET, systemPassword.status );
  return;
}

void runTest_usbhid ( void )
{
  report.data = input;
  report.buf  = output;
  UnitySetTestFile( "test_usbhid.c" );
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
  UnityDefaultTestRun( test_vUSBmeasurementLengthToReport, "Measurement", 0U );
  UnityDefaultTestRun( test_eUSBreportToTime, "Parsing report to time", 0U );
  //UnityDefaultTestRun( test_eUSBreportToPassword, "Parsing report to password", 0U );
  //UnityDefaultTestRun( test_eUSBcheckupPassword, "Checkup password", 0U );
  UnityDefaultTestRun( test_eUSBreportToFreeData, "Parsing report to free data", 0U );
  UnityDefaultTestRun( test_eUSBreportToConfig, "Parsing report to config data", 0U );
  UnityDefaultTestRun( test_eUSBreportToChart, "Parsing report to chart data", 0U );
  UnityDefaultTestRun( test_eUSBreportToOutput, "Parsing report to output data", 0U );
  UnityDefaultTestRun( test_vUSBmakeReport, "Make USB report", 0U );
  UnityDefaultTestRun( test_vUSBparseReport, "Parsing USB report", 0U );
  //UnityDefaultTestRun( test_eUSBsaveConfigs, "Save config data USB command", 0U );
  //UnityDefaultTestRun( test_eUSBsaveCharts, "Save chart data USB command", 0U );
  //UnityDefaultTestRun( test_eUSBeraseLOG, "Erase log USB command", 0U );
  //UnityDefaultTestRun( test_eUSBerasePassword, "Erase password USB command", 0U );
  vTESTsendReport();
  return;
}



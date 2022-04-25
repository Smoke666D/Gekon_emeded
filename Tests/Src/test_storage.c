/*
 * test_storage.c
 *
 *  Created on: 22 апр. 2022 г.
 *      Author: 79110
 */

#include "tests.h"
#include "unity.h"
#include "storage.h"
#include "config.h"

#define TEST_STORAGE_DELAY 20U

void test_uUint16ToBlob ( void )
{
  uint8_t blob[2U] = { 0U };
  TEST_ASSERT_EQUAL( 2U, uUint16ToBlob( 0x1234U, blob ) );
  TEST_ASSERT_EQUAL( 0x34U, blob[0U] );
  TEST_ASSERT_EQUAL( 0x12U, blob[1U] );
  return;
}
void test_uBlobToUint16 ( void )
{
  uint16_t data     = 0U;
  uint8_t  blob[2U] = { 0x34U, 0x12U };
  TEST_ASSERT_EQUAL( 2U, uBlobToUint16( &data, ( const uint8_t* )blob ) );
  TEST_ASSERT_EQUAL( 0x1234U, data );
  return;
}
void test_uUint32ToBlob ( void )
{
  uint8_t blob[4U] = { 0U };
  TEST_ASSERT_EQUAL( 4U, uUint32ToBlob( 0x12345678U, blob ) );
  TEST_ASSERT_EQUAL( 0x78U , blob[0U] );
  TEST_ASSERT_EQUAL( 0x56U , blob[1U] );
  TEST_ASSERT_EQUAL( 0x34U , blob[2U] );
  TEST_ASSERT_EQUAL( 0x12U , blob[3U] );
  return;
}
void test_uBlobToUint32 ( void )
{
  uint32_t data     = 0U;
  uint8_t  blob[4U] = { 0x78U, 0x56U, 0x34U, 0x12U };
  TEST_ASSERT_EQUAL( 4U, uBlobToUint32( &data, ( const uint8_t* )blob ) );
  TEST_ASSERT_EQUAL( 0x12345678U, data );
  return;
}
void test_uFix16ToBlob ( void )
{
  uint8_t blob[4U] = { 0U };
  TEST_ASSERT_EQUAL( 4U, uFix16ToBlob( ( fix16_t )( 0x12345678U ), blob ) );
  TEST_ASSERT_EQUAL( 0x78U , blob[0U] );
  TEST_ASSERT_EQUAL( 0x56U , blob[1U] );
  TEST_ASSERT_EQUAL( 0x34U , blob[2U] );
  TEST_ASSERT_EQUAL( 0x12U , blob[3U] );
  return;
}
void test_uBlobToFix16 ( void )
{
  fix16_t data     = 0U;
  uint8_t blob[4U] = { 0x78U, 0x56U, 0x34U, 0x12U };
  TEST_ASSERT_EQUAL( 4U, uBlobToFix16( &data, ( const uint8_t* )blob ) );
  TEST_ASSERT_EQUAL( 0x12345678U, ( uint32_t )( data ) );
  return;
}
void test_uConfigToBlob ( void )
{
  uint8_t  blob[CONFIG_MAX_SIZE + 1U] = { 0U };
  uint8_t  i   = 0U;
  uint16_t buf = 0U;

  TEST_ASSERT_EQUAL( ( configReg[RECORD_INTERVAL_ADR]->atrib->len * 2U ), uConfigToBlob( ( const eConfigReg* )( configReg[RECORD_INTERVAL_ADR] ), blob ) );
  for ( i=0U; i<configReg[RECORD_INTERVAL_ADR]->atrib->len; i++ )
  {
    ( void )uBlobToUint16( &buf, &blob[2U * i] );
    TEST_ASSERT_EQUAL( buf, configReg[RECORD_INTERVAL_ADR]->value[i] );
  }
  return;
}
void test_uBlobToConfig ( void )
{
  uint8_t blob[2U] = { 0x34U, 0x12U };
  TEST_ASSERT_EQUAL( 2U, uBlobToConfig( configReg[RECORD_INTERVAL_ADR], ( const uint8_t* )blob ) );
  TEST_ASSERT_EQUAL( 0x1234U, configReg[RECORD_INTERVAL_ADR]->value[0U] );
  return;
}
void test_uSTORAGEgetSize ( void )
{
  TEST_ASSERT_EQUAL( storageEEPROM->size, uSTORAGEgetSize() );
  return;
}
void test_eSTORAGEreadUint32 ( void )
{
  uint32_t      data = 0U;
  uint32_t      adr  = storageEEPROM->size - 4U;
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEreadUint32( &data, &adr ) );
  TEST_ASSERT_EQUAL( 0U, data );
  return;
}
void test_eSTORAGEwriteUint32 ( void )
{
  uint32_t      data = 0U;
  uint32_t      adr  = storageEEPROM->size - 4U;
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEwriteUint32( 0x12345678U, &adr ) );
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEreadUint32( &data, &adr ) );
  TEST_ASSERT_EQUAL( 0x12345678U, data );
  ( void )eSTORAGEwriteUint32( 0U, &adr );
  return;
}
void test_eSTORAGEreadSR ( void )
{
  uint8_t       data = 0U;
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEreadSR( &data ) );
  TEST_ASSERT_NOT_EQUAL( 0x00U, data );
  TEST_ASSERT_NOT_EQUAL( 0xFFU, data );
  return;
}
void test_eSTORAGEwriteSR ( void )
{
  uint8_t data  = 0U;
  uint8_t input = 0xAAU;
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEreadSR( &data ) );
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEwriteSR( ( const uint8_t* )( &input ) ) );
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEreadSR( &data ) );
  TEST_ASSERT_EQUAL( 0xAAU, input );
  ( void )eSTORAGEwriteSR( ( const uint8_t* )( &data ) );
  return;
}
void test_uSTORAGEcheckMap ( void )
{
  uint32_t map[STORAGE_MAP_SIZE / 4U] =
  {
    STORAGE_CONFIG_SIZE,      /* 0 */
    STORAGE_CHART_SIZE,       /* 1 */
    STORAGE_FREE_DATA_SIZE,   /* 2 */
    STORAGE_PASSWORD_SIZE,    /* 3 */
    STORAGE_LOG_POINTER_SIZE, /* 4 */
    STORAGE_LOG_SIZE,         /* 5 */
    STORAGE_JOURNAL_SIZE      /* 6 */
  };
  uint8_t res = uSTORAGEcheckMap( ( const uint32_t* )( map ) );
  TEST_ASSERT_EQUAL( 1U, res );
  map[1U] += 5U;
  res = uSTORAGEcheckMap( ( const uint32_t* )( map ) );
  TEST_ASSERT_EQUAL( 0U, res );
  map[1U] -= 5U;
  return;
}
void test_eSTORAGEreadMap ( void )
{
  uint32_t map[STORAGE_MAP_SIZE / 4U] = { 0U };
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEreadMap( map ) );
  TEST_ASSERT_EQUAL( 1U, uSTORAGEcheckMap( ( const uint32_t* )( map ) ) );
  return;
}
void test_eSTORAGEwriteMap ( void )
{
  uint32_t map[STORAGE_MAP_SIZE / 4U] = { 0U };
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEwriteMap() );
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEreadMap( map ) );
  TEST_ASSERT_EQUAL( 1U, uSTORAGEcheckMap( ( const uint32_t* )( map ) ) );
  return;
}
void test_eSTORAGEreadCharts ( void )
{
  uint8_t  i = 0U;
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEreadCharts() );
  for ( i=0U; i<CHART_NUMBER; i++ )
  {
    TEST_ASSERT_GREATER_THAN( 0U, charts[i]->size );
  }
  return;
}
void test_eSTORAGEwriteCharts ( void )
{
  eChartData buffer = { 0U };
  uint8_t    i      = 0U;
  uint8_t    j      = 0U;
  for ( i=0U; i<CHART_NUMBER; i++ )
  {
    buffer = *charts[i];
    charts[i]->size = 3U + i;
    for ( j=0U; j<charts[i]->size; j++ )
    {
      charts[i]->dots[j].x = ( fix16_t )( i + j );
      charts[i]->dots[j].y = ( fix16_t )( 2U * i + j );
    }
    TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEwriteCharts() );
    TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEreadCharts() );
    TEST_ASSERT_EQUAL( ( 3U + i ), charts[i]->size );
    for ( j=0U; j<charts[i]->size; j++ )
    {
      TEST_ASSERT_EQUAL( ( fix16_t )( i + j ), charts[i]->dots[j].x );
      TEST_ASSERT_EQUAL( ( fix16_t )( 2U * i + j ), charts[i]->dots[j].y );
    }
    *charts[i] = buffer;
    TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEwriteCharts() );
  }
  return;
}
void test_eSTORAGEreadConfigs ( void )
{
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEreadConfigs() );
  return;
}
void test_eSTORAGEwriteConfigs ( void )
{
  uint8_t  i    = 0U;
  uint8_t  j    = 0U;
  uint16_t data = 0U;
  for ( i=0U; i<SETTING_REGISTER_NUMBER; i++ )
  {
    for ( j=0U; j<configReg[i]->atrib->len; j++ )
    {
      configReg[i]->value[j] = data++;
    }
  }
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEwriteConfigs() );
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEreadConfigs() );
  data = 0U;
  for ( i=0U; i<SETTING_REGISTER_NUMBER; i++ )
  {
    for ( j=0U; j<configReg[i]->atrib->len; j++ )
    {
      TEST_ASSERT_EQUAL( data++, configReg[i]->value[j] );
    }
  }
  return;
}
void test_eSTORAGEreadLogPointer ( void )
{
  uint16_t data = 0U;
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEreadLogPointer( &data ) );
  TEST_ASSERT_GREATER_THAN( 0U, data );
  return;
}
void test_eSTORAGEwriteLogPointer ( void )
{
  uint16_t data = 0x1234U;
  uint16_t buf  = 0U;
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEreadLogPointer( &buf ) );
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEwriteLogPointer( data ) );
  data = 0U;
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEreadLogPointer( &data ) );
  TEST_ASSERT_EQUAL( 0x1234U, data );
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEwriteLogPointer( buf ) );
  return;
}
void test_eSTORAGEreadLogRecord ( void )
{
  LOG_RECORD_TYPE record = { 0U };
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEreadLogRecord( 0U, &record ) );
  TEST_ASSERT_GREATER_THAN( 0U, record.time );
  TEST_ASSERT_LESS_THAN( 8U,  record.event.action );
  TEST_ASSERT_LESS_THAN( 46U, record.event.type );
  return;
}
void test_eSTORAGEwriteLogRecord ( void )
{
  LOG_RECORD_TYPE record = { 0U };
  LOG_RECORD_TYPE buffer = { 0U };
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEreadLogRecord( 0U, &buffer ) );
  record.time         = 0x12345678U;
  record.event.action = 0xAAU;
  record.event.type   = 0xCCU;
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEwriteLogRecord( 0U, ( const LOG_RECORD_TYPE* )&record ) );
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEreadLogRecord( 0U, &record ) );
  record.time         = 0U;
  record.event.action = 0U;
  record.event.type   = 0U;
  TEST_ASSERT_EQUAL( 0x12345678U, record.time );
  TEST_ASSERT_EQUAL( 0xAAU, record.event.action );
  TEST_ASSERT_EQUAL( 0xCCU, record.event.type );
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEwriteLogRecord( 0U, ( const LOG_RECORD_TYPE* )&buffer ) );
  return;
}
void test_eSTORAGEloadPassword ( void )
{
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEloadPassword() );
  return;
}
void test_eSTORAGEsavePassword ( void )
{
  systemPassword.status = PASSWORD_SET;
  systemPassword.data   = 0x1234U;
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEsavePassword() );
  systemPassword.status = 0U;
  systemPassword.data   = 0U;
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEloadPassword() );
  TEST_ASSERT_EQUAL( PASSWORD_SET, systemPassword.status );
  TEST_ASSERT_EQUAL( 0x1234U, systemPassword.data );
  systemPassword.status = 0U;
  systemPassword.data   = 0U;
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEsavePassword() );
  return;
}
void test_eSTORAGEreadFreeData ( void )
{
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEreadFreeData( 0U ) );
  return;
}
void test_eSTORAGEsaveFreeData ( void )
{
  uint16_t buf = *freeDataArray[0U];
  *freeDataArray[0U] = 0x1234U;
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEsaveFreeData( 0U ) );
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEreadFreeData( 0U ) );
  TEST_ASSERT_EQUAL( 0x1234U, *freeDataArray[0U] );
  *freeDataArray[0U] = buf;
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEsaveFreeData( 0U ) );
  return;
}
void test_eSTORAGEsetFreeData ( void )
{
  uint16_t data = 0x1234U;
  uint16_t buf  = 0U;
  buf = *freeDataArray[0U];
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEsetFreeData( 0U, &data ) );
  TEST_ASSERT_EQUAL( 0x1234U, *freeDataArray[0U] );
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEsetFreeData( 0U, &buf ) );
  return;
}
void test_eSTORAGEreadMeasurement ( void )
{
  uint16_t data = 0U;
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEreadMeasurement( 0U, 1U, &data ) );
  return;
}
void test_eSTORAGEeraseMeasurement ( void )
{
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEeraseMeasurement() );
  return;
}
void test_eSTORAGEaddMeasurement ( void )
{
  uint16_t data = 0U;
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEaddMeasurement( 0U, 1U, &data ) );
  return;
}
void test_eSTORAGEreadMeasurementCounter ( void )
{
  uint16_t data = 0U;
  TEST_ASSERT_EQUAL( EEPROM_OK, eSTORAGEreadMeasurementCounter( &data ) );
  return;
}

void runTets_storage ( void )
{
  UnitySetTestFile( "test_storage.c" );
  UnityDefaultTestRun( test_uUint16ToBlob, "Uint16 to blob", 0U );
  UnityDefaultTestRun( test_uBlobToUint16, "Blob to Uint16", 0U );
  UnityDefaultTestRun( test_uUint32ToBlob, "Uint32 to blob", 0U );
  UnityDefaultTestRun( test_uBlobToUint32, "Blob to Uint32", 0U );
  UnityDefaultTestRun( test_uFix16ToBlob, "Fix16 to blob", 0U );
  UnityDefaultTestRun( test_uBlobToFix16, "Blob to Fix16", 0U );
  UnityDefaultTestRun( test_uConfigToBlob, "Config to blob", 0U );
  UnityDefaultTestRun( test_uBlobToConfig, "Blob to config", 0U );
  UnityDefaultTestRun( test_uSTORAGEgetSize, "Get storage size", 0U );
  UnityDefaultTestRun( test_uSTORAGEcheckMap, "Check memory map", 0U );
  /*
  UnityDefaultTestRun( test_eSTORAGEwriteUint32, "Write uint32 to storage", 0U );
  UnityDefaultTestRun( test_eSTORAGEreadUint32, "Read uint32 from storage", 0U );
  UnityDefaultTestRun( test_eSTORAGEreadSR, "Read SR from storage", 0U );
  UnityDefaultTestRun( test_eSTORAGEwriteSR, "Write SR to storage", 0U );
  UnityDefaultTestRun( test_eSTORAGEreadMap, "Read memory map", 0U );
  UnityDefaultTestRun( test_eSTORAGEwriteMap, "Write memory map", 0U );
  UnityDefaultTestRun( test_eSTORAGEreadCharts, "Read charts from storage", 0U );
  UnityDefaultTestRun( test_eSTORAGEwriteCharts, "Write charts to storage", 0U );
  UnityDefaultTestRun( test_eSTORAGEreadConfigs, "Read configs from storage", 0U );
  UnityDefaultTestRun( test_eSTORAGEwriteConfigs, "Write configs to storage", 0U );
  UnityDefaultTestRun( test_eSTORAGEreadLogPointer, "Read log pointer from storage", 0U );
  UnityDefaultTestRun( test_eSTORAGEwriteLogPointer, "Write log pointer to storage", 0U );
  UnityDefaultTestRun( test_eSTORAGEreadLogRecord, "Read log record from storage", 0U );
  UnityDefaultTestRun( test_eSTORAGEwriteLogRecord, "Write log record to storage", 0U );
  UnityDefaultTestRun( test_eSTORAGEsavePassword, "Save password in storage", 0U );
  UnityDefaultTestRun( test_eSTORAGEloadPassword, "Load password from storage", 0U );
  UnityDefaultTestRun( test_eSTORAGEreadFreeData, "Read free data from storage", 0U );
  UnityDefaultTestRun( test_eSTORAGEsaveFreeData, "Save free data in storage", 0U );
  UnityDefaultTestRun( test_eSTORAGEsetFreeData, "Set free data in storage", 0U );
  UnityDefaultTestRun( test_eSTORAGEreadMeasurement, "Read measurement from storage", 0U );
  UnityDefaultTestRun( test_eSTORAGEeraseMeasurement, "Erase measurement in storage", 0U );
  UnityDefaultTestRun( test_eSTORAGEaddMeasurement, "Add measurement to storage", 0U );
  UnityDefaultTestRun( test_eSTORAGEreadMeasurementCounter, "Read measurement counter from storage", 0U );
  */
  return;
}

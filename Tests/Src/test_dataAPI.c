/*
 * test_dataAPI.c
 *
 *  Created on: 25 апр. 2022 г.
 *      Author: 79110
 */

#include "tests.h"
#include "unity.h"
#include "dataAPI.h"

void test_eDATAAPIlogLoad ( void )
{
  LOG_RECORD_TYPE record = { 0U };
  uint16_t        i      = 0U;
  for ( i=0U; i<LOG_SIZE; i++ )
  {
    TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIlogLoad( i, &record ) );
  }
  return;
}
void test_vDATAAPIincLogSize ( void )
{
  uint16_t buffer = logCash.size;
  vDATAAPIincLogSize();
  TEST_ASSERT_EQUAL( ( buffer + 1U ), logCash.size );
  logCash.size = buffer;
  return;
}
void test_xDATAAPIgetEventGroup ( void )
{
  TEST_ASSERT_NOT_EQUAL( NULL, xDATAAPIgetEventGroup() );
  return;
}
void test_eDATAAPIchart ( void )
{
  eChartData data[CHART_NUMBER] = { 0U };
  uint8_t    i   = 0U;
  uint16_t   buf = 0U;
  /* DATA_API_CMD_READ */
  for ( i=0U; i<CHART_NUMBER; i++ )
  {
    TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIchart( DATA_API_CMD_READ, i, &data[i] ) );
    TEST_ASSERT_GREATER_THAN( 1U, data[i].size );
  }
  /* DATA_API_CMD_WRITE */
  for ( i=0U; i<CHART_NUMBER; i++ )
  {
    data[i].size += i + 1U;
    buf = data[i].size;
    TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIchart( DATA_API_CMD_WRITE, i, &data[i] ) );
    TEST_ASSERT_EQUAL( buf, charts[i]->size );
  }
  /* DATA_API_CMD_SAVE */
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIchart( DATA_API_CMD_SAVE, 0U, NULL ) );
  /* DATA_API_CMD_LOAD */
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIchart( DATA_API_CMD_LOAD, 0U, NULL ) );
  for ( i=0U; i<CHART_NUMBER; i++ )
  {
    TEST_ASSERT_EQUAL( data[i].size, charts[i]->size );
  }
  /* ADDRESS ERROR */
  TEST_ASSERT_EQUAL( DATA_API_STAT_ADR_ERROR, eDATAAPIchart( DATA_API_CMD_READ, 0x0FFFU, &data[0U] ) );
  /* CLEAN */
  for ( i=0U; i<CHART_NUMBER; i++ )
  {
    TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIchart( DATA_API_CMD_READ, i, &data[i] ) );
    data[i].size -= i + 1U;
    TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIchart( DATA_API_CMD_WRITE, i, &data[i] ) );
  }
  /* WRONG COMMANDS */
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIchart( DATA_API_CMD_INC, 0U, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIchart( DATA_API_CMD_DEC, 0U, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIchart( DATA_API_CMD_ERASE, 0U, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIchart( DATA_API_CMD_ADD, 0U, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIchart( DATA_API_CMD_COUNTER, 0U, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIchart( DATA_API_CMD_READ_CASH, 0U, NULL ) );
  return;
}
void test_eDATAAPIconfig ( void )
{
  uint8_t  i     = 0U;
  int8_t   scale = 0U;
  uint16_t adr   = 0U;
  uint16_t data[MAX_VALUE_LENGTH]  = { 0U };
  uint16_t units[MAX_UNITS_LENGTH] = { 0U };
  /* DATA_API_CMD_READ */
  adr = RECORD_INTERVAL_ADR;
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIconfig( DATA_API_CMD_READ, adr, data, &scale, units ) );
  TEST_ASSERT_EQUAL( configReg[adr]->atrib->scale, scale );
  for ( i=0U; i<configReg[adr]->atrib->len; i++ )
  {
    TEST_ASSERT_EQUAL( configReg[adr]->value[i], data[i] );
  }
  for ( i=0U; i<MAX_UNITS_LENGTH; i++ )
  {
    TEST_ASSERT_EQUAL( configReg[adr]->atrib->units[i], units[i] );
  }
  /* DATA_API_CMD_WRITE */
  adr      = RECORD_INTERVAL_ADR;
  data[0U] = 0x1234U;
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIconfig( DATA_API_CMD_WRITE, adr, data, NULL, NULL ) );
  TEST_ASSERT_EQUAL( 0x1234U, configReg[adr]->value[0U] );
  /* DATA_API_CMD_SAVE */
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIconfig( DATA_API_CMD_SAVE, 0U, NULL, NULL, NULL ) );
  adr      = RECORD_INTERVAL_ADR;
  data[0U] = 0U;
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIconfig( DATA_API_CMD_WRITE, adr, data, NULL, NULL ) );
  TEST_ASSERT_EQUAL( 0U, configReg[adr]->value[0U] );
  /* DATA_API_CMD_LOAD */
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIconfig( DATA_API_CMD_LOAD, 0U, NULL, NULL, NULL ) );
  TEST_ASSERT_EQUAL( 0x1234U, configReg[adr]->value[0U] );
  /* ADDRESS ERROR */
  adr = SETTING_REGISTER_NUMBER + 1U;
  TEST_ASSERT_EQUAL( DATA_API_STAT_ADR_ERROR, eDATAAPIconfig( DATA_API_CMD_READ, adr, data, &scale, units ) );
  /* WRONG COMMANDS */
  TEST_ASSERT_EQUAL(DATA_API_STAT_CMD_ERROR, eDATAAPIconfig( DATA_API_CMD_INC, 0U, NULL, NULL, NULL ) );
  TEST_ASSERT_EQUAL(DATA_API_STAT_CMD_ERROR, eDATAAPIconfig( DATA_API_CMD_DEC, 0U, NULL, NULL, NULL ) );
  TEST_ASSERT_EQUAL(DATA_API_STAT_CMD_ERROR, eDATAAPIconfig( DATA_API_CMD_ERASE, 0U, NULL, NULL, NULL ) );
  TEST_ASSERT_EQUAL(DATA_API_STAT_CMD_ERROR, eDATAAPIconfig( DATA_API_CMD_ADD, 0U, NULL, NULL, NULL ) );
  TEST_ASSERT_EQUAL(DATA_API_STAT_CMD_ERROR, eDATAAPIconfig( DATA_API_CMD_COUNTER, 0U, NULL, NULL, NULL ) );
  TEST_ASSERT_EQUAL(DATA_API_STAT_CMD_ERROR, eDATAAPIconfig( DATA_API_CMD_READ_CASH, 0U, NULL, NULL, NULL ) );
  /* CLEAN */
  adr      = RECORD_INTERVAL_ADR;
  data[0U] = 0U;
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIconfig( DATA_API_CMD_WRITE, adr, data, NULL, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIconfig( DATA_API_CMD_SAVE, 0U, NULL, NULL, NULL ) );
  return;
}
void test_eDATAAPIconfigAtrib ( void )
{
  uint8_t           i     = 0U;
  uint16_t          adr   = RECORD_INTERVAL_ADR;
  eConfigAttributes atrib = { 0U };
  /* DATA_API_CMD_READ */
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIconfigAtrib( DATA_API_CMD_READ, adr, &atrib ) );
  TEST_ASSERT_EQUAL( configReg[adr]->atrib->adr, atrib.adr );
  TEST_ASSERT_EQUAL( configReg[adr]->atrib->bitMapSize, atrib.bitMapSize );
  TEST_ASSERT_EQUAL( configReg[adr]->atrib->len, atrib.len );
  TEST_ASSERT_EQUAL( configReg[adr]->atrib->max, atrib.max );
  TEST_ASSERT_EQUAL( configReg[adr]->atrib->min, atrib.min );
  TEST_ASSERT_EQUAL( configReg[adr]->atrib->scale, atrib.scale );
  TEST_ASSERT_EQUAL( configReg[adr]->atrib->type, atrib.type );
  for ( i=0U; i<atrib.bitMapSize; i++ )
  {
    TEST_ASSERT_EQUAL( configReg[adr]->atrib->bitMap[i].mask, atrib.bitMap[i].mask );
    TEST_ASSERT_EQUAL( configReg[adr]->atrib->bitMap[i].shift, atrib.bitMap[i].shift );
  }
  for ( i=0U; i<MAX_UNITS_LENGTH; i++ )
  {
    TEST_ASSERT_EQUAL( configReg[adr]->atrib->units[i], atrib.units[i] );
  }
  /* ADDRESS ERROR */
  TEST_ASSERT_EQUAL( DATA_API_STAT_ADR_ERROR, eDATAAPIconfigAtrib( DATA_API_CMD_READ, ( SETTING_REGISTER_NUMBER + 1U ), &atrib ) );
  /* WRONG COMMANDS */
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIconfigAtrib( DATA_API_CMD_WRITE, 0U, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIconfigAtrib( DATA_API_CMD_INC, 0U, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIconfigAtrib( DATA_API_CMD_DEC, 0U, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIconfigAtrib( DATA_API_CMD_SAVE, 0U, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIconfigAtrib( DATA_API_CMD_LOAD, 0U, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIconfigAtrib( DATA_API_CMD_ERASE, 0U, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIconfigAtrib( DATA_API_CMD_ADD, 0U, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIconfigAtrib( DATA_API_CMD_COUNTER, 0U, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIconfigAtrib( DATA_API_CMD_READ_CASH, 0U, NULL ) );
  return;
}
void test_eDATAAPIfreeData ( void )
{
  FREE_DATA_ADR adr  = ENGINE_WORK_MINUTES_ADR;
  uint16_t      data = 0x1234U;
  /* DATA_API_CMD_READ */
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIfreeData( DATA_API_CMD_READ, adr, &data ) );
  /* DATA_API_CMD_WRITE */
  data = 0x1234U;
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIfreeData( DATA_API_CMD_WRITE, adr, &data ) );
  TEST_ASSERT_EQUAL( 0x1234U, *freeDataArray[adr] );
  /* DATA_API_CMD_INC */
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIfreeData( DATA_API_CMD_INC, adr, &data ) );
  TEST_ASSERT_EQUAL( 0x1235U, *freeDataArray[adr] );
  /* DATA_API_CMD_SAVE */
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIfreeData( DATA_API_CMD_SAVE, 0U, NULL ) );
  /* DATA_API_CMD_LOAD */
  data = 0U;
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIfreeData( DATA_API_CMD_WRITE, adr, &data ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIfreeData( DATA_API_CMD_LOAD, 0U, NULL ) );
  TEST_ASSERT_EQUAL( 0x1235U, *freeDataArray[adr] );
  /* DATA_API_CMD_ERASE */
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIfreeData( DATA_API_CMD_ERASE, adr, NULL ) );
  TEST_ASSERT_EQUAL( 0U, *freeDataArray[adr] );
  /* ADDRESS ERROR */
  TEST_ASSERT_EQUAL( DATA_API_STAT_ADR_ERROR, eDATAAPIfreeData( DATA_API_CMD_READ, ( FREE_DATA_SIZE + 1U ), &data ) );
  /* WRONG COMMANDS */
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIfreeData( DATA_API_CMD_DEC, 0U, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIfreeData( DATA_API_CMD_ADD, 0U, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIfreeData( DATA_API_CMD_COUNTER, 0U, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIfreeData( DATA_API_CMD_READ_CASH, 0U, NULL ) );
  return;
}
void test_eDATAAPIpassword ( void )
{
  PASSWORD_TYPE password = { 0U };
  /* DATA_API_CMD_READ */
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIpassword( DATA_API_CMD_READ, &password ) );
  TEST_ASSERT_EQUAL( PASSWORD_RESET, password.status );
  TEST_ASSERT_EQUAL( 0x0000U, password.data );
  /* DATA_API_CMD_WRITE */
  password.status = PASSWORD_SET;
  password.data   = 0x1234U;
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIpassword( DATA_API_CMD_WRITE, &password ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIpassword( DATA_API_CMD_READ, &password ) );
  TEST_ASSERT_EQUAL( PASSWORD_SET, password.status );
  TEST_ASSERT_EQUAL( 0x1234U, password.data );
  /* DATA_API_CMD_SAVE */
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIpassword( DATA_API_CMD_SAVE, NULL ) );
  /* DATA_API_CMD_LOAD */
  password.status = PASSWORD_RESET;
  password.data   = 0U;
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIpassword( DATA_API_CMD_LOAD, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIpassword( DATA_API_CMD_WRITE, &password ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIpassword( DATA_API_CMD_LOAD, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIpassword( DATA_API_CMD_READ, &password ) );
  TEST_ASSERT_EQUAL( PASSWORD_SET, password.status );
  TEST_ASSERT_EQUAL( 0x1234U, password.data );
  /* DATA_API_CMD_ERASE */
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIpassword( DATA_API_CMD_ERASE, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIpassword( DATA_API_CMD_READ, &password ) );
  TEST_ASSERT_EQUAL( PASSWORD_RESET, password.status );
  TEST_ASSERT_EQUAL( 0x0000U, password.data );
  /* WRONG COMMANDS */
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIpassword( DATA_API_CMD_INC, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIpassword( DATA_API_CMD_DEC, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIpassword( DATA_API_CMD_ADD, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIpassword( DATA_API_CMD_COUNTER, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIpassword( DATA_API_CMD_READ_CASH, NULL ) );
  return;
}
void test_eDATAAPIlogPointer ( void )
{
  uint16_t data = 0U;
  /* DATA_API_CMD_READ */
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIlogPointer( DATA_API_CMD_READ, &data ) );
  /* DATA_API_CMD_READ_CASH */
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIlogPointer( DATA_API_CMD_READ_CASH, &data ) );
  /* WRONG COMMANDS */
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIlogPointer( DATA_API_CMD_WRITE, &data ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIlogPointer( DATA_API_CMD_INC, &data ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIlogPointer( DATA_API_CMD_DEC, &data ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIlogPointer( DATA_API_CMD_SAVE, &data ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIlogPointer( DATA_API_CMD_LOAD, &data ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIlogPointer( DATA_API_CMD_ERASE, &data ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIlogPointer( DATA_API_CMD_ADD, &data ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIlogPointer( DATA_API_CMD_COUNTER, &data ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIlogPointer( DATA_API_CMD_NEW, &data ) );
  return;
}
void test_eDATAAPIlog ( void )
{
  uint16_t        adr    = 0U;
  LOG_RECORD_TYPE record = { 0U };
  /* DATA_API_CMD_ADD */
  record.time         = 0x12345678U;
  record.event.action = ACTION_WARNING;
  record.event.type   = EVENT_BATTERY_LOW;
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIlog( DATA_API_CMD_ADD, NULL, &record ) );
  /* DATA_API_CMD_LOAD & DATA_API_CMD_COUNTER */
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIlog( DATA_API_CMD_COUNTER, &adr, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIlog( DATA_API_CMD_LOAD, &adr, &record ) );
  TEST_ASSERT_EQUAL( 0x12345678U, record.time );
  TEST_ASSERT_EQUAL( ACTION_WARNING, record.event.action );
  TEST_ASSERT_EQUAL( EVENT_BATTERY_LOW, record.event.type );
  /* DATA_API_CMD_READ_CASH */
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIlog( DATA_API_CMD_READ_CASH, &adr, &record ) );
  TEST_ASSERT_EQUAL( 0x12345678U, record.time );
  TEST_ASSERT_EQUAL( ACTION_WARNING, record.event.action );
  TEST_ASSERT_EQUAL( EVENT_BATTERY_LOW, record.event.type );
  /* DATA_API_CMD_ERASE */
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIlog( DATA_API_CMD_ERASE, NULL, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIlog( DATA_API_CMD_LOAD, &adr, &record ) );
  TEST_ASSERT_EQUAL( 0x0U, record.time );
  /* ADDRESS ERROR */
  adr = LOG_SIZE + 1U;
  TEST_ASSERT_EQUAL( DATA_API_STAT_ADR_ERROR, eDATAAPIlog( DATA_API_CMD_LOAD, &adr, &record ) );
  /* WRONG COMMANDS */
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIlog( DATA_API_CMD_READ, NULL, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIlog( DATA_API_CMD_WRITE, NULL, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIlog( DATA_API_CMD_INC, NULL, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIlog( DATA_API_CMD_DEC, NULL, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIlog( DATA_API_CMD_SAVE, NULL, NULL ) );
  return;
}
void test_eDATAAPIconfigValue ( void )
{
  uint16_t adr = RECORD_INTERVAL_ADR;
  uint16_t data[MAX_VALUE_LENGTH] = { 0U };
  uint8_t  i = 0U;
  /* DATA_API_CMD_READ */
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIconfigValue( DATA_API_CMD_READ, adr, data ) );
  /* DATA_API_CMD_WRITE */
  for ( i=0U; i<configReg[adr]->atrib->len; i++ )
  {
    data[i] = i + 1U;
  }
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIconfigValue( DATA_API_CMD_WRITE, adr, data ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIconfigValue( DATA_API_CMD_READ, adr, data ) );
  for ( i=0U; i<configReg[adr]->atrib->len; i++ )
  {
    TEST_ASSERT_EQUAL( ( i + 1U ), configReg[adr]->value[i] );
  }
  /* DATA_API_CMD_INC */
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIconfigValue( DATA_API_CMD_INC, adr, NULL ) );
  TEST_ASSERT_EQUAL( ( data[0U] + 1U ), configReg[adr]->value[0U] );
  data[0U] = configReg[adr]->atrib->max;
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIconfigValue( DATA_API_CMD_WRITE, adr, data ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_MAX_ERROR, eDATAAPIconfigValue( DATA_API_CMD_INC, adr, NULL ) );
  /* DATA_API_CMD_DEC */
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIconfigValue( DATA_API_CMD_DEC, adr, NULL ) );
  TEST_ASSERT_EQUAL( ( configReg[adr]->atrib->max - 1U ), configReg[adr]->value[0U] );
  data[0U] = configReg[adr]->atrib->min;
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIconfigValue( DATA_API_CMD_WRITE, adr, data ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_MIN_ERROR, eDATAAPIconfigValue( DATA_API_CMD_DEC, adr, NULL ) );
  /* DATA_API_CMD_SAVE */
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIconfigValue( DATA_API_CMD_SAVE, 0U, NULL ) );
  /* DATA_API_CMD_LOAD */
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIconfigValue( DATA_API_CMD_LOAD, 0U, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_OK, eDATAAPIconfigValue( DATA_API_CMD_READ, adr, data ) );
  TEST_ASSERT_EQUAL( 1U, configReg[adr]->value[0U] );
  /* ADDRESS ERROR */
  adr = SETTING_REGISTER_NUMBER + 1U;
  TEST_ASSERT_EQUAL( DATA_API_STAT_ADR_ERROR, eDATAAPIconfigValue( DATA_API_CMD_READ, adr, data ) );
  /* WRONG COMMANDS */
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIconfigValue( DATA_API_CMD_ERASE, 0U, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIconfigValue( DATA_API_CMD_ADD, 0U, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIconfigValue( DATA_API_CMD_COUNTER, 0U, NULL ) );
  TEST_ASSERT_EQUAL( DATA_API_STAT_CMD_ERROR, eDATAAPIconfigValue( DATA_API_CMD_READ_CASH, 0U, NULL ) );
  return;
}

void runTets_dataAPI ( void )
{
  UnitySetTestFile( "test_dataAPI.c" );
  UnityDefaultTestRun( test_xDATAAPIgetEventGroup, "Get API event group", 0U );
  UnityDefaultTestRun( test_eDATAAPIlogLoad, "LOG private", 0U );
  UnityDefaultTestRun( test_vDATAAPIincLogSize, "LOG size API", 0U );
  UnityDefaultTestRun( test_eDATAAPIlogPointer, "LOG pointer API", 0U );
  UnityDefaultTestRun( test_eDATAAPIlog, "LOG API", 0U );
  UnityDefaultTestRun( test_eDATAAPIchart, "Charts API", 0U );
  UnityDefaultTestRun( test_eDATAAPIconfigAtrib, "Configs attributes API", 0U );
  UnityDefaultTestRun( test_eDATAAPIconfig, "Configs API", 0U );
  UnityDefaultTestRun( test_eDATAAPIconfigValue, "Configs value API", 0U );
  UnityDefaultTestRun( test_eDATAAPIfreeData, "Free data API", 0U );
  UnityDefaultTestRun( test_eDATAAPIpassword, "Password API", 0U );
  return;
}

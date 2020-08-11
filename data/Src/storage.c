/*
 * storage.c
 *
 *  Created on: Jun 11, 2020
 *      Author: photo_Mickey
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "storage.h"
#include "chart.h"
/*----------------------- Structures ----------------------------------------------------------------*/
/*----------------------- Constant ------------------------------------------------------------------*/
/*----------------------- Variables -----------------------------------------------------------------*/
/*----------------------- Functions -----------------------------------------------------------------*/
uint8_t uConfigToBlob ( eConfigReg* reg, uint8_t* blob );
uint8_t uBlobToConfig ( eConfigReg* reg, uint8_t* blob );
uint8_t uChartToBlob ( eChartData* chart, uint8_t* blob );
uint8_t uBlobToChart ( eChartData* chart, uint8_t* blob );

uint8_t uFix16ToBlob ( fix16_t val, uint8_t* blob );
uint8_t uBlobToFix16 ( fix16_t* val, uint8_t* blob );
uint8_t uUint16ToBlob( uint16_t val, uint8_t* blob );
uint8_t uBlobToUint16 ( uint16_t* val, uint8_t* blob );
uint8_t uBlobToUint32 ( uint32_t* val, uint8_t* blob );
uint8_t uUint32ToBlob ( uint32_t val, uint8_t* blob );
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PRIVATE -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
uint8_t uFix16ToBlob ( fix16_t val, uint8_t* blob )
{
  uint8_t count = 0U;

  blob[count++] = ( uint8_t )( ( uint32_t )val );
  blob[count++] = ( uint8_t )( ( uint32_t )val >> 8U );
  blob[count++] = ( uint8_t )( ( uint32_t )val >> 16U );
  blob[count++] = ( uint8_t )( ( uint32_t )val >> 24U );

  return count;
}

uint8_t uBlobToFix16 ( fix16_t* val, uint8_t* blob )
{
  *val = ( fix16_t )( ( uint32_t )blob[0U] | ( ( uint32_t )val >> 8U ) | ( ( uint32_t )val >> 16U ) | ( ( uint32_t )val >> 24U ) );
  return 4U;
}

uint8_t uUint16ToBlob ( uint16_t val, uint8_t* blob )
{
  uint8_t count = 0U;

  blob[count++] = ( uint8_t )  val;
  blob[count++] = ( uint8_t )( val >> 8U );

  return count;
}

uint8_t uBlobToUint16 ( uint16_t* val, uint8_t* blob )
{
  *val = ( uint16_t )blob[0U] | ( ( uint16_t )blob[1U] << 8U );
  return 2U;
}

uint8_t uBlobToUint32 ( uint32_t* val, uint8_t* blob )
{
  *val = ( uint32_t )blob[0U] | ( ( uint32_t )blob[1U] << 8U ) | ( ( uint32_t )blob[2U] << 16U ) | ( ( uint32_t )blob[3U] << 24U );
  return 4U;
}

uint8_t uUint32ToBlob ( uint32_t val, uint8_t* blob )
{
  uint8_t count = 0U;

  blob[count++] = ( uint8_t )  val;
  blob[count++] = ( uint8_t )( val >> 8U );
  blob[count++] = ( uint8_t )( val >> 16U );
  blob[count++] = ( uint8_t )( val >> 24U );

  return count;
}
/*
 * Convert configuration register to byte array
 * Input:  reg  - configuration register
 *         blob - target data array. Size need to be minimum CONFIG_MAX_SIZE ( from config.h )
 * Output: Size of output blob
 */
uint8_t uConfigToBlob ( eConfigReg* reg, uint8_t* blob )
{
  uint8_t i     = 0U;
  uint8_t count = 0U;
  count += uUint16ToBlob( reg->page, &blob[count] );
  count += uUint16ToBlob( reg->adr, &blob[count] );
  blob[count++] = ( uint8_t ) reg->scale;
  blob[count++] = ( uint8_t ) reg->len;
  blob[count++] = ( uint8_t ) reg->rw;
  blob[count++] = ( uint8_t ) reg->bitMapSize;
  count += uUint16ToBlob( reg->min, &blob[count] );
  count += uUint16ToBlob( reg->max, &blob[count] );
  count += uUint16ToBlob( reg->type, &blob[count] );
  for ( i=0U; i<MAX_UNITS_LENGTH; i++ )
  {
    count += uUint16ToBlob( reg->units[i], &blob[count] );
  }
  for ( i=0U; i<reg->len; i++ )
  {
    count += uUint16ToBlob( reg->value[i], &blob[count] );
  }
  for ( i=0U; i<reg->bitMapSize; i++ )
  {
    count += uUint16ToBlob( reg->bitMap[i].mask, &blob[count] );
    blob[count++] = ( uint8_t )  reg->bitMap[i].shift;
    blob[count++] = ( uint8_t )  reg->bitMap[i].min;
    blob[count++] = ( uint8_t )  reg->bitMap[i].max;
  }
  return count;
}
/*
 * Convert byte array to configuration register
 * Input:  reg  - output configuration register
 *         blob - data array
 * Output: Size of output blob
 */
uint8_t uBlobToConfig ( eConfigReg* reg, uint8_t* blob )
{
  uint8_t i     = 0U;
  uint8_t count = 0U;
  count += uBlobToUint16( &reg->page, &blob[count] );
  count += uBlobToUint16( &reg->adr, &blob[count] );
  reg->scale      = ( uint8_t ) blob[count++];
  reg->len        = ( uint8_t ) blob[count++];
  reg->rw         = ( uint8_t ) blob[count++];
  reg->bitMapSize = ( uint8_t ) blob[count++];
  count += uBlobToUint16( &reg->min, &blob[count] );
  count += uBlobToUint16( &reg->max, &blob[count] );
  count += uBlobToUint16( &reg->type, &blob[count] );
  for ( i=0U; i<MAX_UNITS_LENGTH; i++ )
  {
    count += uBlobToUint16( &reg->units[i], &blob[count] );
  }
  for ( i=0U; i<reg->len; i++ )
  {
    count += uBlobToUint16( &reg->value[i], &blob[count] );
  }
  for ( i=0U; i<reg->bitMapSize; i++ )
  {
    count += uBlobToUint16( &reg->bitMap[i].mask, &blob[count] );
    reg->bitMap[i].shift = ( uint8_t ) blob[count++];
    reg->bitMap[i].min   = ( uint8_t ) blob[count++];
    reg->bitMap[i].max   = ( uint8_t ) blob[count++];
  }
  return count;
}

uint8_t uChartToBlob( eChartData* chart, uint8_t* blob )
{
  uint8_t count = 0U;
  uint8_t i     = 0U;
  count += uFix16ToBlob( chart->xmin, &blob[count] );
  count += uFix16ToBlob( chart->xmax, &blob[count] );
  count += uFix16ToBlob( chart->ymin, &blob[count] );
  count += uFix16ToBlob( chart->ymax, &blob[count] );
  for ( i=0U; i<CHART_UNIT_LENGTH; i++ )
  {
    count += uUint16ToBlob( chart->xunit[i], &blob[count] );
  }
  for ( i=0U; i<CHART_UNIT_LENGTH; i++ )
  {
    count += uUint16ToBlob( chart->yunit[i], &blob[count] );
  }
  count += uUint16ToBlob( chart->size, &blob[count] );
  for ( i=0U; i<chart->size; i++ )
  {
    count += uFix16ToBlob( chart->dots[i].x, &blob[count] );
    count += uFix16ToBlob( chart->dots[i].y, &blob[count] );
  }
  return count;
}

uint8_t uBlobToChart ( eChartData* chart, uint8_t* blob )
{
  uint8_t count = 0U;
  uint8_t i     = 0U;
  count += uBlobToFix16(  &chart->xmin, &blob[count] );
  count += uBlobToFix16(  &chart->xmax, &blob[count] );
  count += uBlobToFix16(  &chart->ymin, &blob[count] );
  count += uBlobToFix16(  &chart->ymax, &blob[count] );
  for ( i=0U; i<CHART_UNIT_LENGTH; i++ )
  {
    count += uBlobToUint16( &chart->xunit[i], &blob[count] );
  }
  for ( i=0U; i<CHART_UNIT_LENGTH; i++ )
  {
    count += uBlobToUint16( &chart->yunit[i], &blob[count] );
  }
  count += uBlobToUint16( &chart->size, &blob[count] );
  for ( i=0U; i<chart->size; i++ )
  {
    count += uBlobToFix16( &chart->dots[i].x, &blob[count] );
    count += uBlobToFix16( &chart->dots[i].y, &blob[count] );
  }
  return count;
}
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLICK -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEwriteCharts ( void )
{
  EEPROM_STATUS res  = EEPROM_OK;
  uint8_t       i    = 0U;
  uint32_t      adr  = STORAGE_CHART_ADR * 1024U;
  uint8_t       size = 0U;
  uint8_t       adrForm[3U] = { 0x00U, 0x00U, 0x00U };
  uint8_t       buffer[CHART_CHART_MAX_SIZE + 1U];

  for ( i=0U; i<CHART_NUMBER; i++ )
  {
    vEEPROMformAdr( adr, adrForm );
    size = uChartToBlob( charts[i], &buffer[1U] );
    buffer[0U] = size;
    res = eEEPROMWriteMemory( adrForm, buffer, ( size + 1U ) );
    adr += size + 1U;
    if ( res != EEPROM_OK )
    {
      break;
    }
  }
  return res;
}

EEPROM_STATUS eSTORAGEreadCharts( void )
{
  EEPROM_STATUS res  = EEPROM_OK;
  uint8_t       i    = 0U;
  uint8_t       size = 0U;
  uint8_t       calc = 0U;
  uint32_t      adr  = STORAGE_CHART_ADR * 1024U;
  uint8_t       adrForm[3U] = { 0x00U, 0x00U, 0x00U };
  uint8_t       buffer[CHART_CHART_MAX_SIZE];

  for ( i=0U; i<SETTING_REGISTER_NUMBER; i++ )
  {
    vEEPROMformAdr( adr, adrForm );
    res = eEEPROMReadMemory( adrForm, &size, 1U );
    if ( ( res == EEPROM_OK ) && ( size > 0U ) && ( size < CHART_CHART_MAX_SIZE ) )
    {
      vEEPROMformAdr( ++adr, adrForm );
      res = eEEPROMReadMemory( adrForm, buffer, size );
      if ( res == EEPROM_OK )
      {
        adr += size;
        calc = uBlobToChart( charts[i], buffer );
        if ( calc != size )
        {
          res = EEPROM_ERROR;
          break;
        }
      }
      else
      {
        break;
      }
    }
    else
    {
      res = EEPROM_ERROR;
      break;
    }
  }
  return res;
}
/*
EEPROM_STATUS eSTORAGEwriteLogRecord( LOG_RECORD_TYPE* record )
{
  EEPROM_STATUS res         = EEPROM_OK;
  uint8_t       size        = 0U;
  uint8_t       adrForm[3U] = { 0x00U, 0x00U, 0x00U };
  uint32_t      adr         = ( STORAGE_LOG_ADR * 1024U ) + 1U + ( record->number * LOG_RECORD_SIZE );
  uint8_t       buffer[LOG_RECORD_SIZE];

  vEEPROMformAdr( adr, adrForm );
  size += uUint32ToBlob( record->time, &buffer[size] );
  buffer[size++] = ( uint8_t )( record->event.type );
  buffer[size++] = ( uint8_t )( record->event.action );
  res  = eEEPROMWriteMemory( adrForm, buffer , size );

  return res;
}

EEPROM_STATUS eSTORAGEwriteLogQuant( uint16_t quant )
{
  EEPROM_STATUS res         = EEPROM_OK;
  uint8_t       adrForm[3U] = { 0x00U, 0x00U, 0x00U };
  uint32_t      adr         = STORAGE_LOG_ADR * 1024U;
  uint8_t       buffer[2U]  = { 0x00U, 0x00U };
  uint8_t       size        = 0U;

  vEEPROMformAdr( adr, adrForm );
  size = uUint16ToBlob( quant, buffer );
  res  = eEEPROMWriteMemory( adrForm, buffer , size );
  return res;
}

EEPROM_STATUS eSTORAGEwriteLog( LOG_TYPE* log )
{
  EEPROM_STATUS res         = EEPROM_OK;
  uint16_t      i           = 0U;

  res  = eSTORAGEwriteLogQuant( log->quant );
  if ( res == EEPROM_OK )
  {
    for ( i=0U; i<log->quant; i++ )
    {
      res = eSTORAGEwriteLogRecord( &log->records[i] );
      if ( res != EEPROM_ERROR )
      {
        break;
      }
    }
  }

  return res;
}

EEPROM_STATUS eSTORAGEreadLog( LOG_TYPE* log )
{
  EEPROM_STATUS res         = EEPROM_OK;
  uint8_t       adrForm[3U] = { 0x00U, 0x00U, 0x00U };
  uint32_t      adr         = STORAGE_LOG_ADR * 1024U;
  uint16_t      i           = 0U;
  uint8_t       buffer[7U]  = { 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U };

  vEEPROMformAdr( adr, adrForm );
  res = eEEPROMReadMemory( adrForm, buffer, 2U );
  if ( res == EEPROM_OK )
  {
    adr += uBlobToUint16( &log->quant, buffer );
    for ( i=0U; i<log->quant; i++ )
    {
      vEEPROMformAdr( adr, adrForm );
      res = eEEPROMReadMemory( adrForm, buffer, 7U );
      if ( res != EEPROM_OK )
      {
        break;
      }
      log->records[i].number = i;
      adr += uBlobToUint32( &log->records[i].time, &buffer[0U] );
      log->records[i].event.type   = buffer[6U];
      log->records[i].event.action = buffer[6U];
      adr += 3U;
    }
  }
  return res;
}
*/
EEPROM_STATUS eSTORAGEwriteConfigs ( void )
{
  EEPROM_STATUS res  = EEPROM_OK;
  uint8_t       i    = 0U;
  uint32_t      adr  = STORAGE_CONFIG_ADR * 1024U;
  uint8_t       size = 0U;
  uint8_t       adrForm[3U] = { 0x00U, 0x00U, 0x00U };
  uint8_t       buffer[CONFIG_MAX_SIZE + 1U];

  for( i=0U; i<SETTING_REGISTER_NUMBER; i++ )
  {
    vEEPROMformAdr( adr, adrForm );
    size = uConfigToBlob( configReg[i], &buffer[1U] );
    buffer[0U] = size;
    res = eEEPROMWriteMemory( adrForm, buffer, ( size + 1U ) );
    adr += size + 1U;
    if ( res != EEPROM_OK )
    {
      break;
    }
  }
  return res;
}

EEPROM_STATUS eSTORAGEreadConfigs( void )
{
  EEPROM_STATUS res  = EEPROM_OK;
  uint8_t       i    = 0U;
  uint8_t       size = 0U;
  uint8_t       calc = 0U;
  uint32_t      adr  = STORAGE_CONFIG_ADR * 1024U;
  uint8_t       adrForm[3U] = { 0x00U, 0x00U, 0x00U };
  uint8_t       buffer[CONFIG_MAX_SIZE];

  for ( i=0U; i<SETTING_REGISTER_NUMBER; i++ )
  {
    vEEPROMformAdr( adr, adrForm );
    res = eEEPROMReadMemory( adrForm, &size, 1U );
    if ( ( res == EEPROM_OK ) && ( size > 0U ) && ( size < CONFIG_MAX_SIZE ) )
    {
      vEEPROMformAdr( ++adr, adrForm );
      res = eEEPROMReadMemory( adrForm, buffer, size );
      if ( res == EEPROM_OK )
      {
        adr += size;
        calc = uBlobToConfig( configReg[i], buffer );
        if ( calc != size )
        {
          res = EEPROM_ERROR;
          break;
        }
      }
      else
      {
        break;
      }
    }
    else
    {
      res = EEPROM_ERROR;
      break;
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/


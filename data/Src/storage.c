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

uint8_t uFix16ToBlob ( fix16_t val, uint8_t* blob );
uint8_t uBlobToFix16 ( fix16_t* val, uint8_t* blob );
uint8_t uUint16ToBlob( uint16_t val, uint8_t* blob );
uint8_t uBlobToUint16 ( uint16_t* val, uint8_t* blob );
uint8_t uBlobToUint32 ( uint32_t* val, uint8_t* blob );
uint8_t uUint32ToBlob ( uint32_t val, uint8_t* blob );
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PRIVATE -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
uint8_t uUint16ToBlob ( uint16_t val, uint8_t* blob )
{
  blob[0U] = ( uint8_t )  val;
  blob[1U] = ( uint8_t )( val >> 8U );

  return 2U;
}

uint8_t uBlobToUint16 ( uint16_t* val, uint8_t* blob )
{
  *val = ( uint16_t )blob[0U] | ( ( uint16_t )blob[1U] << 8U );
  return 2U;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uBlobToUint32 ( uint32_t* val, uint8_t* blob )
{
  *val = ( uint32_t )blob[0U] | ( ( uint32_t )blob[1U] << 8U ) | ( ( uint32_t )blob[2U] << 16U ) | ( ( uint32_t )blob[3U] << 24U );
  return 4U;
}

uint8_t uUint32ToBlob ( uint32_t val, uint8_t* blob )
{
  blob[0U] = ( uint8_t )  val;
  blob[1U] = ( uint8_t )( val >> 8U );
  blob[2U] = ( uint8_t )( val >> 16U );
  blob[3U] = ( uint8_t )( val >> 24U );

  return 4U;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uFix16ToBlob ( fix16_t val, uint8_t* blob )
{
  return uUint32ToBlob( ( uint32_t )val, blob );
}

uint8_t uBlobToFix16 ( fix16_t* val, uint8_t* blob )
{
  return uBlobToUint32( ( ( uint32_t* )val ), blob );
}
/*---------------------------------------------------------------------------------------------------*/
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

  blob[count++] = ( uint8_t ) reg->scale;
  for ( i=0U; i<reg->atrib->len; i++ )
  {
    count += uUint16ToBlob( reg->value[i], &blob[count] );
  }
  for ( i=0U; i<MAX_UNITS_LENGTH; i++ )
  {
    count += uUint16ToBlob( reg->units[i], &blob[count] );
  }
  for ( i=0U; i<reg->atrib->bitMapSize; i++ )
  {
    count += uUint16ToBlob( reg->bitMap[i].mask, &blob[count] );
    blob[count++] = ( uint8_t )  reg->bitMap[i].shift;
  }
  return count;
}
/*---------------------------------------------------------------------------------------------------*/
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

  reg->scale      = ( uint8_t ) blob[count++];
  for ( i=0U; i<reg->atrib->len; i++ )
  {
    count += uBlobToUint16( &reg->value[i], &blob[count] );
  }
  for ( i=0U; i<MAX_UNITS_LENGTH; i++ )
  {
    count += uBlobToUint16( &reg->units[i], &blob[count] );
  }
  for ( i=0U; i<reg->atrib->bitMapSize; i++ )
  {
    count += uBlobToUint16( &reg->bitMap[i].mask, &blob[count] );
    reg->bitMap[i].shift = ( uint8_t ) blob[count++];
  }
  return count;
}
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLICK -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEwriteCharts ( void )
{
  EEPROM_STATUS res        = EEPROM_OK;
  uint8_t       i          = 0U;
  uint8_t       j          = 0U;
  uint32_t      adr        = STORAGE_CHART_ADR;
  uint8_t       size       = 0U;
  uint8_t       len        = 0U;
  uint8_t       buffer[4U] = { 0U, 0U, 0U, 0U };

  for ( i=0U; i<CHART_NUMBER; i++ )
  {
    size  = 0U;
    len   = uFix16ToBlob( charts[i]->xmin, buffer );
    res   = eEEPROMWriteMemory( ( adr + size + 1U ), buffer, len );
    size += len;
    len   = uFix16ToBlob( charts[i]->xmax, buffer );
    res   = eEEPROMWriteMemory( ( adr + size + 1U ), buffer, len );
    size += len;
    len   = uFix16ToBlob( charts[i]->ymin, buffer );
    res   = eEEPROMWriteMemory( ( adr + size + 1U ), buffer, len );
    size += len;
    len   = uFix16ToBlob( charts[i]->ymax, buffer );
    res   = eEEPROMWriteMemory( ( adr + size + 1U ), buffer, len );
    size += len;
    for ( j=0U; j<CHART_UNIT_LENGTH; j++ )
    {
      len   = uUint16ToBlob( charts[i]->xunit[j], buffer );
      res   = eEEPROMWriteMemory( ( adr + size + 1U ), buffer, len );
      size += len;
    }
    for ( j=0U; j<CHART_UNIT_LENGTH; j++ )
    {
      len  = uUint16ToBlob( charts[i]->yunit[j], buffer );
      res   = eEEPROMWriteMemory( ( adr + size + 1U ), buffer, len );
      size += len;
    }
    len   = uUint16ToBlob( charts[i]->size, buffer );
    res   = eEEPROMWriteMemory( ( adr + size + 1U ), buffer, len );
    size += len;
    for ( j=0U; j<charts[i]->size; j++ )
    {
      len   = uFix16ToBlob( charts[i]->dots[j].x, buffer );
      res   = eEEPROMWriteMemory( ( adr + size + 1U ), buffer, len );
      size += len;
      len   = uFix16ToBlob( charts[i]->dots[j].y, buffer );
      res   = eEEPROMWriteMemory( ( adr + size + 1U ), buffer, len );
      size += len;
    }
    res  = eEEPROMWriteMemory( adr, &size, 1U );
    adr += size + 1U;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEreadCharts( void )
{
  EEPROM_STATUS res        = EEPROM_OK;
  uint8_t       i          = 0U;
  uint8_t       j          = 0U;
  uint8_t       len        = 0U;
  uint8_t       size       = 0U;
  uint8_t       calc       = 0U;
  uint32_t      adr        = STORAGE_CHART_ADR;
  uint8_t       buffer[4U] = { 0U, 0U, 0U, 0U };

  for ( i=0U; i<CHART_NUMBER; i++ )
  {
    res = eEEPROMReadMemory( adr, &calc, 1U );
    if ( ( res == EEPROM_OK ) && ( calc > 0U ) && ( calc < CHART_CHART_SIZE ) )
    {
      size = 0U;
      adr++;
      res   = eEEPROMReadMemory( ( adr + size ), buffer, 4U );
      len   = uBlobToFix16( &charts[i]->xmin, buffer );
      size += len;
      res   = eEEPROMReadMemory( ( adr + size ), buffer, 4U );
      len   = uBlobToFix16( &charts[i]->xmax, buffer );
      size += len;
      res   = eEEPROMReadMemory( ( adr + size ), buffer, 4U );
      len   = uBlobToFix16( &charts[i]->ymin, buffer );
      size += len;
      res   = eEEPROMReadMemory( ( adr + size ), buffer, 4U );
      len   = uBlobToFix16( &charts[i]->ymax, buffer );
      size += len;
      for ( j=0U; j<CHART_UNIT_LENGTH; j++ )
      {
	res   = eEEPROMReadMemory( ( adr + size ), buffer, 4U );
	len   = uBlobToUint16( &charts[i]->xunit[j], buffer );
	size += len;
      }
      for ( j=0U; j<CHART_UNIT_LENGTH; j++ )
      {
	res   = eEEPROMReadMemory( ( adr + size ), buffer, 4U );
	len   = uBlobToUint16( &charts[i]->yunit[j], buffer );
	size += len;
      }
      res   = eEEPROMReadMemory( ( adr + size ), buffer, 4U );
      len   = uBlobToUint16( &charts[i]->size, buffer );
      size += len;
      for ( j=0U; j<charts[i]->size; j++ )
      {
	res   = eEEPROMReadMemory( ( adr + size ), buffer, 4U );
	len   = uBlobToFix16( &charts[i]->dots[j].x, buffer );
	size += len;
	res   = eEEPROMReadMemory( ( adr + size ), buffer, 4U );
	len   = uBlobToFix16( &charts[i]->dots[j].y, buffer );
	size += len;
      }
      adr += size;

      if ( calc != size )
      {
        res = EEPROM_ERROR;
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
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEwriteConfigs ( void )
{
  EEPROM_STATUS res  = EEPROM_OK;
  uint8_t       i    = 0U;
  uint32_t      adr  = STORAGE_CONFIG_ADR;
  uint8_t       size = 0U;
  uint8_t       buffer[CONFIG_MAX_SIZE + 1U];

  for( i=0U; i<SETTING_REGISTER_NUMBER; i++ )
  {
    size       = uConfigToBlob( configReg[i], &buffer[1U] );
    if ( ( adr + size ) > ( STORAGE_CONFIG_ADR + CONFIG_TOTAL_SIZE ) )
    {
      res = EEPROM_ADR_ERROR;
      break;
    }
    buffer[0U] = size;
    res        = eEEPROMWriteMemory( adr, buffer, ( size + 1U ) );
    adr       += size + 1U;
    if ( res != EEPROM_OK )
    {
      break;
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEdeleteConfigs( void )
{
  EEPROM_STATUS res        = EEPROM_OK;
  uint32_t      i          = 0U;
  uint32_t      adr        = STORAGE_CONFIG_ADR;
  uint8_t       buffer[1U] = { 0U };

  for ( i=0U; i<CONFIG_TOTAL_SIZE; i++ )
  {
    res = eEEPROMWriteMemory( adr, buffer, 1U );
    adr++;
    if ( res != EEPROM_OK )
    {
      break;
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEreadConfigs( void )
{
  EEPROM_STATUS res  = EEPROM_OK;
  uint8_t       i    = 0U;
  uint8_t       size = 0U;
  uint8_t       calc = 0U;
  uint32_t      adr  = STORAGE_CONFIG_ADR;
  uint8_t       buffer[CONFIG_MAX_SIZE];

  for ( i=0U; i<SETTING_REGISTER_NUMBER; i++ )
  {
    res = eEEPROMReadMemory( adr, &size, 1U );
    if ( ( res == EEPROM_OK ) && ( size > 0U ) && ( size < CONFIG_MAX_SIZE ) )
    {
      adr++;
      res = eEEPROMReadMemory( adr, buffer, size );
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

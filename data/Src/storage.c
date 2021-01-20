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
uint8_t uConfigToBlob ( const eConfigReg* reg, uint8_t* blob );
uint8_t uBlobToConfig ( eConfigReg* reg, const uint8_t* blob );
uint8_t uFix16ToBlob ( fix16_t val, uint8_t* blob );
uint8_t uBlobToFix16 ( fix16_t* val, const uint8_t* blob );
uint8_t uUint16ToBlob ( uint16_t val, uint8_t* blob );
uint8_t uBlobToUint16 ( uint16_t* val, const uint8_t* blob );
uint8_t uBlobToUint32 ( uint32_t* val, const uint8_t* blob );
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

uint8_t uBlobToUint16 ( uint16_t* val, const uint8_t* blob )
{
  *val = ( uint16_t )blob[0U] | ( ( uint16_t )blob[1U] << 8U );
  return 2U;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uBlobToUint32 ( uint32_t* val, const uint8_t* blob )
{
  *val = ( uint32_t )blob[0U] | ( ( uint32_t )blob[1U] << 8U ) | ( ( uint32_t )blob[2U] << 16U ) | ( ( uint32_t )blob[3U] << 24U );
  return 4U;
}

uint8_t uUint32ToBlob ( uint32_t val, uint8_t* blob )
{
  blob[0U] = ( uint8_t )( val        );
  blob[1U] = ( uint8_t )( val >> 8U  );
  blob[2U] = ( uint8_t )( val >> 16U );
  blob[3U] = ( uint8_t )( val >> 24U );
  return 4U;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uFix16ToBlob ( fix16_t val, uint8_t* blob )
{
  return uUint32ToBlob( ( uint32_t )val, blob );
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uBlobToFix16 ( fix16_t* val, const uint8_t* blob )
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
uint8_t uConfigToBlob ( const eConfigReg* reg, uint8_t* blob )
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
  return count;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Convert byte array to configuration register
 * Input:  reg  - output configuration register
 *         blob - data array
 * Output: Size of output blob
 */
uint8_t uBlobToConfig ( eConfigReg* reg, const uint8_t* blob )
{
  uint8_t i     = 0U;
  uint8_t count = 0U;
  reg->scale = ( uint8_t )blob[count++];
  for ( i=0U; i<reg->atrib->len; i++ )
  {
    count += uBlobToUint16( &reg->value[i], &blob[count] );
  }
  for ( i=0U; i<MAX_UNITS_LENGTH; i++ )
  {
    count += uBlobToUint16( &reg->units[i], &blob[count] );
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
  uint8_t       buffer[4U] = { 0U };
  for ( i=0U; i<CHART_NUMBER; i++ )
  {
    size  = 0U;
    len   = uFix16ToBlob( charts[i]->xmin, buffer );
    res   = eEEPROMwriteMemory( ( adr + size + 1U ), buffer, len );
    size += len;
    len   = uFix16ToBlob( charts[i]->xmax, buffer );
    res   = eEEPROMwriteMemory( ( adr + size + 1U ), buffer, len );
    size += len;
    len   = uFix16ToBlob( charts[i]->ymin, buffer );
    res   = eEEPROMwriteMemory( ( adr + size + 1U ), buffer, len );
    size += len;
    len   = uFix16ToBlob( charts[i]->ymax, buffer );
    res   = eEEPROMwriteMemory( ( adr + size + 1U ), buffer, len );
    size += len;
    for ( j=0U; j<CHART_UNIT_LENGTH; j++ )
    {
      len   = uUint16ToBlob( charts[i]->xunit[j], buffer );
      res   = eEEPROMwriteMemory( ( adr + size + 1U ), buffer, len );
      size += len;
    }
    for ( j=0U; j<CHART_UNIT_LENGTH; j++ )
    {
      len  = uUint16ToBlob( charts[i]->yunit[j], buffer );
      res   = eEEPROMwriteMemory( ( adr + size + 1U ), buffer, len );
      size += len;
    }
    len   = uUint16ToBlob( charts[i]->size, buffer );
    res   = eEEPROMwriteMemory( ( adr + size + 1U ), buffer, len );
    size += len;
    for ( j=0U; j<charts[i]->size; j++ )
    {
      len   = uFix16ToBlob( charts[i]->dots[j].x, buffer );
      res   = eEEPROMwriteMemory( ( adr + size + 1U ), buffer, len );
      size += len;
      len   = uFix16ToBlob( charts[i]->dots[j].y, buffer );
      res   = eEEPROMwriteMemory( ( adr + size + 1U ), buffer, len );
      size += len;
    }
    res  = eEEPROMwriteMemory( adr, &size, 1U );
    adr += size + 1U;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEreadCharts ( void )
{
  EEPROM_STATUS res        = EEPROM_OK;
  uint8_t       i          = 0U;
  uint8_t       j          = 0U;
  uint8_t       len        = 0U;
  uint8_t       size       = 0U;
  uint8_t       calc       = 0U;
  uint32_t      adr        = STORAGE_CHART_ADR;
  uint8_t       buffer[4U] = { 0U };
  for ( i=0U; i<CHART_NUMBER; i++ )
  {
    res = eEEPROMreadMemory( adr, &calc, 1U );
    if ( ( res == EEPROM_OK ) && ( calc > 0U ) && ( calc < CHART_CHART_SIZE ) )
    {
      size = 0U;
      adr++;
      res   = eEEPROMreadMemory( ( adr + size ), buffer, 4U );
      len   = uBlobToFix16( &charts[i]->xmin, buffer );
      size += len;
      res   = eEEPROMreadMemory( ( adr + size ), buffer, 4U );
      len   = uBlobToFix16( &charts[i]->xmax, buffer );
      size += len;
      res   = eEEPROMreadMemory( ( adr + size ), buffer, 4U );
      len   = uBlobToFix16( &charts[i]->ymin, buffer );
      size += len;
      res   = eEEPROMreadMemory( ( adr + size ), buffer, 4U );
      len   = uBlobToFix16( &charts[i]->ymax, buffer );
      size += len;
      for ( j=0U; j<CHART_UNIT_LENGTH; j++ )
      {
	      res   = eEEPROMreadMemory( ( adr + size ), buffer, 4U );
	      len   = uBlobToUint16( &charts[i]->xunit[j], buffer );
	      size += len;
      }
      for ( j=0U; j<CHART_UNIT_LENGTH; j++ )
      {
	      res   = eEEPROMreadMemory( ( adr + size ), buffer, 4U );
	      len   = uBlobToUint16( &charts[i]->yunit[j], buffer );
	      size += len;
      }
      res   = eEEPROMreadMemory( ( adr + size ), buffer, 4U );
      len   = uBlobToUint16( &charts[i]->size, buffer );
      size += len;
      for ( j=0U; j<charts[i]->size; j++ )
      {
	      res   = eEEPROMreadMemory( ( adr + size ), buffer, 4U );
	      len   = uBlobToFix16( &charts[i]->dots[j].x, buffer );
	      size += len;
	      res   = eEEPROMreadMemory( ( adr + size ), buffer, 4U );
	      len   = uBlobToFix16( &charts[i]->dots[j].y, buffer );
	      size += len;
      }
      adr += size;

      if ( calc != size )
      {
        res = EEPROM_SIZE_ERROR;
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
EEPROM_STATUS eSTORAGEwriteConfigs ( void )
{
  EEPROM_STATUS res                          = EEPROM_OK;
  uint8_t       i                            = 0U;
  uint32_t      adr                          = STORAGE_CONFIG_ADR;
  uint8_t       size                         = 0U;
  uint8_t       buffer[CONFIG_MAX_SIZE + 1U] = { 0U };
  for( i=0U; i<SETTING_REGISTER_NUMBER; i++ )
  {
    size       = uConfigToBlob( configReg[i], &buffer[1U] );
    if ( ( adr + size ) > ( STORAGE_CONFIG_ADR + CONFIG_TOTAL_SIZE ) )
    {
      res = EEPROM_ADR_ERROR;
      break;
    }
    buffer[0U] = size;
    res        = eEEPROMwriteMemory( adr, buffer, ( size + 1U ) );
    adr       += size + 1U;
    if ( res != EEPROM_OK )
    {
      break;
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEdeleteConfigs ( void )
{
  EEPROM_STATUS res        = EEPROM_OK;
  uint32_t      i          = 0U;
  uint32_t      adr        = STORAGE_CONFIG_ADR;
  uint8_t       buffer[1U] = { 0U };
  for ( i=0U; i<CONFIG_TOTAL_SIZE; i++ )
  {
    res = eEEPROMwriteMemory( adr, buffer, 1U );
    adr++;
    if ( res != EEPROM_OK )
    {
      break;
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEreadConfigs ( void )
{
  EEPROM_STATUS res                     = EEPROM_OK;
  uint8_t       i                       = 0U;
  uint8_t       size                    = 0U;
  uint8_t       calc                    = 0U;
  uint32_t      adr                     = STORAGE_CONFIG_ADR;
  uint8_t       buffer[CONFIG_MAX_SIZE] = { 0U };
  for ( i=0U; i<SETTING_REGISTER_NUMBER; i++ )
  {
    res = eEEPROMreadMemory( adr, &size, 1U );
    if ( ( res == EEPROM_OK ) && ( size > 0U ) && ( size < CONFIG_MAX_SIZE ) )
    {
      adr++;
      res = eEEPROMreadMemory( adr, buffer, size );
      if ( res == EEPROM_OK )
      {
        adr += size;
        calc = uBlobToConfig( configReg[i], buffer );
        if ( calc != size )
        {
          res = EEPROM_SIZE_ERROR;
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
EEPROM_STATUS eSTORAGEreadLogPointer ( uint16_t* pointer )
{
  EEPROM_STATUS res      = EEPROM_OK;
  uint8_t       data[2U] = { 0U };
  res = eEEPROMreadMemory( STORAGE_LOG_POINTER_ADR, data, 2U );
  if ( res == EEPROM_OK )
  {
    *pointer = ( ( ( uint16_t )data[0U] ) << 8U ) | ( ( uint16_t )data[1U] );
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEwriteLogPointer ( uint16_t pointer )
{
  uint8_t data[2U] = { 0U };
  data[0U] = ( uint8_t )( pointer >> 8U );
  data[1U] = ( uint8_t )( pointer       );
  return eEEPROMwriteMemory( STORAGE_LOG_POINTER_ADR, data, 2U );
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEwriteLogRecord ( uint16_t adr, const LOG_RECORD_TYPE* record )
{
  uint8_t data[LOG_RECORD_SIZE] = { 0x00U };
  data[0U] = record->event.type;
  data[1U] = record->event.action;
  data[2U] = ( uint8_t )( record->time >> 24U );
  data[3U] = ( uint8_t )( record->time >> 16U );
  data[4U] = ( uint8_t )( record->time >> 8U  );
  data[5U] = ( uint8_t )( record->time        );
  return eEEPROMwriteMemory( ( STORAGE_LOG_ADR + ( adr * LOG_RECORD_SIZE ) ), data, LOG_RECORD_SIZE );
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEreadLogRecord ( uint16_t adr, LOG_RECORD_TYPE* record )
{
  EEPROM_STATUS res                   = EEPROM_OK;
  uint8_t       data[LOG_RECORD_SIZE] = { 0x00U };
  res = eEEPROMreadMemory( ( STORAGE_LOG_ADR + ( adr * LOG_RECORD_SIZE ) ), data, LOG_RECORD_SIZE );
  if ( res == EEPROM_OK )
  {
    record->event.type   = data[0U];
    record->event.action = data[1U];
    record->time         = ( ( ( uint32_t )data[2U] ) << 24U ) |
                           ( ( ( uint32_t )data[3U] ) << 16U ) |
                           ( ( ( uint32_t )data[4U] ) << 8U  ) |
                             ( ( uint32_t )data[5U] );
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEsavePassword ( void )
{
  uint8_t buffer[PASSWORD_SIZE] = { 0U };
  buffer[0U] = ( uint8_t )( systemPassword.status       );
  buffer[1U] = ( uint8_t )( systemPassword.data   >> 8U );
  buffer[2U] = ( uint8_t )( systemPassword.data         );
  return eEEPROMwriteMemory( STORAGE_PASSWORD_ADR, buffer, PASSWORD_SIZE );
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEloadPassword ( void )
{
  uint8_t       buffer[PASSWORD_SIZE] = { 0U };
  EEPROM_STATUS res                   = EEPROM_OK;
  res = eEEPROMreadMemory( STORAGE_PASSWORD_ADR, buffer, PASSWORD_SIZE );
  if ( res == EEPROM_OK )
  {
    systemPassword.status = buffer[0U];
    systemPassword.data   = ( ( ( uint16_t )buffer[1U] ) << 8U ) | ( ( uint16_t )buffer[2U] );
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEreadFreeData ( FREE_DATA_ADR n )
{
  EEPROM_STATUS res      = EEPROM_OK;
  uint8_t       data[2U] = { 0U };
  res = eEEPROMreadMemory( ( STORAGE_FREE_DATA_ADR + 2 * n ), data, 2U );
  if ( res == EEPROM_OK )
  {
    *freeDataArray[n] = ( ( ( uint16_t )data[0U] ) << 8U ) | ( ( uint16_t )data[0U] );
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEsaveFreeData ( FREE_DATA_ADR n )
{
  return eEEPROMwriteMemory( ( STORAGE_FREE_DATA_ADR + 2 * n ), ( uint8_t* )freeDataArray[n], 2U );
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEsetFreeData ( FREE_DATA_ADR n, const uint16_t* data )
{
  *freeDataArray[n] = *data;
  return eSTORAGEsaveFreeData( ( STORAGE_FREE_DATA_ADR + 2 * n ) );
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEreadMeasurement ( uint16_t adr, uint8_t length, uint16_t* data )
{
  return eEEPROMreadMemory( ( STORAGE_MEASUREMENT_ADR + adr * 2U * length ), ( uint8_t* )data, ( uint16_t )( length * 2U ) );
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEeraseMeasurement ( void )
{
  uint8_t data[1U] = { 0U };
  return eEEPROMwriteMemory( STORAGE_MEASUREMENT_SR_ADR, data, 1U );
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEaddMeasurement ( uint16_t adr, uint8_t length, const uint16_t* data )
{
  EEPROM_STATUS res = EEPROM_OK;
  res = eEEPROMwriteMemory( STORAGE_MEASUREMENT_ADR, ( uint8_t* )data, ( length * 2 ) );
  if ( res == EEPROM_OK )
  {
    res = eEEPROMwriteMemory( STORAGE_MEASUREMENT_SR_ADR, &length, 1U );
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEreadMeasurementCounter ( uint16_t* adr )
{
  return eEEPROMreadMemory( STORAGE_MEASUREMENT_SR_ADR, ( uint8_t* )adr, 2U );
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

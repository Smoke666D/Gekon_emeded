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
#if defined ( UNIT_TEST )
  const EEPROM_TYPE* storageEEPROM = NULL;
#else
  static const EEPROM_TYPE* storageEEPROM = NULL;
#endif
/*----------------------- Constant ------------------------------------------------------------------*/
const uint8_t protectedConfigs[STORAGE_PROTECTED_SIZE] = {
  RELEASE_DATE_ADR,
  SERIAL_NUMBER_ADR
};
/*----------------------- Variables -----------------------------------------------------------------*/
uint8_t configBuffer[CONFIG_MAX_SIZE + 1U] = { 0U };
uint8_t configWriteProtection              = 1U;
/*----------------------- Functions -----------------------------------------------------------------*/
#if !defined( UNIT_TEST )
  uint8_t uConfigToBlob ( const eConfigReg* reg, uint8_t* blob );
  uint8_t uBlobToConfig ( eConfigReg* reg, const uint8_t* blob );
  uint8_t uFix16ToBlob ( fix16_t val, uint8_t* blob );
  uint8_t uBlobToFix16 ( fix16_t* val, const uint8_t* blob );
  uint8_t uUint16ToBlob ( uint16_t val, uint8_t* blob );
  uint8_t uBlobToUint32 ( uint32_t* val, const uint8_t* blob );
  uint8_t uUint32ToBlob ( uint32_t val, uint8_t* blob );
#endif
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
  uint8_t count = 0U;
  for ( uint8_t i=0U; i<reg->atrib->len; i++ )
  {
    count += uUint16ToBlob( reg->value[i], &blob[count] );
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
  uint8_t count = 0U;
  for ( uint8_t i=0U; i<reg->atrib->len; i++ )
  {
    count += uBlobToUint16( &reg->value[i], &blob[count] );
  }
  return count;
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEwriteUint32 ( uint32_t data, uint32_t* adr )
{
  EEPROM_STATUS res        = EEPROM_OK;
  uint8_t       len        = 0U;
  uint8_t       buffer[4U] = { 0U };
  len  = uUint32ToBlob( data, buffer );
  res  = eEEPROMwriteMemory( storageEEPROM, *adr, buffer, len );
  if ( res == EEPROM_OK )
  {
    *adr += len;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEreadUint32 ( uint32_t* data, uint32_t* adr )
{
  EEPROM_STATUS res        = EEPROM_OK;
  uint8_t       buffer[4U] = { 0U };
  res  = eEEPROMreadMemory( storageEEPROM, *adr, buffer, 4U );
  if ( res == EEPROM_OK )
  {
    *adr += uBlobToUint32( data, buffer );
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLICK -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEinit ( const EEPROM_TYPE* eeprom )
{
  storageEEPROM = eeprom;
  return eEEPROMInit( eeprom );
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uSTORAGEgetSize ( void )
{
  return storageEEPROM->size;
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEreadSR ( uint8_t* sr )
{
  return eEEPROMreadMemory( storageEEPROM, STORAGE_SR_ADR, sr, 1U );;
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEwriteSR ( const uint8_t* sr )
{
  return eEEPROMwriteMemory( storageEEPROM, STORAGE_SR_ADR, ( uint8_t* )sr, 1U );
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEwriteMap ( void )
{
  EEPROM_STATUS res = EEPROM_OK;
  uint32_t      adr = STORAGE_MAP_ADR;
  res = eSTORAGEwriteUint32( (uint32_t)STORAGE_CONFIG_SIZE, &adr );
  if ( res == EEPROM_OK )
  {
    res = eSTORAGEwriteUint32( (uint32_t)STORAGE_CHART_SIZE, &adr );
    if ( res == EEPROM_OK )
    {
      res = eSTORAGEwriteUint32( (uint32_t)STORAGE_FREE_DATA_SIZE, &adr );
      if ( res == EEPROM_OK )
      {
        res = eSTORAGEwriteUint32( (uint32_t)STORAGE_PASSWORD_SIZE, &adr );
        if ( res == EEPROM_OK )
        {
          res = eSTORAGEwriteUint32( (uint32_t)STORAGE_LOG_POINTER_SIZE, &adr );
          if ( res == EEPROM_OK )
          {
            res = eSTORAGEwriteUint32( (uint32_t)STORAGE_LOG_SIZE, &adr );
            if ( res == EEPROM_OK )
            {
              res = eSTORAGEwriteUint32( (uint32_t)STORAGE_MAC_SIZE, &adr );
            }
          }
        }
      }
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEreadMap ( uint32_t* output )
{
  EEPROM_STATUS res = EEPROM_OK;
  uint32_t      adr = STORAGE_MAP_ADR;
  for ( uint8_t i=0U; i<( STORAGE_MAP_SIZE / 4U ); i++ )
  {
    res = eSTORAGEreadUint32( &output[i], &adr );
    if ( res != EEPROM_OK )
    {
      break;
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uSTORAGEcheckMap ( const uint32_t* map )
{
  uint8_t res = 0U;
  if ( ( map[0U] == ( uint32_t )( STORAGE_CONFIG_SIZE      ) ) &&
       ( map[1U] == ( uint32_t )( STORAGE_CHART_SIZE       ) ) &&
       ( map[2U] == ( uint32_t )( STORAGE_FREE_DATA_SIZE   ) ) &&
       ( map[3U] == ( uint32_t )( STORAGE_PASSWORD_SIZE    ) ) &&
       ( map[4U] == ( uint32_t )( STORAGE_LOG_POINTER_SIZE ) ) &&
       ( map[5U] == ( uint32_t )( STORAGE_LOG_SIZE         ) ) &&
       ( map[6U] == ( uint32_t )( STORAGE_MAC_SIZE         ) ) )
  {
    res = 1U;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEwriteCharts ( void )
{
  EEPROM_STATUS res        = EEPROM_OK;
  uint32_t      adr        = 0U;
  uint8_t       len        = 0U;
  uint8_t       buffer[4U] = { 0U };
  for ( uint8_t i=0U; i<CHART_NUMBER; i++ )
  {
    adr  = STORAGE_CHART_ADR + ( CHART_CHART_SIZE * i );
    len  = uUint16ToBlob( charts[i]->size, buffer );
    res  = eEEPROMwriteMemory( storageEEPROM, adr, buffer, len );
    adr += len;
    if ( res == EEPROM_OK )
    {
      for ( uint8_t j=0U; j<charts[i]->size; j++ )
      {
        len  = uFix16ToBlob( charts[i]->dots[j].x, buffer );
        res  = eEEPROMwriteMemory( storageEEPROM, adr, buffer, len );
        adr += len;
        if ( res == EEPROM_OK )
        {
          len  = uFix16ToBlob( charts[i]->dots[j].y, buffer );
          res  = eEEPROMwriteMemory( storageEEPROM, adr, buffer, len );
          adr += len;
          if ( res != EEPROM_OK )
          {
            break;
          }
        }
        else
        {
          break;
        }
      }
    }
    else
    {
      break;
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEreadCharts ( void )
{
  EEPROM_STATUS res        = EEPROM_OK;
  uint8_t       len        = 0U;
  uint32_t      adr        = 0U;
  uint8_t       buffer[4U] = { 0U };
  for ( uint8_t i=0U; i<CHART_NUMBER; i++ )
  {
    adr  = STORAGE_CHART_ADR + ( CHART_CHART_SIZE * i );
    res  = eEEPROMreadMemory( storageEEPROM, adr, buffer, 2U );
    len  = uBlobToUint16( &charts[i]->size, buffer );
    adr += len;
    if ( charts[i]->size > CHART_DOTS_SIZE )
    {
      charts[i]->size = CHART_DOTS_SIZE;
    }
    if ( res == EEPROM_OK )
    {
      for ( uint16_t j=0U; j<charts[i]->size; j++ )
      {
        res  = eEEPROMreadMemory( storageEEPROM, adr, buffer, 4U );
        len  = uBlobToFix16( &charts[i]->dots[j].x, buffer );
        adr += len;
        if ( res == EEPROM_OK )
        {
          res  = eEEPROMreadMemory( storageEEPROM, adr, buffer, 4U );
          len  = uBlobToFix16( &charts[i]->dots[j].y, buffer );
          adr += len;
          if ( res != EEPROM_OK )
          {
            break;
          }
        }
        else
        {
          break;
        }
      }
    }
    else
    {
      break;
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
void vSTORAGEresetConfigWriteProtection ( void )
{
  configWriteProtection = 0U;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vSTORAGEsetConfigWriteProtection ( void )
{
  configWriteProtection = 1U;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uSTORAGEisConfigProtected ( uint8_t n )
{
  uint8_t res = 0U;
  if ( configWriteProtection > 1U ) {
    for ( uint8_t i=0U; i<STORAGE_PROTECTED_SIZE; i++ ) {
      if ( n == protectedConfigs[i] )
      {
        res = 1U;
        break;
      }
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEwriteConfigs ( void )
{
  EEPROM_STATUS res  = EEPROM_OK;
  uint32_t      adr  = STORAGE_CONFIG_ADR;
  uint8_t       size = 0U;
  for ( uint8_t i=0U; i<SETTING_REGISTER_NUMBER; i++ )
  {
    if ( uSTORAGEisConfigProtected( i ) > 0U )
    {
      size = uConfigToBlob( configReg[i], &configBuffer[1U] );
      if ( ( adr + size ) > ( STORAGE_CONFIG_ADR + CONFIG_TOTAL_SIZE ) )
      {
        res = EEPROM_ADR_ERROR;
        break;
      }
      configBuffer[0U] = size;
      res  = eEEPROMwriteMemory( storageEEPROM, adr, configBuffer, ( size + 1U ) );
      adr += size + 1U;
      if ( res != EEPROM_OK )
      {
        break;
      }
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEreadConfigs ( void )
{
  EEPROM_STATUS res  = EEPROM_OK;
  uint8_t       size = 0U;
  uint8_t       calc = 0U;
  uint32_t      adr  = STORAGE_CONFIG_ADR;
  for ( uint8_t i=0U; i<SETTING_REGISTER_NUMBER; i++ )
  {
    res = eEEPROMreadMemory( storageEEPROM, adr, &size, 1U );
    if ( ( res == EEPROM_OK ) && ( size > 0U ) && ( size < CONFIG_MAX_SIZE ) )
    {
      adr++;
      res = eEEPROMreadMemory( storageEEPROM, adr, configBuffer, size );
      if ( res == EEPROM_OK )
      {
        adr += size;
        calc = uBlobToConfig( configReg[i], configBuffer );
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
  res = eEEPROMreadMemory( storageEEPROM, STORAGE_LOG_POINTER_ADR, data, STORAGE_LOG_POINTER_SIZE );
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
  return eEEPROMwriteMemory( storageEEPROM, STORAGE_LOG_POINTER_ADR, data, STORAGE_LOG_POINTER_SIZE );
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
  return eEEPROMwriteMemory( storageEEPROM, ( STORAGE_LOG_ADR + ( adr * LOG_RECORD_SIZE ) ), data, LOG_RECORD_SIZE );
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEreadLogRecord ( uint16_t adr, LOG_RECORD_TYPE* record )
{
  EEPROM_STATUS res                   = EEPROM_OK;
  uint8_t       data[LOG_RECORD_SIZE] = { 0x00U };
  res = eEEPROMreadMemory( storageEEPROM, ( STORAGE_LOG_ADR + ( adr * LOG_RECORD_SIZE ) ), data, LOG_RECORD_SIZE );
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
  return eEEPROMwriteMemory( storageEEPROM, STORAGE_PASSWORD_ADR, buffer, PASSWORD_SIZE );
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEloadPassword ( void )
{
  uint8_t       buffer[PASSWORD_SIZE] = { 0U };
  EEPROM_STATUS res                   = EEPROM_OK;
  res = eEEPROMreadMemory( storageEEPROM, STORAGE_PASSWORD_ADR, buffer, PASSWORD_SIZE );
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
  res = eEEPROMreadMemory( storageEEPROM, ( STORAGE_FREE_DATA_ADR + ( 2U * n ) ), data, 2U );
  if ( res == EEPROM_OK )
  {
    *freeDataArray[n] = ( ( ( uint16_t )data[1U] ) << 8U ) | ( ( uint16_t )data[0U] );
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEsaveFreeData ( FREE_DATA_ADR n )
{
  return eEEPROMwriteMemory( storageEEPROM, ( STORAGE_FREE_DATA_ADR + ( 2U * n ) ), ( uint8_t* )freeDataArray[n], 2U );
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEsetFreeData ( FREE_DATA_ADR n, const uint16_t* data )
{
  *freeDataArray[n] = *data;
  return eSTORAGEsaveFreeData( n );
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEwriteMac ( uint8_t* data )
{
  return eEEPROMwriteMemory( storageEEPROM, STORAGE_MAC_ADR, data, STORAGE_MAC_SIZE );
}
/*---------------------------------------------------------------------------------------------------*/
EEPROM_STATUS eSTORAGEreadMac ( uint8_t* data )
{
  return eEEPROMreadMemory( storageEEPROM, STORAGE_MAC_ADR, data, STORAGE_MAC_SIZE );
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

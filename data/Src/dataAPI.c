/*
 * dataAPI.c
 *
 *  Created on: 27 авг. 2020 г.
 *      Author: 79110
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "dataAPI.h"
#include "semphr.h"
#include "storage.h"
#include "freeData.h"
#include "common.h"
#include "version.h"
#include "stdio.h"
/*----------------------- Structures ----------------------------------------------------------------*/
static SemaphoreHandle_t xSemaphore = NULL;
/*----------------------- Constant ------------------------------------------------------------------*/
/*----------------------- Variables -----------------------------------------------------------------*/
static TaskHandle_t* notifyTargets[NOTIFY_TARGETS_NUMBER] = { NULL };
static uint8_t       initDone                             = 0U;
static uint8_t       flTakeSource                         = 0U;
/*----------------------- Functions -----------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PRIVATE -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
void vDATAAPInotfyAll ( uint32_t value )
{
  uint8_t i = 0U;
  for ( i=0U; i<NOTIFY_TARGETS_NUMBER; i++ )
  {
    xTaskNotify( *notifyTargets[i], value, eSetValueWithOverwrite );
  }
}
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLICK -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*
 * All data initialization
 * input:  none
 * output: none
 */
void vDATAAPIdataInit ( void )
{
  EEPROM_STATUS res                                   = EEPROM_OK;
  uint8_t       sr                                    = 0xFFU;
  uint16_t      i                                     = 0U;
  uint16_t      serialBuffer[serialNumber.atrib->len];

  if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
  {
    res = eEEPROMreadMemory( STORAGE_SR_ADR, &sr, 1U );
    if ( ( sr == STORAGE_SR_EMPTY ) || ( REWRITE_ALL_EEPROM > 0U ) )
    {
      vSYSSerial( ">>EEPROM empty. All data is default.\n\r" );
      vSYSgetUniqueID16( serialBuffer );                                                   /* Get serial number */
      for ( i=0U; i<serialNumber.atrib->len; i++ )
      {
        serialNumber.value[i] = serialBuffer[i];
      }
      versionController.value[0U] = HARDWARE_VERSION;
      versionFirmware.value[0U]   = SOFTWARE_VERSION;
      res = eSTORAGEwriteConfigs();
      if ( res == EEPROM_OK )
      {
        res = eSTORAGEwriteCharts();
        if ( res == EEPROM_OK )
        {
          for ( i=0U; i<FREE_DATA_SIZE; i++ )
          {
            res = eSTORAGEsaveFreeData( i );
            if ( res == EEPROM_OK )
            {
              break;
            }
          }
          if ( res == EEPROM_OK )
          {
            res = eSTORAGEsavePassword();
            if ( res == EEPROM_OK )
            {
              sr  = 0x00U;
              res = eEEPROMwriteMemory( STORAGE_SR_ADR, &sr, 1U );
              if ( res == EEPROM_OK )
              {
                vSYSSerial( ">>EEPROM data initialization: done!\n\r" );
                initDone = 1U;
              }
            }
          }
        }
      }
      if ( initDone == 0U )
      {
        vSYSSerial( ">>EEPROM data initialization: fail!\n\r" );
      }
    }
    else
    {
      if ( eSTORAGEreadConfigs() == EEPROM_OK )
      {
        vSYSSerial( ">>EEPROM configurations read: done!\n\r" );
      }
      else
      {
        vSYSSerial( ">>EEPROM configurations read: fail!\n\r" );
      }

      if ( eSTORAGEreadCharts() == EEPROM_OK )
      {
        vSYSSerial( ">>EEPROM charts read: done!\n\r" );
      }
      else
      {
        vSYSSerial( ">>EEPROM charts read: fail!\n\r" );
      }
      for ( i=0U; i<FREE_DATA_SIZE; i++ )
      {
        res = eSTORAGEreadFreeData( i );
        if ( res != EEPROM_OK )
        {
          break;
        }
      }
      if ( res == EEPROM_OK )
      {
        vSYSSerial( ">>EEPROM free data read: done!\n\r" );
      }
      else
      {
        vSYSSerial( ">>EEPROM free data read: fail!\n\r" );
      }

      res = eSTORAGEloadPassword();
      if ( res == EEPROM_OK )
      {
        vSYSSerial( ">>EEPROM password read: done!\n\r" );
      }
      else
      {
        vSYSSerial( ">>EEPROM password read: fail!\n\r" );
      }
      initDone = 1U;
    }
    xSemaphoreGive( xSemaphore );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vDATAprintSerialNumber ( void )
{
  char      buf[36U] = { 0U };
  uint8_t   i        = 0U;
  uint8_t   j        = 0U;
  uint8_t   temp     = 0U;
  vSYSSerial( ">>Serial number: " );
  for ( i=0U; i<6U; i++ )
  {
    for ( j=0U; j<2U; j++ )
    {
      temp = ( uint8_t )( serialNumber.value[i] << ( j * 8U ) );
      sprintf( &buf[6U * i + 3U * j], "%02X:", temp );
    }
  }
  buf[35] = 0U;
  vSYSSerial( buf );
  vSYSSerial( "\n\r" );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vDATAAPIprintMemoryMap ( void )
{
  char buf[36U] = { 0U };
  vSYSSerial( "\n\r" );
  vSYSSerial("------------- EEPROM map: -------------\n\r");
  vSYSSerial("System register: ");
  sprintf( buf, "0x%06X", STORAGE_SR_ADR );
  vSYSSerial( buf );
  vSYSSerial( "( ");
  sprintf( buf, "%d", STORAGE_SR_SIZE );
  vSYSSerial( buf );
  vSYSSerial( " bytes )\n\r" );
  vSYSSerial("EWA            : ");
  sprintf( buf, "0x%06X", STORAGE_EWA_ADR );
  vSYSSerial( buf );
  vSYSSerial( "( ");
  sprintf( buf, "%d", STORAGE_WEB_SIZE );
  vSYSSerial( buf );
  vSYSSerial( " bytes )\n\r" );
  vSYSSerial("Reserve        : ");
  sprintf( buf, "0x%06X", STORAGE_RESERVE_ADR );
  vSYSSerial( buf );
  vSYSSerial( "( ");
  sprintf( buf, "%d", STORAGE_RESERVE_SIZE );
  vSYSSerial( buf );
  vSYSSerial( " bytes )\n\r" );
  vSYSSerial("Configurations : ");
  sprintf( buf, "0x%06X", STORAGE_CONFIG_ADR );
  vSYSSerial( buf );
  vSYSSerial( "( ");
  sprintf( buf, "%d", CONFIG_TOTAL_SIZE );
  vSYSSerial( buf );
  vSYSSerial( " bytes )\n\r" );
  vSYSSerial("Charts         : ");
  sprintf( buf, "0x%06X", STORAGE_CHART_ADR );
  vSYSSerial( buf );
  vSYSSerial( "( ");
  sprintf( buf, "%d", STORAGE_CHART_SIZE );
  vSYSSerial( buf );
  vSYSSerial( " bytes )\n\r" );
  vSYSSerial("Free data      : ");
  sprintf( buf, "0x%06X", STORAGE_FREE_DATA_ADR );
  vSYSSerial( buf );
  vSYSSerial( "( ");
  sprintf( buf, "%d", STORAGE_FREE_DATA_SIZE );
  vSYSSerial( buf );
  vSYSSerial( " bytes )\n\r" );
  vSYSSerial("Password       : ");
  sprintf( buf, "0x%06X", STORAGE_PASSWORD_ADR );
  vSYSSerial( buf );
  vSYSSerial( "( ");
  sprintf( buf, "%d", STORAGE_PASSWORD_SIZE );
  vSYSSerial( buf );
  vSYSSerial( " bytes )\n\r" );
  vSYSSerial("Log pointer    : ");
  sprintf( buf, "0x%06X", STORAGE_LOG_POINTER_ADR );
  vSYSSerial( buf );
  vSYSSerial( "( ");
  sprintf( buf, "%d", STORAGE_LOG_POINTER_SIZE );
  vSYSSerial( buf );
  vSYSSerial( " bytes )\n\r" );
  vSYSSerial("Log            : ");
  sprintf( buf, "0x%06X", STORAGE_LOG_ADR );
  vSYSSerial( buf );
  vSYSSerial( "( ");
  sprintf( buf, "%d", STORAGE_LOG_SIZE );
  vSYSSerial( buf );
  vSYSSerial( " bytes )\n\r" );
  vSYSSerial("Free           : ");
  sprintf( buf, "%d", ( ( EEPROM_SIZE * 1024U ) - STORAGE_REQUIRED_SIZE ) );
  vSYSSerial( buf );
  vSYSSerial( " bytes \n\r" );
  vSYSSerial("End            : ");
  sprintf( buf, "0x%06X", ( EEPROM_SIZE * 1024U ) );
  vSYSSerial( buf );
  vSYSSerial( "\n\r" );
  vSYSSerial("---------------------------------------\n\r");
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Structures initialization
 * input:  array of task for notifications
 * output: none
 */
void vDATAAPIinit ( TaskHandle_t* targets )
{
  uint8_t i = 0U;
  for ( i=0U; i<NOTIFY_TARGETS_NUMBER; i++ )
  {
    notifyTargets[i] = targets[i];
  }
  xSemaphore = xSemaphoreCreateMutex();
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Check data initialization
 * input:  none
 * output: initialization status
 */
DATA_API_STATUS eDATAAPIisInit ( void )
{
  DATA_API_STATUS res = DATA_API_STAT_INIT_ERROR;

  if ( ( xSemaphore != NULL ) && ( initDone > 0U) )
  {
    res = DATA_API_STAT_OK;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * API for charts
 * input:  cmd   - command
 *         adr   - address
 *         chart - chart structure
 * output: dtstus of operation
 * available commands:
 * 1. DATA_API_CMD_READ  - return chart with address from local storage
 * 2. DATA_API_CMD_WRITE - write chart with address to local storage
 * 3. DATA_API_CMD_INC   - none
 * 4. DATA_API_CMD_DEC   - none
 * 5. DATA_API_CMD_SAVE  - save all charts from locale storage to the EEPROM
 * 6. DATA_API_CMD_LOAD  - load all charts from the EEPROM to local storage
 * 7. DATA_API_CMD_ERASE - none
 * 8. DATA_API_CMD_ADD   - none
 */
DATA_API_STATUS eDATAAPIchart ( DATA_API_COMMAND cmd, uint16_t adr, eChartData* chart )
{
  DATA_API_STATUS res = DATA_API_STAT_OK;

  if ( adr < CHART_NUMBER )
  {
    if ( ( xSemaphore != NULL ) && ( initDone > 0U ) && ( xCHARTSemaphore != NULL) )
    {
      switch ( cmd )
      {
        case DATA_API_CMD_READ:
          *chart = *charts[adr];
          break;
        case DATA_API_CMD_WRITE:
          if ( xSemaphoreTake( xCHARTSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            flTakeSource = 1U;
            *charts[adr] = *chart;
            xSemaphoreGive( xCHARTSemaphore );
          }
          else
          {
            res = DATA_API_STAT_BUSY;
          }
          break;
        case DATA_API_CMD_SAVE:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            flTakeSource = 2U;
            if ( eSTORAGEwriteCharts() != EEPROM_OK )
            {
              res = DATA_API_STAT_EEPROM_ERROR;
            }
            else
            {
              vDATAAPInotfyAll( DATA_API_MESSAGE_REINIT );
            }
            xSemaphoreGive( xSemaphore );
          }
          else
          {
            res = DATA_API_STAT_BUSY;
          }
          break;
        case DATA_API_CMD_LOAD:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            if ( xSemaphoreTake( xCHARTSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
            {
              flTakeSource = 3U;
              if ( eSTORAGEreadCharts() != EEPROM_OK )
              {
                res = DATA_API_STAT_EEPROM_ERROR;
              }
              xSemaphoreGive( xCHARTSemaphore );
            }
            xSemaphoreGive( xSemaphore );
          }
          else
          {
            res = DATA_API_STAT_BUSY;
          }
          break;
        case DATA_API_CMD_DEC:
          res = DATA_API_STAT_CMD_ERROR;
          break;
        default:
          res = DATA_API_STAT_CMD_ERROR;
          break;
      }
    }
    else
    {
      res = DATA_API_STAT_INIT_ERROR;
    }
  }
  else
  {
    res = DATA_API_STAT_ADR_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * API for Embedded Web Application
 * input:  cmd    - command
 *         adr    - absolute address in the EEPROM
 *         data   - EWA data array
 *         length - length of the EWA data
 * output: dtstus of operation
 * available commands:
 * 1. DATA_API_CMD_READ  - none
 * 2. DATA_API_CMD_WRITE - none
 * 3. DATA_API_CMD_INC   - none
 * 4. DATA_API_CMD_DEC   - none
 * 5. DATA_API_CMD_SAVE  - save data to the EEPROM
 * 6. DATA_API_CMD_LOAD  - load data from the EEPROM
 * 7. DATA_API_CMD_ERASE - erase all EWA sector in EEPROM
 * 8. DATA_API_CMD_ADD   - none
 */
DATA_API_STATUS eDATAAPIewa ( DATA_API_COMMAND cmd, uint32_t adr, uint8_t* data, uint16_t length )
{
  DATA_API_STATUS res                 = DATA_API_STAT_OK;
  uint32_t        i                   = 0U;
  uint8_t         buf[EWA_ERASE_SIZE] = { 0U };

  if ( ( adr + length ) < STORAGE_WEB_SIZE )
  {
    if ( xSemaphore != NULL )
    {
      switch ( cmd )
      {
        case DATA_API_CMD_SAVE:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            flTakeSource = 4U;
            if ( eEEPROMwriteMemory( adr, data, length )  != EEPROM_OK )
            {
              res = DATA_API_STAT_EEPROM_ERROR;
            }
            xSemaphoreGive( xSemaphore );
          }
          else
          {
            res = DATA_API_STAT_BUSY;
          }
          break;
        case DATA_API_CMD_LOAD:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            flTakeSource = 5U;
            if ( eEEPROMreadMemory( adr, data, length )  != EEPROM_OK )
            {
              res = DATA_API_STAT_EEPROM_ERROR;
            }
            xSemaphoreGive( xSemaphore );
          }
          else
          {
            res = DATA_API_STAT_BUSY;
          }
          break;
        case DATA_API_CMD_ERASE:
          for ( i=0U; i<EWA_ERASE_SIZE; i++ )
          {
            buf[i] = 0xFF;
          }
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            flTakeSource = 6U;
            for ( i=0; i<( STORAGE_WEB_SIZE / EWA_ERASE_SIZE ); i++ )
            {
              if ( eEEPROMwriteMemory( ( STORAGE_EWA_ADR + i * EWA_ERASE_SIZE ), buf, EWA_ERASE_SIZE )  != EEPROM_OK )
              {
                res = DATA_API_STAT_EEPROM_ERROR;
                break;
              }
            }
            xSemaphoreGive( xSemaphore );
          }
          else
          {
            res = DATA_API_STAT_BUSY;
          }
          break;
        default:
          res = DATA_API_STAT_CMD_ERROR;
          break;
      }
    }
  }
  else
  {
    res = DATA_API_STAT_ADR_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * API for Embedded Web Application
 * input:  cmd    - command
 *         adr    - configuration address in local storage
 *         value  - configuration value array
 *         scale  - configuration scale
 *         units  - configuration units array
 *         bitMap - bit map structure of configuration
 * output: dtstus of operation
 * available commands:
 * 1. DATA_API_CMD_READ  - read configuration from the locale storage
 * 2. DATA_API_CMD_WRITE - write configuration to the locale storage
 * 3. DATA_API_CMD_INC   - none
 * 4. DATA_API_CMD_DEC   - none
 * 5. DATA_API_CMD_SAVE  - save all configurations from the locale storage to the EEPROM
 * 6. DATA_API_CMD_LOAD  - save all configurations from the EEPROM to the locale storage
 * 7. DATA_API_CMD_ERASE - none
 * 8. DATA_API_CMD_ADD   - none
 */
DATA_API_STATUS eDATAAPIconfig ( DATA_API_COMMAND cmd, uint16_t adr, uint16_t* value, int8_t* scale, uint16_t* units, eConfigBitMap* bitMap )
{
  DATA_API_STATUS res = DATA_API_STAT_OK;
  uint8_t         i   = 0U;

  if ( adr < SETTING_REGISTER_NUMBER )
  {
    if ( ( xSemaphore != NULL ) && ( initDone > 0U ) )
    {
      switch ( cmd )
      {
        case DATA_API_CMD_READ:
          for ( i=0; i<configReg[adr]->atrib->len; i++ )
          {
            value[i] = configReg[adr]->value[i];
          }
          *scale = configReg[adr]->scale;
          for ( i=0U; i<MAX_UNITS_LENGTH; i++ )
          {
            units[i] = configReg[adr]->units[i];
          }
          for ( i=0U; i<configReg[adr]->atrib->bitMapSize; i++ )
          {
            bitMap[i].mask  = configReg[adr]->bitMap[i].mask;
            bitMap[i].shift = configReg[adr]->bitMap[i].shift;
          }
          break;
        case DATA_API_CMD_WRITE:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            flTakeSource = 7U;
            for ( i=0; i<configReg[adr]->atrib->len; i++ )
            {
              configReg[adr]->value[i] = value[i];
            }
            configReg[adr]->scale = *scale;
            for ( i=0U; i<MAX_UNITS_LENGTH; i++ )
            {
              configReg[adr]->units[i] = units[i];
            }
            for ( i=0U; i<configReg[adr]->atrib->bitMapSize; i++ )
            {
              configReg[adr]->bitMap[i].mask  = bitMap[i].mask;
              configReg[adr]->bitMap[i].shift = bitMap[i].shift;
            }
            xSemaphoreGive( xSemaphore );
          }
          else
          {
            res = DATA_API_STAT_BUSY;
          }
          break;
        case DATA_API_CMD_SAVE:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            flTakeSource = 8U;
            if ( eSTORAGEwriteConfigs() != EEPROM_OK )
            {
              res = DATA_API_STAT_EEPROM_ERROR;
            }
            else
            {
              vDATAAPInotfyAll( DATA_API_MESSAGE_REINIT );
            }
            xSemaphoreGive( xSemaphore );
          }
          else
          {
            res = DATA_API_STAT_BUSY;
          }
          break;
        case DATA_API_CMD_LOAD:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            flTakeSource = 9U;
            if ( eSTORAGEreadConfigs() != EEPROM_OK )
            {
              res = DATA_API_STAT_EEPROM_ERROR;
            }
            xSemaphoreGive( xSemaphore );
          }
          else
          {
            res = DATA_API_STAT_BUSY;
          }
          break;
        default:
          res = DATA_API_STAT_CMD_ERROR;
          break;
      }
    }
    else
    {
      res = DATA_API_STAT_INIT_ERROR;
    }
  }
  else
  {
    res = DATA_API_STAT_ADR_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * API for Embedded Web Application
 * input:  cmd    - command
 *         adr    - configuration address in local storage
 *         atrib  - configuration attributes
 * output: dtstus of operation
 * available commands:
 * 1. DATA_API_CMD_READ  - read configuration from the locale storage
 * 2. DATA_API_CMD_WRITE - none
 * 3. DATA_API_CMD_INC   - none
 * 4. DATA_API_CMD_DEC   - none
 * 5. DATA_API_CMD_SAVE  - none
 * 6. DATA_API_CMD_LOAD  - none
 * 7. DATA_API_CMD_ERASE - none
 * 8. DATA_API_CMD_ADD   - none
 */
DATA_API_STATUS eDATAAPIconfigAtrib ( DATA_API_COMMAND cmd, uint16_t adr, eConfigAttributes* atrib )
{
  DATA_API_STATUS res = DATA_API_STAT_OK;

  if ( adr < SETTING_REGISTER_NUMBER )
  {
    switch ( cmd )
    {
      case DATA_API_CMD_READ:
        *atrib = *configReg[adr]->atrib;
        break;
      default:
        res = DATA_API_STAT_CMD_ERROR;
        break;
    }
  }
  else
  {
    res = DATA_API_STAT_ADR_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * API for Embedded Web Application
 * input:  cmd  - command
 *         adr  - free data address in local storage
 *         data - free data value
 * output: dtstus of operation
 * available commands:
 * 1. DATA_API_CMD_READ  - read free data value from the locale storage
 * 2. DATA_API_CMD_WRITE - write free data value to the locale storage
 * 3. DATA_API_CMD_INC   - none
 * 4. DATA_API_CMD_DEC   - none
 * 5. DATA_API_CMD_SAVE  - save all free data to the EEPROM from the locale storage
 * 6. DATA_API_CMD_LOAD  - load all free data to the local storage from the EEPROM
 * 7. DATA_API_CMD_ERASE - none
 * 8. DATA_API_CMD_ADD   - none
 */
DATA_API_STATUS eDATAAPIfreeData ( DATA_API_COMMAND cmd, uint16_t adr, uint16_t* data )
{
  DATA_API_STATUS res = DATA_API_STAT_OK;
  uint16_t        i   = 0U;

  if ( adr < FREE_DATA_SIZE )
  {
    if ( ( xSemaphore != NULL ) && ( initDone > 0U ) )
    {
      switch ( cmd )
      {
        case DATA_API_CMD_READ:
          *data = *freeDataArray[adr];
          break;
        case DATA_API_CMD_WRITE:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            flTakeSource = 10U;
            *freeDataArray[adr] = *data;
            xSemaphoreGive( xSemaphore );
          }
          else
          {
            res = DATA_API_STAT_BUSY;
          }
          break;
        case DATA_API_CMD_SAVE:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            flTakeSource = 11U;
            for ( i=0U; i<FREE_DATA_SIZE; i++ )
            {
              if ( eSTORAGEsaveFreeData( i ) != EEPROM_OK )
              {
                res = DATA_API_STAT_EEPROM_ERROR;
                break;
              }
            }
            if ( res == DATA_API_STAT_OK )
            {
              vDATAAPInotfyAll( DATA_API_MESSAGE_REINIT );
            }
            xSemaphoreGive( xSemaphore );
          }
          else
          {
            res = DATA_API_STAT_BUSY;
          }
          break;
        case DATA_API_CMD_LOAD:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            flTakeSource = 12U;
            for ( i=0U; i<FREE_DATA_SIZE; i++ )
            {
              if ( eSTORAGEreadFreeData( i ) != EEPROM_OK )
              {
                res = DATA_API_STAT_EEPROM_ERROR;
                break;
              }
            }
            xSemaphoreGive( xSemaphore );
          }
          else
          {
            res = DATA_API_STAT_BUSY;
          }
          break;
        default:
          res = DATA_API_STAT_CMD_ERROR;
          break;
      }
    }
    else
    {
      res = DATA_API_STAT_INIT_ERROR;
    }
  }
  else
  {
    res = DATA_API_STAT_ADR_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * API for Embedded Web Application
 * input:  cmd - command
 *         pas - password structure
 * output: status of operation
 * available commands:
 * 1. DATA_API_CMD_READ  - read password from the locale storage
 * 2. DATA_API_CMD_WRITE - write password to the locale storage
 * 3. DATA_API_CMD_INC   - none
 * 4. DATA_API_CMD_DEC   - none
 * 5. DATA_API_CMD_SAVE  - save password to the EEPROM from the locale storage
 * 6. DATA_API_CMD_LOAD  - load password to the local storage from the EEPROM
 * 7. DATA_API_CMD_ERASE - reset password from the local storage and the EEPROM
 * 8. DATA_API_CMD_ADD   - none
 */
DATA_API_STATUS eDATAAPIpassword ( DATA_API_COMMAND cmd, PASSWORD_TYPE* pas )
{
  DATA_API_STATUS res = DATA_API_STAT_OK;

  if ( ( xSemaphore != NULL ) && ( initDone > 0U ) )
  {
    switch( cmd )
    {
      case DATA_API_CMD_READ:
        pas->data   = systemPassword.data;
        pas->status = systemPassword.status;
        break;
      case DATA_API_CMD_WRITE:
        if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
        {
          flTakeSource = 13U;
          if ( pas->status <= PASSWORD_SET )
          {
            systemPassword.data   = pas->data;
            systemPassword.status = pas->status;
          }
          else
          {
            res = DATA_API_STAT_ERROR;
          }
          xSemaphoreGive( xSemaphore );
        }
        else
        {
          res = DATA_API_STAT_BUSY;
        }
        break;
      case DATA_API_CMD_SAVE:
        if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
        {
          flTakeSource = 14U;
          if ( eSTORAGEsavePassword() != EEPROM_OK )
          {
            res = DATA_API_STAT_EEPROM_ERROR;
          }
          xSemaphoreGive( xSemaphore );
        }
        else
        {
          res = DATA_API_STAT_BUSY;
        }
        break;
      case DATA_API_CMD_LOAD:
        if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
        {
          flTakeSource = 15U;
          if ( eSTORAGEloadPassword() != EEPROM_OK )
          {
            res = DATA_API_STAT_EEPROM_ERROR;
          }
          xSemaphoreGive( xSemaphore );
        }
        else
        {
          res = DATA_API_STAT_BUSY;
        }
        break;
      case DATA_API_CMD_ERASE:
        if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
        {
          flTakeSource = 16U;
          systemPassword.status = PASSWORD_RESET;
          systemPassword.data   = 0U;
          if ( eSTORAGEsavePassword() != EEPROM_OK )
          {
            res = DATA_API_STAT_EEPROM_ERROR;
          }
          xSemaphoreGive( xSemaphore );
        }
        else
        {
          res = DATA_API_STAT_BUSY;
        }
        break;
      default:
        res = DATA_API_STAT_CMD_ERROR;
        break;
    }
  }
  else
  {
    res = DATA_API_STAT_INIT_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * API for Embedded Web Application
 * input:  cmd    - command
 *         adr    - address of log record
 *         record - log record structure
 * output: status of operation
 * available commands:
 * 1. DATA_API_CMD_READ  - none
 * 2. DATA_API_CMD_WRITE - none
 * 3. DATA_API_CMD_INC   - none
 * 4. DATA_API_CMD_DEC   - none
 * 5. DATA_API_CMD_SAVE  - none
 * 6. DATA_API_CMD_LOAD  - load addressed log record to the buffer
 * 7. DATA_API_CMD_ERASE - erase all log records
 * 8. DATA_API_CMD_ADD   - add new log record to the EEPROM
 */
DATA_API_STATUS eDATAAPIlog ( DATA_API_COMMAND cmd, uint16_t adr, LOG_RECORD_TYPE* record )
{
  DATA_API_STATUS res      = DATA_API_STAT_OK;
  uint16_t        pointer  = 0U;
  uint16_t        i        = 0U;
  SYSTEM_EVENT    eraseEv  = { .type = EVENT_NONE, .action = HMI_CMD_NONE };
  LOG_RECORD_TYPE eraseRec = { .time = 0U, .event = eraseEv };

  if ( adr < LOG_SIZE )
  {
    if ( ( xSemaphore != NULL ) && ( initDone > 0U ) )
    {
      switch ( cmd )
      {
        case DATA_API_CMD_ADD:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            flTakeSource = 17U;
            if ( eSTORAGEreadLogPointer( &pointer ) == EEPROM_OK )
            {
              if ( eSTORAGEwriteLogRecord( pointer, record ) == EEPROM_OK )
              {
                if ( pointer < LOG_SIZE )
                {
                  pointer++;
                }
                else
                {
                  pointer = 0U;
                }
                if ( eSTORAGEwriteLogPointer( pointer ) != EEPROM_OK )
                {
                  res = DATA_API_STAT_EEPROM_ERROR;
                }
              }
              else
              {
                res = DATA_API_STAT_EEPROM_ERROR;
              }
            }
            else
            {
              res = DATA_API_STAT_EEPROM_ERROR;
            }
            xSemaphoreGive( xSemaphore );
          }
          break;
        case DATA_API_CMD_LOAD:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            flTakeSource = 18U;
            if ( eSTORAGEreadLogPointer( &pointer ) == EEPROM_OK )
            {
              if ( adr < pointer)
              {
                if ( eSTORAGEreadLogRecord( adr, record ) != EEPROM_OK )
                {
                  res = DATA_API_STAT_EEPROM_ERROR;
                }
              }
              else
              {
                record->time         = 0U;
                record->event.type   = EVENT_NONE;
                record->event.action = ACTION_NONE;
              }
            }
            xSemaphoreGive( xSemaphore );
          }
          break;
        case DATA_API_CMD_ERASE:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            flTakeSource = 19U;
            for ( i=0U; i<LOG_SIZE; i++ )
            {
              if ( eSTORAGEwriteLogRecord( i, &eraseRec ) != EEPROM_OK )
              {
                res = DATA_API_STAT_EEPROM_ERROR;
                break;
              }
            }
            if ( res == DATA_API_STAT_OK )
            {
              if ( eSTORAGEwriteLogPointer( 0U ) != EEPROM_OK )
              {
                res = DATA_API_STAT_EEPROM_ERROR;
              }
            }
            xSemaphoreGive( xSemaphore );
          }
          break;
        default:
          res = DATA_API_STAT_CMD_ERROR;
          break;
      }
    }
  }
  else
  {
    res = DATA_API_STAT_ADR_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Write API for configurations
 * input:  cmd  - command for operation
 *         adr  - address of configuration in configReg array
 *         data - pointer to the external buffer. The length of buffer is in attribute of configuration
 * output: status of operation
 * available commands:
 * 1. DATA_API_CMD_READ  - read addressed configuration value from the local storage tj th buffer
 * 2. DATA_API_CMD_WRITE - write addressed configuration value to the locale storage
 * 3. DATA_API_CMD_INC   - increment addressed configuration value in locale storage
 * 4. DATA_API_CMD_DEC   - decrement addressed configuration value in locale storage
 * 5. DATA_API_CMD_SAVE  - save all configurations to the EEPROM from the locale storage
 * 6. DATA_API_CMD_LOAD  - load all configurations from the EEPROM to the locale storage
 * 7. DATA_API_CMD_ERASE - none
 * 8. DATA_API_CMD_ADD   - none
 */
DATA_API_STATUS eDATAAPIconfigValue ( DATA_API_COMMAND cmd, uint16_t adr, uint16_t* data )
{
  DATA_API_STATUS res = DATA_API_STAT_OK;
  uint8_t         i   = 0U;

  if ( adr < SETTING_REGISTER_NUMBER )
  {
    if ( ( xSemaphore != NULL ) && ( initDone > 0U ) )
    {
      switch ( cmd )
      {
        case DATA_API_CMD_READ:
          for ( i=0U; i<configReg[adr]->atrib->len; i++ )
          {
            data[i] = configReg[adr]->value[i];
          }
          break;
        case DATA_API_CMD_WRITE:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            flTakeSource = 20U;
            if ( configReg[adr]->atrib->max != 0U )
            {
              if ( data[0U] > configReg[adr]->atrib->max )
              {
                res = DATA_API_STAT_MAX_ERROR;
              }
              if ( data[0U] < configReg[adr]->atrib->min )
              {
                res = DATA_API_STAT_MIN_ERROR;
              }
            }
            if ( res == DATA_API_STAT_OK )
            {
              for ( i=0; i<configReg[adr]->atrib->len; i++ )
              {
                configReg[adr]->value[i] = data[i];
              }
            }
            xSemaphoreGive( xSemaphore );
          }
          else
          {
            res = DATA_API_STAT_BUSY;
          }
          break;
        case DATA_API_CMD_SAVE:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            flTakeSource = 21U;
            if ( eSTORAGEwriteConfigs() != EEPROM_OK )
            {
              res = DATA_API_STAT_EEPROM_ERROR;
            }
            else
            {
              vDATAAPInotfyAll( DATA_API_MESSAGE_REINIT );
            }
            xSemaphoreGive( xSemaphore );
          }
          else
          {
            res = DATA_API_STAT_BUSY;
          }
          break;
        case DATA_API_CMD_INC:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            flTakeSource = 22U;
            if ( ( configReg[adr]->atrib->max != 0U ) && ( configReg[adr]->value[0U] < configReg[adr]->atrib->max ) )
            {
              configReg[adr]->value[0U]++;
            }
            else
            {
              res = DATA_API_STAT_MAX_ERROR;
            }
            xSemaphoreGive( xSemaphore );
          }
          else
          {
            res = DATA_API_STAT_BUSY;
          }
          break;
        case DATA_API_CMD_LOAD:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            flTakeSource = 23U;
            if ( eSTORAGEreadConfigs() != EEPROM_OK )
            {
              res = DATA_API_STAT_EEPROM_ERROR;
            }
            xSemaphoreGive( xSemaphore );
          }
          else
          {
            res = DATA_API_STAT_BUSY;
          }
          break;
        case DATA_API_CMD_DEC:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            flTakeSource = 24U;
            if ( ( configReg[adr]->atrib->max != 0U ) && ( configReg[adr]->value[0U] > configReg[adr]->atrib->min ) )
            {
              configReg[adr]->value[0U]--;
            }
            else
            {
              res = DATA_API_STAT_MAX_ERROR;
            }
            xSemaphoreGive( xSemaphore );
          }
          else
          {
            res = DATA_API_STAT_BUSY;
          }
          break;
        default:
          res = DATA_API_STAT_CMD_ERROR;
          break;
      }
    }
    else
    {
      res = DATA_API_STAT_INIT_ERROR;
    }
  }
  else
  {
    res = DATA_API_STAT_ADR_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/


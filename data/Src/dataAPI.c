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
#include "system.h"
#include "mac.h"
/*----------------------- Structures ----------------------------------------------------------------*/
static SemaphoreHandle_t  xSemaphore     = NULL;
static EventGroupHandle_t xDataApiEvents = NULL;
/*----------------------- Constant ------------------------------------------------------------------*/
/*----------------------- Variables -----------------------------------------------------------------*/
volatile static uint8_t  initDone              = 0U;
volatile static uint8_t  flTakeSource          = 0U;
volatile static uint16_t measurementNumberCash = 0U;
static EEPROM_TYPE*      eeprom                = NULL;
#if defined ( UNIT_TEST )
volatile LOG_CASH_TYPE logCash               = { 0U };
#else
volatile static LOG_CASH_TYPE logCash               = { 0U };
#endif
/*------------------------ Define -------------------------------------------------------------------*/
/*----------------------- Functions -----------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PRIVATE -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vDATAAPIsendEventAll ( DATA_API_REINIT message )
{
  uint32_t mask = 0U;
  switch ( message )
  {
    case DATA_API_REINIT_CONFIG:
      mask = DATA_API_FLAG_LCD_TASK_CONFIG_REINIT        |
             DATA_API_FLAG_ENGINE_TASK_CONFIG_REINIT     |
             DATA_API_FLAG_CONTROLLER_TASK_CONFIG_REINIT |
             DATA_API_FLAG_ELECTRO_TASK_CONFIG_REINIT    |
             DATA_API_FLAG_FPI_TASK_CONFIG_REINIT        |
             DATA_API_FLAG_ADC_TASK_CONFIG_REINIT        |
             DATA_API_FLAG_MEASUREMENT_TSK_REINIT;
      xEventGroupSetBits( xDataApiEvents, mask );
      break;
    case DATA_API_REINIT_MAINTANCE:
      mask = DATA_API_FLAG_ELECTRO_TASK_CONFIG_REINIT;
      xEventGroupSetBits( xDataApiEvents, mask );
      break;
    case DATA_API_REDRAW_DISPLAY:
      mask = DATA_API_FLAG_REDRAW_DISPLAY;
      xEventGroupSetBits( xDataApiEvents, mask );
      break;
    default:
      break;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
DATA_API_STATUS eDATAAPIlogLoad ( uint16_t adr, LOG_RECORD_TYPE* record )
{
  DATA_API_STATUS res = DATA_API_STAT_OK;
  if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
  {
    flTakeSource = 18U;
    if ( eSTORAGEreadLogRecord( adr, record ) != EEPROM_OK )
    {
      res = DATA_API_STAT_EEPROM_ERROR;
    }
    xSemaphoreGive( xSemaphore );
  }
  else
  {
    res = DATA_API_STAT_BUSY;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLICK -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vDATAAPIincLogSize ( void )
{
  if ( logCash.size < LOG_SIZE )
  {
    logCash.size++;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
EventGroupHandle_t xDATAAPIgetEventGroup ( void )
{
  return xDataApiEvents;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
uint8_t uDATAAPIisValid ( void )
{
  uint8_t  res    = 0U;
  uint8_t  sysReg = 0xFFU;
  uint32_t map[STORAGE_MAP_SIZE / 4U] = { 0U };
  if ( eSTORAGEreadMap( map ) == EEPROM_OK )
  {
    if ( eEEPROMreadMemory( eeprom, STORAGE_SR_ADR, &sysReg, 1U ) == EEPROM_OK )
    {
      if ( ( sysReg                  != STORAGE_SR_EMPTY ) &&
           ( REWRITE_ALL_EEPROM      == 0U               ) &&
           ( uSTORAGEcheckMap( map ) >  0U               ) )
      {
        res = 1U;
      }
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
void vDATAAPIsetConstantConfigs ( void )
{
  uint16_t idBuffer[UNIQUE_ID_LENGTH] = { 0U };
  vSYSgetUniqueID16( idBuffer );
  serialNumber0.value[0U]     = idBuffer[0U];
  serialNumber0.value[1U]     = idBuffer[1U];
  serialNumber0.value[2U]     = idBuffer[2U];
  serialNumber1.value[0U]     = idBuffer[3U];
  serialNumber1.value[1U]     = idBuffer[4U];
  serialNumber1.value[2U]     = idBuffer[5U];
  versionController.value[0U] = HARDWARE_VERSION_MAJOR;
  versionController.value[1U] = HARDWARE_VERSION_MINOR;
  versionController.value[2U] = HARDWARE_VERSION_PATCH;
  versionFirmware.value[0U]   = FIRMWARE_VERSION_MAJOR;
  versionFirmware.value[1U]   = FIRMWARE_VERSION_MINOR;
  versionFirmware.value[2U]   = FIRMWARE_VERSION_PATCH;
  versionBootloader.value[0U] = ( uint16_t )( ( __UNALIGNED_UINT32_READ( BOOTLOADER_VERSION_ADR ) >> 16U ) & 0xFF );
  versionBootloader.value[1U] = ( uint16_t )( ( __UNALIGNED_UINT32_READ( BOOTLOADER_VERSION_ADR ) >> 8U  ) & 0xFF );
  versionBootloader.value[2U] = ( uint16_t )( ( __UNALIGNED_UINT32_READ( BOOTLOADER_VERSION_ADR )        ) & 0xFF );
  deviceID.value[0U]          = DEVICE_ID;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
EEPROM_STATUS eDATAAPIresetStorage ( void )
{
  EEPROM_STATUS         res      = EEPROM_OK;
  uint8_t               sysReg   = 0x00U;
  const SYSTEM_EVENT    eraseEv  = { .type = EVENT_NONE, .action = HMI_CMD_NONE };
  const LOG_RECORD_TYPE eraseRec = { .time = 0U, .event = eraseEv };
  vFREEDATAerase();
  vDATAAPIsetConstantConfigs();
  res = eSTORAGEwriteConfigs();
  if ( res == EEPROM_OK )
  {
    res = eSTORAGEwriteCharts();
    if ( res == EEPROM_OK )
    {
      for ( uint8_t i=0U; i<FREE_DATA_SIZE; i++ )
      {
        res = eSTORAGEsaveFreeData( i );
        if ( res != EEPROM_OK )
        {
          break;
        }
      }
      if ( res == EEPROM_OK )
      {
        res = eSTORAGEsavePassword();
        if ( res == EEPROM_OK )
        {
          res = eSTORAGEwriteLogPointer( 0U );
          if ( res == EEPROM_OK )
          {
            for ( uint8_t i=0U; i<LOG_SIZE; i++ )
            {
              res = eSTORAGEwriteLogRecord( i, &eraseRec );
              if ( res != EEPROM_OK )
              {
                break;
              }
            }
          }
        }
        if ( res == EEPROM_OK )
        {
          res = eEEPROMwriteMemory( eeprom, STORAGE_SR_ADR, &sysReg, 1U );
          if ( res == EEPROM_OK )
          {
            res = eSTORAGEwriteMap();
            if ( res == EEPROM_OK )
            {
              initDone = 1U;
            }
          }
        }
      }
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
EEPROM_STATUS eDATAAPIreadData ( void )
{
  EEPROM_STATUS res = eSTORAGEreadConfigs();
  uint16_t      buf = 0U;
  vDATAAPIsetConstantConfigs();
  for ( uint8_t i=0U; ( ( i<FREE_DATA_SIZE ) && ( res == EEPROM_OK ) ); i++ )
  {
    res = eSTORAGEreadFreeData( i );
    if ( *freeDataArray[i] == 0xFFFFU )
    {
      res = eSTORAGEsetFreeData( i, &buf );
    }
  }
  if ( ( res == EEPROM_OK ) && ( *freeDataArray[ENGINE_WORK_MINUTES_ADR] > 60U ) )
  {
    eSTORAGEsetFreeData( ENGINE_WORK_MINUTES_ADR, &buf );
  }
  res = eSTORAGEloadPassword();
  initDone = 1U;
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * All data initialization
 * input:  none
 * output: none
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
EEPROM_STATUS eDATAAPIdataInit ( void )
{
  EEPROM_STATUS res = EEPROM_OK;
  if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
  {
    if ( uDATAAPIisValid() == 0U )
    {
      res = eDATAAPIresetStorage();
    }
    else
    {
      res = eDATAAPIreadData();
    }
    xSemaphoreGive( xSemaphore );
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Structures initialization
 * input:  array of task for notifications
 * output: none
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vDATAAPIinit ( const EEPROM_TYPE* storage )
{
  eeprom         = ( EEPROM_TYPE* ) storage;
  xSemaphore     = xSemaphoreCreateMutex();
  xDataApiEvents = xEventGroupCreate();
  return;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
EEPROM_STATUS eDATAAPIlogInit ( void )
{
  EEPROM_STATUS   res     = EEPROM_OK;
  LOG_RECORD_TYPE record  = { 0U };
  uint16_t        pointer = 0U;
  logCash.adr = 0xFFFFU;
  res         = eSTORAGEreadLogPointer( &pointer );
  if ( res == EEPROM_OK )
  {
    if ( pointer == 0xFFFF )
    {
      logCash.size = 0U;
    }
    else
    {
      res = eSTORAGEreadLogRecord( pointer, &record );
      if ( res == EEPROM_OK )
      {
        logCash.pointer = pointer;
        if ( record.time == 0U )
        {
          logCash.size = pointer;
        }
        else
        {
          logCash.size = LOG_SIZE;
        }
      }
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Check data initialization
 * input:  none
 * output: initialization status
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
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
 * 1.  DATA_API_CMD_READ  - return chart with address from local storage
 * 2.  DATA_API_CMD_WRITE - write chart with address to local storage
 * 3.  DATA_API_CMD_INC   - none
 * 4.  DATA_API_CMD_DEC   - none
 * 5.  DATA_API_CMD_SAVE  - save all charts from locale storage to the EEPROM
 * 6.  DATA_API_CMD_LOAD  - load all charts from the EEPROM to local storage
 * 7.  DATA_API_CMD_ERASE - none
 * 8.  DATA_API_CMD_ADD   - none
 * 9.  DATA_API_CMD_COUNTER - none
 * 10. DATA_API_CMD_READ_CASH - none
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
DATA_API_STATUS eDATAAPIchart ( DATA_API_COMMAND cmd, uint16_t adr, eChartData* chart )
{
  DATA_API_STATUS res = DATA_API_STAT_OK;
  if ( adr < CHART_NUMBER )
  {
    if ( ( xSemaphore != NULL ) && ( initDone > 0U ) && ( xCHARTgetSemophore() != NULL) )
    {
      switch ( cmd )
      {
        case DATA_API_CMD_READ:
          *chart = *charts[adr];
          break;
        case DATA_API_CMD_WRITE:
          if ( xSemaphoreTake( xCHARTgetSemophore(), SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            flTakeSource = 1U;
            *charts[adr] = *chart;
            xSemaphoreGive( xCHARTgetSemophore() );
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
            if ( xSemaphoreTake( xCHARTgetSemophore(), SEMAPHORE_TAKE_DELAY ) == pdTRUE )
            {
              flTakeSource = 3U;
              if ( eSTORAGEreadCharts() != EEPROM_OK )
              {
                res = DATA_API_STAT_EEPROM_ERROR;
              }
              vCHARTupdateAtrib();
              xSemaphoreGive( xCHARTgetSemophore() );
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
 *         adr    - configuration address in local storage
 *         value  - configuration value array
 *         scale  - configuration scale
 *         units  - configuration units array
 *         bitMap - bit map structure of configuration
 * output: dtstus of operation
 * available commands:
 * 1.  DATA_API_CMD_READ  - read configuration from the locale storage
 * 2.  DATA_API_CMD_WRITE - write configuration to the locale storage
 * 3.  DATA_API_CMD_INC   - none
 * 4.  DATA_API_CMD_DEC   - none
 * 5.  DATA_API_CMD_SAVE  - save all configurations from the locale storage to the EEPROM
 * 6.  DATA_API_CMD_LOAD  - save all configurations from the EEPROM to the locale storage
 * 7.  DATA_API_CMD_ERASE - none
 * 8.  DATA_API_CMD_ADD   - none
 * 9.  DATA_API_CMD_COUNTER - none
 * 10. DATA_API_CMD_READ_CASH - none
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
DATA_API_STATUS eDATAAPIconfig ( DATA_API_COMMAND cmd, uint16_t adr, uint16_t* value, int8_t* scale, uint16_t* units )
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
            value[i] = configReg[adr]->value[i];
          }
          *scale = configReg[adr]->atrib->scale;
          for ( i=0U; i<MAX_UNITS_LENGTH; i++ )
          {
            units[i] = configReg[adr]->atrib->units[i];
          }
          break;
        case DATA_API_CMD_WRITE:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            flTakeSource = 7U;
            for ( i=0U; i<configReg[adr]->atrib->len; i++ )
            {
              configReg[adr]->value[i] = value[i];
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
              vDATAAPIsendEventAll( DATA_API_REINIT_CONFIG );
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
 * 1.  DATA_API_CMD_READ  - read configuration from the locale storage
 * 2.  DATA_API_CMD_WRITE - none
 * 3.  DATA_API_CMD_INC   - none
 * 4.  DATA_API_CMD_DEC   - none
 * 5.  DATA_API_CMD_SAVE  - none
 * 6.  DATA_API_CMD_LOAD  - none
 * 7.  DATA_API_CMD_ERASE - none
 * 8.  DATA_API_CMD_ADD   - none
 * 9.  DATA_API_CMD_COUNTER - none
 * 10. DATA_API_CMD_READ_CASH - none
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
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
 * 1.  DATA_API_CMD_READ  - read free data value from the locale storage
 * 2.  DATA_API_CMD_WRITE - write free data value to the locale storage
 * 3.  DATA_API_CMD_INC   - Increment value and read result to data
 * 4.  DATA_API_CMD_DEC   - none
 * 5.  DATA_API_CMD_SAVE  - save all free data to the EEPROM from the locale storage
 * 6.  DATA_API_CMD_LOAD  - load all free data to the local storage from the EEPROM
 * 7.  DATA_API_CMD_ERASE - set 0 to free data by address in local storage
 * 8.  DATA_API_CMD_ADD   - none
 * 9.  DATA_API_CMD_COUNTER - none
 * 10. DATA_API_CMD_READ_CASH - none
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
DATA_API_STATUS eDATAAPIfreeData ( DATA_API_COMMAND cmd, FREE_DATA_ADR adr, uint16_t* data )
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
            if ( adr == ENGINE_WORK_TIME_ADR )
            {
              *freeDataArray[ENGINE_WORK_MINUTES_ADR] = 0U;
            }
            flTakeSource = 10U;
            *freeDataArray[adr] = *data;
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
            *freeDataArray[adr] += 1U;
            *data = *freeDataArray[adr];
            xSemaphoreGive( xSemaphore );
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
        case DATA_API_CMD_ERASE:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            *freeDataArray[adr] = 0U;
            xSemaphoreGive( xSemaphore );
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
 * 1.  DATA_API_CMD_READ  - read password from the locale storage
 * 2.  DATA_API_CMD_WRITE - write password to the locale storage
 * 3.  DATA_API_CMD_INC   - none
 * 4.  DATA_API_CMD_DEC   - none
 * 5.  DATA_API_CMD_SAVE  - save password to the EEPROM from the locale storage
 * 6.  DATA_API_CMD_LOAD  - load password to the local storage from the EEPROM
 * 7.  DATA_API_CMD_ERASE - reset password from the local storage and the EEPROM
 * 8.  DATA_API_CMD_ADD   - none
 * 9.  DATA_API_CMD_COUNTER - none
 * 10. DATA_API_CMD_READ_CASH - none
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
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
          flTakeSource          = 16U;
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
 * API for Log pointer
 * input:  cmd     - command
 *         pointer - pointer for last record in the log
 * output: status of operation
 * available commands:
 * 1. DATA_API_CMD_READ  - read pointer
 * 2. DATA_API_CMD_WRITE - none
 * 3. DATA_API_CMD_INC   - none
 * 4. DATA_API_CMD_DEC   - none
 * 5. DATA_API_CMD_SAVE  - none
 * 6. DATA_API_CMD_LOAD  - none
 * 7. DATA_API_CMD_ERASE - none
 * 8. DATA_API_CMD_ADD   - none
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
DATA_API_STATUS eDATAAPIlogPointer ( DATA_API_COMMAND cmd, uint16_t* pointer )
{
  DATA_API_STATUS res = DATA_API_STAT_OK;
  if ( ( xSemaphore != NULL ) && ( initDone > 0U ) )
  {
    switch ( cmd )
    {
      case DATA_API_CMD_READ:
        if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
        {
          if ( eSTORAGEreadLogPointer( pointer ) != EEPROM_OK )
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
      case DATA_API_CMD_READ_CASH:
        *pointer = logCash.pointer;
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
 * API for log
 * input:  cmd    - command
 *         adr    - address of log record
 *         record - log record structure
 * output: status of operation
 * available commands:
 * 1.  DATA_API_CMD_READ      - none
 * 2.  DATA_API_CMD_WRITE     - none
 * 3.  DATA_API_CMD_INC       - none
 * 4.  DATA_API_CMD_DEC       - none
 * 5.  DATA_API_CMD_SAVE      - none
 * 6.  DATA_API_CMD_LOAD      - load addressed log record to the buffer
 * 7.  DATA_API_CMD_ERASE     - erase all log records
 * 8.  DATA_API_CMD_ADD       - add new log record to the EEPROM
 * 9.  DATA_API_CMD_READ_CASH - read record with cash
 * 10. DATA_API_CMD_COUNTER   - read to adr size of log,
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
DATA_API_STATUS eDATAAPIlog ( DATA_API_COMMAND cmd, uint16_t* adr, LOG_RECORD_TYPE* record )
{
  DATA_API_STATUS res      = DATA_API_STAT_OK;
  uint16_t        pointer  = 0U;
  uint16_t        i        = 0U;
  SYSTEM_EVENT    eraseEv  = { .type = EVENT_NONE, .action = HMI_CMD_NONE };
  LOG_RECORD_TYPE eraseRec = { .time = 0U, .event = eraseEv };
  if ( *adr < LOG_SIZE )
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
                if ( eSTORAGEwriteLogPointer( pointer ) == EEPROM_OK )
                {
                  logCash.pointer = pointer;
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
            }
            else
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

        case DATA_API_CMD_READ_CASH:
          if ( logCash.adr == *adr )
          {
            *record = logCash.record;
          }
          else
          {
            res = eDATAAPIlogLoad( *adr, record );
            if ( res == DATA_API_STAT_OK )
            {
              if ( eSTORAGEreadLogPointer( &pointer ) == EEPROM_OK )
              {
                logCash.pointer = pointer;
                logCash.adr     = *adr;
                logCash.record  = *record;
              }
              else
              {
                res = DATA_API_STAT_EEPROM_ERROR;
              }
            }
          }
          break;
        case DATA_API_CMD_COUNTER:
          *adr = logCash.size;
          break;
        case DATA_API_CMD_LOAD:
          res = eDATAAPIlogLoad( *adr, record );
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
              else
              {
                logCash.size = 0U;
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
 * Write API for configurations
 * input:  cmd  - command for operation
 *         adr  - address of configuration in configReg array
 *         data - pointer to the external buffer. The length of buffer is in attribute of configuration
 * output: status of operation
 * available commands:
 * 1.  DATA_API_CMD_READ  - read addressed configuration value from the local storage tj th buffer
 * 2.  DATA_API_CMD_WRITE - write addressed configuration value to the locale storage
 * 3.  DATA_API_CMD_INC   - increment addressed configuration value in locale storage
 * 4.  DATA_API_CMD_DEC   - decrement addressed configuration value in locale storage
 * 5.  DATA_API_CMD_SAVE  - save all configurations to the EEPROM from the locale storage
 * 6.  DATA_API_CMD_LOAD  - load all configurations from the EEPROM to the locale storage
 * 7.  DATA_API_CMD_ERASE - none
 * 8.  DATA_API_CMD_ADD   - none
 * 9.  DATA_API_CMD_COUNTER - none
 * 10. DATA_API_CMD_READ_CASH - none
 */
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
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
              for ( i=0U; i<configReg[adr]->atrib->len; i++ )
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
            res = DATA_API_STAT_EEPROM_ERROR;
            if ( eSTORAGEwriteConfigs() == EEPROM_OK )
            {
              if ( eSTORAGEeraseMeasurement() == EEPROM_OK )
              {
                measurementNumberCash = 0U;
                res = DATA_API_STAT_OK;
                vDATAAPIsendEventAll( DATA_API_REINIT_CONFIG );
              }
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
              res = DATA_API_STAT_MIN_ERROR;
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
void vDATAAPIinitMac ( const EEPROM_TYPE* mac )
{
  EEPROM_STATUS res = EEPROM_ERROR;
  #if defined ( EXTERNAL_MAC )
    res = eMACinit( mac );
    if ( res == EEPROM_OK )
    {
      eSTORAGEwriteMac( uMACget48() );
    }
  #endif
  if ( res != EEPROM_OK )
  {
    if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
    {
      if ( eSTORAGEreadMac( uMACget48() ) != EEPROM_OK )
      {
        vMACsetDefault();
      }
      xSemaphoreGive( xSemaphore );
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uDATAAPIisInitDone ( void )
{
  return initDone;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/


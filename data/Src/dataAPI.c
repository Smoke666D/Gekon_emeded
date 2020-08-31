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
#include "storage.h"
#include "freeData.h"
/*----------------------- Structures ----------------------------------------------------------------*/
static SemaphoreHandle_t xSemaphore;
/*----------------------- Constant ------------------------------------------------------------------*/
/*----------------------- Variables -----------------------------------------------------------------*/
static TaskHandle_t* notifyTargets[NOTIFY_TARGETS_NUMBER];
/*----------------------- Functions -----------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PRIVATE -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
void vDATAAPInotfyAll ( uint32_t value )
{
  uint8_t i = 0U;
  for (i=0U; i<NOTIFY_TARGETS_NUMBER; i++ )
  {
    xTaskNotify( *notifyTargets[i], value, eSetValueWithOverwrite );
  }
}
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLICK -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
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
DATA_API_STATUS eDATAAPIchart ( DATA_API_COMMAND cmd, uint16_t adr, eChartData* chart )
{
  DATA_API_STATUS res = DATA_API_STAT_OK;

  if ( adr < CHART_NUMBER )
  {
    if ( xSemaphore != NULL )
    {
      switch ( cmd )
      {
        case DATA_API_CMD_READ:
          *chart = *charts[adr];
          break;
        case DATA_API_CMD_WRITE:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            *charts[adr] = *chart;
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
        case DATA_API_CMD_INC:
          res = DATA_API_STAT_CMD_ERROR;
          break;
        case DATA_API_CMD_LOAD:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            if ( eSTORAGEreadCharts() != EEPROM_OK )
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
DATA_API_STATUS eDATAAPIconfig ( DATA_API_COMMAND cmd, uint16_t adr, uint16_t* value, signed char scale, uint16_t* units, eConfigBitMap* bitMap )
{
  DATA_API_STATUS res = DATA_API_STAT_OK;
  uint8_t         i   = 0U;

  if ( adr < SETTING_REGISTER_NUMBER )
  {
    if ( xSemaphore != NULL )
    {
      switch ( cmd )
      {
        case DATA_API_CMD_READ:
          for ( i=0; i<configReg[adr]->atrib->len; i++ )
          {
            value[i] = configReg[adr]->value[i];
          }
          scale = configReg[adr]->scale;
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
            for ( i=0; i<configReg[adr]->atrib->len; i++ )
            {
              configReg[adr]->value[i] = value[i];
            }
            configReg[adr]->scale = scale;
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
          res = DATA_API_STAT_CMD_ERROR;
          break;
        case DATA_API_CMD_LOAD:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
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
DATA_API_STATUS eDATAAPIfreeData ( DATA_API_COMMAND cmd, uint16_t adr, uint16_t data )
{
  DATA_API_STATUS res = DATA_API_STAT_OK;
  uint16_t        i   = 0U;

  if ( adr < FREE_DATA_SIZE )
  {
    if ( xSemaphore != NULL )
    {
      switch ( cmd )
      {
        case DATA_API_CMD_READ:
          data = *freeDataArray[adr];
          break;
        case DATA_API_CMD_WRITE:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
            *freeDataArray[adr] = data;
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
            for ( i=0U; i<FREE_DATA_SIZE; i++ )
            {
              if ( eSTORAGEsaveFreeData( i ) != EEPROM_OK )
              {
                res = DATA_API_STAT_EEPROM_ERROR;
                break;
              }
              if ( res == DATA_API_STAT_OK )
              {
                vDATAAPInotfyAll( DATA_API_MESSAGE_REINIT );
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
          res = DATA_API_STAT_CMD_ERROR;
          break;
        case DATA_API_CMD_LOAD:
          if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
          {
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
 * Write API for configurations
 * input:  cmd  - command for operation
 *         adr  - address of configuration in configReg array
 *         data - pointer to the external buffer. The length of buffer is in attribute of configuration
 * output: status of operation
 */
DATA_API_STATUS eDATAAPIconfigValue ( DATA_API_COMMAND cmd, uint16_t adr, uint16_t* data )
{
  DATA_API_STATUS res = DATA_API_STAT_OK;
  uint8_t         i   = 0U;

  if ( adr < SETTING_REGISTER_NUMBER )
  {
    if ( xSemaphore != NULL )
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


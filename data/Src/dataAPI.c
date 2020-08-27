/*
 * dataAPI.c
 *
 *  Created on: 27 авг. 2020 г.
 *      Author: 79110
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "dataAPI.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "storage.h"
#include "chart.h"
#include "storage.h"
/*----------------------- Structures ----------------------------------------------------------------*/
static SemaphoreHandle_t xSemaphore;
/*----------------------- Constant ------------------------------------------------------------------*/
/*----------------------- Variables -----------------------------------------------------------------*/
/*----------------------- Functions -----------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PRIVATE -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLICK -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
void vDATAAPIinit ( void )
{
  xSemaphore = xSemaphoreCreateMutex();
  return;
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

  if ( xSemaphore != NULL )
  {
    if ( xSemaphoreTake( xSemaphore, SEMAPHORE_TAKE_DELAY ) == pdTRUE )
    {
      switch ( cmd )
      {
        case DATA_API_CMD_WRITE:
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
          break;
        case DATA_API_CMD_SAVE:
          if ( eSTORAGEwriteConfigs() != EEPROM_OK )
          {
            res = DATA_API_STAT_EEPROM_ERROR;
          }
          break;
        case DATA_API_CMD_ESC:
          for ( i=0; i<configReg[adr]->atrib->len; i++ )
          {
            data[i] = configReg[adr]->value[i];
          }
          break;
        case DATA_API_CMD_INC:
          if ( ( configReg[adr]->atrib->max != 0U ) && ( configReg[adr]->value[0U] < configReg[adr]->atrib->max ) )
          {
            configReg[adr]->value[0U]++;
          }
          else
          {
            res = DATA_API_STAT_MAX_ERROR;
          }
          break;
        case DATA_API_CMD_LOAD:
          if ( eSTORAGEreadConfigs() != EEPROM_OK )
          {
            data[i] = configReg[adr]->value[i];
          }
          break;
        case DATA_API_CMD_DEC:
          if ( ( configReg[adr]->atrib->max != 0U ) && ( configReg[adr]->value[0U] > configReg[adr]->atrib->min ) )
          {
	    configReg[adr]->value[0U]--;
          }
          else
          {
            res = DATA_API_STAT_MAX_ERROR;
          }
          break;
        default:
          break;
      }
      xSemaphoreGive( xSemaphore );
    }
    else
    {
      res = DATA_API_STAT_BUSY;
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/


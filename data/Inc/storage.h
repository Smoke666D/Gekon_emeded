/*
 * storage.h
 *
 *  Created on: Jun 11, 2020
 *      Author: photo_Mickey
 */

#ifndef INC_STORAGE_H_
#define INC_STORAGE_H_
/*----------------------- Includes -------------------------------------*/
#include <freeData.h>
#include "stm32f2xx_hal.h"
#include "EEPROM.h"
#include "config.h"
//#include "logicCommon.h"
/*------------------------ Define --------------------------------------*/
#define  STORAGE_WEB_SIZE       307200U                            /* byte */
#define  STORAGE_RESERVE_SIZE   10240U                             /* byte */
#define  STORAGE_CONFIG_SIZE    CONFIG_TOTAL_SIZE                  /* byte */
#define  STORAGE_CHART_SIZE     ( CHART_CHART_SIZE * CHART_NUMBER) /* byte */
#define  STORAGE_FREE_DATA_SIZE ( FREE_DATA_SIZE * 2U )            /* byte */
#define  STORAGE_LOG_SIZE       9216                               /* byte */
#define  STORAGE_REQUIRED_SIZE  ( STORAGE_WEB_SIZE + STORAGE_CONFIG_SIZE + STORAGE_CHART_SIZE + STORAGE_RESERVE_SIZE + STORAGE_LOG_SIZE )

#if ( ( EEPROM_SIZE * 1024U ) < STORAGE_REQUIRED_SIZE )
  #error EEPROM size is too small!
#endif

#define  STORAGE_START_ADR      0x0000U
#define  STORAGE_EWA_ADR        ( STORAGE_START_ADR )
#define  STORAGE_EWA_DATA_ADR   ( STORAGE_START_ADR + EEPROM_LENGTH_SIZE )
#define  STORAGE_RESERVE_ADR    ( STORAGE_EWA_ADR + STORAGE_WEB_SIZE )
#define  STORAGE_CONFIG_ADR     ( STORAGE_RESERVE_ADR + STORAGE_RESERVE_SIZE )
#define  STORAGE_CHART_ADR      ( STORAGE_CONFIG_ADR + CONFIG_TOTAL_SIZE )
#define  STORAGE_DATA_ADR       ( STORAGE_CHART_ADR + CHART_CHART_SIZE )
#define  STORAGE_LOG_ADR        ( STORAGE_DATA_ADR + STORAGE_FREE_DATA_SIZE )
/*------------------------- Macros -------------------------------------*/

/*-------------------------- ENUM --------------------------------------*/

/*----------------------- Structures -----------------------------------*/

/*------------------------ Functions -----------------------------------*/
EEPROM_STATUS eSTORAGEwriteConfigs ( void );
EEPROM_STATUS eSTORAGEreadConfigs ( void );
EEPROM_STATUS eSTORAGEdeleteConfigs ( void );
EEPROM_STATUS eSTORAGEwriteCharts ( void );
EEPROM_STATUS eSTORAGEreadCharts ( void );

EEPROM_STATUS eSTORAGEreadFreeData ( DATA_ADR n );
EEPROM_STATUS eSTORAGEsaveFreeData ( DATA_ADR n );
EEPROM_STATUS eSTORAGEsetFreeData ( DATA_ADR n, const uint16_t* data );
//EEPROM_STATUS eSTORAGEwriteLogRecord( LOG_RECORD_TYPE* record );
//EEPROM_STATUS eSTORAGEwriteLogQuant( uint16_t quant );
//EEPROM_STATUS eSTORAGEwriteLog( LOG_TYPE* log );
//EEPROM_STATUS eSTORAGEreadLog( LOG_TYPE* log );
/*----------------------------------------------------------------------*/
#endif /* INC_STORAGE_H_ */

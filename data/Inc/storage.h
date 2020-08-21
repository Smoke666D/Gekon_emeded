/*
 * storage.h
 *
 *  Created on: Jun 11, 2020
 *      Author: photo_Mickey
 */

#ifndef INC_STORAGE_H_
#define INC_STORAGE_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "EEPROM.h"
#include "config.h"
//#include "logicCommon.h"
/*------------------------ Define --------------------------------------*/
#define  STORAGE_WEB_SIZE       300U                   /* Kb */
#define  STORAGE_RESERVE_SIZE   10U                    /* Kb */
#define  STORAGE_CONFIG_SIZE    ( CONFIG_TOTAL_SIZE )  /* Kb */
#define  STORAGE_CHART_SIZE     6U                     /* Kb */
#define  STORAGE_LOG_SIZE       9U                     /* Kb */
#define  STORAGE_REQUIRED_SIZE  ( STORAGE_WEB_SIZE + STORAGE_CONFIG_SIZE + STORAGE_CHART_SIZE + STORAGE_RESERVE_SIZE + STORAGE_LOG_SIZE )

#if ( EEPROM_SIZE < STORAGE_REQUIRED_SIZE )
  #error EEPROM size is too small!
#endif

#define  STORAGE_START_ADR      0x0000U
#define  STORAGE_EWA_ADR        ( STORAGE_START_ADR )
#define  STORAGE_EWA_DATA_ADR   ( STORAGE_START_ADR + EEPROM_LENGTH_SIZE )
#define  STORAGE_RESERVE_ADR    ( STORAGE_EWA_ADR + ( STORAGE_WEB_SIZE * 1024U ) )
#define  STORAGE_CONFIG_ADR     ( STORAGE_RESERVE_ADR + ( STORAGE_RESERVE_SIZE * 1024U ) )
#define  STORAGE_CHART_ADR      ( STORAGE_CONFIG_ADR +  ( STORAGE_CONFIG_SIZE * 1024U ) )
#define  STORAGE_LOG_ADR        ( STORAGE_CHART_ADR + ( STORAGE_CHART_SIZE * 1024U ) )
/*------------------------- Macros -------------------------------------*/

/*-------------------------- ENUM --------------------------------------*/

/*----------------------- Structures -----------------------------------*/

/*------------------------ Functions -----------------------------------*/
EEPROM_STATUS eSTORAGEwriteConfigs( void );
EEPROM_STATUS eSTORAGEreadConfigs( void );
EEPROM_STATUS eSTORAGEwriteCharts( void );
EEPROM_STATUS eSTORAGEreadCharts( void );

//EEPROM_STATUS eSTORAGEwriteLogRecord( LOG_RECORD_TYPE* record );
//EEPROM_STATUS eSTORAGEwriteLogQuant( uint16_t quant );
//EEPROM_STATUS eSTORAGEwriteLog( LOG_TYPE* log );
//EEPROM_STATUS eSTORAGEreadLog( LOG_TYPE* log );
/*----------------------------------------------------------------------*/
#endif /* INC_STORAGE_H_ */

/*
 * storage.h
 *
 *  Created on: Jun 11, 2020
 *      Author: photo_Mickey
 */
#ifndef INC_STORAGE_H_
#define INC_STORAGE_H_
/*----------------------- Includes -------------------------------------*/
#include "freeData.h"
#include "stm32f2xx_hal.h"
#include "EEPROM.h"
#include "config.h"
#include "controllerTypes.h"
/*------------------------ Define --------------------------------------*/
#define  STORAGE_SR_SIZE             1U                                 /* byte */
#define  STORAGE_MAP_SIZE            40U                                /* byte */
#define  STORAGE_WEB_SIZE            307200U                            /* byte */
#define  STORAGE_RESERVE_SIZE        10240U                             /* byte */
#define  STORAGE_CONFIG_SIZE         CONFIG_TOTAL_SIZE                  /* byte */
#define  STORAGE_CHART_SIZE          ( CHART_CHART_SIZE * CHART_NUMBER) /* byte */
#define  STORAGE_FREE_DATA_SIZE      ( FREE_DATA_SIZE * 2U )            /* byte */
#define  STORAGE_PASSWORD_SIZE       PASSWORD_SIZE                      /* byte */
#define  STORAGE_LOG_POINTER_SIZE    2U                                 /* byte */
#define  STORAGE_LOG_SIZE            ( LOG_SIZE * LOG_RECORD_SIZE )     /* byte */
#define  STORAGE_JOURNAL_SIZE        ( 1000U )                          /* byte */
#define  STORAGE_MEASUREMENT_SR_SIZE 2U                                 /* byte */
#define  STORAGE_REQUIRED_SIZE       ( STORAGE_SR_SIZE + STORAGE_MAP_SIZE + STORAGE_WEB_SIZE + STORAGE_RESERVE_SIZE + STORAGE_CONFIG_SIZE + STORAGE_CHART_SIZE + STORAGE_FREE_DATA_SIZE + STORAGE_PASSWORD_SIZE + STORAGE_LOG_POINTER_SIZE + STORAGE_LOG_SIZE + STORAGE_JOURNAL_SIZE + STORAGE_MEASUREMENT_SR_SIZE )
#define  STORAGE_FREE_SIZE           ( EEPROM_SIZE * 1024U - STORAGE_REQUIRED_SIZE ) /* byte */

#if ( ( EEPROM_SIZE * 1024U ) < STORAGE_REQUIRED_SIZE )
  #error EEPROM size is too small!
#endif

#define  STORAGE_START_ADR          0x0000U
#define  STORAGE_SR_ADR             ( STORAGE_START_ADR )
#define  STORAGE_MAP_ADR            ( STORAGE_SR_ADR             + STORAGE_SR_SIZE             )
#define  STORAGE_CONFIG_ADR         ( STORAGE_RESERVE_ADR        + STORAGE_MAP_SIZE            )
#define  STORAGE_CHART_ADR          ( STORAGE_CONFIG_ADR         + CONFIG_TOTAL_SIZE           )
#define  STORAGE_FREE_DATA_ADR      ( STORAGE_CHART_ADR          + STORAGE_CHART_SIZE          )
#define  STORAGE_PASSWORD_ADR       ( STORAGE_FREE_DATA_ADR      + STORAGE_FREE_DATA_SIZE      )
#define  STORAGE_LOG_POINTER_ADR    ( STORAGE_PASSWORD_ADR       + STORAGE_PASSWORD_SIZE       )
#define  STORAGE_LOG_ADR            ( STORAGE_LOG_POINTER_ADR    + STORAGE_LOG_POINTER_SIZE    )
#define  STORAGE_JOURNAL_ADR        ( STORAGE_LOG_ADR            + STORAGE_LOG_SIZE            )

#define  STORAGE_EWA_ADR            ( STORAGE_MAP_ADR            + STORAGE_JOURNAL_SIZE        )
#define  STORAGE_EWA_DATA_ADR       ( STORAGE_EWA_ADR            + EEPROM_LENGTH_SIZE          )
#define  STORAGE_RESERVE_ADR        ( STORAGE_EWA_ADR            + STORAGE_WEB_SIZE            )
#define  STORAGE_MEASUREMENT_SR_ADR ( STORAGE_JOURNAL_ADR        + STORAGE_RESERVE_SIZE        )
#define  STORAGE_MEASUREMENT_ADR    ( STORAGE_MEASUREMENT_SR_ADR + STORAGE_MEASUREMENT_SR_SIZE )


#define  STORAGE_SR_EMPTY           0xFFU  /* The register as default is 0xFF */
/*------------------------- Macros -------------------------------------*/

/*-------------------------- ENUM --------------------------------------*/

/*----------------------- Structures -----------------------------------*/

/*------------------------ Functions -----------------------------------*/
EEPROM_STATUS eSTORAGEwriteMap ( void );
EEPROM_STATUS eSTORAGEreadMap ( uint32_t* output );
uint8_t       uSTORAGEcheckMap ( const uint32_t* map );
EEPROM_STATUS eSTORAGEwriteConfigs ( void );
EEPROM_STATUS eSTORAGEreadConfigs ( void );
EEPROM_STATUS eSTORAGEdeleteConfigs ( void );
EEPROM_STATUS eSTORAGEwriteCharts ( void );
EEPROM_STATUS eSTORAGEreadCharts ( void );
EEPROM_STATUS eSTORAGEreadFreeData ( FREE_DATA_ADR n );
EEPROM_STATUS eSTORAGEsaveFreeData ( FREE_DATA_ADR n );
EEPROM_STATUS eSTORAGEsetFreeData ( FREE_DATA_ADR n, const uint16_t* data );
EEPROM_STATUS eSTORAGEreadLogPointer ( uint16_t* pointer );
EEPROM_STATUS eSTORAGEwriteLogPointer ( uint16_t pointer );
EEPROM_STATUS eSTORAGEreadLogRecord ( uint16_t adr, LOG_RECORD_TYPE* record );
EEPROM_STATUS eSTORAGEwriteLogRecord ( uint16_t adr, const LOG_RECORD_TYPE* record );
EEPROM_STATUS eSTORAGEsavePassword ( void );
EEPROM_STATUS eSTORAGEloadPassword ( void );
EEPROM_STATUS eSTORAGEreadMeasurement ( uint16_t adr, uint8_t length, uint16_t* data );
EEPROM_STATUS eSTORAGEeraseMeasurement ( void );
EEPROM_STATUS eSTORAGEaddMeasurement ( uint16_t adr, uint8_t length, const uint16_t* data );
EEPROM_STATUS eSTORAGEreadMeasurementCounter ( uint16_t* length );
/*----------------------------------------------------------------------*/
#endif /* INC_STORAGE_H_ */

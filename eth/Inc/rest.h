/*
 * rest.h
 *
 *  Created on: 19 февр. 2020 г.
 *      Author: mikhail.mikhailov
 */

#ifndef INC_REST_H_
#define INC_REST_H_
/*----------------------------- Includes -------------------------------------*/
#include <freeData.h>
#include "stm32f2xx_hal.h"
#include "config.h"
#include "chart.h"
#include "RTC.h"
/*------------------------------ Enum ----------------------------------------*/
typedef enum
{
  REST_OK,
  REST_MESSAGE_FORMAT_ERROR,
  REST_RECORD_LOST_ERROR,
  REST_RECORD_FORMAT_ERROR,
  REST_RECORD_NO_DATA_ERROR,
  REST_RECORD_LENGTH_ERROR,
  REST_RECORD_COPY_ERROR,
  REST_RECORD_ARRAY_FORMAT_ERROR,
  REST_RECORD_ARRAY_SIZE_ERROR,
} REST_ERROR;

typedef enum
{
  REST_LAST_RECORD,
  REST_CONT_RECORD
} RESTrecordPos;

typedef enum
{
  REST_CONFIGS       = 0x00U,
  REST_CHARTS        = 0x01U,
  REST_SAVE_CONFIGS  = 0x02U,
  REST_SAVE_CHARTS   = 0x03U,
  REST_TIME          = 0x04U,
  REST_DATA          = 0x05U,
  REST_REQUEST_ERROR = 0xFFU,
} REST_REQUEST;

typedef enum
{
  REST_NO_ADR,
  REST_YES_ADR,
} REST_ADDRESS;

/*------------------------------ Default -------------------------------------*/
#define	QUOTES_ANCII              0x22U
#define	REST_BUFFER_SIZE          700U
#define	REST_DIGIT_BUFFER_SIZE    7U
#define	REST_REQUEST_NUMBER       6U            // Number of active REST requests
#define	REST_REQUEST_CONFIGS      "configs"     // GET and PUT configuration
#define	REST_REQUEST_CHARTS       "charts"      // GET and PUT chart
#define REST_REQUEST_SAVE_CONFIGS "saveConfigs" // PUT command to save all configurations to EEPROM
#define REST_REQUEST_SAVE_CHARTS  "saveCharts"  // PUT command to save all charts to EEPROM
#define REST_REQUEST_TIME         "time"        // GET and PUT time
#define REST_REQUEST_DATA         "data"        // GET and PUT data ( 2 byte data )

#define TIME_HOUR_STR             "hour"
#define TIME_MIN_STR              "min"
#define TIME_SEC_STR              "sec"
#define TIME_YEAR_STR             "year"
#define TIME_MONTH_STR            "month"
#define TIME_DAY_STR              "day"
#define TIME_WDAY_STR             "wday"

#define FREE_DATA_STR             "value"
/*------------------------------ Extern --------------------------------------*/
extern const char 	*restRequeststr[REST_REQUEST_NUMBER];
/*----------------------------- Functions ------------------------------------*/
uint32_t     uRESTmakeConfig ( const eConfigReg* reg, char* output );
uint32_t     uRESTmakeChart ( const eChartData* chart, char* output );
uint32_t     uRESTmakeTime ( const RTC_TIME* time, char* output );
uint32_t     uRESTmakeData( uint16_t data, char* output );
REST_ERROR   eRESTparsingConfig ( char* input, eConfigReg* reg );
REST_ADDRESS eRESTgetRequest ( char* path, REST_REQUEST* request, uint16_t* adr );
REST_ERROR   eRESTparsingChart ( char* input, eChartData* chart );
REST_ERROR   eRESTparsingTime( char* input, RTC_TIME* time );
REST_ERROR   eRESTparsingData ( char* input, uint16_t* data );
/*---------------------------------------------------------------------------------------------------*/

#endif /* INC_REST_H_ */

/*
 * rest.h
 *
 *  Created on: 19 февр. 2020 г.
 *      Author: mikhail.mikhailov
 */

#ifndef INC_REST_H_
#define INC_REST_H_
/*----------------------------- Includes -------------------------------------*/
#include "freeData.h"
#include "stm32f2xx_hal.h"
#include "config.h"
#include "chart.h"
#include "RTC.h"
#include "controllerTypes.h"
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
  REST_CONFIGS        = 0x00U,
  REST_CHARTS         = 0x01U,
  REST_SAVE_CONFIGS   = 0x02U,
  REST_SAVE_CHARTS    = 0x03U,
  REST_TIME           = 0x04U,
  REST_FREE_DATA      = 0x05U,
  REST_LOG            = 0x06U,
  REST_LOG_ERASE      = 0x07U,
  REST_PASSWORD       = 0x08U,
  REST_AUTH           = 0x09U,
  REST_ERASE_PASSWORD = 0x0AU,

  REST_MEMORY             = 0x0B,
  REST_MEASUREMENT        = 0x0C,
  REST_MEASUREMENT_ERASE  = 0x0D,
  REST_MEASUREMENT_LENGTH = 0x0E,
  REST_OUTPUT             = 0x0F,
  REST_SYSTEM             = 0x10,

  REST_REQUEST_ERROR  = 0xFFU,
} REST_REQUEST;

typedef enum
{
  REST_NO_ADR,
  REST_YES_ADR,
} REST_ADDRESS;

/*------------------------------ Default -------------------------------------*/
#define	QUOTES_ANCII                    0x22U
#define	REST_BUFFER_SIZE                500U
#define	REST_DIGIT_BUFFER_SIZE          7U
#define	REST_REQUEST_NUMBER             17U                   /* Number of active REST requests */
#define	REST_REQUEST_CONFIGS            "configs"             /* GET and PUT configuration */
#define	REST_REQUEST_CHARTS             "charts"              /* GET and PUT chart */
#define REST_REQUEST_SAVE_CONFIGS       "saveConfigs"         /* PUT command to save all configurations to EEPROM */
#define REST_REQUEST_SAVE_CHARTS        "saveCharts"          /* PUT command to save all charts to EEPROM */
#define REST_REQUEST_TIME               "time"                /* GET and PUT time */
#define REST_REQUEST_DATA               "data"                /* GET and PUT data ( 2 byte data ) */
#define REST_REQUEST_LOG                "log"                 /* GET log */
#define REST_REQUEST_LOG_ERASE          "eraseLog"            /* PUT command erase log */
#define REST_REQUEST_PASSWORD           "password"            /* PUT password */
#define REST_REQUEST_AUTH               "auth"                /* PUT authorization */
#define REST_REQUEST_ERASE_PASSWORD     "erasePassword"       /* PUT reset password */
#define REST_REQUEST_MEMORY             "memory"              /* GET memory size */
#define REST_REQUEST_MEASUREMENT        "measurement"         /* GET measurement data */
#define REST_REQUEST_MEASUREMENT_ERASE  "eraseMeasurement"    /* PUT measurement erase command */
#define REST_REQUEST_MEASUREMENT_LENGTH "lengthMeasurement"   /* GET measurement data length */
#define REST_REQUEST_OUTPUT             "output"              /* GET and PUT output data */
#define REST_REQUEST_SYSTEM             "system"              /* GET system data */

#define TIME_HOUR_STR               "hour"
#define TIME_MIN_STR                "min"
#define TIME_SEC_STR                "sec"
#define TIME_YEAR_STR               "year"
#define TIME_MONTH_STR              "month"
#define TIME_DAY_STR                "day"
#define TIME_WDAY_STR               "wday"
#define FREE_DATA_STR               "value"
#define PASSWORD_STATUS_STR         "status"
#define PASSWORD_DATA_STR           "data"
#define LOG_TIME_STR                "time"
#define LOG_TYPE_STR                "type"
#define LOG_ACTION_STR              "action"
#define MEMORY_SIZE_STR             "size"
#define MESUREMENT_LENGTH_STR       "length"
#define SYSTEM_TOTAL_STR            "total"
#define SYSTEM_USAGE_STR            "usage"
/*------------------------------ Extern --------------------------------------*/
extern const char 	*restRequestStr[REST_REQUEST_NUMBER];
/*----------------------------- Functions ------------------------------------*/
uint32_t     uRESTmakeConfig ( const eConfigReg* reg, char* output );
uint32_t     uRESTmakeChart ( const eChartData* chart, char* output );
uint32_t     uRESTmakeTime ( const RTC_TIME* time, char* output );
uint32_t     uRESTmakeMemorySize ( char* output );
uint32_t     uRESTmakeMeasurementLength ( char* output );
uint32_t     uRESTmakeSystem ( char* output );
uint32_t     uRESTmakeData ( uint16_t data, char* output );
uint32_t     uRESTmakeLog ( const LOG_RECORD_TYPE* record, char* output );
uint32_t     uRESTmakeOutput ( const eConfigReg* data, const char* name, char* output );
uint32_t     uRESTmakeMeasurement ( uint32_t n, char* output );
REST_ADDRESS eRESTgetRequest ( char* path, REST_REQUEST* request, uint16_t* adr );
REST_ERROR   eRESTparsingConfig ( char* input, uint16_t adr );
REST_ERROR   eRESTparsingChart ( char* input, uint16_t adr );
REST_ERROR   eRESTparsingTime ( char* input, RTC_TIME* time );
REST_ERROR   eRESTparsingFreeData ( char* input, uint16_t* data );
REST_ERROR   eRESTparsingPassword ( char* input, PASSWORD_TYPE* password );
REST_ERROR   eRESTparsingAuth ( char* input, PASSWORD_TYPE* password );
/*---------------------------------------------------------------------------------------------------*/

#endif /* INC_REST_H_ */

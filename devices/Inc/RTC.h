/*
 * RTC3231.h
 *
 *  Created on: 8 июн. 2020 г.
 *      Author: mikhail.mikhailov
 */

#ifndef INC_RTC_H_
#define INC_RTC_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "stm32f2xx_hal_i2c.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#if defined( RTC_DEVICE_DS3231 )
  #include "RTC_DS3231.h"
  #define  RTC_CODE  0U
#elif defined( RTC_DEVICE_MAX31329 )
  #include "RTC_MAX31329.h"
  #define  RTC_CODE  1U
#else
  #error "There is no RTC device defined!"
#endif
/*------------------------ Define --------------------------------------*/
//#define  RTC_ADDITIONAL
#define  RTC_TIMEOUT          1000U
#define  RTC_BUSY_TIMEOUT     1U
#define  RTC_POOL_TIMEOUT     10U
#define  RTC_TASK_DELAY       60000U
/*--------- Time encoding ---------------*/
#define  RTC_TIME_SIZE        7U
#define  RTC_SEC_MSK          0x7FU
#define  RTC_MIN_MSK          0x7FU
#define  RTC_HOUR_MSK         0x3FU
#define  RTC_UOUR_PMnAP       0x20U
#define  RTC_HOUR_FORMAT_MSK  0x40U
#define  RTC_WDAY_MSK         0x07U
#define  RTC_DAY_MSK          0x3FU
#define  RTC_DATE_CENTURY     0x80U
#define  RTC_MONTH_MSK        0x1F
#define  RTC_YEAR_MSK         0xFFU

#define  RTC_ALARMn_DYDT      0x40U
/*---------- Alarm rates ----------------*/
#define  RTC_AM               0x80U  /* Alarm rate bit mask */
#define  RTC_AM_PSEC          0x0FU  /* Alarm once per second */
#define  RTC_AM_WSEC          0x0EU  /* Alarm when seconRTC match */
#define  RTC_AM_WMIN          0x0CU  /* Alarm when minutes and seconRTC match */
#define  RTC_AM_WHOUR         0x08U  /* Alarm when hours, minutes, and seconRTC match */
#define  RTC_AM_WDATE         0x00U  /* Alarm when date, hours, minutes, and seconRTC match */
#define  RTC_AM_WDAY          0x10U  /* Alarm when day, hours, minutes, and seconRTC match */
/*---------- Temperature ----------------*/
#define  RTC_UTR_MSK          0xFFU
#define  RTC_LTR_MSK          0xC0U
#define  RTC_LTR_SHIFT        6U
#define  RTC_LTR_DIV          4U
/*------------------------- Macros -------------------------------------*/
#define  RTC_GET_TEMP( utr, ltr )    ( ( float )( ( signed char ) utr ) + ( ( ( float )( ( ltr & RTC_LTR_MSK ) >> RTC_LTR_SHIFT ) ) / RTC_LTR_DIV ) )
/*-------------------------- ENUM --------------------------------------*/
typedef enum
{
  RTC_OK,
  RTC_BUSY,
  RTC_ERROR,
  RTC_INIT_ERROR,
  RTC_FORMAT_ERROR,
} RTC_STATUS;

typedef enum
{
  AM = 0U,
  PM = 1U,
} TIME_FORMAT;

typedef enum
{
  RTC_FREQ_1HZ,
  RTC_FREQ_1024HZ,
  RTC_FREQ_4096HZ,
  RTC_FREQ_8192Hz,
} RTC_FREQ;

typedef enum
{
  SUNDAY    = 0x01U,
  MONDAY    = 0x02U,
  TUESDAY   = 0x03U,
  WEDNESDAY = 0x04U,
  THURSDAY  = 0x05U,
  FRIDAY    = 0x06U,
  SATURDAY  = 0x07U,
} DAY_TYPE;

typedef enum
{
  MONTH_JAN = 0x01U,
  MONTH_FEB = 0x02U,
  MONTH_MAR = 0x03U,
  MONTH_APR = 0x04U,
  MONTH_MAY = 0x05U,
  MONTH_JUN = 0x06U,
  MONTH_JUL = 0x07U,
  MONTH_AUG = 0x08U,
  MONTH_SEP = 0x09U,
  MONTH_OCT = 0x0AU,
  MONTH_NOV = 0x0BU,
  MONTH_DEC = 0x0CU,
} MONTH_TYPE;

typedef enum
{
  ALARM_ON_DATE,
  ALARM_ON_DAY_OF_WEEK,
} ALARM_RTC_TYPE;
/*----------------------------------------------------------------------*/
#define  RTC_SEC_MAX          59U
#define  RTC_MIN_MAX          59U
#define  RTC_HOUR_MAX         23U
#define  RTC_WDAY_MIN         SUNDAY
#define  RTC_WDAY_MAX         SATURDAY
#define  RTC_DAY_MIN          1U
#define  RTC_DAY_MAX          31U
#define  RTC_MONTH_MIN        MONTH_JAN
#define  RTC_MONTH_MAX        MONTH_DEC
#define  RTC_YEAR_MIN         0U
#define  RTC_YEAR_MAX         99U
/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  uint8_t      sec;
  uint8_t      min;
  uint8_t      hour;
  DAY_TYPE     wday;
  uint8_t      day;
  MONTH_TYPE   month;
  uint8_t      year;
} RTC_TIME; /* 7 byte */

#if defined( RTC_ADDITIONAL )
typedef struct __packed
{
  uint8_t         sec;
  uint8_t         min;
  uint8_t         hour;
  DAY_TYPE        wday;
  uint8_t         day;
  ALARM_RTC_TYPE  type;      /* Alarm can be activate on date or on day of the week */
  uint8_t         rate;      /* Rate of alarm */
  uint8_t         ie   : 1U; /* Interrupt enable */
} RTC_ALARM;
#endif
/*------------------------ Functions -----------------------------------*/
void       vRTCinit ( I2C_HandleTypeDef* hi2c );             /* Ok */
void       vRTCgetCashTime ( RTC_TIME* time );               /* Ok */
void       vRTCcleanTime ( RTC_TIME* time );                 /* Ok */
RTC_STATUS eRTCgetTime ( RTC_TIME* time );                   /* Ok */
RTC_STATUS eRTCsetTime ( RTC_TIME* time );                   /* Ok */
#if defined( RTC_ADDITIONAL )
  RTC_STATUS eRTCsetOscillator ( uint8_t enb, RTC_FREQ freq ); /**/
  RTC_STATUS eRTCreadFreq ( RTC_FREQ* freq );                  /**/
  RTC_STATUS vRTCsetCalibration ( signed char value );         /**/
  RTC_STATUS eRTCgetCalibration ( signed char* value );        /**/
  RTC_STATUS eRTCgetTemperature ( float* data );               /**/
  RTC_STATUS eRTCsetTemperatureCompensation ( uint8_t enb );   /**/
  RTC_STATUS eRTCsetAlarm ( uint8_t n, RTC_ALARM* alarm );     /**/
  RTC_STATUS eRTCgetAlarm ( uint8_t n, RTC_ALARM* alarm );     /**/
  RTC_STATUS vRTCclearAlarm ( uint8_t n );                     /**/
  uint8_t    uRTCcheckIfAlarm ( void );                        /**/
  RTC_STATUS eRTCsetExternSquareWave ( uint8_t enb );          /**/
  RTC_STATUS eRTCsetInterrupt ( uint8_t ext );                 /**/
#endif
/*----------------------------------------------------------------------*/
#endif /* INC_RTC_H_ */

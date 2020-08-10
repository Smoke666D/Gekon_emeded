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
/*------------------------ Define --------------------------------------*/
#define  RTC_SEC_MAX          59U
#define  RTC_MIN_MAX          59U
#define  RTC_HOUR_MAX         23U
#define  RTC_WDAY_MIN         1U
#define  RTC_WDAY_MAX         7U
#define  RTC_DAY_MIN          1U
#define  RTC_DAY_MAX          31U
#define  RTC_MONTH_MIN        1U
#define  RTC_MONTH_MAX        12U
#define  RTC_YEAR_MIN         0U
#define  RTC_YEAR_MAX         99U

#define  RTC_DEVICE_ADR       0xD0U
#define  RTC_MEMORY_SIZE      19U
#define  RTC_TIMEOUT          1000U

#define  RTC_SECONRTC         0x00U
#define  RTC_MINUTES          0x01U
#define  RTC_HOURS            0X02U
#define  RTC_WDAY             0x03U
#define  RTC_DAY              0x04U
#define  RTC_MONTH            0x05U
#define  RTC_YEAR             0x06U
#define  RTC_ALARM1_SECONRTC  0x07U
#define  RTC_ALARM1_MINUTES   0x08U
#define  RTC_ALARM1_HOURS     0x09U
#define  RTC_ALARM1_DAY       0x0AU
#define  RTC_ALARM2_MINUTES   0x0BU
#define  RTC_ALARM2_HOURS     0x0CU
#define  RTC_ALARM2_DAY       0x0DU
#define  RTC_CR               0x0EU  /* Control Register */
#define  RTC_SR               0x0FU  /* Status Register */
#define  RTC_AO               0x10U  /* Aging Offset */
#define  RTC_UTR              0x11U  /* Temperature Register (Upper Byte) signed char */
#define  RTC_LTR              0x12U  /* Temperature Register (Lower Byte) */

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
/*---------- Control Register -----------*/
#define  RTC_CR_A1IE          0x01U  /* Alarm 1 Interrupt Enable */
#define  RTC_CR_A2IE          0x02U  /* Alarm 2 Interrupt Enable */
#define  RTC_CR_INTCN         0x04U  /* Interrupt Control */
#define  RTC_CR_RS1           0x08U  /* Rate Select */
#define  RTC_CR_RS2           0x10U  /* Rate Select */
#define  RTC_CR_CONV          0x20U  /* Convert Temperature */
#define  RTC_CR_BBSQW         0x40U  /* Battery-Backed Square-Wave Enable */
#define  RTC_CR_EOSC          0x80U  /* Enable Oscillator */
#define  RTC_CR_RS_1Hz        0x00U  /* Frequency of the square-wave output is 1 Hz */
#define  RTC_CR_RS_1024Hz     0x08U  /* Frequency of the square-wave output is 1.024 kHz */
#define  RTC_CR_RS_4096Hz     0x10U  /* Frequency of the square-wave output is 4.096 kHz */
#define  RTC_CR_RS_8192Hz     0x18U  /* Frequency of the square-wave output is 8.192 kHz */
/*---------- Status Register ------------*/
#define  RTC_SR_A1F           0x01U  /* Alarm 1 Flag */
#define  RTC_SR_A2F           0x02U  /* Alarm 2 Flag */
#define  RTC_SR_BSY           0x04U  /* Busy */
#define  RTC_SR_EN32KHZ       0x08U  /* Enable 32kHz Output */
#define  RTC_SR_OSF           0x80U  /* Oscillator Stop Flag */
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
  MONTH_JAN,
  MONTH_FEB,
  MONTH_MAR,
  MONTH_APR,
  MONTH_MAY,
  MONTH_JUN,
  MONTH_JUL,
  MONTH_AUG,
  MONTH_SEP,
  MONTH_OCT,
  MONTH_NOV,
  MONTH_DEC,
} MONTH_TYPE;

typedef enum
{
  ALARM_ON_DATE,
  ALARM_ON_DAY_OF_WEEK,
} ALARM_RTC_TYPE;
/*----------------------- Structures -----------------------------------*/
typedef struct
{
  uint8_t      sec;
  uint8_t      min;
  uint8_t      hour;
  DAY_TYPE     wday;
  uint8_t      day;
  MONTH_TYPE   month;
  uint8_t      year;
} RTC_TIME; /* 7 byte */

typedef struct
{
  uint8_t         sec;
  uint8_t         min;
  uint8_t         hour;
  DAY_TYPE        wday;
  uint8_t         day;
  ALARM_RTC_TYPE  type;   /* Alarm can be activate on date or on day of the week */
  uint8_t         rate;   /* Rate of alarm */
  uint8_t         ie;     /* Interrupt enable */
} RTC_ALARM;
/*------------------------ Functions -----------------------------------*/
void        vInitRTC( I2C_HandleTypeDef* hi2c );
RTC_STATUS  eRTCsetOscillator( uint8_t enb, RTC_FREQ freq );
RTC_STATUS  eRTCreadFreq( RTC_FREQ* freq );
RTC_STATUS  eRTCgetTime( RTC_TIME* time );
RTC_STATUS  vRTCsetTime( RTC_TIME* time );
RTC_STATUS  vRTCsetCalibration( signed char value );
RTC_STATUS  eRTCgetCalibration( signed char* value );
RTC_STATUS  eRTCgetTemperature( float* data );
RTC_STATUS  eRTCsetTemperatureCompensation( uint8_t enb );
RTC_STATUS  eRTCsetAlarm( uint8_t n, RTC_ALARM* alarm );
RTC_STATUS  eRTCgetAlarm( uint8_t n, RTC_ALARM* alarm );
RTC_STATUS  vRTCclearAlarm( uint8_t n );
uint8_t     uRTCcheckIfAlarm( void );
RTC_STATUS  eRTCsetExternSquareWave( uint8_t enb );
RTC_STATUS  eRTCsetInterrupt( uint8_t ext );
/*----------------------------------------------------------------------*/
#endif /* INC_RTC_H_ */

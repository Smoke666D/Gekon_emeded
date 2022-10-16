/*
 * DS3231.h
 *
 *  Created on: Feb 2, 2022
 *      Author: 79110
 */

#ifndef INC_RTC_M41T62_H_
#define INC_RTC_M41T62_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
/*------------------------ Define --------------------------------------*/
#define  RTC_DEVICE_ADR           0xD0
#define  RTC_MEMORY_SIZE          0U
#define  RTC_RAM_SIZE             0U
/*---------- Status Register ------------*/
/*---------- Control Register -----------*/
#define  RTC_FLAG_OF   0x04U  /* Oscillator fail        */
#define  RTC_FLAG_AF   0x40U  /* Alarm Interrupt Enable */
#define  RTC_FLAG_WDF  0x80U  /* Watchdog flag          */
/*------------------------- Macros -------------------------------------*/
/*-------------------------- Enum --------------------------------------*/
typedef enum
{
  RTC_THS_SECONDS,      /* 00 */
  RTC_SECONDS,          /* 01 */
  RTC_MINUTES,          /* 02 */
  RTC_HOURS,            /* 03 */
  RTC_WDAY,             /* 04 */
  RTC_DAY,              /* 05 */
  RTC_MONTH,            /* 06 */
  RTC_YEAR,             /* 07 */
  RTC_CALIBRATION,      /* 08 */
  RTC_WATCHDOG,         /* 09 */
  RTC_ALARM_MONTH,      /* 10 */
  RTC_ALARM_DAY,        /* 11 */
  RTC_ALARM_HOUR,       /* 12 */
  RTC_ALARM_MIN,        /* 13 */
  RTC_ALARM_SEC,        /* 14 */
  RTC_FLAG              /* 15 */
} RTC_REGISTER;
/*----------------------- Structures -----------------------------------*/
/*------------------------ Functions -----------------------------------*/
/*----------------------------------------------------------------------*/
#endif /* INC_RTC_DS3231_H_ */

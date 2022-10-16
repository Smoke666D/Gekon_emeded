/*
 * DS3231.h
 *
 *  Created on: Feb 2, 2022
 *      Author: 79110
 */

#ifndef INC_RTC_DS3231_H_
#define INC_RTC_DS3231_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
/*------------------------ Define --------------------------------------*/
#define  RTC_DEVICE_ADR           0xD0
#define  RTC_MEMORY_SIZE          19U
#define  RTC_RAM_SIZE             0U
/*---------- Status Register ------------*/
#define  RTC_SR_A1F               0x01U  /* Alarm 1 Flag */
#define  RTC_SR_A2F               0x02U  /* Alarm 2 Flag */
#define  RTC_SR_BSY               0x04U  /* Busy */
#define  RTC_SR_EN32KHZ           0x08U  /* Enable 32kHz Output */
#define  RTC_SR_OSF               0x80U  /* Oscillator Stop Flag */
/*---------- Control Register -----------*/
#define  RTC_CR_A1IE              0x01U  /* Alarm 1 Interrupt Enable */
#define  RTC_CR_A2IE              0x02U  /* Alarm 2 Interrupt Enable */
#define  RTC_CR_INTCN             0x04U  /* Interrupt Control */
#define  RTC_CR_RS1               0x08U  /* Rate Select */
#define  RTC_CR_RS2               0x10U  /* Rate Select */
#define  RTC_CR_CONV              0x20U  /* Convert Temperature */
#define  RTC_CR_BBSQW             0x40U  /* Battery-Backed Square-Wave Enable */
#define  RTC_CR_EOSC              0x80U  /* Enable Oscillator */
#define  RTC_CR_RS_1Hz            0x00U  /* Frequency of the square-wave output is 1 Hz */
#define  RTC_CR_RS_1024Hz         0x08U  /* Frequency of the square-wave output is 1.024 kHz */
#define  RTC_CR_RS_4096Hz         0x10U  /* Frequency of the square-wave output is 4.096 kHz */
#define  RTC_CR_RS_8192Hz         0x18U  /* Frequency of the square-wave output is 8.192 kHz */
/*------------------------- Macros -------------------------------------*/
/*-------------------------- Enum --------------------------------------*/
typedef enum
{
  RTC_SECONDS,          /* 00 */
  RTC_MINUTES,          /* 01 */
  RTC_HOURS,            /* 02 */
  RTC_WDAY,             /* 03 */
  RTC_DAY,              /* 04 */
  RTC_MONTH,            /* 05 */
  RTC_YEAR,             /* 06 */
  RTC_ALARM1_SECONDS,   /* 07 */
  RTC_ALARM1_MINUTES,   /* 08 */
  RTC_ALARM1_HOURS,     /* 09 */
  RTC_ALARM1_DAY,       /* 0A */
  RTC_ALARM2_MINUTES,   /* 0B */
  RTC_ALARM2_HOURS,     /* 0C */
  RTC_ALARM2_DAY,       /* 0D */
  RTC_CR,               /* 0E - Control Register */
  RTC_SR,               /* 0F - Status Register */
  RTC_AO,               /* 10 - Aging Offset */
  RTC_UTR,              /* 11 - Temperature Register (Upper Byte) signed char */
  RTC_LTR               /* 12 - Temperature Register (Lower Byte) */
} RTC_REGISTER;
/*----------------------- Structures -----------------------------------*/
/*------------------------ Functions -----------------------------------*/
/*----------------------------------------------------------------------*/
#endif /* INC_RTC_DS3231_H_ */

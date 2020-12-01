/*
 * log.h
 *
 *  Created on: 16 мар. 2020 г.
 *      Author: mikhail.mikhailov
 */
#ifndef INC_JOURNAL_H_
#define INC_JOURNAL_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "controllerTypes.h"
#include "RTC.h"
#include "storage.h"
/*------------------------ Define --------------------------------------*/
#define  LOG_TIME_ERROR   1U
#define  LOG_START_YEAR   2020U
#define  LOG_DAY_MASK     0xF8000000U
#define  LOG_DAY_SHIFT    27U
#define  LOG_MONTH_MASK   0x07800000U
#define  LOG_MONTH_SHIFT  23U
#define  LOG_YEAR_MASK    0x007E0000U
#define  LOG_YEAR_SHIFT   17U
#define  LOG_HOUR_MASK    0x0001F000U
#define  LOG_HOUR_SHIFT   12U
#define  LOG_MIN_MASK     0x00000FC0U
#define  LOG_MIN_SHIFT    6U
#define  LOG_SEC_MASK     0x0000003FU
#define  LOG_SEC_SHIFT    0U
/*------------------------ Macros --------------------------------------*/
#define  GET_LOG_DAY( d )                                 ( ( d & LOG_DAY_MASK   ) >> LOG_DAY_SHIFT )
#define  GET_LOG_MONTH( d )                               ( ( d & LOG_MONTH_MASK ) >> LOG_MONTH_SHIFT )
#define  GET_LOG_YEAR( d )                                ( ( d & LOG_YEAR_MASK  ) >> LOG_YEAR_SHIFT )
#define  GET_LOG_HOUR( d )                                ( ( d & LOG_HOUR_MASK  ) >> LOG_HOUR_SHIFT )
#define  GET_LOG_MIN( d )                                 ( ( d & LOG_MIN_MASK   ) >> LOG_MIN_SHIFT )
#define  GET_LOG_SEC( d )                                 ( ( d & LOG_SEC_MASK   ) >> LOG_SEC_SHIFT )
#define  SET_LOG_DATE( day, month, year, hour, min, sec)  ( ( ( uint32_t )day << LOG_DAY_SHIFT ) | ( ( uint32_t )month << LOG_MONTH_SHIFT ) | ( ( uint32_t )year << LOG_YEAR_SHIFT ) | ( ( uint32_t )hour << LOG_HOUR_SHIFT ) | ( ( uint32_t )min << LOG_MIN_SHIFT ) | ( ( uint32_t )sec << LOG_SEC_SHIFT ) )
/*------------------------- Enum ---------------------------------------*/
typedef enum
{
  JOURNAL_MEASUR_ENB_TIME,
  JOURNAL_MEASUR_ENB_DATE,
  JOURNAL_MEASUR_ENB_OIL_PRESSURE,
  JOURNAL_MEASUR_ENB_COOLANT_TEMP,
  JOURNAL_MEASUR_ENB_FUEL_LEVEL,
  JOURNAL_MEASUR_ENB_SPEED,
  JOURNAL_MEASUR_ENB_DIN_A,
  JOURNAL_MEASUR_ENB_DIN_B,
  JOURNAL_MEASUR_ENB_DIN_C,
  JOURNAL_MEASUR_ENB_DIN_D,
  JOURNAL_MEASUR_ENB_GENERATOR_VOLTAGE,
  JOURNAL_MEASUR_ENB_GENERATOR_FREQ,
  JOURNAL_MEASUR_ENB_MAINS_VOLTAGE,
  JOURNAL_MEASUR_ENB_MAINS_FREQ,
  JOURNAL_MEASUR_ENB_BATTERY_VOLTAGE
} JOURNAL_MEASUR_ENB;
/*----------------------- Structures -----------------------------------*/
/*---------------------- Functions -------------------------------------*/
LOG_STATUS vLOGinit ( void );
LOG_STATUS vLOGclear ( void );
LOG_STATUS eLOGmakeRecord ( SYSTEM_EVENT event, LOG_RECORD_TYPE* record );
LOG_STATUS eLOGaddRecord ( LOG_RECORD_TYPE* record );
/*----------------------------------------------------------------------*/
#endif /* INC_JOURNAL_H_ */

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
#define  LOG_TIME_ERROR        1U
#define  LOG_START_YEAR        2020U
#define  LOG_DAY_MASK          0xF8000000U
#define  LOG_DAY_SHIFT         27U
#define  LOG_MONTH_MASK        0x07800000U
#define  LOG_MONTH_SHIFT       23U
#define  LOG_YEAR_MASK         0x007E0000U
#define  LOG_YEAR_SHIFT        17U
#define  LOG_HOUR_MASK         0x0001F000U
#define  LOG_HOUR_SHIFT        12U
#define  LOG_MIN_MASK          0x00000FC0U
#define  LOG_MIN_SHIFT         6U
#define  LOG_SEC_MASK          0x0000003FU
#define  LOG_SEC_SHIFT         0U
#define  JOURNAL_BLOB_MAX_SIZE 27U
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
  JOURNAL_MEASUR_TIME,              /*  0 */
  JOURNAL_MEASUR_OIL,               /*  2 */
  JOURNAL_MEASUR_COOLANT,           /*  3 */
  JOURNAL_MEASUR_FUEL,              /*  4 */
  JOURNAL_MEASUR_SPEED,             /*  5 */
  JOURNAL_MEASUR_DIN,               /*  6 */
  JOURNAL_MEASUR_GENERATOR_VOLTAGE, /*  7 */
  JOURNAL_MEASUR_GENERATOR_FREQ,    /*  8 */
  JOURNAL_MEASUR_GENERATOR_CURRENT, /*  9 */
  JOURNAL_MEASUR_GENERATOR_POWER,   /* 10 */
  JOURNAL_MEASUR_MAINS_VOLTAGE,     /* 11 */
  JOURNAL_MEASUR_MAINS_FREQ,        /* 12 */
  JOURNAL_MEASUR_BATTERY,           /* 13 */
} JOURNAL_MEASUR_ENB;

/*
typedef enum
{
  JOURNAL_MEASUR_DIN_A = 0x01U,
  JOURNAL_MEASUR_DIN_B = 0x02U,
  JOURNAL_MEASUR_DIN_C = 0x04U,
  JOURNAL_MEASUR_DIN_D = 0x08U,
} JOURNAL_MEASUR_DIN;
*/
typedef enum
{
  JOURNAL_MEASUR_STATUS_IDLE,
  JOURNAL_MEASUR_STATUS_
} JOURNAL_MEASUR_STATUS;
/*------------------------- Typesc--------------------------------------*/
typedef uint16_t length_t;
/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  PERMISSION time    : 1U; /* 4 */
  PERMISSION oil     : 1U; /* 2 */
  PERMISSION coolant : 1U; /* 2 */
  PERMISSION fuel    : 1U; /* 2 */
  PERMISSION speed   : 1U; /* 2 */
  PERMISSION din     : 1U; /* 1 */
  PERMISSION genV    : 1U; /* 2 */
  PERMISSION genF    : 1U; /* 2 */
  PERMISSION genC    : 1U; /* 2 */
  PERMISSION genP    : 1U; /* 2 */
  PERMISSION mainsV  : 1U; /* 2 */
  PERMISSION mainsF  : 1U; /* 2 */
  PERMISSION battery : 1U; /* 2 */
} JOURNAL_MEASURMENT_ENB;

typedef struct __packed
{
  length_t length;
  length_t counter;
  uint8_t  blob[JOURNAL_BLOB_MAX_SIZE];
  /*
  uint32_t time;
  uint16_t oil;
  uint16_t coolant;
  uint16_t fuel;
  uint16_t speed;
  uint8_t  din;
  uint16_t genV;
  uint16_t genF;
  uint16_t genC;
  uint16_t mainsV;
  uint16_t mainsF;
  uint16_t battery;
  */
} JOURNAL_MEASURMENT_DATA;

typedef struct __packed
{
  PERMISSION              enb    : 1U;
  JOURNAL_MEASURMENT_ENB  option;
  JOURNAL_MEASURMENT_DATA data;
  SYSTEM_TIMER            timer;
  uint32_t                size;
} JOURNAL_MEASURMENT;
/*---------------------- Functions -------------------------------------*/
LOG_STATUS vLOGinit ( void );
LOG_STATUS vLOGclear ( void );
LOG_STATUS eLOGmakeRecord ( SYSTEM_EVENT event, LOG_RECORD_TYPE* record );
LOG_STATUS eLOGaddRecord ( LOG_RECORD_TYPE* record );
/*----------------------------------------------------------------------*/
#endif /* INC_JOURNAL_H_ */

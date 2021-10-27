/*
 * statistics.h
 *
 *  Created on: 26 апр. 2021 г.
 *      Author: 79110
 */
#ifndef INC_STATISTICS_H_
#define INC_STATISTICS_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "controllerTypes.h"
/*------------------------ Macros --------------------------------------*/
/*------------------------ Define --------------------------------------*/
#define STATISTICS_TIMER_STR  "Statistics timer    "
/*------------------------- Enum ---------------------------------------*/
typedef enum
{
  STATISTICS_STATUS_STOP,          /* 00 Statistics process inactive      */
  STATISTICS_STATUS_RUN,           /* 01 Statistics waiting minutes delay */
  STATISTICS_STATUS_HOUR_CHECK,    /* 02 Statistics every hour checker    */
  STATISTICS_STATUS_MINUTES_CHECK, /* 03 Statistics every minutes checker */
} STATISTICS_STATUS;
/*----------------------- Callbacks ------------------------------------*/

/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  SYSTEM_TIMER      timer;
  STATISTICS_STATUS status;
} STATISTICS_TYPE;

typedef struct __packed
{
  fix16_t  momental;
  fix16_t  average;
  fix16_t  power;
  fix16_t  fuel;
  fix16_t  cutout;
  uint16_t size;
} FUEL_RATE_TYPE;

typedef struct __packed
{
  fix16_t        tankSize;
  FUEL_RATE_TYPE rate;
  fix16_t        usage;
  ERROR_TYPE     stopLeakError;
  ALARM_TYPE     idleLeakAlarm;
  ALARM_TYPE     leakAlarm;
} FUEL_STATISTIC_TYPE;

typedef struct __packed
{
  ALARM_TYPE alarm;
  uint16_t   data;
} MAINTENCE_VALUE;

typedef struct __packed
{
  MAINTENCE_VALUE    oil;
  MAINTENCE_VALUE    air;
  MAINTENCE_VALUE    fuel;
} MAINTENCE_TYPE;

/*----------------------- Extern ---------------------------------------*/
/*----------------------- Functions ------------------------------------*/
void     vSTATISTICSinit ( void );
void     vSTATISTICSreset ( void );
uint16_t uSTATISTICSgetFuelMomentalRate ( void );
void     vSTATISTICSprocessing ( void );
/*----------------------------------------------------------------------*/
#endif /* INC_STATISTICS_H_ */

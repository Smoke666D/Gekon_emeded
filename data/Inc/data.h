/*
 * data.h
 *
 *  Created on: 25 авг. 2020 г.
 *      Author: 79110
 */

#ifndef INC_DATA_H_
#define INC_DATA_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
/*------------------------ Define --------------------------------------*/
#define  DATA_SIZE  5U

typedef enum
{
  ENGINE_WORK_TIME_ADR                 = 0x00U,
  ENGINE_STARTS_NUMBER_ADR             = 0x01U,
  MAINTENANCE_ALARM_OIL_TIME_LEFT_ADR  = 0x02U,
  MAINTENANCE_ALARM_AIR_TIME_LEFT_ADR  = 0x03U,
  MAINTENANCE_ALARM_FUEL_TIME_LEFT_ADR = 0x04U,
} DATA_ADR;
/*------------------------ Extern --------------------------------------*/
extern uint16_t engineWorkTime;               /* 0 */
extern uint16_t engineStartsNumber;           /* 1 */
extern uint16_t maintenanceAlarmOilTimeLeft;  /* 2 */
extern uint16_t maintenanceAlarmAirTimeLeft;  /* 3 */
extern uint16_t maintenanceAlarmFuelTimeLeft; /* 4 */

extern uint16_t* const dataArray[DATA_SIZE];
/*----------------------------------------------------------------------*/
#endif /* INC_DATA_H_ */

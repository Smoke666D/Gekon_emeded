/*
 * data.h
 *
 *  Created on: 25 авг. 2020 г.
 *      Author: 79110
 */

#ifndef INC_FREEDATA_H_
#define INC_FREEDATA_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
/*------------------------ Define --------------------------------------*/
#define  FREE_DATA_SIZE  9U
#define  PASSWORD_LEN    4U  /* digits */
#define  PASSWORD_SIZE   3U  /* bytes */
/*------------------------------ Enum ----------------------------------------*/
typedef enum
{
  ENGINE_WORK_TIME_ADR                 = 0x00U,
  ENGINE_WORK_MINUTES_ADR              = 0x01U,
  ENGINE_STARTS_NUMBER_ADR             = 0x02U,
  MAINTENANCE_ALARM_OIL_TIME_LEFT_ADR  = 0x03U,
  MAINTENANCE_ALARM_AIR_TIME_LEFT_ADR  = 0x04U,
  MAINTENANCE_ALARM_FUEL_TIME_LEFT_ADR = 0x05U,
  POWER_REACTIVE_USAGE_ADR             = 0x06U,
  POWER_ACTIVE_USAGE_ADR               = 0x07U,
  POWER_FULL_USAGE_ADR                 = 0x08U
} FREE_DATA_ADR;

typedef enum
{
  PASSWORD_RESET,
  PASSWORD_SET,
} PASSWORD_STATUS;

typedef enum
{
  AUTH_VOID,
  AUTH_DONE,
} AUTH_STATUS;
/*---------------------------- Structures ------------------------------------*/
typedef struct __packed
{
  PASSWORD_STATUS status;
  uint16_t        data;
} PASSWORD_TYPE;
/*------------------------ Extern --------------------------------------*/
extern uint16_t        engineWorkTime;                /* 0 */
extern uint16_t        engineStartsNumber;            /* 1 */
extern uint16_t        maintenanceAlarmOilTimeLeft;   /* 2 */
extern uint16_t        maintenanceAlarmAirTimeLeft;   /* 3 */
extern uint16_t        maintenanceAlarmFuelTimeLeft;  /* 4 */
extern uint16_t* const freeDataArray[FREE_DATA_SIZE];
extern PASSWORD_TYPE   systemPassword;
/*----------------------------------------------------------------------*/
#endif /* INC_FREEDATA_H_ */

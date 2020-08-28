/*
 * data.c
 *
 *  Created on: 25 авг. 2020 г.
 *      Author: 79110
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "freeData.h"

uint16_t engineWorkTime               = 0U;  /* 0 */
uint16_t engineStartsNumber           = 0U;  /* 1 */
uint16_t maintenanceAlarmOilTimeLeft  = 0U;  /* 2 */
uint16_t maintenanceAlarmAirTimeLeft  = 0U;  /* 3 */
uint16_t maintenanceAlarmFuelTimeLeft = 0U;  /* 4 */

uint16_t* const freeDataArray[FREE_DATA_SIZE] = { &engineWorkTime,
                                                  &engineStartsNumber,
					          &maintenanceAlarmOilTimeLeft,
					          &maintenanceAlarmAirTimeLeft,
					          &maintenanceAlarmFuelTimeLeft };

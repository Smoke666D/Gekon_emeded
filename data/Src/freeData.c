/*
 * data.c
 *
 *  Created on: 25 авг. 2020 г.
 *      Author: 79110
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "freeData.h"

PASSWORD_TYPE systemPassword =
{
  .data   = 0U,
  .status = PASSWORD_RESET,
};

uint16_t engineWorkTime               = 0U;  /*  0 */
uint16_t engineWorkMinutes            = 0U;  /*  1 */
uint16_t engineStartsNumber           = 0U;  /*  2 */
uint16_t maintenanceAlarmOilTimeLeft  = 0U;  /*  3 */
uint16_t maintenanceAlarmAirTimeLeft  = 0U;  /*  4 */
uint16_t maintenanceAlarmFuelTimeLeft = 0U;  /*  5 */
uint16_t powerReactiveUsage           = 0U;  /*  6 */
uint16_t powerActiveUsage             = 0U;  /*  7 */
uint16_t powerFullUsage               = 0U;  /*  8 */
uint16_t fuelUsage                    = 0U;  /*  9 */
uint16_t fuelRate                     = 0U;  /* 10 */
uint16_t fuelAverageSize              = 0U;  /* 11 */
uint16_t sdSizeLow                    = 0U;  /* 12 */
uint16_t sdSizeHight                  = 0U;  /* 13 */
uint16_t sdFreeLow                    = 0U;  /* 14 */
uint16_t sdFreeHight                  = 0U;  /* 15 */

uint16_t* const freeDataArray[FREE_DATA_SIZE] =
{
  &engineWorkTime,               /*  0 */
  &engineWorkMinutes,            /*  1 */
  &engineStartsNumber,           /*  2 */
  &maintenanceAlarmOilTimeLeft,  /*  3 */
  &maintenanceAlarmAirTimeLeft,  /*  4 */
  &maintenanceAlarmFuelTimeLeft, /*  5 */
  &powerReactiveUsage,           /*  6 */
  &powerActiveUsage,             /*  7 */
  &powerFullUsage,               /*  8 */
  &fuelUsage,                    /*  9 */
  &fuelRate,                     /* 10 */
  &fuelAverageSize,              /* 11 */
  &sdSizeLow,                    /* 12 */
  &sdSizeHight,                  /* 13 */
  &sdFreeLow,                    /* 14 */
  &sdFreeHight                   /* 15 */
};

void vFREEDATAerase ( void )
{
  uint8_t i = 0U;
  for ( i=0U; i<FREE_DATA_SIZE; i++ )
  {
    *freeDataArray[i] = 0U;
  }
  return;
}

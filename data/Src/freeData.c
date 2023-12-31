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

uint16_t* const freeDataArray[FREE_DATA_SIZE] =
{
  &engineWorkTime,
  &engineWorkMinutes,
  &engineStartsNumber,
  &maintenanceAlarmOilTimeLeft,
  &maintenanceAlarmAirTimeLeft,
  &maintenanceAlarmFuelTimeLeft,
  &powerReactiveUsage,
  &powerActiveUsage,
  &powerFullUsage,
  &fuelUsage,
  &fuelRate,
  &fuelAverageSize
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

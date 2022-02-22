/*
 * measurment.h
 *
 *  Created on: 19 янв. 2021 г.
 *      Author: 79110
 */
#ifndef INC_MEASUREMENT_H_
#define INC_MEASUREMENT_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "dataAPI.h"
#include "controllerTypes.h"
#include "outputProcessing.h"
/*------------------------ Define --------------------------------------*/
#define MEASUREMENT_COMMAND_QUEUE_LENGTH 8U
#define MEASUREMENT_SETTING_NUMBER       18U
#define MEASUREMENT_DATE_CHANEL          ( OUTPUT_DATA_REGISTER_NUMBER      )
#define MEASUREMENT_TIME_CHANEL          ( OUTPUT_DATA_REGISTER_NUMBER + 1U )
#define MEASUREMENT_INFINITY             0xFFFFU
/*------------------------- Macros -------------------------------------*/
/*-------------------------- ENUM --------------------------------------*/
typedef enum
{
  MEASURMENT_STATE_IDLE,
  MEASURMENT_STATE_START,
  MEASURMENT_STATE_WAIT,
  MEASURMENT_STATE_WRITE,
  MEASURMENT_STATE_ERROR,
} MEASURMENT_STATE;

typedef enum
{
  MEASURMENT_CMD_NONE,
  MEASURMENT_CMD_START,
  MEASURMENT_CMD_STOP
} MEASURMENT_CMD;

typedef enum
{
  OUTPUT_SETTING_OIL_PRESSURE = 0U,
  OUTPUT_SETTING_COOLANT_TEMP,
  OUTPUT_SETTING_FUEL_LEVEL,
  OUTPUT_SETTING_SPEED,
  OUTPUT_SETTING_INPUTS,
  OUTPUT_SETTING_OUTPUTS,
  OUTPUT_SETTING_GEN_PHASE_VOLTAGE,
  OUTPUT_SETTING_GEN_LINE_VOLTAGE,
  OUTPUT_SETTING_CURRENT,
  OUTPUT_SETTING_FREQ_GEN,
  OUTPUT_SETTING_COS_FI,
  OUTPUT_SETTING_POWER_ACTIVE,
  OUTPUT_SETTING_POWER_REACTIVE,
  OUTPUT_SETTING_POWER_FULL,
  OUTPUT_SETTING_MAINS_PHASE_VOLTAGE,
  OUTPUT_SETTING_MAINS_LINE_VOLTAGE,
  OUTPUT_SETTING_MAINS_FREQ,
  OUTPUT_SETTING_VOLTAGE_ACC,
} MEASURMENT_SETTING;
/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  PERMISSION       enb   : 1U;   /* Enable flag                                         */
  MEASURMENT_STATE state : 3U;   /* Current state of the measurement tread              */
  MEASURMENT_CMD   cmd   : 2U;   /* Current command for measurement tread               */
  uint8_t          length;       /* Number of active measurement channels               */
  SYSTEM_TIMER     timer;        /* Timer to calculate measurement delay                */
  uint8_t          channels[MEASUREMENT_CHANNEL_NUMBER];
} MEASUREMENT_TYPE;
/*------------------------ Functions -----------------------------------*/
void          vMEASUREMENTinit ( void );
void          vMEASUREMENTsendCmd ( MEASURMENT_CMD cmd );
QueueHandle_t pMEASUREMENTgetCommandQueue ( void );
uint16_t      uMEASUREMENTgetSize ( void );
void          vMEASUREMENTtask ( void* argument );
/*----------------------------------------------------------------------*/
#endif /* INC_MEASUREMENT_H_ */

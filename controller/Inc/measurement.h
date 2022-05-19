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
#define MEASUREMENT_DATE_CHANEL          ( OUTPUT_DATA_REGISTER_NUMBER      )
#define MEASUREMENT_TIME_CHANEL          ( OUTPUT_DATA_REGISTER_NUMBER + 1U )
#define MEASUREMENT_RECORD_SIZE          ( MEASUREMENT_CHANNEL_NUMBER * 2U )
/*------------------------- Macros -------------------------------------*/
/*-------------------------- ENUM --------------------------------------*/
typedef enum
{
  MEASURMENT_STATE_IDLE,  /* 0 */
  MEASURMENT_STATE_START, /* 1 */
  MEASURMENT_STATE_WAIT,  /* 2 */
  MEASURMENT_STATE_WRITE, /* 3 */
  MEASURMENT_STATE_ERROR  /* 4 */
} MEASURMENT_STATE;

typedef enum
{
  MEASURMENT_CMD_NONE,  /* 0 */
  MEASURMENT_CMD_START, /* 1 */
  MEASURMENT_CMD_STOP   /* 2 */
} MEASURMENT_CMD;

typedef enum
{
  OUTPUT_SETTING_OIL_PRESSURE,        /* 0 */
  OUTPUT_SETTING_COOLANT_TEMP,        /* 1 */
  OUTPUT_SETTING_FUEL_LEVEL,          /* 2 */
  OUTPUT_SETTING_SPEED,               /* 3 */
  OUTPUT_SETTING_INPUTS,              /* 4 */
  OUTPUT_SETTING_OUTPUTS,             /* 5 */
  OUTPUT_SETTING_GEN_PHASE_VOLTAGE,   /* 6 */
  OUTPUT_SETTING_GEN_LINE_VOLTAGE,    /* 7 */
  OUTPUT_SETTING_CURRENT,             /* 8 */
  OUTPUT_SETTING_FREQ_GEN,            /* 9 */
  OUTPUT_SETTING_COS_FI,              /* 10 */
  OUTPUT_SETTING_POWER_ACTIVE,        /* 11 */
  OUTPUT_SETTING_POWER_REACTIVE,      /* 12 */
  OUTPUT_SETTING_POWER_FULL,          /* 13 */
  OUTPUT_SETTING_MAINS_PHASE_VOLTAGE, /* 14 */
  OUTPUT_SETTING_MAINS_LINE_VOLTAGE,  /* 15 */
  OUTPUT_SETTING_MAINS_FREQ,          /* 16 */
  OUTPUT_SETTING_VOLTAGE_ACC          /* 17 */
} MEASURMENT_SETTING;
/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  PERMISSION       enb   : 1U;   /* Enable flag                                         */
  MEASURMENT_STATE state : 3U;   /* Current state of the measurement tread              */
  MEASURMENT_CMD   cmd;          /* Current command for measurement tread               */
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

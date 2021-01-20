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
/*------------------------ Define --------------------------------------*/
#define MEASUREMENT_ENB                  1U
#define MEASUREMENT_CHANNEL_NUMBER       15U
#define MEASUREMENT_COMMAND_QUEUE_LENGTH 8U
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
  MEASURMENT_CMD_RESET,
  MEASURMENT_CMD_START,
  MEASURMENT_CMD_STOP
} MEASURMENT_CMD;
/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  PERMISSION       enb : 1U; /* Enable flag of measurement channel */
  getValueCallBack get;      /* Callback to get data of channel    */
} MEASURMENT_CHANNEL;

typedef struct __packed
{
  PERMISSION         enb   : 1U;
  MEASURMENT_STATE   state : 3U;
  MEASURMENT_CMD     cmd   : 2U;
  uint8_t            length;
  uint16_t           size;
  uint16_t           counter;
  SYSTEM_TIMER       timer;
  MEASURMENT_CHANNEL channels[MEASUREMENT_CHANNEL_NUMBER];
} MEASUREMENT_TYPE;
/*------------------------ Functions -----------------------------------*/
void          vMEASUREMENTinit ( void );
void          vMEASUREMENTsendCmd ( MEASURMENT_CMD cmd );
QueueHandle_t pMEASUREMENTgetCommandQueue ( void );
void          vMEASUREMENTtask ( void* argument );
/*----------------------------------------------------------------------*/
#endif /* INC_MEASUREMENT_H_ */

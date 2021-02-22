/*
 * st atus.h
 *
 *  Created on: 22 февр. 2021 г.
 *      Author: 79110
 */
#ifndef INC_STATUS_H_
#define INC_STATUS_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "controllerTypes.h"
/*------------------------ Macros --------------------------------------*/
/*------------------------ Define --------------------------------------*/
/*------------------------- Enum ---------------------------------------*/
typedef enum
{
  DEVICE_STATUS_IDLE,            /* 0 Loading condition                 */
  DEVICE_STATUS_READY_TO_START,  /* 0 Engine stop, ready to start       */
  DEVICE_STATUS_START_DELAY,     /* 1 External start signal detected    */
  DEVICE_STATUS_CRANKING,        /* 1 Engine cranking                   */
  DEVICE_STATUS_CRANK_DELAY,     /* 1 Engine pause cranking             */
  DEVICE_STATUS_CONTROL_BLOCK,   /* 1 Engine have started, block alarms */
  DEVICE_STATUS_IDLE_WORK,       /* 1 Engine warming on idle speed      */
  DEVICE_STATUS_MOVE_TO_NOMINAL, /* 1 Engine go to nominal speed        */
  DEVICE_STATUS_WARMING,         /* 1 Engine warming on nominal speed   */
  DEVICE_STATUS_WORKING,         /* 0 Engine work, generator ready      */
  DEVICE_STATUS_COOLDOWN,        /* 1 Engine cool down on nominal speed */
  DEVICE_STATUS_IDLE_COOLDOWN,   /* 1 Engine cool down on idle speed    */
  DEVICE_STATUS_STOP_PROCESSING, /* 1 Waiting for engine stop           */
  DEVICE_STATUS_EMERGENCY_STOP,  /* 0 Fatal error. Need to reset manual */
  DEVICE_STATUS_BAN_START,       /* 0 Start is banned by error          */
} DEVICE_STATUS;

/*----------------------- Callbacks ------------------------------------*/
/*----------------------- Structures -----------------------------------*/
/*----------------------- Extern ---------------------------------------*/
/*----------------------- Functions ------------------------------------*/
void vSTATUSget ( DEVICE_STATUS* status, PERMISSION* timer, fix16_t time );
/*----------------------------------------------------------------------*/
#endif /* INC_STATUS_H_ */

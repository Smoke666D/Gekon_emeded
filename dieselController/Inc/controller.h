/*
 * controller.h
 *
 *  Created on: 13 июл. 2020 г.
 *      Author: 79110
 */

#ifndef INC_CONTROLLER_H_
#define INC_CONTROLLER_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "controllerTypes.h"
/*------------------------ Macros --------------------------------------*/
/*------------------------ Define --------------------------------------*/
/*------------------------- Enum ---------------------------------------*/
typedef enum
{
  CONTROLLER_MODE_MANUAL,
  CONTROLLER_MODE_AUTO,
} CONTROLLER_MODE;

typedef enum
{
  CONTROLLER_STATUS_IDLE,
  CONTROLLER_STATUS_ALARM,
  CONTROLLER_STATUS_START,
  CONTROLLER_STATUS_PLAN_STOP,
  CONTROLLER_STATUS_PLAN_STOP_DELAY,
  CONTROLLER_STATUS_SHUTDOWN,
  CONTROLLER_STATUS_WORK,
} CONTROLLER_STATE;

typedef enum
{
  CONTROLLER_TURNING_IDLE,
  CONTROLLER_TURNING_DELAY,
  CONTROLLER_TURNING_RELOAD,
  CONTROLLER_TURNING_ENGINE,
  CONTROLLER_TURNING_FINISH,
} CONTROLLER_TURNING;
/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  CONTROLLER_MODE   mode;
  CONTROLLER_STATE  state;
  uint8_t           banAutoShutdown;
  uint8_t           banAutoStart;
  uint8_t           banGenLoad;
  fix16_t           stopDelay;
  timerID_t         timerID;
} CONTROLLER_TYPE;
/*----------------------- Functions ------------------------------------*/
void vCONTROLLERinit ( void );
void vCONTROLLERtask ( void const* argument );
/*----------------------------------------------------------------------*/
#endif /* INC_CONTROLLER_H_ */

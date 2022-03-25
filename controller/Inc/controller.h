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
#include "fix16.h"
/*------------------------ Macros --------------------------------------*/
/*------------------------ Define --------------------------------------*/
#define  CONTROLLER_LOAD_BTN_EXIST     0U
#define  CONTROLLER_MANUAL_BTN_EXIST   0U
#define  CONTROLLER_HMI_EXTERN_TIMEOUT ( ( TickType_t ) 1000U )
/*------------------------- Enum ---------------------------------------*/
typedef enum
{
  CONTROLLER_MODE_MANUAL,
  CONTROLLER_MODE_AUTO,
} CONTROLLER_MODE;

typedef enum
{
  CONTROLLER_STATUS_IDLE,
  CONTROLLER_STATUS_ERROR,
  CONTROLLER_STATUS_START,
  CONTROLLER_STATUS_START_WITH_DELAY,
  CONTROLLER_STATUS_PLAN_STOP,
  CONTROLLER_STATUS_PLAN_STOP_WITH_DELAY,
  CONTROLLER_STATUS_WORK,
} CONTROLLER_STATE;

typedef enum
{
  CONTROLLER_TURNING_IDLE,
  CONTROLLER_TURNING_READY,
  CONTROLLER_TURNING_START_DELAY,
  CONTROLLER_TURNING_DELAY,
  CONTROLLER_TURNING_RELOAD,
  CONTROLLER_TURNING_ENGINE,
  CONTROLLER_TURNING_FINISH,
} CONTROLLER_TURNING;
/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  uint16_t      startPIN;
  GPIO_TypeDef* startGPIO;
  uint16_t      loadPIN;
  GPIO_TypeDef* loadGPIO;
  uint16_t      autoPIN;
  GPIO_TypeDef* autoGPIO;
  uint16_t      manualPIN;
  GPIO_TypeDef* manualGPIO;
  uint16_t      stopPIN;
  GPIO_TypeDef* stopGPIO;
} CONTROLLER_INIT;

typedef struct __packed
{
  CONTROLLER_MODE   mode               : 1U;
  CONTROLLER_STATE  state              : 3U;
  PERMISSION        banAutoShutdown    : 1U;
  PERMISSION        banAutoStart       : 1U;
  PERMISSION        banGenLoad         : 1U;
  PERMISSION        powerOffImidiately : 1U;
  PERMISSION        logWarning         : 1U;
  PERMISSION        logPositive        : 1U;
  PERMISSION        errorAfterStop     : 1U;
  fix16_t           stopDelay;
  fix16_t           startDelay;
  SYSTEM_TIMER      timer;
} CONTROLLER_TYPE;
/*----------------------- Extern ---------------------------------------*/
extern osThreadId_t controllerHandle;
/*----------------------- Functions ------------------------------------*/
void             vCONTROLLERinit ( const CONTROLLER_INIT* init );
CONTROLLER_STATE eCONTROLLERgetStatus ( void );
CONTROLLER_MODE  eCONTROLLERgetMode ( void );
void             vCONTROLLERswitchMode ( void );
void             vCONTRILLERsetStart ( void );
void             vCONTROLLERsetStop ( void );
void             vCONTROLLERsetACK ( void );
void             vCONTROLLERsetSwitchLoad ( void );
void             vCONTROLLERsetLed ( uint8_t led, uint8_t state );
/*----------------------------------------------------------------------*/
#endif /* INC_CONTROLLER_H_ */

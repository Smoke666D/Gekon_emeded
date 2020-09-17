/*
 * oilPressureSensor.h
 *
 *  Created on: 15 июн. 2020 г.
 *      Author: photo_Mickey
 */

#ifndef INC_OIL_H_
#define INC_OIL_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "chart.h"
#include "fix16.h"
#include "controllerTypes.h"
#include "fpo.h"
/*------------------------ Macros --------------------------------------*/
/*------------------------ Define --------------------------------------*/
#define  ENGINE_EVENT_QUEUE_LENGTH    16U
#define  ENGINE_COMMAND_QUEUE_LENGTH  8U
/*------------------------- Enum ---------------------------------------*/
typedef enum
{
  SENSOR_TYPE_NONE,
  SENSOR_TYPE_NORMAL_OPEN,
  SENSOR_TYPE_NORMAL_CLOSE,
  SENSOR_TYPE_RESISTIVE,
  SENSOR_TYPE_CURRENT,
} SENSOR_TYPE;

typedef enum
{
  SENSOR_STATUS_NORMAL,
  SENSOR_STATUS_ERROR,
  SENSOR_STATUS_OPEN_CIRCUIT_ERROR,
} SENSOR_STATUS;

typedef enum
{
  ENGINE_CMD_NONE,
  ENGINE_CMD_START,          /* Ignition */
  ENGINE_CMD_PLAN_STOP,
  ENGINE_CMD_PLAN_STOP_WITH_DELAY,
  ENGINE_CMD_GOTO_IDLE,
  ENGINE_CMD_GOTO_NORMAL,
  ENGINE_CMD_EMEGENCY_STOP,
  ENGINE_CMD_RESET_TO_IDLE,
} ENGINE_COMMAND;

typedef enum
{
  ENGINE_STATUS_IDLE,
  ENGINE_STATUS_BUSY_STARTING,
  ENGINE_STATUS_BUSY_STOPPING,
  ENGINE_STATUS_WORK,
  ENGINE_STATUS_WORK_ON_IDLE,
  ENGINE_STATUS_WORK_GOTO_NOMINAL,
  ENGINE_STATUS_FAIL_STARTING,
  ENGINE_STATUS_FAIL_STOPPING,
} ENGINE_STATUS;

typedef enum
{
  STARTER_IDLE,
  STARTER_START_DELAY,
  STARTER_READY,
  STARTER_CRANKING,
  STARTER_CRANK_DELAY,
  STARTER_CONTROL_BLOCK,
  STARTER_IDLE_WORK,
  STARTER_MOVE_TO_NOMINAL,
  STARTER_WARMING,
  STARTER_FAIL,
  STARTER_OK,
} STARTER_STATUS;

typedef enum
{
  STOP_IDLE,
  STOP_COOLDOWN,
  STOP_IDLE_COOLDOWN,
  STOP_PROCESSING,
  STOP_FAIL,
  STOP_OK,
} PLAN_STOP_STATUS;
/*----------------------- Callbacks ------------------------------------*/

/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  uint8_t       enb;
  SYSTEM_EVENT  event;
} CUTOUT_TYPE;

typedef struct __packed
{
  SENSOR_TYPE       type;
  eChartData*       chart;
  getValueCallBack  get;
  CUTOUT_TYPE       cutout;
  SENSOR_STATUS     status;
} SENSOR;

typedef struct __packed
{
  SENSOR      pressure;
  ALARM_TYPE  alarm;
  ALARM_TYPE  preAlarm;
} OIL_TYPE;

typedef struct __packed
{
  SENSOR             temp;
  ALARM_TYPE         alarm;
  ALARM_TYPE         preAlarm;
  RELAY_AUTO_DEVICE  cooler;
  RELAY_AUTO_DEVICE  heater;
} COOLANT_TYPE;

typedef struct __packed
{
  SENSOR             level;
  ALARM_TYPE         lowAlarm;
  ALARM_TYPE         lowPreAlarm;
  ALARM_TYPE         hightAlarm;
  ALARM_TYPE         hightPreAlarm;
  RELAY_AUTO_DEVICE  booster;
  RELAY_DEVICE       pump;
} FUEL_TYPE;

typedef struct __packed
{
  getValueCallBack  get;
  ALARM_TYPE        lowAlarm;
  ALARM_TYPE        hightAlarm;
  SENSOR_STATUS     status;
} SPEED_TYPE;

typedef struct __packed
{
  getValueCallBack  get;
  ALARM_TYPE        lowAlarm;
  ALARM_TYPE        hightAlarm;
} BATTERY_TYPE;

typedef struct __packed
{
  getValueCallBack  get;
  ALARM_TYPE        hightAlarm;
  ALARM_TYPE        hightPreAlarm;
} CHARGER_TYPE;

typedef struct __packed
{
  uint8_t  critGenFreqEnb;
  fix16_t  critGenFreqLevel;
  uint8_t  critOilPressEnb;
  fix16_t  critOilPressLevel;
  uint8_t  critChargeEnb;
  fix16_t  critChargeLevel;
  uint8_t  critSpeedEnb;
  fix16_t  critSpeedLevel;
} START_CRITERIONS_TYPE;

typedef struct __packed
{
  ALARM_TYPE  oil;
  ALARM_TYPE  air;
  ALARM_TYPE  fuel;
} MAINTENCE_TYPE;

typedef struct __packed
{
  setRelayCallBack       set;
  fix16_t                startDelay;      /* sec */
  fix16_t                crankingDelay;   /* sec */
  fix16_t                crankDelay;      /* sec */
  fix16_t                blockDelay;      /* sec */
  fix16_t                idlingDelay;     /* sec */
  fix16_t                nominalDelay;    /* sec */
  fix16_t                warmingDelay;    /* sec */
  uint8_t                startAttempts;
  uint8_t                startIteration;
  START_CRITERIONS_TYPE  startCrit;
  STARTER_STATUS         status;
} STARTER_TYPE;

typedef struct __packed
{
  fix16_t           coolingDelay;      /* sec */
  fix16_t           coolingIdleDelay;  /* sec */
  fix16_t           processDelay;      /* sec */
  PLAN_STOP_STATUS  status;
} PLAN_STOP_TYPE;

typedef struct __packed
{
  ENGINE_COMMAND  cmd;
  uint8_t         startCheckOil;
  ENGINE_STATUS   status;
} ENGINE_TYPE;
/*----------------------- Extern ---------------------------------------*/
/*----------------------- Functions ------------------------------------*/
void          vENGINEinit ( void );
void          vENGINEemergencyStop ( void );
QueueHandle_t pENGINEgetCommandQueue ( void );
uint8_t       uENGINEisStarterScrollFinish ( void );
uint8_t       uENGINEisBlockTimerFinish ( void );
ENGINE_STATUS eENGINEgetEngineStatus ( void );
void          vENGINEmaintenanceReset ( void );
/*----------------------------------------------------------------------*/
#endif /* INC_OIL_H_ */

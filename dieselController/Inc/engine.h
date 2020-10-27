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
#define  ENGINE_EVENT_QUEUE_LENGTH      16U
#define  ENGINE_COMMAND_QUEUE_LENGTH    8U
#define  ENGINE_OIL_PRESSURE_TRESH_HOLD 3000U
#define  SENSOR_CUTOUT_LEVEL            ( fix16_from_int( MAX_RESISTANCE - 200U ) )
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
  SENSOR_STATUS_LINE_ERROR,
  SENSOR_STATUS_COMMON_ERROR,
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
  ENGINE_STATUS_EMERGENCY_STOP,
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

typedef enum
{
  MAINTENCE_STATUS_STOP,
  MAINTENCE_STATUS_RUN,
} MAINTENCE_STATUS;
/*----------------------- Callbacks ------------------------------------*/

/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  SENSOR_TYPE       type;
  eChartData*       chart;
  getValueCallBack  get;
  ALARM_TYPE        cutout;
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
  uint8_t           enb;
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
  MAINTENCE_STATUS status;
  SYSTEM_TIMER     timer;
  ALARM_TYPE       oil;
  ALARM_TYPE       air;
  ALARM_TYPE       fuel;
} MAINTENCE_TYPE;

typedef struct __packed
{
  /* Callback */
  setRelayCallBack       set;
  /* Delays */
  fix16_t                startDelay;      /* sec */
  fix16_t                crankingDelay;   /* sec */
  fix16_t                crankDelay;      /* sec */
  fix16_t                blockDelay;      /* sec */
  fix16_t                idlingDelay;     /* sec */
  fix16_t                nominalDelay;    /* sec */
  fix16_t                warmingDelay;    /* sec */
  /* Counters */
  uint8_t                startAttempts;
  uint8_t                startIteration;
  /* Structures */
  START_CRITERIONS_TYPE  startCrit;
  /* Status */
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
  ERROR_TYPE      stopError;
  ERROR_TYPE      startError;
} ENGINE_TYPE;
/*----------------------- Extern ---------------------------------------*/
extern osThreadId_t engineHandle;
/*----------------------- Functions ------------------------------------*/
void          vENGINEinit ( void );
void          vENGINEemergencyStop ( void );
QueueHandle_t pENGINEgetCommandQueue ( void );
uint8_t       uENGINEisStarterScrollFinish ( void );
uint8_t       uENGINEisBlockTimerFinish ( void );
ENGINE_STATUS eENGINEgetEngineStatus ( void );
/*----------------------------------------------------------------------*/
#endif /* INC_OIL_H_ */

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
#define  SENSOR_CUTOUT_LEVEL            1U//( MAX_RESISTANCE )
#define  CHARGER_IMPULSE_DURATION       5U                                          /* sec */
#define  CHARGER_ATTEMPTS_NUMBER        5U
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
  SENSOR_CHANNEL_OIL,
  SENSOR_CHANNEL_COOLANT,
  SENSOR_CHANNEL_FUEL,
  SENSOR_CHANNEL_COMMON,
} SENSOR_CHANNEL;

typedef enum
{
  ENGINE_CMD_NONE,                    /*  0 */
  ENGINE_CMD_START,                   /*  1 Ignition */
  ENGINE_CMD_PLAN_STOP,               /*  2 */
  ENGINE_CMD_PLAN_STOP_WITH_DELAY,    /*  3 */
  ENGINE_CMD_GOTO_IDLE,               /*  4 */
  ENGINE_CMD_GOTO_NORMAL,             /*  5 */
  ENGINE_CMD_EMEGENCY_STOP,           /*  6 */
  ENGINE_CMD_RESET_TO_IDLE,           /*  7 */
  ENGINE_CMD_BAN_START,               /*  8 */
  ENGINE_CMD_PLAN_STOP_AND_BAN_START, /* 10 */
} ENGINE_COMMAND;

typedef enum
{
  ENGINE_STATUS_IDLE,
  ENGINE_STATUS_EMERGENCY_STOP,
  ENGINE_STATUS_BUSY_STARTING,
  ENGINE_STATUS_BUSY_STOPPING,
  ENGINE_STATUS_WORK,
  ENGINE_STATUS_WORK_WAIT_ELECTRO,
  ENGINE_STATUS_WORK_ON_IDLE,
  ENGINE_STATUS_WORK_GOTO_NOMINAL,
  ENGINE_STATUS_FAIL_STARTING,
  ENGINE_STATUS_FAIL_STOPPING,
} ENGINE_STATUS;

typedef enum
{
  STARTER_IDLE,
  STARTER_START_PREPARATION,
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
  STOP_WAIT_ELECTRO,
  STOP_IDLE_COOLDOWN,
  STOP_PROCESSING,
  STOP_FAIL,
  STOP_OK,
} PLAN_STOP_STATUS;

typedef enum
{
  MAINTENCE_STATUS_STOP,
  MAINTENCE_STATUS_RUN,
  MAINTENCE_STATUS_CHECK,
} MAINTENCE_STATUS;

typedef enum
{
  CHARGER_STATUS_IDLE,
  CHARGER_STATUS_STARTUP,
  CHARGER_STATUS_IMPULSE,
  CHARGER_STATUS_DELAY,
  CHARGER_STATUS_MEASURING,
  CHARGER_STATUS_ERROR,
} CHARGER_STATUS;
/*----------------------- Callbacks ------------------------------------*/

/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  SENSOR_TYPE       type    : 3U;
  SENSOR_STATUS     status  : 2U;
  SENSOR_CHANNEL    channel : 2U;
  eChartData*       chart;
  getValueCallBack  get;
  ERROR_TYPE        cutout;
} SENSOR;

typedef struct __packed
{
  SENSOR      pressure;
  fix16_t     trashhold;
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
  PERMISSION        enb         : 1U;
  SENSOR_STATUS     status      : 2U;
  getValueCallBack  get;
  ALARM_TYPE        lowAlarm;
  ALARM_TYPE        hightAlarm;
} SPEED_TYPE;

typedef struct __packed
{
  getValueCallBack  get;
  ALARM_TYPE        lowAlarm;
  ALARM_TYPE        hightAlarm;
} BATTERY_TYPE;

typedef struct __packed
{
  PERMISSION        enb       : 1U;
  CHARGER_STATUS    status    : 3U;
  uint8_t           attempts;
  uint8_t           iteration;
  getValueCallBack  get;
  RELAY_DEVICE      relay;
  SYSTEM_TIMER      timer;
  fix16_t           setpoint;
  ERROR_TYPE        error;
  ALARM_TYPE        warning;
} CHARGER_TYPE;

typedef struct __packed
{
  PERMISSION  critGenFreqEnb    : 1U;
  PERMISSION  critOilPressEnb   : 1U;
  PERMISSION  critChargeEnb     : 1U;
  PERMISSION  critSpeedEnb      : 1U;
  fix16_t     critGenFreqLevel;
  fix16_t     critOilPressLevel;
  fix16_t     critChargeLevel;
  fix16_t     critSpeedLevel;
} START_CRITERIONS_TYPE;

typedef struct __packed
{
  ALARM_TYPE alarm;
  uint16_t   data;
} MAINTENCE_VALUE;

typedef struct __packed
{
  MAINTENCE_STATUS status;
  SYSTEM_TIMER     timer;
  MAINTENCE_VALUE  oil;
  MAINTENCE_VALUE  air;
  MAINTENCE_VALUE  fuel;
} MAINTENCE_TYPE;

typedef struct __packed
{
  /* Callback */
  setRelayCallBack       set;
  /* Delays */
  fix16_t                crankingDelay;   /* sec */
  fix16_t                crankDelay;      /* sec */
  fix16_t                blockDelay;      /* sec */
  fix16_t                idlingDelay;     /* sec */
  fix16_t                nominalDelay;    /* sec */
  fix16_t                warmingDelay;    /* sec */
  /* Counters */
  uint8_t                attempts;
  uint8_t                iteration;
  /* Structures */
  START_CRITERIONS_TYPE  startCrit;
  /* Status */
  STARTER_STATUS         status       : 4U;
} STARTER_TYPE;

typedef struct __packed
{
  fix16_t           coolingDelay;          /* sec */
  fix16_t           coolingIdleDelay;      /* sec */
  fix16_t           processDelay;          /* sec */
  PLAN_STOP_STATUS  status           : 4U;
} PLAN_STOP_TYPE;

typedef struct __packed
{
  ENGINE_COMMAND  cmd               : 4U;
  ENGINE_STATUS   status            : 4U;
  PERMISSION      startCheckOil     : 1U;
  PERMISSION      banStart          : 1U;
  ERROR_TYPE      sensorCommonError;
  ERROR_TYPE      stopError;
  ERROR_TYPE      startError;
} ENGINE_TYPE;
/*----------------------- Extern ---------------------------------------*/
extern osThreadId_t engineHandle;
/*----------------------- Functions ------------------------------------*/
void          vENGINEinit ( void );
void          vENGINEsendCmd ( ENGINE_COMMAND cmd );
QueueHandle_t pENGINEgetCommandQueue ( void );
uint8_t       uENGINEisStarterScrollFinish ( void );
uint8_t       uENGINEisBlockTimerFinish ( void );
PERMISSION    eENGINEisStartBan ( void );
ENGINE_STATUS eENGINEgetEngineStatus ( void );
TRIGGER_STATE eENGINEgetOilSensorState ( void );
TRIGGER_STATE eENGINEgetCoolantSensorState ( void );
/*----------------------------------------------------------------------*/
#endif /* INC_OIL_H_ */

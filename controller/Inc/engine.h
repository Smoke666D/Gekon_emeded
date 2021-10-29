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
#define  SENSOR_CUTOUT_LEVEL            1U //( MAX_RESISTANCE )
#define  CHARGER_IMPULSE_DURATION       5U /* sec */
#define  CHARGER_ATTEMPTS_NUMBER        5U
/*------------------------- Enum ---------------------------------------*/
typedef enum
{
  ENGINE_CMD_NONE,                    /* 00 None                                                      */
  ENGINE_CMD_START,                   /* 01 Ignition                                                  */
  ENGINE_CMD_PLAN_STOP,               /* 02 Cool down stop process without error                      */
  ENGINE_CMD_GOTO_IDLE,               /* 03 Switch engine to idle from work                           */
  ENGINE_CMD_GOTO_NORMAL,             /* 04 Switch engine to work from idle                           */
  ENGINE_CMD_EMEGENCY_STOP,           /* 05 Immediate engine stop with error                          */
  ENGINE_CMD_RESET_TO_IDLE,           /* 06 Reset all errors                                          */
  ENGINE_CMD_BAN_START,               /* 07 Ban next start without stop                               */
} ENGINE_COMMAND;

typedef enum
{
  ENGINE_STATUS_IDLE,                 /* 00 Engine ready to start                                     */
  ENGINE_STATUS_ERROR,                /* 01 Engine error, need to reset to idle                       */
  ENGINE_STATUS_BUSY_STARTING,        /* 02 Engine is starting                                        */
  ENGINE_STATUS_BUSY_STOPPING,        /* 03 Engine is stopping                                        */
  ENGINE_STATUS_WORK,                 /* 04 Engine is working normal                                  */
  ENGINE_STATUS_WORK_GOTO_IDLE,       /* 05 Engine go to idle from normal                             */
  ENGINE_STATUS_WORK_ON_IDLE,         /* 06 Engine work on idle                                       */
  ENGINE_STATUS_WORK_GOTO_NOMINAL,    /* 07 Engine go to normal from idle                             */
} ENGINE_STATUS;

typedef enum
{
  STARTER_STATUS_IDLE,                /* 00 Starter don't active                                      */
  STARTER_STATUS_PREHEATING,          /* 01 Engine preheating process                                 */
  STARTER_STATUS_FUEL_PREPUMPING,     /* 02 Fuel pumping without cranking                             */
  STARTER_STATUS_START_PREPARATION,   /* 03 Waiting electrical system ready                           */
  STARTER_STATUS_READY,               /* 04 Starter ready for cranking                                */
  STARTER_STATUS_CRANKING,            /* 05 Cranking iteration                                        */
  STARTER_STATUS_CRANK_DELAY,         /* 06 Delay between cranking iterations                         */
  STARTER_STATUS_CONTROL_BLOCK,       /* 07 Engine have started, wait valid data                      */
  STARTER_STATUS_IDLE_WORK,           /* 08 Engine warming on idle                                    */
  STARTER_STATUS_MOVE_TO_NOMINAL,     /* 09 Move from idle to nominal                                 */
  STARTER_STATUS_WARMING,             /* 10 Engine warming on nominal                                 */
  STARTER_STATUS_FAIL,                /* 11 Starting process fail                                     */
  STARTER_STATUS_OK,                  /* 12 Starting process finish successfully                      */
} STARTER_STATUS;

typedef enum
{
  STOP_STATUS_IDLE,                   /* 00 Stop process inactive                                     */
  STOP_STATUS_COOLDOWN,               /* 01 Engine cool down on nominal                               */
  STOP_STATUS_WAIT_ELECTRO,           /* 02 Waiting for electrical system ready                       */
  STOP_STATUS_IDLE_COOLDOWN,          /* 03 Engine cool down on idle                                  */
  STOP_STATUS_PROCESSING,             /* 04 Switch of fuel, waiting for stop state                    */
  STOP_STATUS_FAIL,                   /* 05 Stopping process fail                                     */
  STOP_STATUS_OK,                     /* 06 Stopping process finish successfully                      */
} STOP_STATUS;

typedef enum
{
  EMERGENCY_STATUS_IDLE,              /* 00 Emergency stop process inactive                           */
  EMERGENCY_STATUS_PROCESSING,        /* 01 Fuel have switched off, waiting for stop state            */
  EMERGENCY_STATUS_END,               /* 02 Finish Emergency stop process                             */
} EMERGENCY_STATUS;

typedef enum
{
  CHARGER_STATUS_IDLE,                /* 00 Charger controller analysis charger voltage               */
  CHARGER_STATUS_DELAY,               /* 01 Charger controller excites the charger                    */
  CHARGER_STATUS_MEASURING,           /* 02 Charger controller measuring charger voltage              */
} CHARGER_STATUS;
/*----------------------- Callbacks ------------------------------------*/

/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  SENSOR_TYPE       type    : 3U;
  SENSOR_STATUS     status  : 2U;
  SENSOR_CHANNEL    channel : 2U;
  TRIGGER_STATE     trig    : 1U; /* Only for DI mode*/
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
  ALARM_TYPE         electroAlarm;
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
  fix16_t           polePairs;
} SPEED_TYPE;

typedef struct __packed
{
  getValueCallBack  get;
  ALARM_TYPE        lowAlarm;
  ALARM_TYPE        hightAlarm;
} BATTERY_TYPE;

typedef struct __packed
{
  PERMISSION        enb    : 1U;
  PERMISSION        start  : 1U;
  CHARGER_STATUS    status : 3U;
  getValueCallBack  get;
  RELAY_DEVICE      relay;
  SYSTEM_TIMER      timer;
  ALARM_TYPE        alarm;
} CHARGER_TYPE;

typedef struct __packed
{
  RELAY_DEVICE relay;
  fix16_t      level;
  fix16_t      delay;
} PREHEATER_TYPE;

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
  STOP_STATUS  status           : 4U;
} PLAN_STOP_TYPE;

typedef struct __packed
{
  ENGINE_COMMAND  cmd               : 4U;
  ENGINE_STATUS   status            : 4U;
  PERMISSION      startCheckOil     : 1U;
  PERMISSION      banStart          : 1U;
  TRIGGER_STATE   firstTurnout      : 1U;
  ERROR_TYPE      sensorCommonError;
  ERROR_TYPE      stopError;
  ERROR_TYPE      startError;
} ENGINE_TYPE;
/*----------------------- Extern ---------------------------------------*/
extern osThreadId_t engineHandle;
/*----------------------- Functions ------------------------------------*/
void             vENGINEinit ( void );
void             vENGINEsendCmd ( ENGINE_COMMAND cmd );
QueueHandle_t    pENGINEgetCommandQueue ( void );
TRIGGER_STATE    uENGINEisStarterScrollFinish ( void );
TRIGGER_STATE    uENGINEisBlockTimerFinish ( void );
PERMISSION       eENGINEisStartBan ( void );
ENGINE_STATUS    eENGINEgetEngineStatus ( void );
STARTER_STATUS   eENGINEgetStarterStatus ( void );
EMERGENCY_STATUS eENGINEgetEmergencyStatus ( void );
STOP_STATUS      eENGINEgetPlanStopStatus ( void );
TRIGGER_STATE    eENGINEgetOilSensorState ( void );
TRIGGER_STATE    eENGINEgetCoolantSensorState ( void );
TRIGGER_STATE    eENGINEgetFuelSensorState ( void );
fix16_t          fENGINEgetSpeed ( void );
fix16_t          fENGINEgetOilPressure ( void );
fix16_t          fENGINEgetCoolantTemp ( void );
fix16_t          fENGINEgetFuelLevel ( void );
RELAY_STATUS     eENGINEgetChargerState ( void );
TRIGGER_STATE    eENGINEgetTurnout ( void );
/*----------------------------------------------------------------------*/
#endif /* INC_OIL_H_ */

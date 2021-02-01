/*
 * controllerTypes.h
 *
 *  Created on: 15 июн. 2020 г.
 *      Author: photo_Mickey
 */

#ifndef INC_LOGICTYPES_H_
#define INC_LOGICTYPES_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "chart.h"
#include "fix16.h"
#include "RTC.h"
#include "EEPROM.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
/*------------------------ Define --------------------------------------*/
#define  LOG_SIZE                   255U
#define  LOG_RECORD_SIZE            6U
#define  LOGIC_TIMER_STEP           100U  /* ms */
#define  LOGIC_COUNTERS_SIZE        10U
#define  LOGIC_DEFAULT_TIMER_ID     ( LOGIC_COUNTERS_SIZE + 1U )
#define  EVENT_QUEUE_LENGTH         16U
#define  LOG_TYPES_SIZE             39U
#define  LOG_ACTION_SIZE            8U
#define  HMI_CMD_MASK               0xFFU
#define  TASK_NOTIFY_WAIT_DELAY     10U
#define  SYS_TIMER_SEMAPHORE_DELAY  200U
#define  DEBUG_SERIAL_ALARM         1U    /* Set 1 to print in serial all warnings and alarms */
#define  DEBUG_SERIAL_STATUS        1U    /* Set 1 to print in serial all state transfer */
#define  SEMAPHORE_AEL_TAKE_DELAY   10U
/*------------------------ Tasks ---------------------------------------*/
#define  FPI_TASK_PRIORITY           osPriorityLow
#define  ENGINE_TASK_PRIORITY        osPriorityLow     /* Engine and Electro priority need to be equal */
#define  ELECTRO_TASK_PRIORITY       osPriorityLow     /* Engine and Electro priority need to be equal */
#define  CONTROLLER_TASK_PRIORITY    osPriorityNormal
#define  MEASUREMENT_TASK_PRIORITY   osPriorityLow
#define  FPI_TASK_STACK_SIZE         1024U
#define  ENGINE_TASK_STACK_SIZE      1024U
#define  ELECTRO_TASK_STACK_SIZE     1024U
#define  CONTROLLER_TASK_STACK_SIZE  1024U
#define  MEASUREMENT_TASK_STACK_SIZE 1024U

#define  OIL_SENSOR_SOURCE          xADCGetSOP
#define  FUEL_SENSOR_SOURCE         xADCGetSFL
#define  COOLANT_SENSOR_SOURCE      xADCGetSCT
/*------------------------ Types ---------------------------------------*/
typedef  uint8_t timerID_t;
/*------------------------ Macros --------------------------------------*/
/*----------------------- Constant -------------------------------------*/
extern const char*   logTypesDictionary[LOG_TYPES_SIZE];
extern const char*   logActionsDictionary[LOG_ACTION_SIZE];
extern const fix16_t fix100U;
/*------------------------- Enum ---------------------------------------*/
typedef enum
{
  DATA_API_REINIT_CONFIG,
  DATA_API_REINIT_MAINTANCE,
} DATA_API_REINIT;

typedef enum
{
  DATA_API_FLAG_LCD_TASK_CONFIG_REINIT        = 0x01,
  DATA_API_FLAG_ENGINE_TASK_CONFIG_REINIT     = 0x02,
  DATA_API_FLAG_CONTROLLER_TASK_CONFIG_REINIT = 0x04,
  DATA_API_FLAG_ELECTRO_TASK_CONFIG_REINIT    = 0x08,
  DATA_API_FLAG_FPI_TASK_CONFIG_REINIT        = 0x10,
} DATA_API_EVENT_FLAG;

typedef enum
{
  ACTION_NONE,                    /* 0 */
  ACTION_WARNING,                 /* 1 */
  ACTION_EMERGENCY_STOP,          /* 2 */
  ACTION_PLAN_STOP,               /* 3 */
  ACTION_PLAN_STOP_AND_BAN_START, /* 4 */
  ACTION_BAN_START,               /* 5 */
  ACTION_AUTO_START,              /* 6 */
  ACTION_AUTO_STOP,               /* 7 */
} SYSTEM_ACTION;

typedef enum
{
  EVENT_NONE,                       /* 0  NONE */
  EVENT_EXTERN_EMERGENCY_STOP,      /* 1  EMERGENCY_STOP */
  EVENT_START_FAIL,                 /* 2  EMERGENCY_STOP */
  EVENT_STOP_FAIL,                  /* 3  EMERGENCY_STOP */
  EVENT_OIL_LOW_PRESSURE,           /* 4  WARNING & EMERGENCY_STOP */
  EVENT_OIL_SENSOR_ERROR,           /* 5  EMERGENCY_STOP */
  EVENT_ENGINE_HIGHT_TEMP,          /* 6  WARNING & EMERGENCY_STOP */
  EVENT_ENGINE_TEMP_SENSOR_ERROR,   /* 7  EMERGENCY_STOP */
  EVENT_FUEL_LOW_LEVEL,             /* 8  WARNING & PLAN_STOP */
  EVENT_FUEL_HIGHT_LEVEL,           /* 9  WARNING & PLAN_STOP */
  EVENT_FUEL_LEVEL_SENSOR_ERROR,    /* 10 EMERGENCY_STOP */
  EVENT_SPEED_HIGHT,                /* 11 EMERGENCY_STOP */
  EVENT_SPEED_LOW,                  /* 12 EMERGENCY_STOP */
  EVENT_SPEED_SENSOR_ERROR,         /* 13 EMERGENCY_STOP */
  EVENT_CHARGER_FAIL,               /* 14 WARNING & EMERGENCY_STOP */
  EVENT_BATTERY_LOW,                /* 15 WARNING */
  EVENT_BATTERY_HIGHT,              /* 16 WARNING */
  EVENT_GENERATOR_LOW_VOLTAGE,      /* 17 WARNING & EMERGENCY_STOP */
  EVENT_GENERATOR_HIGHT_VOLTAGE,    /* 18 WARNING & EMERGENCY_STOP */
  EVENT_GENERATOR_LOW_FREQUENCY,    /* 19 WARNING & EMERGENCY_STOP */
  EVENT_GENERATOR_HIGHT_FREQUENCY,  /* 20 WARNING & EMERGENCY_STOP */
  EVENT_PHASE_IMBALANCE,            /* 21 EMERGENCY_STOP */
  EVENT_OVER_CURRENT,               /* 22 PLAN_STOP */
  EVENT_OVER_POWER,                 /* 23 PLAN_STOP */
  EVENT_SHORT_CIRCUIT,              /* 24 PLAN_STOP */
  EVENT_MAINS_LOW_VOLTAGE,          /* 25 WARNING */
  EVENT_MAINS_HIGHT_VOLTAGE,        /* 26 WARNING */
  EVENT_MAINS_LOW_FREQUENCY,        /* 27 WARNING */
  EVENT_MAINS_HIGHT_FREQUENCY,      /* 28 WARNING */
  EVENT_MAINTENANCE_OIL,            /* 29 WARNING & BAN_START */
  EVENT_MAINTENANCE_AIR,            /* 30 WARNING & BAN_START */
  EVENT_MAINTENANCE_FUEL,           /* 31 WARNING & BAN_START */
  EVENT_ENGINE_START,               /* 32 NONE */
  EVENT_ENGINE_STOP,                /* 33 NONE */
  EVENT_MAINS_OK,                   /* 34 AUTO_STOP */
  EVENT_MAINS_FAIL,                 /* 35 AUTO_START */
  EVENT_INTERRUPTED_START,          /* 36 EMERGENCY_STOP */
  EVENT_INTERRUPTED_STOP,           /* 37 EMERGENCY_STOP */
  EVENT_SENSOR_COMMON_ERROR,        /* 38 EMERGENCY_STOP */
} SYSTEM_EVENT_TYPE;

typedef enum
{
  PERMISSION_DISABLE = 0U,
  PERMISSION_ENABLE  = 1U,
} PERMISSION;

typedef enum
{
  TRIGGER_IDLE = 0U,
  TRIGGER_SET  = 1U,
} TRIGGER_STATE;

typedef enum
{
  HMI_CMD_NONE   = 0x00U,
  HMI_CMD_START  = 0x01U,
  HMI_CMD_STOP   = 0x02U,
  HMI_CMD_AUTO   = 0x04U,
  HMI_CMD_LOAD   = 0x08U,
  HMI_CMD_MANUAL = 0x10U,
  HMI_CMD_ACK    = 0x20U,
} HMI_COMMAND;

typedef enum
{
  TIMER_OK,
  TIMER_NO_SPACE,
  TIMER_ACCESS,
} TIMER_ERROR;

typedef enum
{
  ALARM_STATUS_IDLE,
  ALARM_STATUS_WAIT_DELAY,
  ALARM_STATUS_TRIG,
  ALARM_STATUS_RELAX,
  ALARM_STATUS_HOLD,
} ALARM_STATUS;

typedef enum
{
  ALARM_LEVEL_LOW,
  ALARM_LEVEL_HIGHT,
} ALARM_LEVEL;

typedef enum
{
  RELAY_OFF = 0U,
  RELAY_ON,
} RELAY_STATUS;

typedef enum
{
  RELAY_DELAY_IDLE,
  RELAY_DELAY_WORK,
} RELAY_DELAY_STATUS;

typedef enum
{
  RELAY_IMPULSE_IDLE,
  RELAY_IMPULSE_START,
  RELAY_IMPULSE_DONE,
} RELAY_IMPULSE_STATUS;

typedef enum
{
  LOG_STATUS_OK,
  LOG_STATUS_ERROR,
} LOG_STATUS;

typedef enum
{
  ERROR_LIST_CMD_ERASE,
  ERROR_LIST_CMD_ADD,
  ERROR_LIST_CMD_READ,
  ERROR_LIST_CMD_ACK,
  ERROR_LIST_CMD_COUNTER,
} ERROR_LIST_CMD;
/*----------------------- Callbacks ------------------------------------*/
typedef uint16_t ( *getDataCallBack )( void );           /* Callback to get 2 byte data */
typedef fix16_t  ( *getValueCallBack )( void );          /* Callback to get sensor value */
typedef void     ( *setRelayCallBack )( RELAY_STATUS );  /* Callback to setup relay state */
/*----------------------- Structures -----------------------------------*/
 typedef struct __packed
{
  SYSTEM_ACTION      action;
  SYSTEM_EVENT_TYPE  type;
} SYSTEM_EVENT;

typedef struct __packed
{
  fix16_t    delay; /* Delay to active event after triggered, seconds */
  timerID_t  id;    /* Number of system timer */
} SYSTEM_TIMER;

typedef struct __packed
{
  PERMISSION     enb    : 1U; /* Enable alarm at initialization */
  PERMISSION     active : 1U; /* On/Off alarm check in work flow */
  PERMISSION     ack    : 1U; /* Auto acknowledgment on/off */
  PERMISSION     ignor  : 1U; /* Ignoring in active error list */
  ALARM_STATUS   status : 3U; /* Status of the alarm */
  TRIGGER_STATE  trig   : 1U; /* Alarm triggered flag. Reset from outside */
  SYSTEM_EVENT   event;       /* Event data of alarm */
} ERROR_TYPE;

typedef struct __packed
{
  ALARM_LEVEL    type  : 1U; /* Type of alarm above or below set point */
  ERROR_TYPE     error;      /* Error of the alarm */
  fix16_t        level;      /* Set point */
  SYSTEM_TIMER   timer;      /* Timer for triggering delay */
} ALARM_TYPE;

typedef struct __packed
{
  PERMISSION        enb    : 1U; /* Enable flag of relay */
  RELAY_STATUS      status : 1U; /* Current status of relay */
  setRelayCallBack  set;         /* Callback for relay control */
} RELAY_DEVICE;                  /* Simple on/off relay device */

typedef struct __packed
{
  fix16_t           onLevel;   /* Level to relay turn on */
  fix16_t           offLevel;  /* Level to relay turn off */
  RELAY_DEVICE      relay;     /* Relay device */
} RELAY_AUTO_DEVICE;           /* Auto on/off relay: On at onLevel and Off at offLevel. offLevel van be less, than offLevel  */

typedef struct __packed
{
  RELAY_DELAY_STATUS status : 1U; /* Current status of device */
  TRIGGER_STATE      triger : 1U; /* Input to start relay processing */
  RELAY_DEVICE       relay;       /* Relay device */
  SYSTEM_TIMER       timer;       /* Timer for delay */
} RELAY_DELAY_DEVICE;             /* Relay with auto turn off after delay */

typedef struct __packed
{
  PERMISSION            active : 1U; /* Flag of comparison activation */
  RELAY_IMPULSE_STATUS  status : 2U; /* Current status of device */
  RELAY_DEVICE          relay;       /* Relay device */
  fix16_t               level;       /* Level for comparison */
  SYSTEM_TIMER          timer;       /* Timer for delay */
} RELAY_IMPULSE_DEVICE;              /* Relay, that work like RELAY_DELAY_DEVICE, but the trigger is comparison with level */

typedef struct __packed
{
  uint32_t      time;   /* 4 bytes */
  SYSTEM_EVENT  event;  /* 2 bytes */
} LOG_RECORD_TYPE;
/*----------------------- Extern ---------------------------------------*/
extern const char* logActionsDictionary[LOG_ACTION_SIZE];
extern const char* logTypesDictionary[LOG_TYPES_SIZE];
#if ( DEBUG_SERIAL_ALARM > 0U )
  extern const char* alarmActionStr[LOG_ACTION_SIZE];
  extern const char* eventTypesStr[LOG_TYPES_SIZE];
#endif
/*----------------------- Functions ------------------------------------*/
void              vLOGICinit ( TIM_HandleTypeDef* tim );
QueueHandle_t     pLOGICgetEventQueue ( void );
void              vRELAYset ( RELAY_DEVICE* relay, RELAY_STATUS status );
void              vRELAYautoProces ( RELAY_AUTO_DEVICE* relay, fix16_t val );
void              vRELAYdelayTrig ( RELAY_DELAY_DEVICE* device );
void              vRELAYdelayProcess ( RELAY_DELAY_DEVICE* device );
void              vRELAYimpulseProcess ( RELAY_IMPULSE_DEVICE* device, fix16_t val );
void              vRELAYimpulseReset ( RELAY_IMPULSE_DEVICE* device );
void              vLOGICtimerHandler ( void );
TIMER_ERROR       vLOGICstartTimer ( SYSTEM_TIMER* timer );
uint8_t           uLOGICisTimer ( SYSTEM_TIMER timer );
TIMER_ERROR       vLOGICresetTimer ( SYSTEM_TIMER timer );
void              vLOGICtimerCallback ( void );
void              vSYSeventSend ( SYSTEM_EVENT event, LOG_RECORD_TYPE* record );
void              vLOGICprintEvent ( SYSTEM_EVENT event );
void              vLOGICprintDebug ( const char* str );
void              vLOGICprintLogRecord ( LOG_RECORD_TYPE record );
/*----------------------------------------------------------------------*/
#endif /* INC_LOGICTYPES_H_ */

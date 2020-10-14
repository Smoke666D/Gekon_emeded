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
#define  EVENT_QUEUE_LENGTH         16U
#define  LOG_TYPES_SIZE             34U
#define  LOG_ACTION_SIZE            7U
#define  HMI_CMD_MASK               0xFFU
#define  TASK_NOTIFY_WAIT_DELAY     10U
#define  SYS_TIMER_SEMAPHORE_DELAY  200U
#define  DEBUG_SERIAL_ALARM         1U    /* Set 1 to print in serial all warnings and alarms */
#define  DEBUG_SERIAL_STATUS        1U    /* Set 1 to print in serial all state transfer */
/*------------------------ Tasks ---------------------------------------*/
#define  FPI_TASK_PRIORITY          osPriorityLow
#define  ENGINE_TASK_PRIORITY       osPriorityLow
#define  ELECTRO_TASK_PRIORITY      osPriorityLow
#define  CONTROLLER_TASK_PRIORITY   osPriorityNormal
#define  FPI_TASK_STACK_SIZE        1024U
#define  ENGINE_TASK_STACK_SIZE     1024U
#define  ELECTRO_TASK_STACK_SIZE    1024U
#define  CONTROLLER_TASK_STACK_SIZE 1024U
typedef  uint8_t timerID_t;
/*------------------------ Macros --------------------------------------*/
/*----------------------- Constant -------------------------------------*/
extern const char* logTypesDictionary[LOG_TYPES_SIZE];
extern const char* logActionsDictionary[LOG_ACTION_SIZE];
/*------------------------- Enum ---------------------------------------*/
typedef enum
{
  DATA_API_MESSAGE_REINIT = 0x00010000,
} DATA_API_MESSAGE;

typedef enum
{
  ACTION_NONE,
  ACTION_WARNING,
  ACTION_EMERGENCY_STOP,
  ACTION_LOAD_GENERATOR,
  ACTION_LOAD_MAINS,
  ACTION_PLAN_STOP,
  ACTION_LOAD_SHUTDOWN,
} SYSTEM_ACTION;

typedef enum
{
  EVENT_NONE,                       /* NONE */
  EVENT_EXTERN_EMERGENCY_STOP,      /* EMERGENCY_STOP */
  EVENT_START_FAIL,                 /* EMERGENCY_STOP */
  EVENT_STOP_FAIL,                  /* EMERGENCY_STOP */
  EVENT_OIL_LOW_PRESSURE,           /* WARNING & EMERGENCY_STOP */
  EVENT_OIL_SENSOR_ERROR,           /* EMERGENCY_STOP */
  EVENT_ENGINE_HIGHT_TEMP,          /* WARNING & EMERGENCY_STOP */
  EVENT_ENGINE_TEMP_SENSOR_ERROR,   /* EMERGENCY_STOP */
  EVENT_FUEL_LOW_LEVEL,             /* WARNING & EMERGENCY_STOP */
  EVENT_FUEL_HIGHT_LEVEL,           /* WARNING & EMERGENCY_STOP */
  EVENT_FUEL_LEVEL_SENSOR_ERROR,    /* EMERGENCY_STOP */
  EVENT_SPEED_HIGHT,                /* EMERGENCY_STOP */
  EVENT_SPEED_LOW,                  /* EMERGENCY_STOP */
  EVENT_SPEED_SENSOR_ERROR,         /* EMERGENCY_STOP */
  EVENT_CHARGER_FAIL,               /* WARNING & EMERGENCY_STOP */
  EVENT_BATTERY_LOW,                /* WARNING */
  EVENT_BATTERY_HIGHT,              /* WARNING */
  EVENT_GENERATOR_LOW_VOLTAGE,      /* WARNING & EMERGENCY_STOP */
  EVENT_GENERATOR_HIGHT_VOLTAGE,    /* WARNING & EMERGENCY_STOP */
  EVENT_GENERATOR_LOW_FREQUENCY,    /* WARNING & EMERGENCY_STOP */
  EVENT_GENERATOR_HIGHT_FREQUENCY,  /* WARNING & EMERGENCY_STOP */
  EVENT_PHASE_IMBALANCE,            /* EMERGENCY_STOP */
  EVENT_OVER_CURRENT,               /* EMERGENCY_STOP */
  EVENT_OVER_POWER,                 /* EMERGENCY_STOP */
  EVENT_SHORT_CIRCUIT,              /* EMERGENCY_STOP */
  EVENT_MAINS_LOW_VOLTAGE,          /* WARNING */
  EVENT_MAINS_HIGHT_VOLTAGE,        /* WARNING */
  EVENT_MAINS_LOW_FREQUENCY,        /* WARNING */
  EVENT_MAINS_HIGHT_FREQUENCY,      /* WARNING */
  EVENT_MAINTENANCE_OIL,            /* WARNING */
  EVENT_MAINTENANCE_AIR,            /* WARNING */
  EVENT_MAINTENANCE_FUEL,           /* WARNING */
  EVENT_MAINS_VOLTAGE_RELAX,
  EVENT_MAINS_FREQUENCY_RELAX,
  //EVENT_GENERATOR_CIRCUIT_BREAKER_TURN_ON_FAIL,
  //EVENT_GENERATOR_CIRCUIT_BREAKER_TURN_OFF_FAIL,
} SYSTEM_EVENT_TYPE;

typedef enum
{
  HMI_CMD_NONE   = 0x00U,
  HMI_CMD_START  = 0x01U,
  HMI_CMD_STOP   = 0x02U,
  HMI_CMD_AUTO   = 0x04U,
  HMI_CMD_LOAD   = 0x08U,
  HMI_CMD_MANUAL = 0x10U,
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
  RELAY_DELAY_START,
  RELAY_DELAY_DONE,
} RELAY_IMPULSE_STATUS;

typedef enum
{
  LOG_STATUS_OK,
  LOG_STATUS_ERROR,
} LOG_STATUS;
/*----------------------- Callbacks ------------------------------------*/
typedef fix16_t ( *getValueCallBack )( void );          /* Callback to get sensor value */
typedef void    ( *setRelayCallBack )( RELAY_STATUS );  /* Callback to setup relay state */
/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  SYSTEM_EVENT_TYPE  type;
  SYSTEM_ACTION      action;
} SYSTEM_EVENT;

typedef struct __packed
{
  uint8_t       enb;
  SYSTEM_EVENT  event;
} ALARM_RELAX_TYPE;

typedef struct __packed
{
  uint8_t           enb;      /* Enable alarm at initialization */
  uint8_t           active;   /* On/Off alarm check in work flow */
  ALARM_LEVEL       type;     /* Type of alarm above or below set point  */
  fix16_t           level;    /* Set point */
  fix16_t           delay;    /* Delay to active event after triggered, seconds */
  timerID_t         timerID;  /* Number of system timer */
  SYSTEM_EVENT      event;    /* Event of event */
  ALARM_RELAX_TYPE  relax;    /* Event on alarm relaxation. Set point for relaxation calculate with level and hysteresis */
  ALARM_STATUS      status;   /* Status of the alarm */
  uint8_t           trig;     /* Alarm triggered flag. Reset from outside */
} ALARM_TYPE;

typedef struct __packed
{
  uint8_t           enb;
  RELAY_STATUS      status;
  setRelayCallBack  set;
} RELAY_DEVICE;

typedef struct __packed
{
  fix16_t       onLevel;
  fix16_t       offLevel;
  RELAY_DEVICE  relay;
} RELAY_AUTO_DEVICE;

typedef struct __packed
{
  uint8_t               active;
  RELAY_DEVICE          relay;
  fix16_t               level;
  fix16_t               delay;
  timerID_t             timerID;
  RELAY_IMPULSE_STATUS  status;
} RELAY_IMPULSE_DEVICE;

typedef struct __packed
{
  uint32_t      time;   /* 4 bytes */
  SYSTEM_EVENT  event;  /* 2 bytes */
} LOG_RECORD_TYPE;
/*----------------------- Extern ---------------------------------------*/
/*----------------------- Functions ------------------------------------*/
void          vLOGICinit ( TIM_HandleTypeDef* tim );
QueueHandle_t pLOGICgetEventQueue ( void );
void          vALARMcheck ( ALARM_TYPE* alarm, fix16_t val, QueueHandle_t queue );
void          vRELAYproces ( RELAY_AUTO_DEVICE* relay, fix16_t val );
void          vRELAYimpulseProcess ( RELAY_IMPULSE_DEVICE* device, fix16_t val );
void          vRELAYimpulseReset ( RELAY_IMPULSE_DEVICE* device );
void          vLOGICtimerHandler ( void );
TIMER_ERROR   vLOGICstartTimer ( fix16_t delay, timerID_t* id );
uint8_t       uLOGICisTimer ( timerID_t id );
TIMER_ERROR   vLOGICresetTimer ( timerID_t id );
void          vLOGICtimerCallback ( void );
void          vLOGICtoogle ( uint8_t* input );
void          vLOGICprintEvent ( SYSTEM_EVENT event );
/*----------------------------------------------------------------------*/
#endif /* INC_LOGICTYPES_H_ */

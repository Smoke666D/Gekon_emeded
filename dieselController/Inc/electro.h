/*
 * generator.h
 *
 *  Created on: 24 июн. 2020 г.
 *      Author: photo_Mickey
 */

#ifndef INC_ELECTRO_H_
#define INC_ELECTRO_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "fix16.h"
#include "controllerTypes.h"
/*------------------------ Macros --------------------------------------*/
/*------------------------ Define --------------------------------------*/
#define  GENERATOR_LINE_NUMBER             3U
#define  MAINS_LINE_NUMBER                 3U
#define  ELECTRO_COMMAND_QUEUE_LENGTH      10U
#define  POWER_USAGE_CALC_TIMEOUT          5U     /* sec */

#define  TEMP_PROTECTION_TIME_MULTIPLIER   36U    /* t */
#define  CUTOUT_PROTECTION_TRIPPING_CURVE  0.001f /* K */
#define  SHORT_CIRCUIT_CONSTANT            0.14f
#define  CUTOUT_POWER                      0.02f
/*------------------------- Enum ---------------------------------------*/
typedef enum
{
  ELECTRO_SCHEME_START,
  ELECTRO_SCHEME_TRIANGLE,
  ELECTRO_SCHEME_SINGLE_PHASE,
} ELECTRO_SCHEME;

typedef enum
{
  ELECTRO_STATUS_IDLE,
  ELECTRO_STATUS_LOAD,
} ELECTRO_STATUS;

typedef enum
{
  ELECTRO_CMD_NONE,
  ELECTRO_CMD_LOAD_MAINS,
  ELECTRO_CMD_LOAD_GENERATOR,
  ELECTRO_CMD_ENABLE_STOP_ALARMS,
  ELECTRO_CMD_DISABLE_START_ALARMS,
  ELECTRO_CMD_ENABLE_START_TO_IDLE_ALARMS,
  ELECTRO_CMD_DISABLE_IDLE_ALARMS,
  ELECTRO_CMD_ENABLE_IDLE_ALARMS,
  ELECTRO_CMD_RESET_TO_IDLE,
} ELECTRO_COMMAND;

typedef enum
{
  ELECTRO_PROC_STATUS_IDLE,
  ELECTRO_PROC_STATUS_DISCONNECT,
  ELECTRO_PROC_STATUS_CONNECT,
  ELECTRO_PROC_STATUS_DONE,
} ELECTRO_PROCESS_STATUS;

typedef enum
{
  ELECTRO_CURRENT_STATUS_IDLE,
  ELECTRO_CURRENT_STATUS_OVER_TRIG,
  ELECTRO_CURRENT_STATUS_OVER_COOLDOWN,
  ELECTRO_CURRENT_STATUS_CUTOUT_TRIG,
  ELECTRO_CURRENT_STATUS_ALARM,
} ELECTRO_CURRENT_STATUS;

typedef enum
{
  ELECTRO_ALARM_STATUS_STOP,
  ELECTRO_ALARM_STATUS_START,
  ELECTRO_ALARM_STATUS_START_ON_IDLE,
  ELECTRO_ALARM_STATUS_WORK,
  ELECTRO_ALARM_STATUS_WORK_ON_IDLE,
} ELECTRO_ALARM_STATUS;
/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  fix16_t  voltage;
  fix16_t  freq;
  fix16_t  current;
} ELECTRO_CHANNEL_VALUE;

typedef struct __packed
{
  getValueCallBack  getVoltage;
  getValueCallBack  getCurrent;
  getValueCallBack  getPower;
} ELECTRO_CHANNEL;

typedef struct __packed
{
  fix16_t active;
  fix16_t reactive;
  fix16_t full;
} POWER_TYPE;

typedef struct __packed
{
  fix16_t primary;
  fix16_t nominal;
} RATING_CURRENT;

typedef struct __packed
{
  POWER_TYPE     power;
  fix16_t        cosFi;
  fix16_t        freq;
  RATING_CURRENT current;
} GENERATOR_RATING;

typedef struct __packed
{
  fix16_t      current;
  fix16_t      delay;
  SYSTEM_EVENT event;
} CURRENT_SETTING_TYPE;

typedef struct __packed
{
  ELECTRO_CURRENT_STATUS state;
  CURRENT_SETTING_TYPE   over;
  CURRENT_SETTING_TYPE   cutout;
  TIM_HandleTypeDef*     tim;
} CURRENT_ALARM_TYPE;

typedef struct __packed
{
  ELECTRO_COMMAND         cmd;
  ELECTRO_SCHEME          scheme;
  ELECTRO_PROCESS_STATUS  state;
  ELECTRO_ALARM_STATUS    alarmState;
  SYSTEM_TIMER            timer;
} ELECTRO_SYSTEM_TYPE;

typedef struct __packed
{
  uint8_t            enb;
  ELECTRO_STATUS     state;
  GENERATOR_RATING   rating;
  ELECTRO_CHANNEL    line[GENERATOR_LINE_NUMBER];
  getValueCallBack   getFreq;

  SYSTEM_TIMER       timer;
  /*---------- ALARMS ----------*/
  ALARM_TYPE         lowVoltageAlarm;
  ALARM_TYPE         lowVoltagePreAlarm;
  ALARM_TYPE         hightVoltageAlarm;
  ALARM_TYPE         hightVoltagePreAlarm;
  ALARM_TYPE         lowFreqAlarm;
  ALARM_TYPE         lowFreqPreAlarm;
  ALARM_TYPE         hightFreqAlarm;
  ALARM_TYPE         hightFreqPreAlarm;
  ALARM_TYPE         overloadAlarm;
  ALARM_TYPE         phaseImbalanceAlarm;
  CURRENT_ALARM_TYPE currentAlarm;
  /*---------- OUTPUT ----------*/
  RELAY_DEVICE       relay;
  RELAY_DELAY_DEVICE relayOn;
  RELAY_DELAY_DEVICE relayOff;
} GENERATOR_TYPE;

typedef struct __packed
{
  uint8_t            enb;
  ELECTRO_STATUS     state;
  ELECTRO_CHANNEL    line[MAINS_LINE_NUMBER];
  getValueCallBack   getFreq;
  /*---------- ALARMS ----------*/
  PERMISSION         alarmsIgnor;
  ALARM_TYPE         lowVoltageAlarm;
  ALARM_TYPE         hightVoltageAlarm;
  ALARM_TYPE         lowFreqAlarm;
  ALARM_TYPE         hightFreqAlarm;
  /*---------- EVENTS ----------*/
  ERROR_TYPE         autoStart;
  ERROR_TYPE         autoStop;
  /*---------- OUTPUT ----------*/
  RELAY_DEVICE       relay;
  RELAY_DELAY_DEVICE relayOn;
  RELAY_DELAY_DEVICE relayOff;
} MAINS_TYPE;
/*----------------------- Extern ---------------------------------------*/
extern osThreadId_t electroHandle;
/*----------------------- Functions ------------------------------------*/
void                   vELECTROinit ( void );
ELECTRO_STATUS         eELECTROgetGeneratorStatus ( void );
ELECTRO_STATUS         eELECTROgetMainsStatus ( void );
ELECTRO_PROCESS_STATUS eELECTROgetStatus( void );
ELECTRO_ALARM_STATUS   eELECTROgetAlarmStatus ( void );
QueueHandle_t          pELECTROgetCommandQueue ( void );
void                   vELECTROsendCmd ( ELECTRO_COMMAND cmd );
fix16_t                fELECTROgetMaxGenVoltage ( void );
TRIGGER_STATE          eELECTROgetMainsErrorFlag ( void );
/*----------------------------------------------------------------------*/
#endif /* INC_ELECTRO_H_ */

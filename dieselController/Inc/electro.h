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
#define  GENERATOR_LINE_NUMBER             4U
#define  MAINS_LINE_NUMBER                 3U
#define  TEMP_PROTECTION_TIME_MULTIPLIER   ( F16( 36U ) )    /* t */
#define  CUTOUT_PROTECTION_TRIPPING_CURVE  ( F16( 0.001f ) ) /* K */
#define  CUTOUT_POWER                      ( F16( 0.02f ) )
#define  ELECTRO_COMMAND_QUEUE_LENGTH      10U
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
  ELECTRO_STATUS_ALARM,
} ELECTRO_STATUS;

typedef enum
{
  ELECTRO_CMD_NONE,
  ELECTRO_CMD_LOAD_MAINS,
  ELECTRO_CMD_LOAD_GENERATOR,
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
/*----------------------- Structures -----------------------------------*/
typedef struct
{
  fix16_t  voltage;
  fix16_t  freq;
  fix16_t  current;
} ELECTRO_CHANNEL_VALUE;

typedef struct
{
  getValueCallBack  getVoltage;
  getValueCallBack  getFreq;
  getValueCallBack  getCurrent;
} ELECTRO_CHANNEL;

typedef struct
{
  fix16_t active;
  fix16_t reactive;
  fix16_t apparent;
} POWER_TYPE;

typedef struct
{
  fix16_t primary;
  fix16_t nominal;
} RATING_CURRENT;

typedef struct
{
  POWER_TYPE     power;
  fix16_t        cosFi;
  fix16_t        freq;
  RATING_CURRENT current;
} GENERATOR_RATING;

typedef struct
{
  fix16_t      current;
  fix16_t      delay;
  SYSTEM_EVENT event;
} CURRENT_SETTING_TYPE;

typedef struct
{
  ELECTRO_CURRENT_STATUS state;
  CURRENT_SETTING_TYPE   over;
  CURRENT_SETTING_TYPE   cutout;
  TIM_HandleTypeDef*     tim;
} CURRENT_ALARM_TYPE;

typedef struct
{
  ELECTRO_SCHEME          scheme;
  ELECTRO_PROCESS_STATUS  state;
  ELECTRO_COMMAND         cmd;
  timerID_t               timerID;
  fix16_t                 switchDelay;
} ELECTRO_SYSTEM_TYPE;

typedef struct
{
  uint8_t               enb;
  ELECTRO_STATUS        state;
  GENERATOR_RATING      rating;
  ELECTRO_CHANNEL       line[GENERATOR_LINE_NUMBER];
  /*---------- ALARMS ----------*/
  ALARM_TYPE            lowVoltageAlarm;
  ALARM_TYPE            lowVoltagePreAlarm;
  ALARM_TYPE            hightVoltageAlarm;
  ALARM_TYPE            hightVoltagePreAlarm;
  ALARM_TYPE            lowFreqAlarm;
  ALARM_TYPE            lowFreqPreAlarm;
  ALARM_TYPE            hightFreqAlarm;
  ALARM_TYPE            hightFreqPreAlarm;
  ALARM_TYPE            overloadAlarm;
  ALARM_TYPE            phaseImbalanceAlarm;
  CURRENT_ALARM_TYPE    currentAlarm;
  /*---------- OUTPUT ----------*/
  RELAY_DEVICE          relay;
  RELAY_IMPULSE_DEVICE  relayOn;
  RELAY_IMPULSE_DEVICE  relayOff;
} GENERATOR_TYPE;

typedef struct
{
  uint8_t               enb;
  ELECTRO_STATUS        state;
  ELECTRO_CHANNEL       line[MAINS_LINE_NUMBER];
  /*---------- ALARMS ----------*/
  ALARM_TYPE            lowVoltageAlarm;
  ALARM_TYPE            hightVoltageAlarm;
  ALARM_TYPE            lowFreqAlarm;
  ALARM_TYPE            hightFreqAlarm;
  /*---------- OUTPUT ----------*/
  RELAY_DEVICE          relay;
  RELAY_IMPULSE_DEVICE  relayOn;
  RELAY_IMPULSE_DEVICE  relayOff;
} MAINS_TYPE;
/*----------------------- Functions ------------------------------------*/
void           vELECTROinit ( TIM_HandleTypeDef* currentTIM );
ELECTRO_STATUS eELECTROgetGeneratorStatus ( void );
ELECTRO_STATUS eELECTROgetMainsStatus ( void );
QueueHandle_t  pELECTROgetCommandQueue ( void );
void           vELECTROtask ( void const* argument );
void           vELECTROalarmStartDisable ( void );
void           vELECTROalarmStartToIdle ( void );
void           vELECTROalarmIdleDisable ( void );
void           vELECTROalarmIdleEnable ( void );
/*----------------------------------------------------------------------*/
#endif /* INC_ELECTRO_H_ */

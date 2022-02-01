/*
 * pid.h
 *
 *  Created on: Feb 1, 2022
 *      Author: 79110
 */
#ifndef INC_PID_H_
#define INC_PID_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "fix16.h"
#include "common.h"
/*------------------------ Define --------------------------------------*/
#define PID_MAX_NUMBER 2U
/*------------------------ Macros --------------------------------------*/
/*------------------------ Enum ----------------------------------------*/
typedef enum
{
  PID_TUNER_STATE_IDLE,
  PID_TUNER_STATE_FIRST_LEVEL,
  PID_TUNER_STATE_SECOND_LEVEL,
  PID_TUNER_STATE_RETURN_TO_FIRST_LEVEL,
  PID_TUNER_STATE_FIRST_PERIOD,
  PID_TUNER_STATE_SECOND_PERIOD,
  PID_TUNER_STATE_CALC,
  PID_TUNER_STATE_FINISH
} PID_TUNER_STATE;
/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  fix16_t          Kp;
  fix16_t          Kd;
  fix16_t          Ki;
  uint32_t         sampleTime;
  fix16_t          min;
  fix16_t          max;
  getValueCallBack get;
  getValueCallBack setpoint;
  setValueCallBack set;
} PID_INIT;

typedef struct __packed
{
  fix16_t  input;
} PID_LAST_TYPE;

typedef struct __packed
{
  /* Settings */
  fix16_t  Kp;
  fix16_t  Kd;
  fix16_t  Ki;
  uint32_t SampleTime;
  fix16_t  min;
  fix16_t  max;
  /* Calculation */
  fix16_t  error;
  fix16_t  input;
  fix16_t  output;
  fix16_t  iterm;
  fix16_t  delta;
  /* Callbacks */
  getValueCallBack get;
  getValueCallBack setpoint;
  setValueCallBack set;
  /* Data */
  PID_LAST_TYPE last;
} PID_TYPE;

typedef struct __packed
{
  PID_TYPE*       pid;
  uint8_t         dir : 1U;
  fix16_t         start;
  fix16_t         end;
  fix16_t         window;

  fix16_t         startValue;
  fix16_t         beta;


  uint32_t        time;
  fix16_t         period1;
  fix16_t         period2;
  fix16_t         value;
  PID_TUNER_STATE state;

} PID_TUNER_TYPE;
/*----------------------- Functions ------------------------------------*/
/*----------------------------------------------------------------------*/
#endif /* INC_PID_H_ */

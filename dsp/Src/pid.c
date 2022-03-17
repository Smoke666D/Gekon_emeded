/*
 * pid.c
 *
 *  Created on: Feb 1, 2022
 *      Author: 79110
 */
/*----------------------- Includes -------------------------------------*/
#include "pid.h"
#include "system.h"
#include "stdio.h"
#include "cmsis_os2.h"
#include "FreeRTOS.h"
/*------------------------ Define --------------------------------------*/
/*----------------------- Constant -------------------------------------*/
/*---------------------- Structures ------------------------------------*/
static PID_TYPE       pids[PID_MAX_NUMBER]       = { 0U };
static osThreadId_t   pidHandles[PID_MAX_NUMBER] = { 0U };
static PID_TUNER_TYPE tuner                      = { 0U };
/*----------------------- Variables ------------------------------------*/
/*----------------------- External -------------------------------------*/
/*----------------------- Functions ------------------------------------*/
void vPIDtuning ( void );
void vPIDtask ( void* argument );
/*----------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------*/
/*----------------------------------------------------------------------*/
void vPIDtuning ( void )
{
  fix16_t current = tuner.pid->get();
  fix16_t buffer  = 0U;
  fix16_t tcalc   = 0U;
  fix16_t tauI    = 0U;
  fix16_t tauD    = 0U;
  fix16_t koef    = 0U;
  switch ( tuner.state )
  {
    case PID_TUNER_STATE_IDLE:
      tuner.pid->set( tuner.start );
      tuner.state = PID_TUNER_STATE_FIRST_LEVEL;
      break;

    case PID_TUNER_STATE_FIRST_LEVEL:
      if ( fix16_abs( fix16_sub( current, tuner.value ) ) < tuner.window )
      {
        tuner.pid->set( tuner.end );
        tuner.startValue = current;
        tuner.state      = PID_TUNER_STATE_SECOND_LEVEL;
      }
      tuner.value = current;
      break;

    case PID_TUNER_STATE_SECOND_LEVEL:
      if ( fix16_abs( fix16_sub( current, tuner.value ) ) < tuner.window )
      {
        tuner.pid->set( tuner.start );
        tuner.beta  = fix16_abs( fix16_sub( current, tuner.startValue ) );
        tuner.state = PID_TUNER_STATE_RETURN_TO_FIRST_LEVEL;
      }
      tuner.value = current;
      break;

    case PID_TUNER_STATE_RETURN_TO_FIRST_LEVEL:
      if ( fix16_abs( fix16_sub( current, tuner.value ) ) < tuner.window )
      {
        tuner.pid->set( tuner.end );
        tuner.time  = 0U;
        tuner.state = PID_TUNER_STATE_FIRST_PERIOD;
      }
      tuner.value = current;
      break;

    case PID_TUNER_STATE_FIRST_PERIOD:
      buffer = fix16_add( tuner.startValue, fix16_div( tuner.beta, fix16_from_int( 2U ) ) );
      if ( ( ( tuner.dir == 0U ) && ( current > buffer ) ) ||
           ( ( tuner.dir  > 0U ) && ( current < buffer ) ) )
      {
        tuner.period1 = fix16_div( tuner.time, fix16_from_int( 1000U ) );
        tuner.state   = PID_TUNER_STATE_SECOND_PERIOD;
      }
      tuner.time++;
      break;

    case PID_TUNER_STATE_SECOND_PERIOD:
      buffer = fix16_add( tuner.startValue, fix16_mul( tuner.beta, fix16_to_float( 0.632f ) ) );
      if ( ( ( tuner.dir == 0U ) && ( current > buffer ) ) ||
           ( ( tuner.dir  > 0U ) && ( current < buffer ) ) )
      {
        tuner.period2 = fix16_div( tuner.time, fix16_from_int( 1000U ) );
        tuner.state   = PID_TUNER_STATE_CALC;
      }
      tuner.time++;
      break;

    case PID_TUNER_STATE_CALC:   // считаем
      tcalc  = fix16_div( fix16_sub( tuner.period1, fix16_mul( fix16_to_float( 0.693f ), tuner.period2 ) ), fix16_to_float( 0.307f ) );
      koef   = fix16_div( tuner.beta, fix16_abs( fix16_sub( tuner.end, tuner.start ) ) );
      buffer = fix16_div( tcalc, fix16_sub( tuner.period2, tcalc ) );
      tauI   = fix16_mul( tcalc, fix16_div( fix16_add( fix16_from_int( 32U ), fix16_mul( fix16_from_int( 6U ), buffer ) ), fix16_add( fix16_from_int( 13U ), fix16_mul( fix16_from_int( 8U ),  buffer ) ) ) );
      tauD   = fix16_div( fix16_mul( tcalc, fix16_from_int( 4U ) ), fix16_add( fix16_from_int( 11U ), fix16_mul( fix16_from_int( 2U ), buffer ) ) );
      tuner.pid->Kp = fix16_div( fix16_add( fix16_to_float( 1.33f ), fix16_div( buffer, 4.0 ) ), fix16_mul( buffer, koef ) );
      tuner.pid->Ki = fix16_div( tuner.pid->Kp, tauI );
      tuner.pid->Kd = fix16_mul( tuner.pid->Kp, tauD );
      tuner.state = PID_TUNER_STATE_FINISH;
      break;

    case PID_TUNER_STATE_FINISH:
      break;
    default:
      break;
  }
  return;
}
/*----------------------------------------------------------------------*/
void vPIDtask ( void* argument )
{
  PID_TYPE* pid = ( PID_TYPE* )( argument );
  for(;;)
  {
    pid->input      = pid->get();
    pid->error      = fix16_sub( pid->setpoint(), pid->input );
    pid->delta      = fix16_sub( pid->input, pid->last.input );
    pid->iterm      = fix16_clamp( fix16_add( pid->iterm, fix16_mul( pid->Ki, pid->error ) ), pid->min, pid->max );
    pid->output     = fix16_sub( fix16_mul( pid->Kp, pid->error ), fix16_mul( pid->Kd, pid->delta ) );
    pid->output     = fix16_add( pid->output, pid->iterm );
    pid->output     = fix16_clamp( pid->output, pid->min, pid->max );
    pid->last.input = pid->input;
    pid->set( pid->output );
    osDelay( pid->SampleTime );
  }
  return;
}
/*----------------------------------------------------------------------*/
/*------------------------ PUBLIC --------------------------------------*/
/*----------------------------------------------------------------------*/
void vPIDinit ( const PID_INIT* data, uint8_t length )
{
  uint8_t i         = 0U;
  char    name[10U] = { 0U };
  if ( length <= PID_MAX_NUMBER )
  {
    for ( i=0U; i<length; i++ )
    {
      pids[i].Kd         = data->Kd;
      pids[i].Ki         = data->Ki;
      pids[i].Kp         = data->Kp;
      pids[i].SampleTime = data->sampleTime;
      pids[i].min        = data->min;
      pids[i].max        = data->max;
      pids[i].get        = data->get;
      pids[i].setpoint   = data->setpoint;
      pids[i].set        = data->set;
      ( void )sprintf( name, "pidTask%d", i );
      const osThreadAttr_t attributes = {
        .name       = name,
        .priority   = ( osPriority_t ) PID_TASK_PRIORITY,
        .stack_size = PID_TASK_STACK_SIZE
      };
      pidHandles[i] = osThreadNew( vPIDtask, ( void* )( &pids[i] ), &attributes );
    }
  }
  return;
}
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

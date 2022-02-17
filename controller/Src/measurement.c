/*
 * measerment.c
 *
 *  Created on: 19 янв. 2021 г.
 *      Author: 79110
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "measurement.h"
#include "config.h"
#include "dataProces.h"
#include "storage.h"
#include "engine.h"
#include "adc.h"
#include "fpo.h"
#include "fpi.h"
#include "journal.h"
#include "system.h"
#include "dataSD.h"
#include "stdio.h"
/*----------------------- Structures ----------------------------------------------------------------*/
static MEASUREMENT_TYPE measurement              = { 0U };
static osThreadId_t     measurementHandle        = NULL;
static QueueHandle_t    pMeasurementCommandQueue = NULL;
static StaticQueue_t    xMeasurementCommandQueue = { 0U };
static SD_ROUTINE       sdRoutine                = { 0U };
/*----------------------- Functions -----------------------------------------------------------------*/
uint8_t  vMEASUREMENTgetChannel ( MEASURMENT_SETTING setting, uint8_t* queue, uint8_t index );
uint16_t uMEASUREMENTgetData ( uint8_t chanel );
void     vMEASUREMENTdataInit ( void );
/*----------------------- Constant ------------------------------------------------------------------*/
/*----------------------- Variables -----------------------------------------------------------------*/
static MEASURMENT_CMD measurementCommandBuffer[MEASUREMENT_COMMAND_QUEUE_LENGTH] = { 0U };
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PRIVATE -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
uint8_t vMEASUREMENTgetChannel ( MEASURMENT_SETTING setting, uint8_t* queue, uint8_t index )
{
  ELECTRO_SCHEME data   = getBitMap( &genSetup, GEN_AC_SYS_ADR );
  uint8_t        length = 0U;

  switch ( setting )
  {
    case OUTPUT_SETTING_OIL_PRESSURE:
      queue[index] = OIL_PRESSURE_ADR;
      length++;
      break;
    case OUTPUT_SETTING_COOLANT_TEMP:
      queue[index] = COOLANT_TEMP_ADR;
      length++;
      break;
    case OUTPUT_SETTING_FUEL_LEVEL:
      queue[index] = FUEL_LEVEL_ADR;
      length++;
      break;
    case OUTPUT_SETTING_SPEED:
      queue[index] = SPEED_LEVEL_ADR;
      length++;
      break;
    case OUTPUT_SETTING_INPUTS:
      queue[index] = DIGITAL_INPUT_ADR;
      length++;
      break;
    case OUTPUT_SETTING_OUTPUTS:
      queue[index] = DIGITAL_OUTPUT_ADR;
      length++;
      break;
    case OUTPUT_SETTING_GEN_PHASE_VOLTAGE:
      if ( data == ELECTRO_SCHEME_SINGLE_PHASE )
      {
        queue[index] = GENERATOR_PHASE_VOLTAGE_L1_ADR;
        length++;
      }
      else
      {
        queue[index]      = GENERATOR_PHASE_VOLTAGE_L1_ADR;
        queue[index + 1U] = GENERATOR_PHASE_VOLTAGE_L2_ADR;
        queue[index + 2U] = GENERATOR_PHASE_VOLTAGE_L3_ADR;
        length += 3U;
      }
      break;
    case OUTPUT_SETTING_GEN_LINE_VOLTAGE:
      if ( data == ELECTRO_SCHEME_SINGLE_PHASE )
      {
        queue[index] = GENERATOR_LINE_VOLTAGE_L1_ADR;
        length++;
      }
      else
      {
        queue[index]      = GENERATOR_LINE_VOLTAGE_L1_ADR;
        queue[index + 1U] = GENERATOR_LINE_VOLTAGE_L2_ADR;
        queue[index + 2U] = GENERATOR_LINE_VOLTAGE_L3_ADR;
        length += 3U;
      }
      break;
    case OUTPUT_SETTING_CURRENT:
      if ( data == ELECTRO_SCHEME_SINGLE_PHASE )
      {
        queue[index] = GENERATOR_CURRENT_L1_ADR;
        length++;
      }
      else
      {
        queue[index]      = GENERATOR_CURRENT_L1_ADR;
        queue[index + 1U] = GENERATOR_CURRENT_L2_ADR;
        queue[index + 2U] = GENERATOR_CURRENT_L3_ADR;
        length += 3U;
      }
      break;
    case OUTPUT_SETTING_FREQ_GEN:
      queue[index] = GENERATOR_FREQ_ADR;
      length++;
      break;
    case OUTPUT_SETTING_COS_FI:
      queue[index] = GENERATOR_COS_FI_ADR;
      length++;
      break;
    case OUTPUT_SETTING_POWER_ACTIVE:
      queue[index] = GENERATOR_POWER_ACTIVE_ADR;
      length++;
      break;
    case OUTPUT_SETTING_POWER_REACTIVE:
      queue[index] = GENERATOR_POWER_REACTIVE_ADR;
      length++;
      break;
    case OUTPUT_SETTING_POWER_FULL:
      queue[index] = GENERATOR_POWER_FULL_ADR;
      length++;
      break;
    case OUTPUT_SETTING_MAINS_PHASE_VOLTAGE:
      if ( data == ELECTRO_SCHEME_SINGLE_PHASE )
      {
        queue[index] = MAINS_PHASE_VOLTAGE_L1_ADR;
        length++;
      }
      else
      {
        queue[index]      = MAINS_PHASE_VOLTAGE_L1_ADR;
        queue[index + 1U] = MAINS_PHASE_VOLTAGE_L2_ADR;
        queue[index + 2U] = MAINS_PHASE_VOLTAGE_L3_ADR;
        length += 3U;
      }
      break;
    case OUTPUT_SETTING_MAINS_LINE_VOLTAGE:
      if ( data == ELECTRO_SCHEME_SINGLE_PHASE )
      {
        queue[index] = MAINS_LINE_VOLTAGE_L1_ADR;
        length++;
      }
      else
      {
        queue[index]      = MAINS_LINE_VOLTAGE_L1_ADR;
        queue[index + 1U] = MAINS_LINE_VOLTAGE_L2_ADR;
        queue[index + 2U] = MAINS_LINE_VOLTAGE_L3_ADR;
        length += 3U;
      }
      break;
    case OUTPUT_SETTING_MAINS_FREQ:
      queue[index] = MAINS_FREQ_ADR;
      length++;
      break;
    case OUTPUT_SETTING_VOLTAGE_ACC:
      queue[index] = BATTERY_VOLTAGE_ADR;
      length++;
      break;
    default:
      break;
  }
  return length;
}
/*---------------------------------------------------------------------------------------------------*/
uint16_t uMEASUREMENTgetData ( uint8_t chanel )
{
  uint32_t time = 0U;
  uint16_t res  = 0U;
  if ( chanel < OUTPUT_DATA_REGISTER_NUMBER )
  {
    vOUTPUTupdate( chanel );
    res = outputDataReg[chanel]->value[0U];
  }
  else
  {
    time = uLOGgetTime();
    if ( chanel == MEASUREMENT_TIME_CHANEL )
    {
      res = ( uint16_t )time;
    }
    else
    {
      res = ( uint16_t )( time >> 16U );
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
void vMEASUREMENTdataInit ( void )
{
  uint8_t i = 0U;
  uint8_t j = 0U;
  measurement.state        = MEASURMENT_STATE_IDLE;
  measurement.timer.delay  = getValue( &recordInterval );
  measurement.timer.id     = LOGIC_DEFAULT_TIMER_ID;
  measurement.length       = 2U;
  measurement.channels[0U] = MEASUREMENT_DATE_CHANEL;
  measurement.channels[1U] = MEASUREMENT_TIME_CHANEL;
  for ( i=0U; i<( recordSetup0.atrib->bitMapSize - 1U ); i++ )
  {
    if ( getBitMap( &recordSetup0, ( RECORD_ENB_ADR + 1U + i ) ) > 0U )
    {
      measurement.length += vMEASUREMENTgetChannel( j, measurement.channels, measurement.length );
    }
    j++;
  }
  for ( i=0; i<recordSetup1.atrib->bitMapSize; i++ )
  {
    if ( getBitMap( &recordSetup1, ( RECORD_CURRENT_ENB_ADR + i ) ) > 0U )
    {
      measurement.length += vMEASUREMENTgetChannel( j, measurement.channels, measurement.length );
    }
    j++;
  }
  if ( measurement.length > 0U )
  {
    measurement.length += 2U; /* For time and date */
  }
  else
  {
    measurement.size = 0U;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vMEASUREMENTmakeStartLine ( SD_ROUTINE* routine )
{
  uint32_t length = 0U;
  RTC_TIME time   = { 0U };
  vRTCgetCashTime( &time );
  routine->length = ( uint32_t )sprintf( routine->buffer, "//** %d.%d.%d %d:%d:%d\n", time.year, time.month, time.day, time.hour, time.min, time.sec );
  routine->length = uSYSendString( routine->buffer, length );
  vSDsendRoutine( routine );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vMEASUREMENTmakeLegendLine ( SD_ROUTINE* routine )
{
  uint8_t i = 0U;
  routine->length = 2U;
  routine->buffer[0U] = '/';
  routine->buffer[1U] = '/';
  for ( i=0U; i<measurement.length; i++ )
  {
    routine->length += ( uint32_t )sprintf( routine->buffer, "%d ", measurement.channels[i] );
  }
  routine->length = uSYSendString( routine->buffer, routine->length );
  vSDsendRoutine( routine );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vMEASUREMENTmakeDataLine ( SD_ROUTINE* routine )
{
  uint8_t i = 0U;
  sdRoutine.length = 0U;
  for ( i=0U; i<measurement.length; i++ )
  {
    routine->length += ( uint32_t )sprintf( &routine->buffer[routine->length], "%d ", uMEASUREMENTgetData( measurement.channels[i] ) );
  }
  routine->length = uSYSendString( routine->buffer, routine->length );
  vSDsendRoutine( routine );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vMEASUREMENTtask ( void* argument )
{
  MEASURMENT_CMD  inputCmd = MEASURMENT_CMD_NONE;
  for (;;)
  {
    /*------------------------------------------------------------------*/
    /*----------------------- Read input command -----------------------*/
    /*------------------------------------------------------------------*/
    if ( xQueueReceive( pMEASUREMENTgetCommandQueue(), &inputCmd, 0U ) == pdPASS )
    {
      if ( measurement.cmd != inputCmd )
      {
        measurement.cmd = inputCmd;
      }
    }
    /*------------------------------------------------------------------*/
    switch( measurement.cmd )
    {
      case MEASURMENT_CMD_NONE:
        break;
      case MEASURMENT_CMD_START:
        vLOGICprintDebug( ">>Measurement     : Start command...\r\n" );
        if ( measurement.state == MEASURMENT_STATE_IDLE )
        {
          measurement.state = MEASURMENT_STATE_START;
        }
        measurement.cmd = MEASURMENT_CMD_NONE;
        break;
      case MEASURMENT_CMD_STOP:
        vLOGICprintDebug( ">>Measurement     : Stop command...\r\n" );
        switch ( measurement.state )
        {
          case MEASURMENT_STATE_WAIT:
            vLOGICresetTimer( &measurement.timer );
            measurement.state = MEASURMENT_STATE_IDLE;
            measurement.cmd   = MEASURMENT_CMD_NONE;
            vLOGICprintDebug( ">>Measurement     : Stop!\r\n" );
            break;
          case MEASURMENT_STATE_IDLE:
            measurement.cmd   = MEASURMENT_CMD_NONE;
            break;
          case MEASURMENT_STATE_ERROR:
            measurement.cmd   = MEASURMENT_CMD_NONE;
            break;
          default:
            break;
        }
        break;
      default:
        measurement.state = MEASURMENT_STATE_ERROR;
        measurement.cmd   = MEASURMENT_CMD_NONE;
        break;
    }
    /*------------------------------------------------------------------*/
    /*--------------------------- Processing ---------------------------*/
    /*------------------------------------------------------------------*/
    if ( measurement.enb == PERMISSION_ENABLE )
    {
      switch ( measurement.state )
      {
        case MEASURMENT_STATE_IDLE:
          osDelay( 1000U );
          break;
        /*--------------------------------------------------------------*/
        case MEASURMENT_STATE_START:
          vMEASUREMENTmakeStartLine( &sdRoutine );
          vMEASUREMENTmakeLegendLine( &sdRoutine );
          vLOGICprintDebug( ">>Measurement     : Status start\r\n" );
          vLOGICstartTimer( &measurement.timer, "Measurement timer   " );
          measurement.counter = 0U;
          measurement.state   = MEASURMENT_STATE_WAIT;
          break;
        /*--------------------------------------------------------------*/
        case MEASURMENT_STATE_WAIT:
          if ( uLOGICisTimer( &measurement.timer ) > 0U )
          {
            vLOGICprintDebug( ">>Measurement     : Read data\r\n" );
            measurement.state = MEASURMENT_STATE_WRITE;
          }
          break;
        /*--------------------------------------------------------------*/
        case MEASURMENT_STATE_WRITE:
          vMEASUREMENTmakeDataLine( &sdRoutine );
          measurement.counter++;
          if ( ( measurement.size == MEASUREMENT_INFINITY ) || ( measurement.size < measurement.counter ) )
          {
            vLOGICstartTimer( &measurement.timer, "Measurement timer   " );
            measurement.state = MEASURMENT_STATE_WAIT;
          }
          else
          {
            vLOGICprintDebug( ">>Measurement     : Done!\r\n" );
            measurement.state = MEASURMENT_STATE_IDLE;
          }
          break;
        /*--------------------------------------------------------------*/
        case MEASURMENT_STATE_ERROR:
          osDelay( 0xFFFFFFFFU );
          break;
        default:
          measurement.state = MEASURMENT_STATE_START;
          break;
      }
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLICK -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
void vMEASUREMENTinit ( void )
{
  #if ( defined( FATSD ) && defined( MEASUREMENT ) )
    measurement.enb = getBitMap( &recordSetup0, RECORD_ENB_ADR );
    if ( measurement.enb == PERMISSION_ENABLE )
    {
      sdRoutine.buffer = cFATSDgetBuffer();
      sdRoutine.cmd    = SD_COMMAND_WRITE;
      sdRoutine.file   = FATSD_FILE_MEASUREMENT;
      vMEASUREMENTdataInit();
      pMeasurementCommandQueue = xQueueCreateStatic( MEASUREMENT_COMMAND_QUEUE_LENGTH,
                                                     sizeof( MEASURMENT_CMD ),
                                                     measurementCommandBuffer,
                                                     &xMeasurementCommandQueue );
      const osThreadAttr_t measurementTask_attributes = {
        .name       = "measurementTask",
        .priority   = ( osPriority_t ) MEASUREMENT_TASK_PRIORITY,
        .stack_size = MEASUREMENT_TASK_STACK_SIZE
      };
      measurementHandle = osThreadNew( vMEASUREMENTtask, NULL, &measurementTask_attributes );
    }
  #endif
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vMEASUREMENTsendCmd ( MEASURMENT_CMD cmd )
{
  MEASURMENT_CMD measurementCmd = cmd;
  xQueueSend( pMeasurementCommandQueue, &measurementCmd, portMAX_DELAY );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
QueueHandle_t pMEASUREMENTgetCommandQueue ( void )
{
  return pMeasurementCommandQueue;
}
/*---------------------------------------------------------------------------------------------------*/
uint16_t uMEASUREMENTgetSize ( void )
{
  return measurement.length;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

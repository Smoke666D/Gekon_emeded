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
/*----------------------- Structures ----------------------------------------------------------------*/
static MEASUREMENT_TYPE measurement              = { 0U };
static osThreadId_t     measurementHandle        = NULL;
static QueueHandle_t    pMeasurementCommandQueue = NULL;
static StaticQueue_t    xMeasurementCommandQueue = { 0U };
static SD_ROUTINE       sdRoutine                = { 0U };
/*----------------------- Functions -----------------------------------------------------------------*/
uint8_t  uMEASUREMENTgetChannel ( MEASURMENT_SETTING setting, uint8_t* queue, uint8_t index );
uint16_t uMEASUREMENTgetData ( uint8_t chanel );
void     vMEASUREMENTdataInit ( void );
/*----------------------- Constant ------------------------------------------------------------------*/
/*----------------------- Variables -----------------------------------------------------------------*/
static MEASURMENT_CMD measurementCommandBuffer[MEASUREMENT_COMMAND_QUEUE_LENGTH] = { 0U };
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PRIVATE -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
uint8_t uMEASUREMENTgetChannel ( MEASURMENT_SETTING setting, uint8_t* queue, uint8_t index )
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
    switch ( chanel )
    {
      case MEASUREMENT_DATE_CHANEL:
        time = uLOGgetTime();
        res  = ( uint16_t )( time >> 16U );
        break;
      case MEASUREMENT_TIME_CHANEL: /* Time takes 17 bit, the hight bit will be in date */
        time = uLOGgetTime();
        res  = ( uint16_t )( time );
        break;
      default:
        res = 0U;
        break;
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
void vMEASUREMENTdataInit ( void )
{
  MEASURMENT_SETTING j = 0U;
  measurement.state        = MEASURMENT_STATE_IDLE;
  measurement.cmd          = MEASURMENT_CMD_NONE;
  measurement.timer.delay  = getValue( &recordInterval );
  measurement.timer.id     = LOGIC_DEFAULT_TIMER_ID;
  measurement.length       = 2U;
  measurement.channels[0U] = MEASUREMENT_DATE_CHANEL;
  measurement.channels[1U] = MEASUREMENT_TIME_CHANEL;
  for ( uint8_t i=1U; i<recordSetup0.atrib->bitMapSize; i++ )
  {
    if ( getBitMap( &recordSetup0, i ) > 0U )
    {
      measurement.length += uMEASUREMENTgetChannel( j, measurement.channels, measurement.length );
    }
    j++;
  }
  for ( uint8_t i=0U; i<recordSetup1.atrib->bitMapSize; i++ )
  {
    if ( getBitMap( &recordSetup1, i ) > 0U )
    {
      measurement.length += uMEASUREMENTgetChannel( j, measurement.channels, measurement.length );
    }
    j++;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vMEASUREMENTmakeStartLine ( SD_ROUTINE* routine )
{
  char       buffer[4U] = { 0U };
  RTC_TIME   time       = { 0U };
  RTC_STATUS status     = RTC_BUSY;
  while ( status == RTC_BUSY )
  {
    status = eRTCgetTime( &time );
    osDelay( 100U );
  }

  ( void )strcat( buffer, "// " );

  routine->data[0U]  = MEASUREMENT_RECORD_TYPE_PREAMBOLA;
  routine->data[2U]  = time.year;
  routine->data[4U]  = time.month;
  routine->data[6U]  = time.day;
  routine->data[8U]  = time.hour;
  routine->data[10U] = time.min;
  routine->data[12U] = time.sec;
  for ( uint8_t i=0U; i<measurement.length; i++ )
  {
    routine->data[( 2U * i ) + 14U] = measurement.channels[i];
  }
  routine->length = measurement.length + 7U;
  vSDsendRoutine( routine );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vMEASUREMENTmakeDataLine ( SD_ROUTINE* routine )
{
  uint16_t data = 0U;
  routine->data[0U] = MEASUREMENT_RECORD_TYPE_RECORD;
  for ( uint8_t i=0U; i<measurement.length; i++ )
  {
    data = uMEASUREMENTgetData( measurement.channels[i] );
    routine->data[( 2U * i ) + 2U] = ( uint8_t )( data );
    routine->data[( 2U * i ) + 3U] = ( uint8_t )( data >> 8U );
  }
  routine->length = measurement.length + 1U;
  vSDsendRoutine( routine );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vMEASUREMENTtask ( void* argument )
{
  for (;;)
  {
    ( void )xQueueReceive( pMEASUREMENTgetCommandQueue(), &measurement.cmd, 0U );
    if ( measurement.enb == PERMISSION_ENABLE )
    {
      switch ( measurement.state )
      {
        case MEASURMENT_STATE_IDLE:
          if ( measurement.cmd == MEASURMENT_CMD_START )
          {
            if ( eFATSDgetStatus() == SD_STATUS_MOUNTED )
            {
              measurement.cmd   = MEASURMENT_CMD_NONE;
              measurement.state = MEASURMENT_STATE_START;
            }
          } else {
            measurement.cmd = MEASURMENT_STATE_START;//MEASURMENT_CMD_NONE;
          }
          break;
        /*--------------------------------------------------------------*/
        case MEASURMENT_STATE_START:
          vMEASUREMENTmakeStartLine( &sdRoutine );
          vMEASUREMENTmakeDataLine( &sdRoutine );
          vLOGICstartTimer( &measurement.timer, "Measurement timer   " );
          measurement.state   = MEASURMENT_STATE_WAIT;
          break;
        /*--------------------------------------------------------------*/
        case MEASURMENT_STATE_WAIT:
          if ( measurement.cmd == MEASURMENT_CMD_STOP )
          {
            vLOGICresetTimer( &measurement.timer );
            measurement.state = MEASURMENT_STATE_IDLE;
            measurement.cmd   = MEASURMENT_CMD_NONE;
          }
          else
          {
            if ( uLOGICisTimer( &measurement.timer ) > 0U )
            {
              measurement.state = MEASURMENT_STATE_WRITE;
            }
            measurement.cmd = MEASURMENT_CMD_NONE;
          }
          break;
        /*--------------------------------------------------------------*/
        case MEASURMENT_STATE_WRITE:
          vMEASUREMENTmakeDataLine( &sdRoutine );
          vLOGICstartTimer( &measurement.timer, "Measurement timer   " );
          measurement.state = MEASURMENT_STATE_WAIT;
          break;
        /*--------------------------------------------------------------*/
        case MEASURMENT_STATE_ERROR:
          vTaskSuspend( ( TaskHandle_t )measurementHandle );
          break;
        default:
          measurement.state = MEASURMENT_STATE_ERROR;
          measurement.cmd   = MEASURMENT_CMD_NONE;
          break;
      }
    }
    osDelay( 500U );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLICK -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
void vMEASUREMENTinit ( void )
{
  #if ( defined( FATSD ) && defined( MEASUREMENT ) )
    measurement.enb  = getBitMap( &recordSetup0, RECORD_ENB_ADR );
    sdRoutine.cmd    = SD_COMMAND_WRITE;
    sdRoutine.file   = FATSD_FILE_MEASUREMENT;
    if ( measurement.enb == PERMISSION_ENABLE )
    {
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
  #if ( defined( FATSD ) && defined( MEASUREMENT ) )
    MEASURMENT_CMD measurementCmd = cmd;
    xQueueSend( pMeasurementCommandQueue, &measurementCmd, portMAX_DELAY );
  #endif
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

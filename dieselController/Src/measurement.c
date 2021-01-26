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
/*----------------------- Structures ----------------------------------------------------------------*/
static MEASUREMENT_TYPE measurement              = { 0U };
static osThreadId_t     measurementHandle        = NULL;
static QueueHandle_t    pMeasurementCommandQueue = NULL;
static StaticQueue_t    xMeasurementCommandQueue = { 0U };
/*----------------------- Functions -----------------------------------------------------------------*/
fix16_t fMEASUREMENTgetNone ( void );
/*----------------------- Constant ------------------------------------------------------------------*/
static const getValueCallBack measurementCallbacks[MEASUREMENT_CHANNEL_NUMBER] =
{
  fMEASUREMENTgetNone,  /*  0 */
  fMEASUREMENTgetNone,  /*  1 */
  fMEASUREMENTgetNone,  /*  2 */
  fMEASUREMENTgetNone,  /*  3 */
  fMEASUREMENTgetNone,  /*  4 */
  fMEASUREMENTgetNone,  /*  5 */
  fMEASUREMENTgetNone,  /*  6 */
  fMEASUREMENTgetNone,  /*  7 */
  fMEASUREMENTgetNone,  /*  8 */
  fMEASUREMENTgetNone,  /*  9 */
  fMEASUREMENTgetNone,  /* 10 */
  fMEASUREMENTgetNone,  /* 11 */
  fMEASUREMENTgetNone,  /* 12 */
  fMEASUREMENTgetNone,  /* 13 */
  fMEASUREMENTgetNone   /* 14 */
};
static const eConfigReg* measurementRegs[MEASUREMENT_CHANNEL_NUMBER] = { 0U };
/*----------------------- Variables -----------------------------------------------------------------*/
static MEASURMENT_CMD measurementCommandBuffer[MEASUREMENT_COMMAND_QUEUE_LENGTH] = { 0U };
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PRIVATE -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
fix16_t fMEASUREMENTgetNone ( void )
{
  return 0U;
}
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLICK -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
void vMEASUREMENTinit ( void )
{
  uint8_t         i   = 0U;
  DATA_API_STATUS res = DATA_API_STAT_BUSY;
  if ( MEASUREMENT_ENB > 0U )
  {
    measurement.enb = getBitMap( &recordSetup, RECORD_ENB_ADR );
    if ( measurement.enb == PERMISSION_ENABLE )
    {
      while ( res != DATA_API_STAT_OK )
      {
        res = eDATAAPImeasurement( DATA_API_CMD_COUNTER, &measurement.counter, 0U, NULL );
        if ( res == DATA_API_STAT_BUSY )
        {
          osDelay( 1U );
        }
      }
      measurement.state       = MEASURMENT_STATE_START;
      measurement.timer.delay = getValue( &recordInterval );
      measurement.timer.id    = LOGIC_DEFAULT_TIMER_ID;
      for ( i=0U; i<MEASUREMENT_CHANNEL_NUMBER; i++ )
      {
        measurement.channels[i].enb = getBitMap( &recordSetup, ( RECORD_ENB_ADR + 1U + i ) );
        measurement.channels[i].get = measurementCallbacks[i];
        if ( measurement.channels[i].enb == PERMISSION_ENABLE )
        {
          measurement.length++;
        }
      }
      if ( measurement.length > 0U )
      {
        measurement.size = ( uint16_t )( STORAGE_MEASUREMENT_SIZE / ( measurement.length * 2U ) );
      }
      else
      {
        measurement.size = 0U;
      }

      /* TEST DATA */

      /* TEST DATA */

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
  }
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
void vMEASUREMENTtask ( void* argument )
{
  uint16_t        i                                = 0U;
  uint16_t        data[MEASUREMENT_CHANNEL_NUMBER] = { 0U };
  DATA_API_STATUS status                           = DATA_API_STAT_BUSY;
  MEASURMENT_CMD  inputCmd                         = MEASURMENT_CMD_NONE;
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
    switch( measurement.cmd )
    {
      case MEASURMENT_CMD_NONE:
        break;
      case MEASURMENT_CMD_RESET:
        vLOGICprintDebug( ">>Measurement     : Reset command\r\n" );
        status = DATA_API_STAT_BUSY;
        while ( status == DATA_API_STAT_BUSY )
        {
          status = eDATAAPImeasurement( DATA_API_CMD_ERASE, NULL, 0U, NULL );
          if ( status == DATA_API_STAT_BUSY )
          {
            osDelay( 1U );
          }
        }
        if ( status != DATA_API_STAT_OK )
        {
          vLOGICprintDebug( ">>Measurement     : Error!\r\n" );
          measurement.state = MEASURMENT_STATE_ERROR;
        }
        else
        {
          measurement.state   = MEASURMENT_STATE_START;
          measurement.counter = 0U;
          vLOGICresetTimer( measurement.timer );
        }
        measurement.cmd = MEASURMENT_CMD_NONE;
        break;
      case MEASURMENT_CMD_START:
        vLOGICprintDebug( ">>Measurement     : Start command\r\n" );
        if ( measurement.state == MEASURMENT_STATE_IDLE )
        {
          measurement.state = MEASURMENT_STATE_START;
        }
        measurement.cmd = MEASURMENT_CMD_NONE;
        break;
      case MEASURMENT_CMD_STOP:
        vLOGICprintDebug( ">>Measurement     : Stop command\r\n" );
        if ( ( measurement.state != MEASURMENT_STATE_IDLE ) && ( measurement.state != MEASURMENT_STATE_ERROR ) )
        {
          if ( measurement.state == MEASURMENT_STATE_WAIT )
          {
            vLOGICresetTimer( measurement.timer );
            measurement.state = MEASURMENT_STATE_IDLE;
          }
        }
        measurement.cmd = MEASURMENT_CMD_NONE;
        break;
      default:
        measurement.cmd = MEASURMENT_CMD_NONE;
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
          break;
        case MEASURMENT_STATE_START:
          vLOGICprintDebug( ">>Measurement     : Status start\r\n" );
          if ( measurement.counter < measurement.size )
          {
            vLOGICstartTimer( &measurement.timer );
            measurement.state = MEASURMENT_STATE_WAIT;
          }
          else
          {
            measurement.state = MEASURMENT_STATE_IDLE;
          }
          break;
        case MEASURMENT_STATE_WAIT:
          if ( uLOGICisTimer( measurement.timer ) > 0U )
          {
            measurement.state = MEASURMENT_STATE_WRITE;
          }
          break;
        case MEASURMENT_STATE_WRITE:
          if ( measurement.counter < measurement.size )
          {
            vLOGICprintDebug( ">>Measurement     : Read data\r\n" );
            for ( i=0U; i<MEASUREMENT_CHANNEL_NUMBER; i++ )
            {
              if ( measurement.channels[i].enb == PERMISSION_ENABLE )
              {
                data[i] = setValue( measurementRegs[i], measurement.channels[i].get() );
              }
            }
            status = DATA_API_STAT_BUSY;
            while ( status == DATA_API_STAT_BUSY )
            {
              status = eDATAAPImeasurement( DATA_API_CMD_ADD, &measurement.counter, measurement.length, data );
              if ( status == DATA_API_STAT_BUSY )
              {
                osDelay( 1U );
              }
            }
            if ( status != DATA_API_STAT_OK )
            {
              vLOGICprintDebug( ">>Measurement     : Error!\r\n" );
              measurement.state = MEASURMENT_STATE_ERROR;
            }
            else
            {
              vLOGICstartTimer( &measurement.timer );
              measurement.counter++;
              measurement.state = MEASURMENT_STATE_WAIT;
            }
          }
          else
          {
            vLOGICprintDebug( ">>Measurement     : Memory is full\r\n" );
            measurement.state = MEASURMENT_STATE_IDLE;
          }
          break;
        case MEASURMENT_STATE_ERROR:
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
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

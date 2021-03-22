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
  xADCGetSOP,               /*  0 Oil pressure               */
  xADCGetSCT,               /*  1 Coolant temperature        */
  xADCGetSFL,               /*  2 Fuel level                 */
  fENGINEgetSpeed,          /*  3 Speed                      */
  fFPIgetData,              /*  4 Digital inputs             */
  fFPOgetData,              /*  5 Digital outputs            */
  xADCGetGENL1,             /*  6 Generator phase voltage L1 */
  xADCGetGENL2,             /*  7 Generator phase voltage L2 */
  xADCGetGENL3,             /*  8 Generator phase voltage L3 */
  xADCGetGENL1Lin,          /*  9 Generator Line voltage L1  */
  xADCGetGENL2Lin,          /* 10 Generator Line voltage L2  */
  xADCGetGENL3Lin,          /* 11 Generator Line voltage L3  */
  xADCGetGENL1Cur,          /* 12 Current L1                 */
  xADCGetGENL2Cur,          /* 13 Current L2                 */
  xADCGetGENL3Cur,          /* 14 Current L3                 */
  xADCGetGENLFreq,          /* 15 Frequency generator        */
  xADCGetCOSFi,             /* 16 Cos Fi                     */
  xADCGetGENActivePower,    /* 17 Power active               */
  xADCGetGENReactivePower,  /* 18 Power reactive             */
  xADCGetGENRealPower,      /* 19 Power full                 */
  xADCGetNETL1,             /* 20 Mains phase voltage L1     */
  xADCGetNETL2,             /* 21 Mains phase voltage L2     */
  xADCGetNETL3,             /* 22 Mains phase voltage L3     */
  xADCGetNETL1Lin,          /* 23 Mains line voltage L1      */
  xADCGetNETL2Lin,          /* 24 Mains line voltage L2      */
  xADCGetNETL3Lin,          /* 25 Mains line voltage L3      */
  xADCGetNETLFreq,          /* 26 Mains frequency            */
  xADCGetVDD,               /* 27 Voltage accumulator        */
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
    measurement.enb = getBitMap( &recordSetup0, RECORD_ENB_ADR );
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
      measurement.state       = MEASURMENT_STATE_IDLE;
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
      DATA_API_STATUS status = DATA_API_STAT_BUSY;
      uint16_t        data   = 0U;
      for ( data=0; data<10; data++ )
      {
        status = DATA_API_STAT_BUSY;
        while ( status == DATA_API_STAT_BUSY )
        {
          status = eDATAAPImeasurement( DATA_API_CMD_ADD, &data, 1U, &data );
        }
      }
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
          vLOGICresetTimer( &measurement.timer );
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
            vLOGICresetTimer( &measurement.timer );
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
            vLOGICstartTimer( &measurement.timer, "Measurement timer   " );
            measurement.state = MEASURMENT_STATE_WAIT;
          }
          else
          {
            measurement.state = MEASURMENT_STATE_IDLE;
          }
          break;
        case MEASURMENT_STATE_WAIT:
          if ( uLOGICisTimer( &measurement.timer ) > 0U )
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
              vLOGICstartTimer( &measurement.timer, "Measurement timer   " );
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

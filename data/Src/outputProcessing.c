/*
 * outputProcessing.c
 *
 *  Created on: 22 мар. 2021 г.
 *      Author: 79110
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "outputProcessing.h"
#include "dataProces.h"
#include "config.h"
#include "dataProces.h"
#include "storage.h"
#include "engine.h"
#include "adc.h"
#include "fpo.h"
#include "fpi.h"
#include "controller.h"
/*----------------------- Structures ----------------------------------------------------------------*/
/*----------------------- Functions -----------------------------------------------------------------*/
static const getValueCallBack measurementCallbacks[MEASUREMENT_CHANNEL_NUMBER] =
{
  xADCGetSOP,               /*  0 Oil pressure               */
  xADCGetSCT,               /*  1 Coolant temperature        */
  xADCGetSFL,               /*  2 Fuel level                 */
  fENGINEgetSpeed,          /*  3 Speed                      */
  xADCGetNETL1,             /*  4 Mains phase voltage L1     */
  xADCGetNETL2,             /*  5 Mains phase voltage L2     */
  xADCGetNETL3,             /*  6 Mains phase voltage L3     */
  xADCGetNETL1Lin,          /*  7 Mains line voltage L1      */
  xADCGetNETL2Lin,          /*  8 Mains line voltage L2      */
  xADCGetNETL3Lin,          /*  9 Mains line voltage L3      */
  xADCGetNETLFreq,          /* 10 Mains frequency            */
  xADCGetGENL1,             /* 11 Generator phase voltage L1 */
  xADCGetGENL2,             /* 12 Generator phase voltage L2 */
  xADCGetGENL3,             /* 13 Generator phase voltage L3 */
  xADCGetGENL1Lin,          /* 14 Generator Line voltage L1  */
  xADCGetGENL2Lin,          /* 15 Generator Line voltage L2  */
  xADCGetGENL3Lin,          /* 16 Generator Line voltage L3  */
  xADCGetGENL1Cur,          /* 17 Current L1                 */
  xADCGetGENL2Cur,          /* 18 Current L2                 */
  xADCGetGENL3Cur,          /* 19 Current L3                 */
  xADCGetGENLFreq,          /* 20 Frequency generator        */
  xADCGetCOSFi,             /* 21 Cos Fi                     */
  xADCGetGENActivePower,    /* 22 Power active               */
  xADCGetGENReactivePower,  /* 23 Power reactive             */
  xADCGetGENRealPower,      /* 24 Power full                 */
  xADCGetVDD,               /* 25 Voltage accumulator        */
};
static const commandCallBack controllCallBacks[CONTROL_CMD_NUMBER] =
{
  vCONTROLLERswitchMode,    /* 0 switch mode  */
  vCONTRILLERsetStart,      /* 1 start        */
  vCONTROLLERsetStop,       /* 2 stop         */
  vCONTROLLERsetACK,        /* 3 reset errors */
  vCONTROLLERsetSwitchLoad, /* 4 switch mains */
};
/*-------------------------------- Variables ---------------------------------*/
static uint16_t controllBuffer = 0U;
/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/
uint16_t uOUTPUTcodeEvent ( SYSTEM_EVENT event )
{
  return ( uint16_t )( event.type ) || ( ( uint16_t )( event.action ) << 8U );
}
/*----------------------------------------------------------------------------*/
void vOUTPUTprocessControll ( void )
{
  uint16_t dif = 0U;
  uint8_t  i   = 0U;
  if ( outputDataReg[CONTROLL_ADR]->value[0U] != controllBuffer )
  {
    dif = controllBuffer ^ outputDataReg[CONTROLL_ADR]->value[0U];
    for ( i=0U; i<CONTROL_CMD_NUMBER; i++ )
    {
      if ( ( ( dif >> i ) & 0x01U ) > 0U )
      {
        controllCallBacks[i]();
      }
    }
    controllBuffer = outputDataReg[CONTROLL_ADR]->value[0U];
  }
  return;
}
/*----------------------------------------------------------------------------*/
void uOUTPUTupdateStatus ( void )
{
  uint16_t res = 0U;
  if ( eFPOgetAlarm() == TRIGGER_IDLE )
  {
    res |= outputDataReg[STATUS_ADR]->atrib->bitMap[STATUS_ALARM_ADR].mask;
  }
  if ( eFPOgetWarning() == TRIGGER_IDLE )
  {
    res |= outputDataReg[STATUS_ADR]->atrib->bitMap[STATUS_WARNING_ADR].mask;
  }
  if ( eFPOgetMainsFail() == TRIGGER_IDLE )
  {
    res |= outputDataReg[STATUS_ADR]->atrib->bitMap[STATUS_MAINS_FAIL_ADR].mask;
  }
  if ( eFPOgetDpsReady() == TRIGGER_IDLE )
  {
    res |= outputDataReg[STATUS_ADR]->atrib->bitMap[STATUS_D_P_SREADY_ADR].mask;
  }
  if ( eFPOgetReadyToStart() == TRIGGER_IDLE )
  {
    res |= outputDataReg[STATUS_ADR]->atrib->bitMap[STATUS_READY_TO_START_ADR].mask;
  }
  if ( eFPOgetGenReady() == TRIGGER_IDLE )
  {
    res |= outputDataReg[STATUS_ADR]->atrib->bitMap[STATUS_GENERATOR_REDY_ADR].mask;
  }
  outputDataReg[STATUS_ADR]->value[0U] = res;
  return;
}
/*----------------------------------------------------------------------------*/
void vOUTPUTupdateLogLength ( void )
{
  uint16_t length = 0U;
  if ( eDATAAPIlogPointer( DATA_API_CMD_READ_CASH, &length ) == DATA_API_STAT_OK )
  {
    outputDataReg[LOG_LEN_ADR]->value[0U] = length;
  }
  else
  {
    outputDataReg[LOG_LEN_ADR]->value[0U] = 0U;
  }
  return;
}
/*----------------------------------------------------------------------------*/
void vOUTPUTupdateLogRecord ( void )
{
  LOG_RECORD_TYPE record = { 0U };
  if ( eDATAAPIlog( DATA_API_CMD_READ,  outputDataReg[LOG_ADR_ADR]->value, &record ) == DATA_API_STAT_OK )
  {
    outputDataReg[LOG_RECORD_DATA0_ADR]->value[0U] = ( uint16_t )( record.time );
    outputDataReg[LOG_RECORD_DATA1_ADR]->value[0U] = ( uint16_t )( record.time >> 16U );
    outputDataReg[LOG_RECORD_EVENT_ADR]->value[0U] = uOUTPUTcodeEvent( record.event );
  }
  else
  {
    outputDataReg[LOG_RECORD_DATA0_ADR]->value[0U] = 0U;
    outputDataReg[LOG_RECORD_DATA1_ADR]->value[0U] = 0U;
    outputDataReg[LOG_RECORD_EVENT_ADR]->value[0U] = 0U;
  }
  return;
}
/*----------------------------------------------------------------------------*/
void vOUTPUTupdateErrorLength ( void )
{
  uint8_t length = 0U;
  eLOGICERactiveErrorList( ERROR_LIST_CMD_COUNTER, NULL, &length );
  outputDataReg[ERROR_LEN_ADR]->value[0U] = ( uint16_t )( length );
  return;
}
/*----------------------------------------------------------------------------*/
void vOUTPUTupdateErrorRecord ( void )
{
  LOG_RECORD_TYPE record = { 0U };
  uint8_t         adr    = outputDataReg[ERROR_ADR_ADR]->value[0U];
  eLOGICERactiveErrorList( ERROR_LIST_CMD_READ, &record, &adr );
  outputDataReg[ERROR_RECORD_DATA0_ADR]->value[0U] = ( uint16_t )( record.time );
  outputDataReg[ERROR_RECORD_DATA1_ADR]->value[0U] = ( uint16_t )( record.time >> 16U );
  outputDataReg[ERROR_RECORD_EVENT_ADR]->value[0U] = uOUTPUTcodeEvent( record.event );
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
OUTPUT_REGISTER_TYPE eOUTPUTgetType ( uint8_t number )
{
  OUTPUT_REGISTER_TYPE res = OUTPUT_REGISTER_TYPE_RESERVED;
  if ( number <  CONTROL_READ_ONLY_NUMBER )
  {
    res = OUTPUT_REGISTER_TYPE_READ_ONLY;
  }
  else
  {
    res = OUTPUT_REGISTER_TYPE_READ_WRITE;
  }
  return res;
}
/*----------------------------------------------------------------------------*/
void vOUTPUTupdate ( uint8_t chanel )
{
  if ( chanel < MEASUREMENT_CHANNEL_NUMBER )
  {
    outputDataReg[chanel]->value[0U] = setValue( outputDataReg[chanel], measurementCallbacks[chanel]() );
  }
  else
  {
    switch ( chanel )
    {
      case DIGITAL_OUTPUT_ADR:
        outputDataReg[chanel]->value[0U] = uFPOgetData();
        break;
      case DIGITAL_INPUT_ADR:
        outputDataReg[chanel]->value[0U] = uFPIgetData();
        break;
      case STATUS_ADR:
        uOUTPUTupdateStatus();
        break;
      case CONTROLL_ADR:
        vOUTPUTprocessControll();
        break;
      case LOG_LEN_ADR:
        vOUTPUTupdateLogLength();
        break;
      case LOG_ADR_ADR:
        vOUTPUTupdateLogRecord();
        break;
      case ERROR_LEN_ADR:
        vOUTPUTupdateErrorLength();
        break;
      case ERROR_ADR_ADR:
        vOUTPUTupdateError();
        break;
      default:
        break;
    }
  }
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


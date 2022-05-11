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
#include "statistics.h"
/*----------------------- Structures ----------------------------------------------------------------*/
static SemaphoreHandle_t xWriteOutputSemaphore = NULL;
/*----------------------- Functions -----------------------------------------------------------------*/
static const getValueCallBack measurementCallbacks[MEASUREMENT_CHANNEL_NUMBER] =
{
  fENGINEgetOilPressure,    /*  0 Oil pressure               */
  fENGINEgetCoolantTemp,    /*  1 Coolant temperature        */
  fENGINEgetFuelLevel,      /*  2 Fuel level                 */
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
  xADCGetCAC,               /* 26 Voltage charger            */
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
  return ( uint16_t )( event.type ) | ( ( uint16_t )( event.action ) << 8U );
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
void vOUTPUTupdateExternDevice ( void )
{
  uint16_t res = 0U;

  if ( eENGINEgetChargerState() == RELAY_ON )
  {
    res |= outputDataReg[EXTERNAL_DEVICES_ADR]->atrib->bitMap[EXTERNAL_CHARGER_ADR].mask;
  }
  if ( eFPOgetHeater()    == TRIGGER_SET )
  {
    res |= outputDataReg[EXTERNAL_DEVICES_ADR]->atrib->bitMap[EXTERNAL_COOLANT_HEATER_ADR].mask;
  }
  if ( eFPOgetCooler()    == TRIGGER_SET )
  {
    res |= outputDataReg[EXTERNAL_DEVICES_ADR]->atrib->bitMap[EXTERNAL_COOLANT_COOLER_ADR].mask;
  }
  if ( eFPOgetBooster()   == TRIGGER_SET )
  {
    res |= outputDataReg[EXTERNAL_DEVICES_ADR]->atrib->bitMap[EXTERNAL_FUEL_PUMP_ADR].mask;
  }
  if ( eFPOgetPreheater() == TRIGGER_SET )
  {
    res |= outputDataReg[EXTERNAL_DEVICES_ADR]->atrib->bitMap[EXTERNAL_PRE_HEATER_ADR].mask;
  }
  outputDataReg[EXTERNAL_DEVICES_ADR]->value[0U] = res;
  return;
}
/*----------------------------------------------------------------------------*/
void vOUTPUTupdateStatus ( void )
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
  if ( eCONTROLLERgetMode() == CONTROLLER_MODE_AUTO )
  {
    res |= outputDataReg[STATUS_ADR]->atrib->bitMap[STATUS_AUTO_MODE_ADR].mask;
  }
  outputDataReg[STATUS_ADR]->value[0U] = res;
  return;
}
/*----------------------------------------------------------------------------*/
void vOUTPUTupdateDeviceStatus ( void )
{
  DEVICE_INFO info = { 0U };
  vSTATUSget( &info );
  outputDataReg[DEVICE_STATUS_ADR]->value[0U] = ( uint16_t )( info.status ) | ( ( info.time << OUTPUT_TIME_SHIFT ) & OUTPUT_TIME_MASK );
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vOUTPUTinit ( void )
{
  uint8_t i = 0U;
  xWriteOutputSemaphore = xSemaphoreCreateMutex();
  for ( i=0U; i<OUTPUT_DATA_REGISTER_NUMBER; i++ )
  {
    vOUTPUTupdate( i );
  }
  return;
}
/*----------------------------------------------------------------------------*/
OUTPUT_REGISTER_TYPE eOUTPUTgetType ( uint8_t chanel )
{
  OUTPUT_REGISTER_TYPE res = OUTPUT_REGISTER_TYPE_READ_ONLY;
  if ( chanel == CONTROLL_ADR )
  {
    res = OUTPUT_REGISTER_TYPE_READ_WRITE;
  }
  return res;
}
/*----------------------------------------------------------------------------*/
uint16_t uOUTPUTread ( uint8_t chanel )
{
  return outputDataReg[chanel]->value[0U];
}
/*----------------------------------------------------------------------------*/
OUTPUT_STATUS eOUTPUTwrite ( uint8_t chanel, uint16_t data )
{
  OUTPUT_STATUS res = OUTPUT_STATUS_OK;
  if ( eOUTPUTgetType( chanel ) == OUTPUT_REGISTER_TYPE_READ_WRITE )
  {
    if ( xWriteOutputSemaphore != NULL )
    {
      if ( xSemaphoreTake( xWriteOutputSemaphore, OUTPUT_SEMAPHORE_TAKE_DELAY ) == pdTRUE )
      {
        outputDataReg[chanel]->value[0U] = data;
        xSemaphoreGive( xWriteOutputSemaphore );
      }
      else
      {
        res = OUTPUT_STATUS_BUSY;
      }
    }
    else
    {
      res = OUTPUT_STATUS_INIT_ERROR;
    }
  }
  else
  {
    res = OUTPUT_STATUS_ACCESS_DENIED;
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
      case ENGINE_WORK_TIME_OUT_ADR:
        outputDataReg[chanel]->value[0U] = *freeDataArray[ENGINE_WORK_TIME_ADR];
        break;
      case ENGINE_WORK_MIN_OUT_ADR:
        outputDataReg[chanel]->value[0U] = *freeDataArray[ENGINE_WORK_MINUTES_ADR];
        break;
      case ENGINE_STARTS_NUMBER_OUT_ADR:
        outputDataReg[chanel]->value[0U] = *freeDataArray[ENGINE_STARTS_NUMBER_ADR];
        break;
      case POWER_REACTIVE_USAGE_OUT_ADR:
        outputDataReg[chanel]->value[0U] = *freeDataArray[POWER_REACTIVE_USAGE_ADR];
        break;
      case POWER_ACTIVE_USAGE_OUT_ADR:
        outputDataReg[chanel]->value[0U] = *freeDataArray[POWER_ACTIVE_USAGE_ADR];
        break;
      case POWER_FULL_USAGE_OUT_ADR:
        outputDataReg[chanel]->value[0U] = *freeDataArray[POWER_FULL_USAGE_ADR];
        break;
      case FUEL_USAGE_OUT_ADR:
        outputDataReg[chanel]->value[0U] = *freeDataArray[FUEL_USAGE_ADR];
        break;
      case FUEL_MOMENTAL_RATE_OUT_ADR:
        outputDataReg[chanel]->value[0U] = uSTATISTICSgetFuelMomentalRate();
        break;
      case FUEL_AVERAGE_RATE_OUT_ADR:
        outputDataReg[chanel]->value[0U] = *freeDataArray[FUEL_RATE_ADR];
        break;
      case EXTERNAL_DEVICES_ADR:
        vOUTPUTupdateExternDevice();
        break;
      case DIGITAL_OUTPUT_ADR:
        outputDataReg[chanel]->value[0U] = uFPOgetData();
        break;
      case DIGITAL_INPUT_ADR:
        outputDataReg[chanel]->value[0U] = uFPIgetData();
        break;
      case DEVICE_STATUS_ADR:
        vOUTPUTupdateDeviceStatus();
        break;
      case STATUS_ADR:
        vOUTPUTupdateStatus();
        break;
      case ERROR0_ADR:
        /* no need to update */
        break;
      case ERROR1_ADR:
        /* no need to update */
        break;
      case ERROR2_ADR:
        /* no need to update */
        break;
      case WARNING0_ADR:
        /* no need to update */
        break;
      case WARNING1_ADR:
        /* no need to update */
        break;
      case CONTROLL_ADR:
        vOUTPUTprocessControll();
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


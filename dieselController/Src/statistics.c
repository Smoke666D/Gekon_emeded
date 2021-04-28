/*
 * statistics.c
 *
 *  Created on: 26 апр. 2021 г.
 *      Author: 79110
 */
/*--------------------------------- Includes ---------------------------------*/
#include "statistics.h"
#include "common.h"
#include "dataAPI.h"
#include "dataProces.h"
#include "electro.h"
#include "engine.h"
#include "adc.h"
/*-------------------------------- Structures --------------------------------*/
static STATISTICS_TYPE     statistics = { 0U };
static MAINTENCE_TYPE      maintence  = { 0U };
static FUEL_STATISTIC_TYPE fuel       = { 0U };
/*---------------------------------- MACROS ----------------------------------*/

/*--------------------------------- Constant ---------------------------------*/
static const fix16_t fix60  = F16( 5U );  /* --- */
static const fix16_t fix100 = F16( 100U );
/*-------------------------------- Variables ---------------------------------*/
static uint8_t minCounter = 0U;
static uint8_t startFl    = 0U;
/*-------------------------------- External ----------------------------------*/
/*-------------------------------- Functions ---------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vMAINTENCEinc ( MAINTENCE_VALUE* value, uint8_t adr )
{
  if ( ( value->alarm.error.enb    == PERMISSION_ENABLE ) &&
       ( value->alarm.error.active == PERMISSION_ENABLE ) &&
       ( value->alarm.error.status == ALARM_STATUS_IDLE ) )
  {
    eDATAAPIfreeData( DATA_API_CMD_INC, adr, &value->data );
  }
  return;
}
/*----------------------------------------------------------------------------*/
uint8_t uSTATISTICSelectroCalc ( fix16_t timeout )
{
  uint8_t  res      = 0U;
  uint16_t reactive = 0U;  /* kWh */
  uint16_t active   = 0U;  /* kWh */
  uint16_t full     = 0U;  /* kWh */
  uint16_t add      = 0U;  /* kWh */
  if ( eELECTROgetGeneratorStatus() == ELECTRO_STATUS_LOAD )
  {
    eDATAAPIfreeData( DATA_API_CMD_READ, POWER_REACTIVE_USAGE_ADR, &reactive );
    eDATAAPIfreeData( DATA_API_CMD_READ, POWER_ACTIVE_USAGE_ADR,   &active   );
    eDATAAPIfreeData( DATA_API_CMD_READ, POWER_FULL_USAGE_ADR,     &full     );
    add = fix16_from_int( fELECTROpowerToKWH( xADCGetGENReactivePower(), timeout ) );
    if ( add > 0U )
    {
      reactive += add;
      eDATAAPIfreeData( DATA_API_CMD_WRITE, POWER_REACTIVE_USAGE_ADR, &reactive );
    }
    add = fix16_from_int( fELECTROpowerToKWH( xADCGetGENActivePower(), timeout ) );
    if ( add > 0U )
    {
      active += add;
      eDATAAPIfreeData( DATA_API_CMD_WRITE, POWER_ACTIVE_USAGE_ADR, &active );
    }
    add = fix16_from_int( fELECTROpowerToKWH( xADCGetGENRealPower(), timeout ) );
    if ( add > 0U )
    {
      full  += add;
      eDATAAPIfreeData( DATA_API_CMD_WRITE, POWER_FULL_USAGE_ADR, &full );
    }
    res = 1U;
  }
  return res;
}
/*----------------------------------------------------------------------------*/
void vFUELinc ( void )
{
  if ( eELECTROgetGeneratorStatus() == ELECTRO_STATUS_LOAD )
  {
    fuel.rate.power = fix16_add( fELECTROgetPower(), fuel.rate.power );
  }
  return;
}
/*----------------------------------------------------------------------------*/
void fArithmeticMean ( fix16_t* acc, fix16_t data, uint16_t* size )
{
  if ( *size == 0U )
  {
    *acc = data;
  }
  else if ( *size == 1U )
  {
    *acc = fix16_div( fix16_add( *acc, data ), fix16_from_int( 2U ) );
  }
  else
  {
    *acc = fix16_add( fix16_mul( fix16_div( *acc, fix16_from_int( *size + 1U ) ), *size ), fix16_div( data, fix16_from_int( *size + 1U ) ) );
  }
  if ( *size >= 0xFFFEU )
  {
    *size = 1U;
  }
  else
  {
    *size += 1U;
  }
  return;
}
/*----------------------------------------------------------------------------*/
uint8_t uFUELrateCalc ( void )
{
  float temp = 0;
  uint8_t  res   = 0U;
  fix16_t  data  = fENGINEgetFuelLevel();
  fix16_t  delta = fix16_sub( fuel.rate.fuel, data );
  uint16_t usage = 0U;
  if ( delta > fuel.rate.cutout )
  {
    res        = 1U;
    temp = fix16_to_float( delta );
    delta      = fix16_mul( fix16_div( delta, fix100 ), fuel.tankSize );
    temp = fix16_to_float( delta );
    fuel.usage = fix16_add( fuel.usage, delta );
    usage      = ( uint16_t )( fix16_to_int( fuel.usage ) );
    temp = fix16_to_float( fuel.usage );
    eDATAAPIfreeData( DATA_API_CMD_WRITE,  FUEL_USAGE_ADR, &usage );
    if ( fuel.rate.power == 0U )
    {
      vERRORcheck( &fuel.stopLeakError, 1U );
      vALARMcheck( &fuel.idleLeakAlarm, delta );
    }
    else
    {
      fuel.rate.momental = fix16_div( delta, fuel.rate.power );
      vALARMcheck( &fuel.leakAlarm, fuel.rate.momental );
      fArithmeticMean( &fuel.rate.average, fuel.rate.momental, &fuel.rate.size );
    }
  }
  fuel.rate.power = 0U;
  fuel.rate.fuel  = data;
  return res;
}
/*----------------------------------------------------------------------------*/
/*----------------------- PUBLIC ---------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vSTATISTICSinit ( void )
{
  if ( ( getBitMap( &fuelLevelSetup, FUEL_LEVEL_SENSOR_TYPE_ADR ) == SENSOR_TYPE_RESISTIVE ) ||
       ( getBitMap( &fuelLevelSetup, FUEL_LEVEL_SENSOR_TYPE_ADR ) == SENSOR_TYPE_CURRENT ) )
  {
    fuel.stopLeakError.enb       = getBitMap( &fuelLevelSetup, FUEL_LEAK_ALARM_ENB_ADR );
    fuel.idleLeakAlarm.error.enb = getBitMap( &fuelLevelSetup, FUEL_LEAK_ALARM_ENB_ADR );
    fuel.leakAlarm.error.enb     = getBitMap( &fuelLevelSetup, FUEL_LEAK_ALARM_ENB_ADR );
  }
  else
  {
    fuel.stopLeakError.enb       = PERMISSION_DISABLE;
    fuel.idleLeakAlarm.error.enb = PERMISSION_DISABLE;
    fuel.leakAlarm.error.enb     = PERMISSION_DISABLE;
  }
  fuel.usage                            = fix16_from_int( *freeDataArray[FUEL_USAGE_ADR] );
  fuel.rate.size                        = *freeDataArray[FUEL_AVERAGE_SIZE_ADR];
  fuel.rate.average                     = fix16_from_int( *freeDataArray[FUEL_RATE_ADR] );
  fuel.tankSize                         = getValue( &fuelTankLevel );
  fuel.rate.momental                    = 0U;
  fuel.rate.cutout                      = fix16_mul( fix16_div( getValue( &hysteresisLevel ), fix100U ), yAxisAtribs[FUEL_CHART_ADR]->max );
  fuel.stopLeakError.active             = PERMISSION_ENABLE;
  fuel.stopLeakError.event.type         = EVENT_FUEL_LEAK;
  fuel.stopLeakError.event.action       = ACTION_WARNING;
  fuel.stopLeakError.ack                = PERMISSION_DISABLE;
  fuel.stopLeakError.trig               = TRIGGER_IDLE;
  fuel.stopLeakError.status             = ALARM_STATUS_IDLE;
  fuel.idleLeakAlarm.error.active       = PERMISSION_DISABLE;
  fuel.idleLeakAlarm.type               = ALARM_LEVEL_HIGHT;
  fuel.idleLeakAlarm.level              = getValue( &fuelRateIdleLevel );
  fuel.idleLeakAlarm.timer.delay        = 0U;
  fuel.idleLeakAlarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  fuel.idleLeakAlarm.error.event.type   = EVENT_FUEL_LEAK;
  fuel.idleLeakAlarm.error.event.action = ACTION_WARNING;
  fuel.idleLeakAlarm.error.ack          = PERMISSION_DISABLE;
  fuel.idleLeakAlarm.error.trig         = TRIGGER_IDLE;
  fuel.idleLeakAlarm.error.status       = ALARM_STATUS_IDLE;
  fuel.leakAlarm.error.active           = PERMISSION_ENABLE;
  fuel.leakAlarm.type                   = ALARM_LEVEL_HIGHT;
  fuel.leakAlarm.level                  = getValue( &fuelRateLevel );
  fuel.leakAlarm.timer.delay            = 0U;
  fuel.leakAlarm.timer.id               = LOGIC_DEFAULT_TIMER_ID;
  fuel.leakAlarm.error.event.type       = EVENT_FUEL_LEAK;
  fuel.leakAlarm.error.event.action     = ACTION_WARNING;
  fuel.leakAlarm.error.ack              = PERMISSION_DISABLE;
  fuel.leakAlarm.error.trig             = TRIGGER_IDLE;
  fuel.leakAlarm.error.status           = ALARM_STATUS_IDLE;
  statistics.timer.delay                = fix60;
  statistics.timer.id                   = LOGIC_DEFAULT_TIMER_ID;
  statistics.status                     = STATISTICS_STATUS_STOP;
  maintence.oil.data                    = *freeDataArray[MAINTENANCE_ALARM_OIL_TIME_LEFT_ADR];
  maintence.oil.alarm.error.enb         = getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_OIL_ENB_ADR );
  maintence.oil.alarm.error.active      = PERMISSION_ENABLE;
  maintence.oil.alarm.type              = ALARM_LEVEL_HIGHT;
  maintence.oil.alarm.level             = getValue( &maintenanceAlarmOilTime );
  maintence.oil.alarm.timer.delay       = 0U;
  maintence.oil.alarm.timer.id          = LOGIC_DEFAULT_TIMER_ID;
  maintence.oil.alarm.error.event.type  = EVENT_MAINTENANCE_OIL;
  if ( getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_OIL_ACTION_ADR ) == 0U )
  {
    maintence.oil.alarm.error.event.action = ACTION_BAN_START;
  }
  else
  {
    maintence.oil.alarm.error.event.action = ACTION_WARNING;
  }
  maintence.oil.alarm.error.ack        = PERMISSION_ENABLE;
  maintence.oil.alarm.error.trig       = TRIGGER_IDLE;
  maintence.oil.alarm.error.status     = ALARM_STATUS_IDLE;
  maintence.air.data                   = *freeDataArray[MAINTENANCE_ALARM_AIR_TIME_LEFT_ADR];
  maintence.air.alarm.error.enb        = getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_AIR_ENB_ADR );
  maintence.air.alarm.error.active     = PERMISSION_ENABLE;
  maintence.air.alarm.type             = ALARM_LEVEL_HIGHT;
  maintence.air.alarm.level            = getValue( &maintenanceAlarmAirTime );
  maintence.air.alarm.timer.delay      = 0U;
  maintence.air.alarm.timer.id         = LOGIC_DEFAULT_TIMER_ID;
  maintence.air.alarm.error.event.type = EVENT_MAINTENANCE_AIR;
  if ( getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_AIR_ACTION_ADR ) == 0U )
  {
    maintence.air.alarm.error.event.action = ACTION_BAN_START;
  }
  else
  {
    maintence.air.alarm.error.event.action = ACTION_WARNING;
  }
  maintence.air.alarm.error.ack         = PERMISSION_ENABLE;
  maintence.air.alarm.error.trig        = TRIGGER_IDLE;
  maintence.air.alarm.error.status      = ALARM_STATUS_IDLE;
  maintence.fuel.data                   = *freeDataArray[MAINTENANCE_ALARM_FUEL_TIME_LEFT_ADR];
  maintence.fuel.alarm.error.enb        = getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_FUEL_ENB_ADR );
  maintence.fuel.alarm.error.active     = PERMISSION_ENABLE;
  maintence.fuel.alarm.type             = ALARM_LEVEL_HIGHT;
  maintence.fuel.alarm.level            = getValue( &maintenanceAlarmFuelTime );
  maintence.fuel.alarm.timer.delay      = 0U;
  maintence.fuel.alarm.timer.id         = LOGIC_DEFAULT_TIMER_ID;
  maintence.fuel.alarm.error.event.type = EVENT_MAINTENANCE_FUEL;
  if ( getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_FUEL_ACTION_ADR ) == 0U )
  {
    maintence.fuel.alarm.error.event.action = ACTION_BAN_START;
  }
  else
  {
    maintence.fuel.alarm.error.event.action = ACTION_WARNING;
  }
  maintence.fuel.alarm.error.ack    = PERMISSION_ENABLE;
  maintence.fuel.alarm.error.trig   = TRIGGER_IDLE;
  maintence.fuel.alarm.error.status = ALARM_STATUS_IDLE;
  return;
}
/*----------------------------------------------------------------------------*/
void vSTATISTICSreset ( void )
{
  vERRORreset( &fuel.stopLeakError   );
  vALARMreset( &fuel.idleLeakAlarm   );
  vALARMreset( &fuel.leakAlarm       );
  vALARMreset( &maintence.oil.alarm  );
  vALARMreset( &maintence.air.alarm  );
  vALARMreset( &maintence.fuel.alarm );
  statistics.status               = STATISTICS_STATUS_STOP;
  fuel.stopLeakError.active       = PERMISSION_ENABLE;
  fuel.idleLeakAlarm.error.active = PERMISSION_DISABLE;
  vLOGICresetTimer( &statistics.timer );
  return;
}
/*----------------------------------------------------------------------------*/
void vSTATISTICSprocessing ( void )
{
  uint16_t      data   = 0U;
  ENGINE_STATUS status = eENGINEgetEngineStatus();
  uint8_t       update = 0U;

  vALARMcheck( &maintence.oil.alarm,  fix16_from_int( maintence.oil.data  ) );
  vALARMcheck( &maintence.air.alarm,  fix16_from_int( maintence.air.data  ) );
  vALARMcheck( &maintence.fuel.alarm, fix16_from_int( maintence.fuel.data ) );

  if ( ( status == ENGINE_STATUS_WORK ) && ( startFl == 0U ) )
  {
    fuel.stopLeakError.active       = PERMISSION_ENABLE;
    fuel.idleLeakAlarm.error.active = PERMISSION_DISABLE;
    eDATAAPIfreeData( DATA_API_CMD_READ,  ENGINE_WORK_MINUTES_ADR, &data );
    minCounter = ( uint8_t )data;
    startFl    = 1U;
  }
  else if ( startFl > 0U )
  {
    fuel.stopLeakError.active       = PERMISSION_DISABLE;
    fuel.idleLeakAlarm.error.active = PERMISSION_ENABLE;
    startFl = 0U;
  }
  else
  {

  }

  switch ( statistics.status )
  {
    case STATISTICS_STATUS_STOP:
      if ( eENGINEgetTurnout() == TRIGGER_SET )
      {
        fuel.rate.power = 0U;
        fuel.rate.fuel  = fENGINEgetFuelLevel();
        vLOGICstartTimer( &statistics.timer, STATISTICS_TIMER_STR );
        statistics.status = STATISTICS_STATUS_RUN;
      }
      break;
    case STATISTICS_STATUS_RUN:
      if ( uLOGICisTimer( &statistics.timer ) > 0U )
      {
        minCounter++;
        if ( minCounter >= 60U )
        {
          minCounter = 0U;
          statistics.status = STATISTICS_STATUS_HOUR_CHECK;
        }
        else
        {
          statistics.status = STATISTICS_STATUS_MINUTES_CHECK;
        }
      }
      else
      {
        vFUELinc();
      }
      break;
    case STATISTICS_STATUS_MINUTES_CHECK:
      update = 0U;
      if ( status == ENGINE_STATUS_WORK )
      {
        update++;
        eDATAAPIfreeData( DATA_API_CMD_INC,  ENGINE_WORK_MINUTES_ADR, &data );
        update += uSTATISTICSelectroCalc( statistics.timer.delay );
      }
      update += uFUELrateCalc();
      if ( update > 0U )
      {
        eDATAAPIfreeData( DATA_API_CMD_SAVE, 0U, NULL );
      }
      vLOGICstartTimer( &statistics.timer, STATISTICS_TIMER_STR );
      statistics.status = STATISTICS_STATUS_RUN;
      break;
    case STATISTICS_STATUS_HOUR_CHECK:
      update = 0U;
      if ( status == ENGINE_STATUS_WORK )
      {
        update++;
        eDATAAPIfreeData( DATA_API_CMD_ERASE, ENGINE_WORK_MINUTES_ADR, NULL );
        eDATAAPIfreeData( DATA_API_CMD_INC,   ENGINE_WORK_TIME_ADR,    &data );
        vMAINTENCEinc( &maintence.oil,  MAINTENANCE_ALARM_OIL_TIME_LEFT_ADR );
        vMAINTENCEinc( &maintence.air,  MAINTENANCE_ALARM_AIR_TIME_LEFT_ADR );
        vMAINTENCEinc( &maintence.fuel, MAINTENANCE_ALARM_FUEL_TIME_LEFT_ADR );
      }
      update += uFUELrateCalc();
      if ( update > 0U )
      {
        eDATAAPIfreeData( DATA_API_CMD_SAVE, 0U, NULL );
      }
      vLOGICstartTimer( &statistics.timer, STATISTICS_TIMER_STR );
      statistics.status = STATISTICS_STATUS_RUN;
      break;
    default:
      statistics.status = STATISTICS_STATUS_STOP;
      vLOGICresetTimer( &statistics.timer );
      return;
  }
  return;
}

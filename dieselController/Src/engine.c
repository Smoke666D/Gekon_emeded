/*
 * oil.c
 *
 *  Created on: 15 июн. 2020 г.
 *      Author: photo_Mickey
 */
/*--------------------------------- Includes ---------------------------------*/
#include "engine.h"
#include "config.h"
#include "chart.h"
#include "dataProces.h"
#include "vrSensor.h"
#include "electro.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "dataAPI.h"
#include "adc.h"
/*-------------------------------- Structures --------------------------------*/
static ENGINE_TYPE           engine              = { 0U };
static OIL_TYPE              oil                 = { 0U };
static COOLANT_TYPE          coolant             = { 0U };
static FUEL_TYPE             fuel                = { 0U };
static SPEED_TYPE            speed               = { 0U };
static BATTERY_TYPE          battery             = { 0U };
static CHARGER_TYPE          charger             = { 0U };
static STARTER_TYPE          starter             = { 0U };
static PLAN_STOP_TYPE        planStop            = { 0U };
static MAINTENCE_TYPE        maintence           = { 0U };
static RELAY_DEVICE          stopSolenoid        = { 0U };
static RELAY_DEVICE          idleRelay           = { 0U };
static RELAY_IMPULSE_DEVICE  preHeater           = { 0U };
static osThreadId_t          engineHandle        = NULL;
static StaticQueue_t         xEngineCommandQueue = { 0U };
static QueueHandle_t         pEngineCommandQueue = NULL;
/*--------------------------------- Constant ---------------------------------*/
/*-------------------------------- Variables ---------------------------------*/
static ENGINE_COMMAND engineCommandBuffer[ENGINE_COMMAND_QUEUE_LENGTH] = { 0U };
static uint8_t        starterFinish                                    = 0U;
static uint8_t        blockTimerFinish                                 = 0U;
static uint8_t        maintenanceReset                                 = 0U;
/*-------------------------------- Functions ---------------------------------*/
void vENGINEtask ( void const* argument );
/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/
fix16_t getBatteryVoltage ( void )
{
  fix16_t res = 0U;
  return res;
}

fix16_t getChargerVoltage ( void )
{
  fix16_t res = 0U;
  return res;
}

void vSENSORprocess ( SENSOR* sensor, fix16_t* value, QueueHandle_t queue )
{
  eFunctionError funcStat = SENSOR_STATUS_NORMAL;
  if ( ( sensor->type == SENSOR_TYPE_RESISTIVE ) || ( sensor->type == SENSOR_TYPE_CURRENT ) )
  {
    funcStat = eCHARTfunc( sensor->chart, sensor->get(), value );
    switch ( funcStat )
    {
      case FUNC_OK:
        sensor->status = SENSOR_STATUS_NORMAL;
        break;
      case FUNC_OVER_MAX_X_ERROR:
        sensor->status = SENSOR_STATUS_OPEN_CIRCUIT_ERROR;
        xQueueSend( queue, &sensor->cutout.event, portMAX_DELAY );
        break;
      case FUNC_OVER_MIN_X_ERROR:
        sensor->status = SENSOR_STATUS_ERROR;
        break;
      case FUNC_SIZE_ERROR:
        sensor->status = SENSOR_STATUS_ERROR;
        break;
      default:
        sensor->status = SENSOR_STATUS_ERROR;
        break;
    }
  }
  else
  {
    *value         = sensor->get();
    sensor->status = SENSOR_STATUS_NORMAL;
  }
  return;
}

fix16_t fOILprocess ( void )
{
  fix16_t value = 0U;
  vSENSORprocess( &oil.pressure, &value, pLOGICgetEventQueue() );
  vALARMcheck( &oil.alarm, value, pLOGICgetEventQueue() );
  if ( oil.alarm.status == ALARM_STATUS_IDLE )
  {
    vALARMcheck( &oil.preAlarm, value, pLOGICgetEventQueue() );
  }
  return value;
}

fix16_t fCOOLANTprocess ( void )
{
  fix16_t value = 0U;
  vSENSORprocess( &coolant.temp, &value, pLOGICgetEventQueue() );
  vALARMcheck( &coolant.alarm, value, pLOGICgetEventQueue() );
  if ( coolant.alarm.status == ALARM_STATUS_IDLE )
  {
    vALARMcheck( &coolant.preAlarm, value, pLOGICgetEventQueue() );
  }
  vRELAYproces( &coolant.heater, value );
  vRELAYproces( &coolant.cooler, value );
  return value;
}

fix16_t fFUELprocess ( void )
{
  fix16_t value = 0U;
  vSENSORprocess( &fuel.level, &value, pLOGICgetEventQueue() );
  vALARMcheck( &fuel.lowAlarm, value, pLOGICgetEventQueue() );
  if ( fuel.lowAlarm.status == ALARM_STATUS_IDLE )
  {
    vALARMcheck( &fuel.lowPreAlarm, value, pLOGICgetEventQueue() );
    if ( fuel.lowPreAlarm.status == ALARM_STATUS_IDLE )
    {
      vALARMcheck( &fuel.hightPreAlarm, value, pLOGICgetEventQueue() );
      if ( fuel.hightPreAlarm.status == ALARM_STATUS_IDLE )
      {
        vALARMcheck( &fuel.hightAlarm, value, pLOGICgetEventQueue() );
      }
    }
  }
  vRELAYproces( &fuel.booster, value );
  return value;
}

fix16_t fSPEEDprocess ( void )
{
  fix16_t value = speed.get();
  vALARMcheck( &speed.hightAlarm, value, pLOGICgetEventQueue() );
  if ( speed.hightAlarm.status == ALARM_STATUS_IDLE )
  {
    vALARMcheck( &speed.lowAlarm, value, pLOGICgetEventQueue() );
  }
  return value;
}

fix16_t fBATTERYprocess ( void )
{
  fix16_t value = battery.get();
  vALARMcheck( &battery.hightAlarm, value, pLOGICgetEventQueue() );
  if ( battery.hightAlarm.status == ALARM_STATUS_IDLE )
  {
    vALARMcheck( &battery.lowAlarm, value, pLOGICgetEventQueue() );
  }
  return value;
}

fix16_t fCHARGERprocess ( void )
{
  fix16_t value = charger.get();
  vALARMcheck( &charger.hightPreAlarm, value, pLOGICgetEventQueue() );
  if ( charger.hightAlarm.status == ALARM_STATUS_IDLE )
  {
    vALARMcheck( &charger.hightPreAlarm, value, pLOGICgetEventQueue() );
  }
  return value;
}

void vENGINEmileageProcess ( uint8_t* reset )
{
  static timerID_t timerID = LOGIC_COUNTERS_SIZE + 1U;
  static uint8_t   stat    = 0U;
  uint16_t         data    = 0U;
  if ( *reset > 0U )
  {
    eDATAAPIfreeData( DATA_API_CMD_WRITE, maintenanceAlarmOilTimeLeft, &data );
    eDATAAPIfreeData( DATA_API_CMD_WRITE, maintenanceAlarmAirTimeLeft, &data );
    eDATAAPIfreeData( DATA_API_CMD_WRITE, maintenanceAlarmFuelTimeLeft, &data );
    eDATAAPIfreeData( DATA_API_CMD_SAVE, 0U, NULL );
    *reset = 0U;
  }
  if ( engine.status == ENGINE_STATUS_WORK )
  {
    if ( stat == 0U )
    {
      vLOGICstartTimer( 60U, &timerID );
      stat = 1U;
    }
    else if ( uLOGICisTimer( timerID ) > 0U )
    {
      //mileage++;
      if ( ( maintence.oil.enb > 0U ) && ( maintence.oil.active > 0U ) )
      {
        eDATAAPIfreeData( DATA_API_CMD_INC, maintenanceAlarmOilTimeLeft, &data );
        eDATAAPIfreeData( DATA_API_CMD_SAVE, 0U, NULL );
      }
      if ( ( maintence.air.enb > 0U ) && ( maintence.air.active > 0U ) )
      {
        eDATAAPIfreeData( DATA_API_CMD_INC, maintenanceAlarmAirTimeLeft, &data );
        eDATAAPIfreeData( DATA_API_CMD_SAVE, 0U, NULL );
      }
      if ( ( maintence.fuel.enb > 0U ) && ( maintence.fuel.active > 0U ) )
      {
        eDATAAPIfreeData( DATA_API_CMD_INC, maintenanceAlarmFuelTimeLeft, &data );
        eDATAAPIfreeData( DATA_API_CMD_SAVE, 0U, NULL );
      }
      vLOGICstartTimer( 60U, &timerID );
    }
    else
    {

    }
  }
  else if ( stat > 0U )
  {
    stat = 0U;
    vLOGICresetTimer( timerID );
  }
  else
  {

  }
  eDATAAPIfreeData( DATA_API_CMD_READ, maintenanceAlarmOilTimeLeft, &data );
  vALARMcheck( &maintence.oil,  fix16_div( F16( data ),  F16( 60U ) ), pLOGICgetEventQueue() );
  eDATAAPIfreeData( DATA_API_CMD_READ, maintenanceAlarmAirTimeLeft, &data );
  vALARMcheck( &maintence.air,  fix16_div( F16( data ),  F16( 60U ) ), pLOGICgetEventQueue() );
  eDATAAPIfreeData( DATA_API_CMD_READ, maintenanceAlarmFuelTimeLeft, &data );
  vALARMcheck( &maintence.fuel, fix16_div( F16( data ), F16( 60U ) ), pLOGICgetEventQueue() );
  return;
}

uint8_t uENGINEisWork ( fix16_t freq, fix16_t pressure, fix16_t voltage, fix16_t speed )
{
  uint8_t res = 0U;
  if ( ( ( starter.startCrit.critGenFreqEnb  > 0U ) && ( freq     >= starter.startCrit.critGenFreqLevel ) )  ||
       ( ( starter.startCrit.critOilPressEnb > 0U ) && ( pressure >= starter.startCrit.critOilPressLevel ) ) ||
       ( ( starter.startCrit.critChargeEnb   > 0U ) && ( voltage  >= starter.startCrit.critChargeLevel ) )   ||
       ( ( starter.startCrit.critSpeedEnb    > 0U ) && ( speed    >= starter.startCrit.critSpeedLevel ) ) )
  {
    res = 1U;
  }
  return res;
}

uint8_t uENGINEisStop( fix16_t pressure, fix16_t speed  )
{
  uint8_t res = 0U;
  if ( ( pressure == 0U ) && ( speed == 0U ) )
  {
    res = 1U;
  }
  return res;
}
/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vENGINEinit ( void )
{
  oil.pressure.type                = getBitMap( &oilPressureSetup, 0U );
  oil.pressure.chart               = &oilSensorChart;
  oil.pressure.get                 = xADCGetSOP;
  oil.pressure.cutout.enb          = getBitMap( &oilPressureSetup, 1U );
  oil.pressure.cutout.event.action = ACTION_EMERGENCY_STOP;
  oil.pressure.cutout.event.type   = EVENT_OIL_SENSOR_ERROR;
  oil.pressure.status              = SENSOR_STATUS_NORMAL;

  oil.alarm.active = 0U;
  if ( ( oil.pressure.type == SENSOR_TYPE_RESISTIVE ) || ( oil.pressure.type == SENSOR_TYPE_CURRENT ) )
  {
    oil.alarm.enb   = getBitMap( &oilPressureSetup, 2U );
    oil.alarm.level = getValue( &oilPressureAlarmLevel );
    oil.alarm.type  = ALARM_LEVEL_HIGHT;
  }
  else
  {
    oil.alarm.enb   = 1U;
    oil.alarm.level = F16( 0x7FFFU );
    oil.alarm.type  = ALARM_LEVEL_LOW;
  }
  oil.alarm.delay        = 0U;
  oil.alarm.timerID      = 0U;
  oil.alarm.event.type   = EVENT_OIL_LOW_PRESSURE;
  oil.alarm.event.action = ACTION_EMERGENCY_STOP;
  oil.alarm.relax.enb    = 0U;
  oil.alarm.status       = ALARM_STATUS_IDLE;

  if ( ( oil.pressure.type == SENSOR_TYPE_RESISTIVE ) || ( oil.pressure.type == SENSOR_TYPE_CURRENT ) )
  {
    oil.preAlarm.enb          = getBitMap( &oilPressureSetup, 3U );
    oil.preAlarm.active       = 0U;
    oil.preAlarm.type         = ALARM_LEVEL_HIGHT;
    oil.preAlarm.level        = getValue( &oilPressurePreAlarmLevel );
    oil.preAlarm.delay        = 0U;
    oil.preAlarm.timerID      = 0U;
    oil.preAlarm.event.type   = EVENT_OIL_LOW_PRESSURE;
    oil.preAlarm.event.action = ACTION_WARNING;
    oil.preAlarm.relax.enb    = 0U;
    oil.preAlarm.status       = ALARM_STATUS_IDLE;
  }
  else
  {
    oil.preAlarm.enb = 0U;
  }
  /*--------------------------------------------------------------*/
  coolant.temp.type                = getBitMap( &coolantTempSetup, 0U );
  coolant.temp.chart               = &coolantSensorChart;
  coolant.temp.get                 = xADCGetSCT;
  coolant.temp.cutout.enb          = getBitMap( &coolantTempSetup, 1U );
  coolant.temp.cutout.event.action = ACTION_NONE;
  coolant.temp.cutout.event.type   = EVENT_ENGINE_TEMP_SENSOR_ERROR;
  coolant.temp.status              = SENSOR_STATUS_NORMAL;

  if ( ( coolant.temp.type == SENSOR_TYPE_RESISTIVE ) || ( coolant.temp.type == SENSOR_TYPE_CURRENT ) )
  {
    coolant.alarm.enb   = getBitMap( &coolantTempSetup, 2U );
    coolant.alarm.type  = ALARM_LEVEL_HIGHT;
    coolant.alarm.level = getValue( &coolantHightTempAlarmLevel );
  }
  else
  {
    coolant.alarm.enb   = 1U;
    coolant.alarm.type  = ALARM_LEVEL_LOW;
    coolant.alarm.level = F16( 0x7FFFU );
  }
  coolant.alarm.active       = 1U;
  coolant.alarm.delay        = 0U;
  coolant.alarm.timerID      = 0U;
  coolant.alarm.event.type   = EVENT_ENGINE_HIGHT_TEMP;
  coolant.alarm.event.action = ACTION_EMERGENCY_STOP;
  coolant.alarm.relax.enb    = 0U;
  coolant.alarm.status       = ALARM_STATUS_IDLE;

  if ( ( coolant.temp.type == SENSOR_TYPE_RESISTIVE ) || ( coolant.temp.type == SENSOR_TYPE_CURRENT ) )
  {
    coolant.preAlarm.enb           = getBitMap( &coolantTempSetup, 3U );
    coolant.preAlarm.active        = 1U;
    coolant.preAlarm.type          = ALARM_LEVEL_HIGHT;
    coolant.preAlarm.level         = getValue( &coolantHightTempPreAlarmLevel );
    coolant.preAlarm.delay         = 0U;
    coolant.preAlarm.timerID       = 0U;
    coolant.preAlarm.event.type    = EVENT_ENGINE_HIGHT_TEMP;
    coolant.preAlarm.event.action  = ACTION_WARNING;
    coolant.preAlarm.status        = ALARM_STATUS_IDLE;
  }

  coolant.heater.relay.enb    = getBitMap( &coolantTempSetup, 4U );
  coolant.heater.onLevel      = getValue( &coolantTempHeaterOnLevel );
  coolant.heater.offLevel     = getValue( &coolantTempHeaterOffLevel );
  coolant.heater.relay.set    = vFPOsetHeater;
  coolant.heater.relay.status = RELAY_OFF;

  coolant.cooler.relay.enb    = getBitMap( &coolantTempSetup, 5U );
  coolant.cooler.onLevel      = getValue( &coolantTempCoolerOnLevel );
  coolant.cooler.offLevel     = getValue( &coolantTempCoolerOffLevel );
  coolant.cooler.relay.set    = vFPOsetCooler;
  coolant.cooler.relay.status = RELAY_OFF;
  /*--------------------------------------------------------------*/
  preHeater.relay.enb    = 0U;
  preHeater.level        = getValue( &enginePreHeatLevel );
  preHeater.relay.set    = vFPOsetPreheater;
  preHeater.relay.status = RELAY_OFF;
  preHeater.status       = RELAY_DELAY_IDLE;
  preHeater.delay        = getValue( &enginePreHeatDelay );
  /*--------------------------------------------------------------*/
  uint8_t read = getBitMap( &fuelLevelSetup, 0U );
  if ( read == 0U )
  {
    fuel.level.type = SENSOR_TYPE_NONE;
  }
  if ( read == 1U )
  {
    fuel.level.type = SENSOR_TYPE_RESISTIVE;
  }
  if ( read == 2U )
  {
    fuel.level.type = SENSOR_TYPE_CURRENT;
  }
  fuel.level.chart               = &fuelSensorChart;
  fuel.level.get                 = xADCGetSFL;
  fuel.level.cutout.enb          = getBitMap( &fuelLevelSetup, 1U );
  fuel.level.cutout.event.action = ACTION_EMERGENCY_STOP;
  fuel.level.cutout.event.type   = EVENT_FUEL_LEVEL_SENSOR_ERROR;
  fuel.level.cutout.enb          = 0U;
  fuel.level.status              = SENSOR_STATUS_NORMAL;

  fuel.lowAlarm.enb        = getBitMap( &fuelLevelSetup, 2U );
  fuel.lowAlarm.active     = 1U;
  fuel.lowAlarm.type       = ALARM_LEVEL_LOW;
  fuel.lowAlarm.level      = getValue( &fuelLevelLowAlarmLevel );
  fuel.lowAlarm.delay      = getValue( &fuelLevelLowAlarmDelay );
  fuel.lowAlarm.timerID    = 0U;
  fuel.lowAlarm.event.type = EVENT_FUEL_LOW_LEVEL;
  if ( getBitMap( &fuelLevelSetup, 3U ) == 0U )
  {
    fuel.lowAlarm.event.action = ACTION_EMERGENCY_STOP;
  }
  else
  {
    fuel.lowAlarm.event.action = ACTION_LOAD_SHUTDOWN;
  }
  fuel.lowAlarm.relax.enb = 0U;
  fuel.lowAlarm.status    = ALARM_STATUS_IDLE;

  fuel.lowPreAlarm.enb          = getBitMap( &fuelLevelSetup, 4U );
  fuel.lowPreAlarm.active       = 1U;
  fuel.lowPreAlarm.type         = ALARM_LEVEL_LOW;
  fuel.lowPreAlarm.level        = getValue( &fuelLevelLowPreAlarmLevel );
  fuel.lowPreAlarm.delay        = getValue( &fuelLevelLowPreAlarmDelay );
  fuel.lowPreAlarm.timerID      = 0U;
  fuel.lowPreAlarm.event.type   = EVENT_FUEL_LOW_LEVEL;
  fuel.lowPreAlarm.event.action = ACTION_WARNING;
  fuel.lowPreAlarm.relax.enb    = 0U;
  fuel.lowPreAlarm.status       = ALARM_STATUS_IDLE;

  fuel.hightPreAlarm.enb          = getBitMap( &fuelLevelSetup, 5U );
  fuel.hightPreAlarm.active       = 1U;
  fuel.hightPreAlarm.type         = ALARM_LEVEL_HIGHT;
  fuel.hightPreAlarm.level        = getValue( &fuelLevelHightPreAlarmLevel );
  fuel.hightPreAlarm.delay        = getValue( &fuelLevelHightPreAlarmDelay );
  fuel.hightPreAlarm.timerID      = 0U;
  fuel.hightPreAlarm.event.type   = EVENT_FUEL_HIGHT_LEVEL;
  fuel.hightPreAlarm.event.action = ACTION_WARNING;
  fuel.hightPreAlarm.relax.enb    = 0U;
  fuel.hightPreAlarm.status       = ALARM_STATUS_IDLE;

  fuel.hightAlarm.enb        = getBitMap( &fuelLevelSetup, 6U );
  fuel.hightAlarm.active     = 1U;
  fuel.hightAlarm.type       = ALARM_LEVEL_HIGHT;
  fuel.hightAlarm.level      = getValue( &fuelLevelHightAlarmLevel );
  fuel.hightAlarm.delay      = getValue( &fuelLevelHightAlarmDelay );
  fuel.hightAlarm.timerID    = 0U;
  fuel.hightAlarm.event.type = EVENT_FUEL_HIGHT_LEVEL;
  if ( getBitMap( &fuelLevelSetup, 7U ) == 0U )
  {
    fuel.hightAlarm.event.action = ACTION_EMERGENCY_STOP;
  }
  else
  {
    fuel.hightAlarm.event.action = ACTION_LOAD_SHUTDOWN;
  }
  fuel.hightAlarm.relax.enb = 0U;
  fuel.hightAlarm.status    = ALARM_STATUS_IDLE;

  fuel.booster.relay.enb    = getBitMap( &fuelLevelSetup, 8U );
  fuel.booster.onLevel      = getValue( &fuelPumpOnLevel );
  fuel.booster.offLevel     = getValue( &fuelPumpOffLevel );
  fuel.booster.relay.set    = vFPOsetBooster;
  fuel.booster.relay.status = RELAY_OFF;

  fuel.pump.enb    = 1U;
  fuel.pump.set    = vFPOsetPump;
  fuel.pump.status = RELAY_OFF;
  /*--------------------------------------------------------------*/
  battery.get = getBatteryVoltage;

  battery.lowAlarm.enb          = getBitMap( &batteryAlarms, 0U );
  battery.lowAlarm.active       = 0U;
  battery.lowAlarm.type         = ALARM_LEVEL_LOW;
  battery.lowAlarm.level        = getValue( &batteryUnderVoltageLevel );
  battery.lowAlarm.delay        = getValue( &batteryUnderVoltageDelay );
  battery.lowAlarm.timerID      = 0U;
  battery.lowAlarm.event.type   = EVENT_BATTERY_LOW;
  battery.lowAlarm.event.action = ACTION_WARNING;
  battery.lowAlarm.relax.enb    = 0U;
  battery.lowAlarm.status       = ALARM_STATUS_IDLE;

  battery.hightAlarm.enb          = getBitMap( &batteryAlarms, 1U );
  battery.hightAlarm.active       = 0U;
  battery.hightAlarm.type         = ALARM_LEVEL_HIGHT;
  battery.hightAlarm.level        = getValue( &batteryOverVoltageLevel );
  battery.hightAlarm.delay        = getValue( &batteryOverVoltageDelay );
  battery.hightAlarm.timerID      = 0U;
  battery.hightAlarm.event.type   = EVENT_BATTERY_HIGHT;
  battery.hightAlarm.event.action = ACTION_WARNING;
  battery.hightAlarm.relax.enb    = 0U;
  battery.hightAlarm.status       = ALARM_STATUS_IDLE;
  /*--------------------------------------------------------------*/
  charger.get = getChargerVoltage;

  charger.hightAlarm.enb          = getBitMap( &batteryAlarms, 2U );
  charger.hightAlarm.active       = 0U;
  charger.hightAlarm.type         = ALARM_LEVEL_HIGHT;
  charger.hightAlarm.level        = getValue( &batteryChargeShutdownLevel );
  charger.hightAlarm.delay        = getValue( &batteryChargeShutdownDelay );
  charger.hightAlarm.timerID      = 0U;
  charger.hightAlarm.event.type   = EVENT_CHARGER_FAIL;
  charger.hightAlarm.event.action = ACTION_EMERGENCY_STOP;
  charger.hightAlarm.relax.enb    = 0U;
  charger.hightAlarm.status       = ALARM_STATUS_IDLE;

  charger.hightPreAlarm.enb          = getBitMap( &batteryAlarms, 3U );
  charger.hightPreAlarm.active       = 0U;
  charger.hightPreAlarm.type         = ALARM_LEVEL_HIGHT;
  charger.hightPreAlarm.level        = getValue( &batteryChargeWarningLevel );
  charger.hightPreAlarm.delay        = getValue( &batteryChargeWarningDelay );
  charger.hightPreAlarm.timerID      = 0U;
  charger.hightPreAlarm.event.type   = EVENT_CHARGER_FAIL;
  charger.hightPreAlarm.event.action = ACTION_WARNING;
  charger.hightPreAlarm.relax.enb    = 0U;
  charger.hightPreAlarm.status       = ALARM_STATUS_IDLE;
  /*--------------------------------------------------------------*/
  starter.set            = vFPOsetStarter;
  starter.startAttempts  = getBitMap( &engineSetup, 0U );
  starter.startDelay     = getValue( &timerStartDelay );
  starter.crankingDelay  = getValue( &timerCranking );
  starter.crankDelay     = getValue( &timerCrankDelay );
  starter.blockDelay     = getValue( &timerSafetyOnDelay );
  starter.idlingDelay    = getValue( &timerStartupIdleTime );
  starter.nominalDelay   = getValue( &timerNominalRPMDelay );
  starter.warmingDelay   = getValue( &timerWarming );
  starter.startIteration = 0U;
  starter.status         = STARTER_IDLE;

  starter.startCrit.critGenFreqEnb    = getBitMap( &genSetup, 0U );
  starter.startCrit.critGenFreqLevel  = getValue( &starterStopGenFreqLevel );
  starter.startCrit.critOilPressEnb   = getBitMap( &starterStopSetup, 1U );
  starter.startCrit.critOilPressLevel = getValue( &starterStopOilPressureLevel );
  starter.startCrit.critChargeEnb     = getBitMap( &starterStopSetup, 2U );
  starter.startCrit.critChargeLevel   = getValue( &starterStopChargeAlternatorLevel );
  starter.startCrit.critSpeedEnb      = getBitMap( &starterStopSetup, 3U );
  starter.startCrit.critSpeedLevel    = getValue( &starterStopSpeedLevel );
  /*--------------------------------------------------------------*/
  planStop.status           = STOP_IDLE;
  planStop.coolingDelay     = getValue( &timerCooling );
  planStop.coolingIdleDelay = getValue( &timerCoolingIdle );
  planStop.processDelay     = getValue( &timerSolenoidHold );
  /*--------------------------------------------------------------*/
  engine.startCheckOil = getBitMap( &starterStopSetup, 0U );
  engine.status        = ENGINE_STATUS_IDLE;
  /*--------------------------------------------------------------*/
  speed.status = SENSOR_STATUS_NORMAL;
  speed.get    = fVRgetSpeed;

  speed.lowAlarm.enb          = getBitMap( &speedSetup, 0U );
  speed.lowAlarm.active       = 0U;
  speed.lowAlarm.type         = ALARM_LEVEL_LOW;
  speed.lowAlarm.level        = getValue( &speedLowAlarmLevel );
  speed.lowAlarm.delay        = 0U;
  speed.lowAlarm.timerID      = 0U;
  speed.lowAlarm.event.type   = EVENT_SPEED_LOW;
  speed.lowAlarm.event.action = ACTION_EMERGENCY_STOP;
  speed.lowAlarm.relax.enb    = 0U;
  speed.lowAlarm.status       = ALARM_STATUS_IDLE;

  speed.hightAlarm.enb          = 1U;
  speed.hightAlarm.active       = 1U;
  speed.hightAlarm.type         = ALARM_LEVEL_HIGHT;
  speed.hightAlarm.level        = getValue( &speedHightAlarmLevel );
  speed.hightAlarm.delay        = 0U;
  speed.hightAlarm.timerID      = 0U;
  speed.hightAlarm.event.type   = EVENT_SPEED_HIGHT;
  speed.hightAlarm.event.action = ACTION_EMERGENCY_STOP;
  speed.hightAlarm.relax.enb    = 0U;
  speed.hightAlarm.status       = ALARM_STATUS_IDLE;
  /*--------------------------------------------------------------*/
  stopSolenoid.enb    = uFPOisEnable( FPO_FUN_STOP_SOLENOID );
  stopSolenoid.set    = vFPOsetStopSolenoid;
  stopSolenoid.status = RELAY_OFF;
  /*--------------------------------------------------------------*/
  idleRelay.enb    = uFPOisEnable( FPO_FUN_IDLING );
  idleRelay.set    = vFPOsetIdle;
  idleRelay.status = RELAY_OFF;
  /*--------------------------------------------------------------*/
  maintence.oil.enb        = getBitMap( &maintenanceAlarms, 0U );
  maintence.oil.active     = 0U;
  maintence.oil.type       = ALARM_LEVEL_HIGHT;
  maintence.oil.level      = getValue( &maintenanceAlarmOilTime );
  maintence.oil.delay      = 0U;
  maintence.oil.timerID    = 0U;
  maintence.oil.event.type = EVENT_MAINTENANCE_OIL;
  if ( getBitMap( &maintenanceAlarms, 1U ) == 0U )
  {
    maintence.oil.event.action = ACTION_PLAN_STOP;
  }
  else
  {
    maintence.oil.event.action = ACTION_WARNING;
  }
  maintence.oil.relax.enb = 0U;
  maintence.oil.status    = ALARM_STATUS_IDLE;

  maintence.air.enb        = getBitMap( &maintenanceAlarms, 2U );
  maintence.air.active     = 0U;
  maintence.air.type       = ALARM_LEVEL_HIGHT;
  maintence.air.level      = getValue( &maintenanceAlarmAirTime );
  maintence.air.delay      = 0U;
  maintence.air.timerID    = 0U;
  maintence.air.event.type = EVENT_MAINTENANCE_AIR;
  if ( getBitMap( &maintenanceAlarms, 3U ) == 0U )
  {
    maintence.air.event.action = ACTION_PLAN_STOP;
  }
  else
  {
    maintence.air.event.action = ACTION_WARNING;
  }
  maintence.air.relax.enb = 0U;
  maintence.air.status    = ALARM_STATUS_IDLE;

  maintence.fuel.enb        = getBitMap( &maintenanceAlarms, 4U );
  maintence.fuel.active     = 0U;
  maintence.fuel.type       = ALARM_LEVEL_HIGHT;
  maintence.fuel.level      = getValue( &maintenanceAlarmFuelTime );
  maintence.fuel.delay      = 0U;
  maintence.fuel.timerID    = 0U;
  maintence.fuel.event.type = EVENT_MAINTENANCE_FUEL;
  if ( getBitMap( &maintenanceAlarms, 5U ) == 0U )
  {
    maintence.fuel.event.action = ACTION_PLAN_STOP;
  }
  else
  {
    maintence.fuel.event.action = ACTION_WARNING;
  }
  maintence.fuel.relax.enb = 0U;
  maintence.fuel.status    = ALARM_STATUS_IDLE;
  /*--------------------------------------------------------------*/

  pEngineCommandQueue = xQueueCreateStatic( ENGINE_COMMAND_QUEUE_LENGTH, sizeof( ENGINE_COMMAND ), engineCommandBuffer, &xEngineCommandQueue );
  const osThreadAttr_t engineTask_attributes = {
    .name       = "engineTask",
    .priority   = ( osPriority_t ) osPriorityLow,
    .stack_size = 1024U
  };
  engineHandle = osThreadNew( vENGINEtask, NULL, &engineTask_attributes );

  return;
}

QueueHandle_t pENGINEgetCommandQueue ( void )
{
  return pEngineCommandQueue;
}

void vENGINEemergencyStop ( void )
{
  vTaskSuspend ( &engineHandle );
  engine.cmd = ENGINE_CMD_EMEGENCY_STOP;
  starter.set( RELAY_OFF );
  fuel.pump.set( RELAY_OFF );
  stopSolenoid.set( RELAY_ON );
  vTaskResume ( &engineHandle );
  return;
}

uint8_t uENGINEisStarterScrollFinish ( void )
{
  return starterFinish;
}

uint8_t uENGINEisBlockTimerFinish ( void )
{
  return blockTimerFinish;
}

void vENGINEmaintenanceReset ( void )
{
  maintenanceReset = 1U;
  return;
}

ENGINE_STATUS eENGINEgetEngineStatus ( void )
{
  return engine.status;
}

void vENGINEtask ( void const* argument )
{
  fix16_t        oilVal     = 0U;
  fix16_t        coolantVal = 0U;
  fix16_t        fuelVal    = 0U;
  fix16_t        speedVal   = 0U;
  fix16_t        batteryVal = 0U;
  fix16_t        chargerVal = 0U;
  timerID_t      timerID    = 0U;
  ENGINE_COMMAND inputCmd   = ENGINE_CMD_NONE;
  SYSTEM_EVENT   event      = { 0U };

  for (;;)
  {
    /* Read input command */
    if ( ( engine.status == ENGINE_STATUS_IDLE )           ||
         ( engine.status == ENGINE_STATUS_WORK )           ||
         ( engine.status == ENGINE_STATUS_WORK_ON_IDLE ) )
    {
      if ( xQueueReceive( pEngineCommandQueue, &inputCmd, 0U ) == pdPASS )
      {
        engine.cmd = inputCmd;
      }
    }
    /* Process inputs */
    oilVal     = fOILprocess();
    coolantVal = fCOOLANTprocess();
    fuelVal    = fFUELprocess();
    speedVal   = fSPEEDprocess();
    batteryVal = fBATTERYprocess();
    chargerVal = fCHARGERprocess();
    vENGINEmileageProcess( &maintenanceReset );
    /* Input commands */
    switch ( engine.cmd )
    {
      case ENGINE_CMD_NONE:
        break;
      /*----------------------------------------------------------------------------------------*/
      /*-------------------------------------- ENGINE START ------------------------------------*/
      /*----------------------------------------------------------------------------------------*/
      /*|--startDelay--|--crankingDelay--|--crankDelay--|--...--|--blockDelay--|--idlingDelay--|--nominalDelay--|--warmingDelay--> */
      case ENGINE_CMD_START:
        if ( ( engine.status == ENGINE_STATUS_IDLE ) || ( engine.status == ENGINE_STATUS_BUSY_STARTING ) )
        {
          switch ( starter.status )
          {
            case STARTER_IDLE:
              vRELAYimpulseReset( &preHeater );
              preHeater.active             = 0U;
              starterFinish                = 0U;
              speed.lowAlarm.active        = 0U;
              speed.hightAlarm.active      = 0U;
              oil.alarm.active             = 0U;
              oil.preAlarm.active          = 0U;
              battery.hightAlarm.active    = 0U;
              battery.lowAlarm.active      = 0U;
              charger.hightAlarm.active    = 0U;
              charger.hightPreAlarm.active = 0U;
              vELECTROalarmStartDisable();
              if ( ( engine.startCheckOil > 0U ) && ( oilVal != 0U ) )
              {
                starter.status = STARTER_FAIL;
              }
              engine.status = ENGINE_STATUS_BUSY_STARTING;
              fuel.pump.set( RELAY_ON );
              vLOGICstartTimer( starter.startDelay, &timerID );
              starter.status = STARTER_START_DELAY;
              break;
            case STARTER_START_DELAY:
              if ( uLOGICisTimer( timerID ) > 0U )
              {
                starter.status   = STARTER_READY;
                preHeater.active = 1U;
              }
              break;
            case STARTER_READY:
              if ( starter.startIteration < starter.startAttempts )
              {
                starter.startIteration++;
                starter.status = STARTER_CRANKING;
                starter.set( RELAY_ON );
                stopSolenoid.set( RELAY_OFF );
                vLOGICstartTimer( starter.crankingDelay, &timerID );
              }
              else
              {
                fuel.pump.set( RELAY_OFF );
                starter.status = STARTER_FAIL;
              }
              break;
            case STARTER_CRANKING:
              if ( uENGINEisWork( 0U, oilVal, chargerVal, speedVal ) > 0U )
              {
                starter.set( RELAY_OFF );
                starterFinish  = 1U;
                starter.status = STARTER_CONTROL_BLOCK;
                vLOGICresetTimer( timerID );
                vLOGICstartTimer( starter.blockDelay, &timerID );
              }
              if ( uLOGICisTimer( timerID ) > 0U )
              {
                starter.status = STARTER_CRANK_DELAY;
                starter.set( RELAY_OFF );
                vLOGICstartTimer( starter.crankDelay,&timerID );
              }
              break;
            case STARTER_CRANK_DELAY:
              if ( uLOGICisTimer( timerID ) > 0U )
              {
                starter.status = STARTER_READY;
              }
              break;
            case STARTER_CONTROL_BLOCK:
              if ( uLOGICisTimer( timerID ) > 0U )
              {
                idleRelay.set( RELAY_ON );
                vLOGICstartTimer( starter.idlingDelay, &timerID );
                starter.status               = STARTER_IDLE_WORK;
                blockTimerFinish             = 1U;
                speed.hightAlarm.active      = 1U;
                oil.alarm.active             = 1U;
                oil.preAlarm.active          = 1U;
                battery.hightAlarm.active    = 1U;
                battery.lowAlarm.active      = 1U;
                charger.hightAlarm.active    = 1U;
                charger.hightPreAlarm.active = 1U;
                vELECTROalarmStartToIdle();
              }
              break;
            case STARTER_IDLE_WORK:
              if ( uLOGICisTimer( timerID ) > 0U )
              {
                idleRelay.set( RELAY_OFF );
                vLOGICstartTimer( starter.nominalDelay, &timerID );
                starter.status = STARTER_MOVE_TO_NOMINAL;
              }
              break;
            case STARTER_MOVE_TO_NOMINAL:
              if ( uLOGICisTimer( timerID ) > 0U )
              {
                vLOGICstartTimer( starter.warmingDelay, &timerID );
                starter.status        = STARTER_WARMING;
                speed.lowAlarm.active = 1U;
                vELECTROalarmIdleEnable();
              }
              break;
            case STARTER_WARMING:
              if ( uLOGICisTimer( timerID ) > 0U )
              {
                starter.status   = STARTER_OK;
                preHeater.active = 0U;
                preHeater.relay.set( RELAY_OFF );
              }
              break;
            case STARTER_FAIL:
              engine.status          = ENGINE_STATUS_FAIL_STARTING;
              engine.cmd             = ENGINE_CMD_NONE;
              starter.status         = STARTER_IDLE;
              engine.cmd             = ENGINE_CMD_NONE;
              event.action           = EVENT_START_FAIL;
              event.type             = EVENT_START_FAIL;
              starter.startIteration = 0U;
              xQueueSend( pLOGICgetEventQueue(), &event, portMAX_DELAY );
              break;
            case STARTER_OK:
              engine.status          = ENGINE_STATUS_WORK;
              engine.cmd             = ENGINE_CMD_NONE;
              starter.status         = STARTER_IDLE;
              starter.startIteration = 0U;
              eDATAAPIfreeData( DATA_API_CMD_INC,  ENGINE_STARTS_NUMBER_ADR, NULL );
              eDATAAPIfreeData( DATA_API_CMD_SAVE, ENGINE_STARTS_NUMBER_ADR, NULL );
              maintence.oil.active  = 1U;
              maintence.air.active  = 1U;
              maintence.fuel.active = 1U;
              break;
            default:
              vLOGICresetTimer( timerID );
              engine.status  = ENGINE_STATUS_FAIL_STARTING;
              engine.cmd     = ENGINE_CMD_NONE;
              starter.status = STARTER_FAIL;
              starter.set( RELAY_OFF );
              preHeater.relay.set( RELAY_OFF );
              idleRelay.set( RELAY_OFF );
              stopSolenoid.set( RELAY_OFF );
              fuel.pump.set( RELAY_OFF );
              break;
          }
        }
        break;
      /*----------------------------------------------------------------------------------------*/
      /*-------------------------------------- ENGINE STOP -------------------------------------*/
      /*----------------------------------------------------------------------------------------*/
      case ENGINE_CMD_PLAN_STOP:
        if ( ( engine.status == ENGINE_STATUS_WORK )         ||
             ( engine.status == ENGINE_STATUS_WORK_ON_IDLE ) ||
             ( engine.status == ENGINE_STATUS_BUSY_STOPPING ) )
        {
          switch ( planStop.status )
          {
            case STOP_IDLE:
              planStop.status = STOP_COOLDOWN;
              vLOGICstartTimer( planStop.coolingDelay, &timerID );
              break;
            case STOP_COOLDOWN:
              if ( uLOGICisTimer( timerID ) > 0U )
              {
                speed.lowAlarm.active = 0U;
                vELECTROalarmIdleDisable();
                idleRelay.set( RELAY_ON );
                vLOGICstartTimer( planStop.coolingIdleDelay, &timerID );
                engine.status = STOP_IDLE_COOLDOWN;
              }
              break;
            case STOP_IDLE_COOLDOWN:
              if ( uLOGICisTimer( timerID ) > 0U )
              {
                oil.alarm.active    = 0U;
                oil.preAlarm.active = 0U;
                idleRelay.set( RELAY_OFF );
                fuel.pump.set( RELAY_OFF );
                stopSolenoid.set( RELAY_ON );
                vLOGICstartTimer( planStop.processDelay, &timerID );
                planStop.status = STOP_PROCESSING;
              }
              break;
            case STOP_PROCESSING:
              if ( uLOGICisTimer( timerID ) > 0U )
              {
                planStop.status = STOP_FAIL;
              }
              if ( uENGINEisStop( oilVal, speedVal ) > 0U )
              {
                vLOGICresetTimer( timerID );
                stopSolenoid.set( RELAY_OFF );
                planStop.status = STOP_OK;
              }
              break;
            case STOP_FAIL:
              engine.status   = ENGINE_STATUS_FAIL_STOPPING;
              engine.cmd      = ENGINE_CMD_NONE;
              planStop.status = STOP_IDLE;
              event.action    = ACTION_EMERGENCY_STOP;
              event.type      = EVENT_STOP_FAIL;
              xQueueSend( pLOGICgetEventQueue(), &event, portMAX_DELAY );
              break;
            case STOP_OK:
              engine.status         = ENGINE_STATUS_IDLE;
              engine.cmd            = ENGINE_CMD_NONE;
              planStop.status       = STOP_IDLE;
              maintence.oil.active  = 0U;
              maintence.air.active  = 0U;
              maintence.fuel.active = 0U;
              break;
            default:
              vLOGICresetTimer( timerID );
              planStop.status = STOP_FAIL;
              break;
          }
        }
        break;
      /*----------------------------------------------------------------------------------------*/
      /*--------------------------------- ENGINE GO TO IDLE ------------------------------------*/
      /*----------------------------------------------------------------------------------------*/
      case ENGINE_CMD_GOTO_IDLE:
        if ( engine.status == ENGINE_STATUS_WORK )
        {
          speed.lowAlarm.active = 0U;
          vELECTROalarmIdleDisable();
          idleRelay.set( RELAY_ON );
          engine.status = ENGINE_STATUS_WORK_ON_IDLE;
        }
        break;
      /*----------------------------------------------------------------------------------------*/
      /*--------------------------- ENGINE GO TO NOMINAL FROM IDLE -----------------------------*/
      /*----------------------------------------------------------------------------------------*/
      case ENGINE_CMD_GOTO_NORMAL:
        switch ( engine.status )
        {
          case ENGINE_STATUS_WORK_ON_IDLE:
            idleRelay.set( RELAY_OFF );
            engine.status = ENGINE_STATUS_WORK_GOTO_NOMINAL;
            vLOGICstartTimer( starter.nominalDelay, timerID );
            break;
          case ENGINE_STATUS_WORK_GOTO_NOMINAL:
            if ( uLOGICisTimer( timerID ) > 0U )
            {
              engine.status = ENGINE_STATUS_WORK;
              speed.lowAlarm.active = 1U;
              vELECTROalarmIdleEnable();
            }
            break;
          default:
            engine.status = ENGINE_STATUS_WORK_ON_IDLE;
            break;
        }
        break;
      /*----------------------------------------------------------------------------------------*/
      /*------------------------------- ENGINE EMERGENCY STOP ----------------------------------*/
      /*----------------------------------------------------------------------------------------*/
      case ENGINE_CMD_EMEGENCY_STOP:
        starter.set( RELAY_OFF );
        fuel.pump.set( RELAY_OFF );
        stopSolenoid.set( RELAY_ON );
        break;
      /*----------------------------------------------------------------------------------------*/
      /*------------------------------- ENGINE RESET TO IDLE -----------------------------------*/
      /*----------------------------------------------------------------------------------------*/
      case ENGINE_CMD_RESET_TO_IDLE:
        stopSolenoid.set( RELAY_OFF );
        starter.status  = STARTER_IDLE;
        planStop.status = STOP_IDLE;
        engine.status   = ENGINE_STATUS_IDLE;
        break;
      /*----------------------------------------------------------------------------------------*/
      /*----------------------------------------------------------------------------------------*/
      /*----------------------------------------------------------------------------------------*/
      default:
        break;
    }
    /* Process outputs */
    vRELAYimpulseProcess( &preHeater, coolantVal );
    /* Send status */

    /*---------------------*/
  }
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


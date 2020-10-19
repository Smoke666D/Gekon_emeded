/*
 * oil.c
 *
 *  Created on: 15 июн. 2020 г.
 *      Author: photo_Mickey
 */
/*--------------------------------- Includes ---------------------------------*/
#include "engine.h"
#include "config.h"
#include "common.h"
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
#include "stdio.h"
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
static RELAY_DELAY_DEVICE    stopSolenoid        = { 0U };
static RELAY_DEVICE          idleRelay           = { 0U };
static RELAY_IMPULSE_DEVICE  preHeater           = { 0U };
static StaticQueue_t         xEngineCommandQueue = { 0U };
static QueueHandle_t         pEngineCommandQueue = NULL;
/*--------------------------------- Constant ---------------------------------*/
static const fix16_t fix60               = F16( 60U );
static const fix16_t dryContactTrigLevel = F16( 0x7FFFU );

#if ( DEBUG_SERIAL_STATUS > 0U )
  const char* cSensorTypes[5U] = { "NONE", "NORMAL_OPEN", "NORMAL_CLOSE", "RESISTIVE", "CURRENT" };
  const char* engineCmdStr[8U] =
  {
    "NONE",
    "START",
    "PLAN_STOP",
    "PLAN_STOP_WITH_DELAY",
    "GOTO_IDLE",
    "GOTO_NORMAL",
    "EMEGENCY_STOP",
    "RESET_TO_IDLE",
  };
  const char* engineStatusStr[9U] =
  {
    "IDLE",
    "EMERGENCY_STOP",
    "BUSY_STARTING",
    "BUSY_STOPPING",
    "WORK",
    "WORK_ON_IDLE",
    "WORK_GOTO_NOMINAL",
    "FAIL_STARTING",
    "FAIL_STOPPING",
  };
  const char* starterStatusStr[11U] =
  {
    "IDLE",
    "START_DELAY",
    "READY",
    "CRANKING",
    "CRANK_DELAY",
    "CONTROL_BLOCK",
    "IDLE_WORK",
    "MOVE_TO_NOMINAL",
    "WARMING",
    "FAIL",
    "OK",
  };
  const char* planStopStatusStr[6U] =
  {
    "IDLE",
    "COOLDOWN",
    "IDLE_COOLDOWN",
    "PROCESSING",
    "FAIL",
    "OK",
  };
#endif
/*-------------------------------- Variables ---------------------------------*/
static ENGINE_COMMAND engineCommandBuffer[ENGINE_COMMAND_QUEUE_LENGTH] = { 0U };
static uint8_t        starterFinish                                    = 0U;
static uint8_t        blockTimerFinish                                 = 0U;
/*--------------------------------- Extern -----------------------------------*/
osThreadId_t engineHandle = NULL;
/*-------------------------------- Functions ---------------------------------*/
void vENGINEtask ( void const* argument );
/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/
fix16_t getChargerVoltage ( void )
{
  fix16_t res = 0U;
  return res;
}
/*----------------------------------------------------------------------------*/
void vSENSORprocess ( SENSOR* sensor, fix16_t* value )
{
  eFunctionError funcStat = SENSOR_STATUS_NORMAL;

  *value = sensor->get();
  vALARMcheck( &sensor->cutout, *value );
  if ( sensor->cutout.status != ALARM_STATUS_IDLE )
  {
    sensor->status = SENSOR_STATUS_LINE_ERROR;
  }
  else
  {
    if ( ( sensor->type == SENSOR_TYPE_RESISTIVE ) || ( sensor->type == SENSOR_TYPE_CURRENT ) )
    {
      funcStat = eCHARTfunc( sensor->chart, sensor->get(), value );
      switch ( funcStat )
      {
        case FUNC_OK:
          sensor->status = SENSOR_STATUS_NORMAL;
          break;
        case FUNC_OVER_MAX_X_ERROR:
          sensor->status = SENSOR_STATUS_ERROR;
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
      sensor->status = SENSOR_STATUS_NORMAL;
    }
  }
  return;
}
/*----------------------------------------------------------------------------*/
fix16_t fOILprocess ( void )
{
  fix16_t value = 0U;
  vSENSORprocess( &oil.pressure, &value );
  if ( oil.pressure.status == SENSOR_STATUS_NORMAL )
  {
    vALARMcheck( &oil.alarm, value );
    if ( oil.alarm.status == ALARM_STATUS_IDLE )
    {
      vALARMcheck( &oil.preAlarm, value );
    }
  }
  return value;
}
/*----------------------------------------------------------------------------*/
fix16_t fCOOLANTprocess ( void )
{
  fix16_t value = 0U;
  vSENSORprocess( &coolant.temp, &value );
  if ( coolant.temp.status == SENSOR_STATUS_NORMAL )
  {
    vALARMcheck( &coolant.alarm, value );
    if ( coolant.alarm.status == ALARM_STATUS_IDLE )
    {
      vALARMcheck( &coolant.preAlarm, value );
    }
    vRELAYautoProces( &coolant.heater, value );
    vRELAYautoProces( &coolant.cooler, value );
  }
  return value;
}
/*----------------------------------------------------------------------------*/
fix16_t fFUELprocess ( void )
{
  fix16_t value = 0U;

  vSENSORprocess( &fuel.level, &value );
  if ( fuel.level.status == SENSOR_STATUS_NORMAL )
  {
    vALARMcheck( &fuel.lowAlarm, value );
    if ( fuel.lowAlarm.status == ALARM_STATUS_IDLE )
    {
      vALARMcheck( &fuel.lowPreAlarm, value );
    }
    vALARMcheck( &fuel.hightAlarm, value );
    if ( fuel.hightPreAlarm.status == ALARM_STATUS_IDLE )
    {
      vALARMcheck( &fuel.hightPreAlarm, value );
    }
    vRELAYautoProces( &fuel.booster, value );
  }
  return value;
}
/*----------------------------------------------------------------------------*/
fix16_t fSPEEDprocess ( void )
{
  fix16_t value = 0U;
  if ( speed.enb > 0U )
  {
    value = speed.get();
    vALARMcheck( &speed.hightAlarm, value );
    if ( speed.hightAlarm.status == ALARM_STATUS_IDLE )
    {
      vALARMcheck( &speed.lowAlarm, value );
    }
  }
  return value;
}
/*----------------------------------------------------------------------------*/
fix16_t fBATTERYprocess ( void )
{
  fix16_t value = battery.get();
  vALARMcheck( &battery.hightAlarm, value );
  if ( battery.hightAlarm.status == ALARM_STATUS_IDLE )
  {
    vALARMcheck( &battery.lowAlarm, value );
  }
  return value;
}
/*----------------------------------------------------------------------------*/
fix16_t fCHARGERprocess ( void )
{
  fix16_t value = charger.get();
  vALARMcheck( &charger.hightPreAlarm, value );
  if ( charger.hightAlarm.status == ALARM_STATUS_IDLE )
  {
    vALARMcheck( &charger.hightPreAlarm, value );
  }
  return value;
}
/*----------------------------------------------------------------------------*/
void vENGINEmileageProcess ( void )
{
  uint16_t data   = 0U;
  uint8_t  flSave = 0U;

  if ( engine.status == ENGINE_STATUS_WORK )
  {
    switch ( maintence.status )
    {
      case MAINTENCE_STATUS_STOP:
        maintence.timer.delay = fix60;
        vLOGICstartTimer( &maintence.timer );
        maintence.status = MAINTENCE_STATUS_RUN;
        break;
      case MAINTENCE_STATUS_RUN:
        if ( uLOGICisTimer( maintence.timer ) > 0U )
        {
          if ( ( maintence.oil.enb > 0U ) && ( maintence.oil.active > 0U ) )
          {
            eDATAAPIfreeData( DATA_API_CMD_INC,  MAINTENANCE_ALARM_OIL_TIME_LEFT_ADR, &data );
            eDATAAPIfreeData( DATA_API_CMD_READ, MAINTENANCE_ALARM_OIL_TIME_LEFT_ADR, &data );
            vALARMcheck( &maintence.oil,  fix16_div( F16( data ),  F16( 60U ) ) );
            flSave = 1U;
          }
          if ( ( maintence.air.enb > 0U ) && ( maintence.air.active > 0U ) )
          {
            eDATAAPIfreeData( DATA_API_CMD_INC,  MAINTENANCE_ALARM_AIR_TIME_LEFT_ADR, &data );
            eDATAAPIfreeData( DATA_API_CMD_READ, MAINTENANCE_ALARM_AIR_TIME_LEFT_ADR, &data );
            vALARMcheck( &maintence.air,  fix16_div( F16( data ),  F16( 60U ) ) );
            flSave = 1U;
          }
          if ( ( maintence.fuel.enb > 0U ) && ( maintence.fuel.active > 0U ) )
          {
            eDATAAPIfreeData( DATA_API_CMD_INC,  MAINTENANCE_ALARM_FUEL_TIME_LEFT_ADR, &data );
            eDATAAPIfreeData( DATA_API_CMD_READ, MAINTENANCE_ALARM_FUEL_TIME_LEFT_ADR, &data );
            vALARMcheck( &maintence.fuel, fix16_div( F16( data ), F16( 60U ) ) );
            flSave = 1U;
          }
          if ( flSave > 0U )
          {
            eDATAAPIfreeData( DATA_API_CMD_SAVE, 0U, NULL );
          }
          vLOGICstartTimer( &maintence.timer );
        }
        break;
      default:
        maintence.status = MAINTENCE_STATUS_STOP;
        break;
    }
  }
  else if ( maintence.status == MAINTENCE_STATUS_RUN )
  {
    maintence.status = MAINTENCE_STATUS_STOP;
    vLOGICresetTimer( maintence.timer );
  }
  else
  {

  }
  return;
}
/*----------------------------------------------------------------------------*/
uint8_t uENGINEisWork ( fix16_t freq, fix16_t pressure, fix16_t voltage, fix16_t speed )
{
  uint8_t res = 0U;
  #if ( DEBUG_SERIAL_STATUS > 0U )
    char buffer[10U] = { 0U };
  #endif
  if ( ( starter.startCrit.critGenFreqEnb > 0U ) && ( freq >= starter.startCrit.critGenFreqLevel ) )
  {
    res = 1U;
    #if ( DEBUG_SERIAL_STATUS > 0U )
      vSYSSerial( ">>Engine started by generator frequency: " );
      fix16_to_str( freq, buffer, 2U );
      vSYSSerial( buffer );
      vSYSSerial( "\r\n" );
    #endif
  }
  if ( ( starter.startCrit.critOilPressEnb > 0U ) && ( pressure >= starter.startCrit.critOilPressLevel ) )
  {
    res = 1U;
    #if ( DEBUG_SERIAL_STATUS > 0U )
      vSYSSerial( ">>Engine started by oil pressure: " );
      fix16_to_str( pressure, buffer, 2U );
      vSYSSerial( buffer );
      vSYSSerial( "\r\n" );
    #endif
  }
  if ( ( starter.startCrit.critChargeEnb > 0U ) && ( voltage >= starter.startCrit.critChargeLevel ) )
  {
    res = 1U;
    #if ( DEBUG_SERIAL_STATUS > 0U )
      vSYSSerial( ">>Engine started by charger voltage: " );
      fix16_to_str( voltage, buffer, 2U );
      vSYSSerial( buffer );
      vSYSSerial( "\r\n" );
    #endif
  }
  if ( ( starter.startCrit.critSpeedEnb > 0U ) && ( speed >= starter.startCrit.critSpeedLevel ) )
  {
    res = 1U;
    #if ( DEBUG_SERIAL_STATUS > 0U )
      vSYSSerial( ">>Engine started by engine speed: " );
      fix16_to_str( speed, buffer, 2U );
      vSYSSerial( buffer );
      vSYSSerial( "\r\n" );
    #endif
  }
  return res;
}
/*----------------------------------------------------------------------------*/
uint8_t uENGINEisStop ( fix16_t pressure, fix16_t speed  )
{
  uint8_t res = 0U;
  if ( ( pressure < ENGINE_OIL_PRESSURE_TRESH_HOLD ) && ( speed == 0U ) )
  {
    res = 1U;
  }
  return res;
}
/*----------------------------------------------------------------------------*/
void vENGINEenbToStr ( uint8_t enb, char* str )
{
  if ( enb > 0U )
  {
    str[0U] = 'E';
    str[1U] = 'n';
    str[2U] = 'a';
    str[3U] = 'b';
    str[4U] = 'l';
    str[5U] = 'e';
    str[6U] = 0;
  }
  else
  {
    str[0U] = 'D';
    str[1U] = 'i';
    str[2U] = 's';
    str[3U] = 'a';
    str[4U] = 'b';
    str[5U] = 'l';
    str[6U] = 'e';
    str[7U] = 0;
  }
  return;
}
/*----------------------------------------------------------------------------*/
void vENGINEprintSetup ( void )
{
  #if ( DEBUG_SERIAL_STATUS > 0U )
    char buf[8U];
    vSYSSerial( ">>Oil pressure sensor \r\n" );
    vSYSSerial( "    Type           : ");
    vSYSSerial( cSensorTypes[oil.pressure.type] );
    vSYSSerial( "\n\r" );
    vSYSSerial( "    Alarm          : ");
    vENGINEenbToStr( oil.alarm.enb, buf );
    vSYSSerial( buf );
    vSYSSerial( "\r\n" );
    vSYSSerial( "    Prealarm       : ");
    vENGINEenbToStr( oil.alarm.enb, buf );
    vSYSSerial( buf );
    vSYSSerial( "\r\n" );

    vSYSSerial( ">>Fuel level sensor \r\n" );
    vSYSSerial( "    Type           : ");
    vSYSSerial( cSensorTypes[fuel.level.type] );
    vSYSSerial( "\n\r" );
    vSYSSerial( "    Low alarm      : ");
    vENGINEenbToStr( fuel.lowAlarm.enb, buf );
    vSYSSerial( buf );
    vSYSSerial( "\r\n" );
    vSYSSerial( "    Low prealarm   : ");
    vENGINEenbToStr( fuel.lowPreAlarm.enb, buf );
    vSYSSerial( buf );
    vSYSSerial( "\r\n" );
    vSYSSerial( "    Hight alarm    : ");
    vENGINEenbToStr( fuel.hightPreAlarm.enb, buf );
    vSYSSerial( buf );
    vSYSSerial( "\r\n" );
    vSYSSerial( "    Hight prealarm : ");
    vENGINEenbToStr( fuel.hightPreAlarm.enb, buf );
    vSYSSerial( buf );
    vSYSSerial( "\r\n" );

    vSYSSerial( ">>Coolant temperature \r\n" );
    vSYSSerial( "    Type           : ");
    vSYSSerial( cSensorTypes[coolant.temp.type] );
    vSYSSerial( "\r\n" );

    vSYSSerial( ">>Speed sensor     : " );
    vENGINEenbToStr( speed.enb, buf );
    vSYSSerial( buf );
    vSYSSerial( "\r\n" );
    vSYSSerial( "    Teeth number   : " );
    sprintf( buf, "%d", speedToothNumber.value[0U] );
    vSYSSerial( buf );
    vSYSSerial( "\r\n" );
    vSYSSerial( "    Low alarm      : ");
    vENGINEenbToStr( speed.lowAlarm.enb, buf );
    vSYSSerial( buf );
    vSYSSerial( "\r\n" );

    vSYSSerial( "\r\n" );
  #endif
  return;
}
/*----------------------------------------------------------------------------*/
void vLOGICprintEngineStatus ( ENGINE_STATUS status )
{
  #if ( DEBUG_SERIAL_STATUS > 0U )
    vSYSSerial( ">>Engine status: " );
    vSYSSerial( engineStatusStr[status] );
    vSYSSerial( "\r\n" );
  #endif
  return;
}
/*----------------------------------------------------------------------------*/
void vLOGICprintEngineCmd ( ENGINE_COMMAND cmd )
{
  #if ( DEBUG_SERIAL_STATUS > 0U )
    vSYSSerial( ">>Engine command: " );
    vSYSSerial( engineCmdStr[cmd] );
    vSYSSerial( "\r\n" );
  #endif
  return;
}
/*----------------------------------------------------------------------------*/
void vLOGICprintStarterStatus ( STARTER_STATUS status )
{
  #if ( DEBUG_SERIAL_STATUS > 0U )
    vSYSSerial( ">>Starter status: " );
    vSYSSerial( starterStatusStr[status] );
    vSYSSerial( "\r\n" );
  #endif
  return;
}
/*----------------------------------------------------------------------------*/
void vLOGICprintPlanStopStatus ( PLAN_STOP_STATUS status )
{
  #if ( DEBUG_SERIAL_STATUS > 0U )
    vSYSSerial( ">>Plan stop status: " );
    vSYSSerial( planStopStatusStr[status] );
    vSYSSerial( "\r\n" );
  #endif
  return;
}
/*----------------------------------------------------------------------------*/
void vENGINEdataInit ( void )
{
  oil.pressure.type                            = getBitMap( &oilPressureSetup, 0U );
  oil.pressure.chart                           = &oilSensorChart;
  oil.pressure.get                             = xADCGetSOP;
  oil.pressure.cutout.enb                      = getBitMap( &oilPressureSetup, 1U );
  oil.pressure.cutout.active                   = PERMISSION_ENABLE;
  oil.pressure.cutout.level                    = SENSOR_CUTOUT_LEVEL;
  oil.pressure.cutout.status                   = ALARM_STATUS_IDLE;
  oil.pressure.cutout.timer.delay              = 0U;
  oil.pressure.cutout.timer.id                 = LOGIC_DEFAULT_TIMER_ID;
  oil.pressure.cutout.track.ack                = PERMISSION_DISABLE;
  oil.pressure.cutout.track.event.action       = ACTION_EMERGENCY_STOP;
  oil.pressure.cutout.track.event.type         = EVENT_OIL_SENSOR_ERROR;
  oil.pressure.cutout.track.id                 = 0U;
  oil.pressure.cutout.track.relax.enb          = PERMISSION_DISABLE;
  oil.pressure.cutout.track.relax.event.action = ACTION_NONE;
  oil.pressure.cutout.track.relax.event.type   = EVENT_NONE;
  oil.pressure.cutout.track.trig               = TRIGGER_IDLE;
  oil.pressure.cutout.type                     = ALARM_LEVEL_HIGHT;
  oil.pressure.status                          = SENSOR_STATUS_NORMAL;
  if ( ( oil.pressure.type == SENSOR_TYPE_RESISTIVE ) || ( oil.pressure.type == SENSOR_TYPE_CURRENT ) )
  {
    oil.alarm.enb   = getBitMap( &oilPressureSetup, 2U );
    oil.alarm.level = getValue( &oilPressureAlarmLevel );
    oil.alarm.type  = ALARM_LEVEL_HIGHT;
  }
  else
  {
    oil.alarm.enb   = 1U;
    oil.alarm.level = dryContactTrigLevel;
    oil.alarm.type  = ALARM_LEVEL_LOW;
  }
  oil.alarm.timer.delay              = 0U;
  oil.alarm.timer.id                 = LOGIC_DEFAULT_TIMER_ID;
  oil.alarm.track.event.type         = EVENT_OIL_LOW_PRESSURE;
  oil.alarm.track.event.action       = ACTION_EMERGENCY_STOP;
  oil.alarm.track.relax.enb          = PERMISSION_DISABLE;
  oil.alarm.track.relax.event.action = ACTION_NONE;
  oil.alarm.track.relax.event.type   = ACTION_NONE;
  oil.alarm.track.ack                = PERMISSION_DISABLE;
  oil.alarm.track.id                 = 0U;
  oil.alarm.track.trig               = TRIGGER_IDLE;
  oil.alarm.active                   = PERMISSION_DISABLE;
  oil.alarm.status                   = ALARM_STATUS_IDLE;

  if ( ( oil.pressure.type == SENSOR_TYPE_RESISTIVE ) || ( oil.pressure.type == SENSOR_TYPE_CURRENT ) )
  {
    oil.preAlarm.enb                      = getBitMap( &oilPressureSetup, 3U );
    oil.preAlarm.active                   = 0U;
    oil.preAlarm.type                     = ALARM_LEVEL_HIGHT;
    oil.preAlarm.level                    = getValue( &oilPressurePreAlarmLevel );
    oil.preAlarm.timer.delay              = 0U;
    oil.preAlarm.timer.id                 = LOGIC_DEFAULT_TIMER_ID;
    oil.preAlarm.track.event.type         = EVENT_OIL_LOW_PRESSURE;
    oil.preAlarm.track.event.action       = ACTION_WARNING;
    oil.preAlarm.track.relax.enb          = 0U;
    oil.preAlarm.track.relax.event.action = ACTION_NONE;
    oil.preAlarm.track.relax.event.type   = EVENT_NONE;
    oil.preAlarm.track.ack                = PERMISSION_ENABLE;
    oil.preAlarm.track.id                 = 0U;
    oil.preAlarm.track.trig               = TRIGGER_IDLE;
    oil.preAlarm.status                   = ALARM_STATUS_IDLE;
  }
  else
  {
    oil.preAlarm.enb = 0U;
  }
  /*--------------------------------------------------------------*/
  coolant.temp.type                            = getBitMap( &coolantTempSetup, 0U );
  coolant.temp.chart                           = &coolantSensorChart;
  coolant.temp.get                             = xADCGetSCT;
  coolant.temp.cutout.enb                      = getBitMap( &coolantTempSetup, 1U );
  coolant.temp.cutout.active                   = PERMISSION_ENABLE;
  coolant.temp.cutout.level                    = SENSOR_CUTOUT_LEVEL;
  coolant.temp.cutout.timer.delay              = 0U;
  coolant.temp.cutout.timer.id                 = LOGIC_DEFAULT_TIMER_ID;
  coolant.temp.cutout.track.ack                = PERMISSION_DISABLE;
  coolant.temp.cutout.track.event.action       = ACTION_EMERGENCY_STOP;
  coolant.temp.cutout.track.event.type         = EVENT_ENGINE_TEMP_SENSOR_ERROR;
  coolant.temp.cutout.track.id                 = 0U;
  coolant.temp.cutout.track.relax.enb          = PERMISSION_DISABLE;
  coolant.temp.cutout.track.relax.event.action = ACTION_NONE;
  coolant.temp.cutout.track.relax.event.type   = EVENT_NONE;
  coolant.temp.cutout.track.trig               = TRIGGER_IDLE;
  coolant.temp.cutout.type                     = ALARM_LEVEL_HIGHT;
  coolant.temp.cutout.status                   = ALARM_STATUS_IDLE;
  coolant.temp.status                          = SENSOR_STATUS_NORMAL;
  if ( ( coolant.temp.type == SENSOR_TYPE_RESISTIVE ) || ( coolant.temp.type == SENSOR_TYPE_CURRENT ) )
  {
    coolant.alarm.enb   = getBitMap( &coolantTempSetup, 2U );
    coolant.alarm.type  = ALARM_LEVEL_HIGHT;
    coolant.alarm.level = getValue( &coolantHightTempAlarmLevel );
  }
  else
  {
    coolant.alarm.enb   = PERMISSION_ENABLE;
    coolant.alarm.type  = ALARM_LEVEL_LOW;
    coolant.alarm.level = dryContactTrigLevel;
  }
  coolant.alarm.active                   = PERMISSION_ENABLE;
  coolant.alarm.timer.delay              = 0U;
  coolant.alarm.timer.id                 = LOGIC_DEFAULT_TIMER_ID;
  coolant.alarm.track.event.type         = EVENT_ENGINE_HIGHT_TEMP;
  coolant.alarm.track.event.action       = ACTION_EMERGENCY_STOP;
  coolant.alarm.track.relax.enb          = PERMISSION_DISABLE;
  coolant.alarm.track.relax.event.action = ACTION_NONE;
  coolant.alarm.track.relax.event.type   = EVENT_NONE;
  coolant.alarm.track.ack                = PERMISSION_DISABLE;
  coolant.alarm.track.id                 = 0U;
  coolant.alarm.track.trig               = TRIGGER_IDLE;
  coolant.alarm.status                   = ALARM_STATUS_IDLE;
  if ( ( coolant.temp.type == SENSOR_TYPE_RESISTIVE ) || ( coolant.temp.type == SENSOR_TYPE_CURRENT ) )
  {
    coolant.preAlarm.enb                      = getBitMap( &coolantTempSetup, 3U );
    coolant.preAlarm.active                   = PERMISSION_ENABLE;
    coolant.preAlarm.type                     = ALARM_LEVEL_HIGHT;
    coolant.preAlarm.level                    = getValue( &coolantHightTempPreAlarmLevel );
    coolant.preAlarm.timer.delay              = 0U;
    coolant.preAlarm.timer.id                 = LOGIC_DEFAULT_TIMER_ID;
    coolant.preAlarm.track.ack                = PERMISSION_ENABLE;
    coolant.preAlarm.track.event.type         = EVENT_ENGINE_HIGHT_TEMP;
    coolant.preAlarm.track.event.action       = ACTION_WARNING;
    coolant.preAlarm.track.id                 = 0U;
    coolant.preAlarm.track.relax.enb          = PERMISSION_DISABLE;
    coolant.preAlarm.track.relax.event.action = ACTION_NONE;
    coolant.preAlarm.track.relax.event.type   = EVENT_NONE;
    coolant.preAlarm.track.trig               = TRIGGER_IDLE;
    coolant.preAlarm.status                   = ALARM_STATUS_IDLE;
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
  preHeater.relay.enb    = getBitMap( &engineSetup, 1U );
  preHeater.active       = PERMISSION_DISABLE;
  preHeater.level        = getValue( &enginePreHeatLevel );
  preHeater.relay.set    = vFPOsetPreheater;
  preHeater.relay.status = RELAY_OFF;
  preHeater.status       = RELAY_DELAY_IDLE;
  preHeater.timer.delay  = getValue( &enginePreHeatDelay );
  /*--------------------------------------------------------------*/
  fuel.level.type                            = getBitMap( &fuelLevelSetup, 0U );
  fuel.level.chart                           = &fuelSensorChart;
  fuel.level.get                             = xADCGetSFL;
  fuel.level.cutout.enb                      = getBitMap( &fuelLevelSetup, 1U );
  fuel.level.cutout.active                   = PERMISSION_ENABLE;
  fuel.level.cutout.level                    = SENSOR_CUTOUT_LEVEL;
  fuel.level.cutout.status                   = ALARM_STATUS_IDLE;
  fuel.level.cutout.timer.delay              = 0U;
  fuel.level.cutout.timer.id                 = LOGIC_DEFAULT_TIMER_ID;
  fuel.level.cutout.track.ack                = PERMISSION_ENABLE;
  fuel.level.cutout.track.event.action       = ACTION_EMERGENCY_STOP;
  fuel.level.cutout.track.event.type         = EVENT_FUEL_LEVEL_SENSOR_ERROR;
  fuel.level.cutout.track.id                 = 0U;
  fuel.level.cutout.track.relax.enb          = PERMISSION_DISABLE;
  fuel.level.cutout.track.relax.event.action = ACTION_NONE;
  fuel.level.cutout.track.relax.event.type   = EVENT_NONE;
  fuel.level.cutout.track.trig               = TRIGGER_IDLE;
  fuel.level.cutout.type                     = ALARM_LEVEL_HIGHT;
  fuel.level.status                          = SENSOR_STATUS_NORMAL;

  fuel.lowAlarm.enb              = getBitMap( &fuelLevelSetup, 2U );
  fuel.lowAlarm.active           = PERMISSION_ENABLE;
  fuel.lowAlarm.type             = ALARM_LEVEL_LOW;
  fuel.lowAlarm.level            = getValue( &fuelLevelLowAlarmLevel );
  fuel.lowAlarm.timer.delay      = getValue( &fuelLevelLowAlarmDelay );
  fuel.lowAlarm.timer.id         = LOGIC_DEFAULT_TIMER_ID;
  fuel.lowAlarm.track.event.type = EVENT_FUEL_LOW_LEVEL;
  if ( getBitMap( &fuelLevelSetup, 3U ) == 0U )
  {
    fuel.lowAlarm.track.event.action = ACTION_EMERGENCY_STOP;
    fuel.lowAlarm.track.ack          = PERMISSION_DISABLE;
  }
  else
  {
    fuel.lowAlarm.track.event.action = ACTION_LOAD_SHUTDOWN;
    fuel.lowAlarm.track.ack          = PERMISSION_ENABLE;
  }
  fuel.lowAlarm.track.relax.enb          = PERMISSION_DISABLE;
  fuel.lowAlarm.track.relax.event.action = ACTION_NONE;
  fuel.lowAlarm.track.relax.event.type   = EVENT_NONE;
  fuel.lowAlarm.track.id                 = 0U;
  fuel.lowAlarm.track.trig               = TRIGGER_IDLE;
  fuel.lowAlarm.status                   = ALARM_STATUS_IDLE;

  fuel.lowPreAlarm.enb                      = getBitMap( &fuelLevelSetup, 4U );
  fuel.lowPreAlarm.active                   = PERMISSION_ENABLE;
  fuel.lowPreAlarm.type                     = ALARM_LEVEL_LOW;
  fuel.lowPreAlarm.level                    = getValue( &fuelLevelLowPreAlarmLevel );
  fuel.lowPreAlarm.timer.delay              = getValue( &fuelLevelLowPreAlarmDelay );
  fuel.lowPreAlarm.timer.id                 = LOGIC_DEFAULT_TIMER_ID;
  fuel.lowPreAlarm.track.ack                = PERMISSION_ENABLE;
  fuel.lowPreAlarm.track.event.type         = EVENT_FUEL_LOW_LEVEL;
  fuel.lowPreAlarm.track.event.action       = ACTION_WARNING;
  fuel.lowPreAlarm.track.id                 = 0U;
  fuel.lowPreAlarm.track.relax.enb          = PERMISSION_DISABLE;
  fuel.lowPreAlarm.track.relax.event.action = ACTION_NONE;
  fuel.lowPreAlarm.track.relax.event.type   = EVENT_NONE;
  fuel.lowPreAlarm.track.trig               = TRIGGER_IDLE;
  fuel.lowPreAlarm.status                   = ALARM_STATUS_IDLE;

  fuel.hightPreAlarm.enb                      = getBitMap( &fuelLevelSetup, 5U );
  fuel.hightPreAlarm.active                   = PERMISSION_ENABLE;
  fuel.hightPreAlarm.type                     = ALARM_LEVEL_HIGHT;
  fuel.hightPreAlarm.level                    = getValue( &fuelLevelHightPreAlarmLevel );
  fuel.hightPreAlarm.timer.delay              = getValue( &fuelLevelHightPreAlarmDelay );
  fuel.hightPreAlarm.timer.id                 = LOGIC_DEFAULT_TIMER_ID;
  fuel.hightPreAlarm.track.ack                = PERMISSION_ENABLE;
  fuel.hightPreAlarm.track.event.type         = EVENT_FUEL_HIGHT_LEVEL;
  fuel.hightPreAlarm.track.event.action       = ACTION_WARNING;
  fuel.hightPreAlarm.track.id                 = 0U;
  fuel.hightPreAlarm.track.relax.enb          = PERMISSION_DISABLE;
  fuel.hightPreAlarm.track.relax.event.action = ACTION_NONE;
  fuel.hightPreAlarm.track.relax.event.type   = EVENT_NONE;
  fuel.hightPreAlarm.track.trig               = TRIGGER_IDLE;
  fuel.hightPreAlarm.status                   = ALARM_STATUS_IDLE;

  fuel.hightAlarm.enb         = getBitMap( &fuelLevelSetup, 6U );
  fuel.hightAlarm.active      = PERMISSION_ENABLE;
  fuel.hightAlarm.type        = ALARM_LEVEL_HIGHT;
  fuel.hightAlarm.level       = getValue( &fuelLevelHightAlarmLevel );
  fuel.hightAlarm.timer.delay = getValue( &fuelLevelHightAlarmDelay );
  fuel.hightAlarm.timer.id    = LOGIC_DEFAULT_TIMER_ID;
  fuel.hightAlarm.track.event.type = EVENT_FUEL_HIGHT_LEVEL;
  if ( getBitMap( &fuelLevelSetup, 7U ) == 0U )
  {
    fuel.hightAlarm.track.event.action = ACTION_EMERGENCY_STOP;
    fuel.hightAlarm.track.ack          = PERMISSION_DISABLE;
  }
  else
  {
    fuel.hightAlarm.track.event.action = ACTION_LOAD_SHUTDOWN;
    fuel.hightAlarm.track.ack          = PERMISSION_ENABLE;
  }
  fuel.hightAlarm.track.id                 = 0U;
  fuel.hightAlarm.track.relax.enb          = PERMISSION_DISABLE;
  fuel.hightAlarm.track.relax.event.action = ACTION_NONE;
  fuel.hightAlarm.track.relax.event.type   = EVENT_NONE;
  fuel.hightAlarm.track.trig               = TRIGGER_IDLE;
  fuel.hightAlarm.status                   = ALARM_STATUS_IDLE;

  fuel.booster.relay.enb    = getBitMap( &fuelLevelSetup, 8U );
  fuel.booster.onLevel      = getValue( &fuelPumpOnLevel );
  fuel.booster.offLevel     = getValue( &fuelPumpOffLevel );
  fuel.booster.relay.set    = vFPOsetBooster;
  fuel.booster.relay.status = RELAY_OFF;

  fuel.pump.enb    = PERMISSION_ENABLE;
  fuel.pump.set    = vFPOsetPump;
  fuel.pump.status = RELAY_OFF;
  /*--------------------------------------------------------------*/
  battery.get                               = xADCGetVDD;
  battery.lowAlarm.enb                      = getBitMap( &batteryAlarms, 0U );
  battery.lowAlarm.active                   = PERMISSION_DISABLE;
  battery.lowAlarm.type                     = ALARM_LEVEL_LOW;
  battery.lowAlarm.level                    = getValue( &batteryUnderVoltageLevel );
  battery.lowAlarm.timer.delay              = getValue( &batteryUnderVoltageDelay );
  battery.lowAlarm.timer.id                 = LOGIC_DEFAULT_TIMER_ID;
  battery.lowAlarm.track.ack                = PERMISSION_ENABLE;
  battery.lowAlarm.track.event.type         = EVENT_BATTERY_LOW;
  battery.lowAlarm.track.event.action       = ACTION_WARNING;
  battery.lowAlarm.track.id                 = 0U;
  battery.lowAlarm.track.relax.enb          = PERMISSION_DISABLE;
  battery.lowAlarm.track.relax.event.action = ACTION_NONE;
  battery.lowAlarm.track.relax.event.type   = EVENT_NONE;
  battery.lowAlarm.track.trig               = TRIGGER_IDLE;
  battery.lowAlarm.status                   = ALARM_STATUS_IDLE;

  battery.hightAlarm.enb                      = getBitMap( &batteryAlarms, 1U );
  battery.hightAlarm.active                   = PERMISSION_DISABLE;
  battery.hightAlarm.type                     = ALARM_LEVEL_HIGHT;
  battery.hightAlarm.level                    = getValue( &batteryOverVoltageLevel );
  battery.hightAlarm.timer.delay              = getValue( &batteryOverVoltageDelay );
  battery.hightAlarm.timer.id                 = LOGIC_DEFAULT_TIMER_ID;
  battery.hightAlarm.track.ack                = PERMISSION_ENABLE;
  battery.hightAlarm.track.event.type         = EVENT_BATTERY_HIGHT;
  battery.hightAlarm.track.event.action       = ACTION_WARNING;
  battery.hightAlarm.track.id                 = 0U;
  battery.hightAlarm.track.relax.enb          = PERMISSION_DISABLE;
  battery.hightAlarm.track.relax.event.action = ACTION_NONE;
  battery.hightAlarm.track.relax.event.type   = EVENT_NONE;
  battery.hightAlarm.track.trig               = TRIGGER_IDLE;
  battery.hightAlarm.status                   = ALARM_STATUS_IDLE;
  /*--------------------------------------------------------------*/
  charger.get                                 = getChargerVoltage;
  charger.hightAlarm.enb                      = getBitMap( &batteryAlarms, 2U );
  charger.hightAlarm.active                   = PERMISSION_DISABLE;
  charger.hightAlarm.type                     = ALARM_LEVEL_HIGHT;
  charger.hightAlarm.level                    = getValue( &batteryChargeShutdownLevel );
  charger.hightAlarm.timer.delay              = getValue( &batteryChargeShutdownDelay );
  charger.hightAlarm.timer.id                 = LOGIC_DEFAULT_TIMER_ID;
  charger.hightAlarm.track.ack                = PERMISSION_DISABLE;
  charger.hightAlarm.track.event.type         = EVENT_CHARGER_FAIL;
  charger.hightAlarm.track.event.action       = ACTION_EMERGENCY_STOP;
  charger.hightAlarm.track.id                 = 0U;
  charger.hightAlarm.track.relax.enb          = PERMISSION_DISABLE;
  charger.hightAlarm.track.relax.event.action = ACTION_NONE;
  charger.hightAlarm.track.relax.event.type   = EVENT_NONE;
  charger.hightAlarm.track.trig               = TRIGGER_IDLE;
  charger.hightAlarm.status                   = ALARM_STATUS_IDLE;

  charger.hightPreAlarm.enb                      = getBitMap( &batteryAlarms, 3U );
  charger.hightPreAlarm.active                   = PERMISSION_DISABLE;
  charger.hightPreAlarm.type                     = ALARM_LEVEL_HIGHT;
  charger.hightPreAlarm.level                    = getValue( &batteryChargeWarningLevel );
  charger.hightPreAlarm.timer.delay              = getValue( &batteryChargeWarningDelay );
  charger.hightPreAlarm.timer.id                 = LOGIC_DEFAULT_TIMER_ID;
  charger.hightPreAlarm.track.ack                = PERMISSION_ENABLE;
  charger.hightPreAlarm.track.event.type         = EVENT_CHARGER_FAIL;
  charger.hightPreAlarm.track.event.action       = ACTION_WARNING;
  charger.hightPreAlarm.track.id                 = 0U;
  charger.hightPreAlarm.track.relax.enb          = PERMISSION_DISABLE;
  charger.hightPreAlarm.track.relax.event.action = ACTION_NONE;
  charger.hightPreAlarm.track.relax.event.type   = EVENT_NONE;
  charger.hightPreAlarm.track.trig               = TRIGGER_IDLE;
  charger.hightPreAlarm.status                   = ALARM_STATUS_IDLE;
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
  speed.enb    = getBitMap( &speedSetup, 0U );
  speed.status = SENSOR_STATUS_NORMAL;
  speed.get    = fVRgetSpeed;

  speed.lowAlarm.enb                      = getBitMap( &speedSetup, 1U );
  speed.lowAlarm.active                   = PERMISSION_DISABLE;
  speed.lowAlarm.type                     = ALARM_LEVEL_LOW;
  speed.lowAlarm.level                    = getValue( &speedLowAlarmLevel );
  speed.lowAlarm.timer.delay              = 0U;
  speed.lowAlarm.timer.id                 = LOGIC_DEFAULT_TIMER_ID;
  speed.lowAlarm.track.ack                = PERMISSION_DISABLE;
  speed.lowAlarm.track.event.type         = EVENT_SPEED_LOW;
  speed.lowAlarm.track.event.action       = ACTION_EMERGENCY_STOP;
  speed.lowAlarm.track.id                 = 0U;
  speed.lowAlarm.track.relax.enb          = PERMISSION_DISABLE;
  speed.lowAlarm.track.relax.event.action = ACTION_NONE;
  speed.lowAlarm.track.relax.event.type   = EVENT_NONE;
  speed.lowAlarm.track.trig               = TRIGGER_IDLE;
  speed.lowAlarm.status                   = ALARM_STATUS_IDLE;

  speed.hightAlarm.enb                      = PERMISSION_ENABLE;
  speed.hightAlarm.active                   = PERMISSION_ENABLE;
  speed.hightAlarm.type                     = ALARM_LEVEL_HIGHT;
  speed.hightAlarm.level                    = getValue( &speedHightAlarmLevel );
  speed.hightAlarm.timer.delay              = 0U;
  speed.hightAlarm.timer.id                 = LOGIC_DEFAULT_TIMER_ID;
  speed.hightAlarm.track.ack                = PERMISSION_DISABLE;
  speed.hightAlarm.track.event.type         = EVENT_SPEED_HIGHT;
  speed.hightAlarm.track.event.action       = ACTION_EMERGENCY_STOP;
  speed.hightAlarm.track.id                 = 0U;
  speed.hightAlarm.track.relax.enb          = PERMISSION_DISABLE;
  speed.hightAlarm.track.relax.event.action = ACTION_NONE;
  speed.hightAlarm.track.relax.event.type   = EVENT_NONE;
  speed.hightAlarm.track.trig               = TRIGGER_IDLE;
  speed.hightAlarm.status                   = ALARM_STATUS_IDLE;
  /*--------------------------------------------------------------*/
  stopSolenoid.relay.enb    = uFPOisEnable( FPO_FUN_STOP_SOLENOID );
  stopSolenoid.relay.set    = vFPOsetStopSolenoid;
  stopSolenoid.relay.status = RELAY_OFF;
  stopSolenoid.timer.id     = LOGIC_DEFAULT_TIMER_ID;
  stopSolenoid.timer.delay  = getValue( configReg[TIMER_SOLENOID_HOLD_ADR] );
  stopSolenoid.triger       = TRIGGER_IDLE;
  stopSolenoid.status       = RELAY_DELAY_IDLE;
  /*--------------------------------------------------------------*/
  idleRelay.enb    = uFPOisEnable( FPO_FUN_IDLING );
  idleRelay.set    = vFPOsetIdle;
  idleRelay.status = RELAY_OFF;
  /*--------------------------------------------------------------*/
  maintence.oil.enb              = getBitMap( &maintenanceAlarms, 0U );
  maintence.oil.active           = PERMISSION_DISABLE;
  maintence.oil.type             = ALARM_LEVEL_HIGHT;
  maintence.oil.level            = getValue( &maintenanceAlarmOilTime );
  maintence.oil.timer.delay      = 0U;
  maintence.oil.timer.id         = LOGIC_DEFAULT_TIMER_ID;
  maintence.oil.track.event.type = EVENT_MAINTENANCE_OIL;
  maintence.oil.track.id         = 0U;
  if ( getBitMap( &maintenanceAlarms, 1U ) == 0U )
  {
    maintence.oil.track.event.action = ACTION_PLAN_STOP;
    maintence.oil.track.ack          = PERMISSION_DISABLE;
  }
  else
  {
    maintence.oil.track.event.action = ACTION_WARNING;
    maintence.oil.track.ack          = PERMISSION_ENABLE;
  }
  maintence.oil.track.relax.enb          = PERMISSION_DISABLE;
  maintence.oil.track.relax.event.action = ACTION_NONE;
  maintence.oil.track.relax.event.type   = EVENT_NONE;
  maintence.oil.track.trig               = TRIGGER_IDLE;
  maintence.oil.status                   = ALARM_STATUS_IDLE;

  maintence.air.enb              = getBitMap( &maintenanceAlarms, 2U );
  maintence.air.active           = PERMISSION_DISABLE;
  maintence.air.type             = ALARM_LEVEL_HIGHT;
  maintence.air.level            = getValue( &maintenanceAlarmAirTime );
  maintence.air.timer.delay      = 0U;
  maintence.air.timer.id         = LOGIC_DEFAULT_TIMER_ID;
  maintence.air.track.event.type = EVENT_MAINTENANCE_AIR;
  maintence.air.track.id         = 0U;
  if ( getBitMap( &maintenanceAlarms, 3U ) == 0U )
  {
    maintence.air.track.event.action = ACTION_PLAN_STOP;
    maintence.air.track.ack          = PERMISSION_DISABLE;
  }
  else
  {
    maintence.air.track.event.action = ACTION_WARNING;
    maintence.air.track.ack          = PERMISSION_ENABLE;
  }
  maintence.air.track.relax.enb          = PERMISSION_DISABLE;
  maintence.air.track.relax.event.action = ACTION_NONE;
  maintence.air.track.relax.event.type   = EVENT_NONE;
  maintence.air.track.trig               = TRIGGER_IDLE;
  maintence.air.status                   = ALARM_STATUS_IDLE;

  maintence.fuel.enb              = getBitMap( &maintenanceAlarms, 4U );
  maintence.fuel.active           = PERMISSION_DISABLE;
  maintence.fuel.type             = ALARM_LEVEL_HIGHT;
  maintence.fuel.level            = getValue( &maintenanceAlarmFuelTime );
  maintence.fuel.timer.delay      = 0U;
  maintence.fuel.timer.id         = LOGIC_DEFAULT_TIMER_ID;
  maintence.fuel.track.event.type = EVENT_MAINTENANCE_FUEL;
  if ( getBitMap( &maintenanceAlarms, 5U ) == 0U )
  {
    maintence.fuel.track.event.action = ACTION_PLAN_STOP;
    maintence.fuel.track.ack          = PERMISSION_DISABLE;
  }
  else
  {
    maintence.fuel.track.event.action = ACTION_WARNING;
    maintence.fuel.track.ack          = PERMISSION_ENABLE;
  }
  maintence.fuel.track.relax.enb          = PERMISSION_DISABLE;
  maintence.fuel.track.relax.event.action = ACTION_NONE;
  maintence.fuel.track.relax.event.type   = EVENT_NONE;
  maintence.fuel.track.trig               = TRIGGER_IDLE;
  maintence.fuel.status                   = ALARM_STATUS_IDLE;

  return;
}
/*----------------------------------------------------------------------------*/
void vENGINEresetAlarms ( void )
{
  speed.hightAlarm.track.trig      = TRIGGER_IDLE;
  speed.lowAlarm.track.trig        = TRIGGER_IDLE;
  charger.hightAlarm.track.trig    = TRIGGER_IDLE;
  charger.hightPreAlarm.track.trig = TRIGGER_IDLE;
  battery.hightAlarm.track.trig    = TRIGGER_IDLE;
  battery.lowAlarm.track.trig      = TRIGGER_IDLE;
  fuel.hightAlarm.track.trig       = TRIGGER_IDLE;
  fuel.hightPreAlarm.track.trig    = TRIGGER_IDLE;
  fuel.lowAlarm.track.trig         = TRIGGER_IDLE;
  fuel.lowPreAlarm.track.trig      = TRIGGER_IDLE;
  coolant.alarm.track.trig         = TRIGGER_IDLE;
  coolant.preAlarm.track.trig      = TRIGGER_IDLE;
  oil.alarm.track.trig             = TRIGGER_IDLE;
  oil.preAlarm.track.trig          = TRIGGER_IDLE;
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vENGINEinit ( void )
{
  vENGINEdataInit();
  /*--------------------------------------------------------------*/
  vFPOsetReadyToStart( RELAY_ON );
  /*--------------------------------------------------------------*/
  pEngineCommandQueue = xQueueCreateStatic( ENGINE_COMMAND_QUEUE_LENGTH,
                                            sizeof( ENGINE_COMMAND ),
                                            engineCommandBuffer,
                                            &xEngineCommandQueue );
  const osThreadAttr_t engineTask_attributes = {
    .name       = "engineTask",
    .priority   = ( osPriority_t ) ENGINE_TASK_PRIORITY,
    .stack_size = ENGINE_TASK_STACK_SIZE
  };
  engineHandle = osThreadNew( vENGINEtask, NULL, &engineTask_attributes );
  vENGINEprintSetup();
  return;
}

QueueHandle_t pENGINEgetCommandQueue ( void )
{
  return pEngineCommandQueue;
}

void vENGINEemergencyStop ( void )
{
  ENGINE_COMMAND cmd = ENGINE_CMD_EMEGENCY_STOP;
  xQueueSend( pENGINEgetCommandQueue(), &cmd, portMAX_DELAY );
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

ENGINE_STATUS eENGINEgetEngineStatus ( void )
{
  return engine.status;
}

void vENGINEtask ( void const* argument )
{
  fix16_t        oilVal        = 0U;
  fix16_t        coolantVal    = 0U;
  fix16_t        fuelVal       = 0U;
  fix16_t        speedVal      = 0U;
  fix16_t        batteryVal    = 0U;
  fix16_t        chargerVal    = 0U;
  SYSTEM_TIMER   commonTimer   = { 0U };
  ENGINE_COMMAND inputCmd      = ENGINE_CMD_NONE;
  SYSTEM_EVENT   event         = { 0U };
  uint32_t       inputNotifi   = 0U;
  uint8_t        idleStopError = 0U;
  ENGINE_COMMAND lastCmd       = ENGINE_CMD_NONE;
  for (;;)
  {
    /*-------------------- Read system notification --------------------*/
    if ( xTaskNotifyWait( 0U, 0xFFFFFFFFU, &inputNotifi, TASK_NOTIFY_WAIT_DELAY ) == pdPASS )
    {
      if ( ( inputNotifi & DATA_API_MESSAGE_REINIT ) > 0U )
      {
        vENGINEdataInit();
      }
    }
    /*----------------------- Read input command -----------------------*/
    if ( xQueueReceive( pENGINEgetCommandQueue(), &inputCmd, 0U ) == pdPASS )
    {
      if ( engine.cmd != inputCmd )
      {
        switch ( engine.status )
        {
          case ENGINE_STATUS_IDLE:
            engine.cmd = inputCmd;
            vLOGICprintEngineCmd( engine.cmd );
            break;
          case ENGINE_STATUS_EMERGENCY_STOP:
            if ( inputCmd == ENGINE_CMD_RESET_TO_IDLE )
            {
              engine.cmd = inputCmd;
              vLOGICprintEngineCmd( engine.cmd );
            }
            break;
          case ENGINE_STATUS_BUSY_STARTING:
            if ( inputCmd == ENGINE_CMD_EMEGENCY_STOP )
            {
              engine.cmd = inputCmd;
              vLOGICprintEngineCmd( engine.cmd );
            }
            break;
          case ENGINE_STATUS_BUSY_STOPPING:
            if ( inputCmd == ENGINE_CMD_EMEGENCY_STOP )
            {
              engine.cmd = inputCmd;
              vLOGICprintEngineCmd( engine.cmd );
            }
            break;
          case ENGINE_STATUS_WORK:
            engine.cmd = inputCmd;
            vLOGICprintEngineCmd( engine.cmd );
            break;
          case ENGINE_STATUS_WORK_ON_IDLE:
            engine.cmd = inputCmd;
            vLOGICprintEngineCmd( engine.cmd );
            break;
          case ENGINE_STATUS_WORK_GOTO_NOMINAL:
            if ( inputCmd == ENGINE_CMD_EMEGENCY_STOP )
            {
              engine.cmd = inputCmd;
              vLOGICprintEngineCmd( engine.cmd );
            }
            break;
          case ENGINE_STATUS_FAIL_STARTING:
            if ( ( inputCmd == ENGINE_CMD_EMEGENCY_STOP ) ||
                 ( inputCmd == ENGINE_CMD_RESET_TO_IDLE ) )
            {
              engine.cmd = inputCmd;
              vLOGICprintEngineCmd( engine.cmd );
            }
            break;
          case ENGINE_STATUS_FAIL_STOPPING:
            if ( ( inputCmd == ENGINE_CMD_EMEGENCY_STOP ) ||
                 ( inputCmd == ENGINE_CMD_RESET_TO_IDLE ) )
            {
              engine.cmd = inputCmd;
              vLOGICprintEngineCmd( engine.cmd );
            }
            break;
          default:
            engine.cmd = inputCmd;
            vLOGICprintEngineCmd( engine.cmd );
            break;
        }
      }
    }
    /*------------------------- Process inputs -------------------------*/
    oilVal     = fOILprocess();
    coolantVal = fCOOLANTprocess();
    fuelVal    = fFUELprocess();
    speedVal   = fSPEEDprocess();
    batteryVal = fBATTERYprocess();
    chargerVal = fCHARGERprocess();
    vENGINEmileageProcess();
    /*------------------------- Input commands -------------------------*/
    if ( engine.cmd != lastCmd )
    {
      lastCmd = engine.cmd;
    }
    switch ( engine.cmd )
    {
      case ENGINE_CMD_NONE:
        if ( ( idleStopError == 0U ) && ( engine.status == ENGINE_STATUS_IDLE ) )
        {
          if ( uENGINEisStop( oilVal, speedVal ) == 0U )
          {
            idleStopError   = 1U;
            event.action    = ACTION_EMERGENCY_STOP;
            event.type      = EVENT_STOP_FAIL;
            vSYSeventSend( event, NULL );
          }
        }
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
              vFPOsetReadyToStart( RELAY_OFF );
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
              //vELECTROalarmStartDisable();
              if ( ( engine.startCheckOil > 0U ) && ( oilVal != ENGINE_OIL_PRESSURE_TRESH_HOLD ) )
              {
                starter.status = STARTER_FAIL;
              }
              engine.status = ENGINE_STATUS_BUSY_STARTING;
              fuel.pump.set( RELAY_ON );
              commonTimer.delay = starter.startDelay;
              vLOGICstartTimer( &commonTimer );
              starter.status = STARTER_START_DELAY;
              vLOGICprintStarterStatus( starter.status );
              break;
            case STARTER_START_DELAY:
              if ( uLOGICisTimer( commonTimer ) > 0U )
              {
                starter.status   = STARTER_READY;
                preHeater.active = 1U;
                vLOGICprintStarterStatus( starter.status );
              }
              break;
            case STARTER_READY:
              starter.startIteration++;
              starter.status = STARTER_CRANKING;
              starter.set( RELAY_ON );
              commonTimer.delay = starter.crankingDelay;
              vLOGICstartTimer( &commonTimer );
              vLOGICprintStarterStatus( starter.status );
              break;
            case STARTER_CRANKING:
              if ( uENGINEisWork( 0U, oilVal, chargerVal, speedVal ) > 0U )
              {
                starter.set( RELAY_OFF );
                starterFinish  = 1U;
                starter.status = STARTER_CONTROL_BLOCK;
                vLOGICresetTimer( commonTimer );
                commonTimer.delay = starter.blockDelay;
                vLOGICstartTimer( &commonTimer );
                vLOGICprintStarterStatus( starter.status );
              }
              if ( uLOGICisTimer( commonTimer ) > 0U )
              {
                starter.set( RELAY_OFF );
                if ( starter.startIteration < starter.startAttempts )
                {
                  starter.status = STARTER_CRANK_DELAY;
                  commonTimer.delay = starter.crankDelay;
                  vLOGICstartTimer( &commonTimer );
                  vLOGICprintStarterStatus( starter.status );
                }
                else
                {
                  fuel.pump.set( RELAY_OFF );
                  vRELAYdelayTrig( &stopSolenoid );
                  preHeater.active    = 0U;
                  starter.status      = STARTER_FAIL;
                  vLOGICprintStarterStatus( starter.status );
                }
              }
              break;
            case STARTER_CRANK_DELAY:
              if ( uLOGICisTimer( commonTimer ) > 0U )
              {
                starter.status = STARTER_READY;
                vLOGICprintStarterStatus( starter.status );
              }
              break;
            case STARTER_CONTROL_BLOCK:
              if ( uLOGICisTimer( commonTimer ) > 0U )
              {
                idleRelay.set( RELAY_ON );
                commonTimer.delay = starter.idlingDelay;
                vLOGICstartTimer( &commonTimer );
                starter.status               = STARTER_IDLE_WORK;
                blockTimerFinish             = 1U;
                speed.hightAlarm.active      = 1U;
                oil.alarm.active             = 1U;
                oil.preAlarm.active          = 1U;
                battery.hightAlarm.active    = 1U;
                battery.lowAlarm.active      = 1U;
                charger.hightAlarm.active    = 1U;
                charger.hightPreAlarm.active = 1U;
                //vELECTROalarmStartToIdle();
                vLOGICprintStarterStatus( starter.status );
              }
              break;
            case STARTER_IDLE_WORK:
              if ( uLOGICisTimer( commonTimer ) > 0U )
              {
                idleRelay.set( RELAY_OFF );
                commonTimer.delay = starter.nominalDelay;
                vLOGICstartTimer( &commonTimer );
                starter.status = STARTER_MOVE_TO_NOMINAL;
                vLOGICprintStarterStatus( starter.status );
              }
              break;
            case STARTER_MOVE_TO_NOMINAL:
              if ( uLOGICisTimer( commonTimer ) > 0U )
              {
                commonTimer.delay = starter.warmingDelay;
                vLOGICstartTimer( &commonTimer );
                starter.status        = STARTER_WARMING;
                speed.lowAlarm.active = 1U;
                //vELECTROalarmIdleEnable();
                vLOGICprintStarterStatus( starter.status );
              }
              break;
            case STARTER_WARMING:
              if ( uLOGICisTimer( commonTimer ) > 0U )
              {
                starter.status   = STARTER_OK;
                preHeater.active = 0U;
                preHeater.relay.set( RELAY_OFF );
                vFPOsetGenReady( RELAY_ON );
                vLOGICprintStarterStatus( starter.status );
              }
              break;
            case STARTER_FAIL:
              engine.status          = ENGINE_STATUS_FAIL_STARTING;
              engine.cmd             = ENGINE_CMD_NONE;
              starter.status         = STARTER_IDLE;
              engine.cmd             = ENGINE_CMD_NONE;
              event.action           = ACTION_EMERGENCY_STOP;
              event.type             = EVENT_START_FAIL;
              starter.startIteration = 0U;
              vSYSeventSend( event, NULL );
              vLOGICprintStarterStatus( starter.status );
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
              vLOGICprintStarterStatus( starter.status );
              break;
            default:
              vLOGICresetTimer( commonTimer );
              engine.status  = ENGINE_STATUS_FAIL_STARTING;
              engine.cmd     = ENGINE_CMD_NONE;
              starter.status = STARTER_FAIL;
              starter.set( RELAY_OFF );
              preHeater.relay.set( RELAY_OFF );
              idleRelay.set( RELAY_OFF );
              fuel.pump.set( RELAY_OFF );
              vFPOsetGenReady( RELAY_OFF );
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
              vFPOsetGenReady( RELAY_OFF );
              commonTimer.delay = planStop.coolingDelay;
              vLOGICstartTimer( &commonTimer );
              vLOGICprintPlanStopStatus( planStop.status );
              break;
            case STOP_COOLDOWN:
              if ( uLOGICisTimer( commonTimer ) > 0U )
              {
                speed.lowAlarm.active = 0U;
                //vELECTROalarmIdleDisable();
                idleRelay.set( RELAY_ON );
                commonTimer.delay = planStop.coolingIdleDelay;
                vLOGICstartTimer( &commonTimer );
                planStop.status = STOP_IDLE_COOLDOWN;
                vLOGICprintPlanStopStatus( planStop.status );
              }
              break;
            case STOP_IDLE_COOLDOWN:
              if ( uLOGICisTimer( commonTimer ) > 0U )
              {
                oil.alarm.active    = 0U;
                oil.preAlarm.active = 0U;
                idleRelay.set( RELAY_OFF );
                fuel.pump.set( RELAY_OFF );
                vRELAYdelayTrig( &stopSolenoid );
                commonTimer.delay = planStop.processDelay;
                vLOGICstartTimer( &commonTimer );
                planStop.status = STOP_PROCESSING;
                vLOGICprintPlanStopStatus( planStop.status );
              }
              break;
            case STOP_PROCESSING:
              if ( uLOGICisTimer( commonTimer ) > 0U )
              {
                planStop.status = STOP_FAIL;
                vLOGICprintPlanStopStatus( planStop.status );
              }
              if ( uENGINEisStop( oilVal, speedVal ) > 0U )
              {
                vLOGICresetTimer( commonTimer );
                planStop.status = STOP_OK;
                vLOGICprintPlanStopStatus( planStop.status );
              }
              break;
            case STOP_FAIL:
              engine.status   = ENGINE_STATUS_FAIL_STOPPING;
              engine.cmd      = ENGINE_CMD_NONE;
              planStop.status = STOP_IDLE;
              event.action    = ACTION_EMERGENCY_STOP;
              event.type      = EVENT_STOP_FAIL;
              vSYSeventSend( event, NULL );
              vLOGICprintPlanStopStatus( planStop.status );
              break;
            case STOP_OK:
              engine.status         = ENGINE_STATUS_IDLE;
              engine.cmd            = ENGINE_CMD_NONE;
              planStop.status       = STOP_IDLE;
              maintence.oil.active  = 0U;
              maintence.air.active  = 0U;
              maintence.fuel.active = 0U;
              vFPOsetReadyToStart( RELAY_ON );
              vLOGICprintPlanStopStatus( planStop.status );
              break;
            default:
              vLOGICresetTimer( commonTimer );
              planStop.status = STOP_FAIL;
              vLOGICprintPlanStopStatus( planStop.status );
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
          //vELECTROalarmIdleDisable();
          idleRelay.set( RELAY_ON );
          vFPOsetGenReady( RELAY_OFF );
          engine.status = ENGINE_STATUS_WORK_ON_IDLE;
          vLOGICprintEngineStatus( engine.status );
        }
        engine.cmd = ENGINE_CMD_NONE;
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
            vLOGICprintEngineStatus( engine.status );
            commonTimer.delay = starter.nominalDelay;
            vLOGICstartTimer( &commonTimer );
            break;
          case ENGINE_STATUS_WORK_GOTO_NOMINAL:
            if ( uLOGICisTimer( commonTimer ) > 0U )
            {
              engine.status = ENGINE_STATUS_WORK;
              vLOGICprintEngineStatus( engine.status );
              speed.lowAlarm.active = 1U;
              vFPOsetGenReady( RELAY_ON );
              //vELECTROalarmIdleEnable();
              engine.cmd = ENGINE_CMD_NONE;
            }
            break;
          default:
            engine.cmd = ENGINE_CMD_NONE;
            vLOGICprintEngineStatus( engine.status );
            break;
        }
        break;
      /*----------------------------------------------------------------------------------------*/
      /*------------------------------- ENGINE EMERGENCY STOP ----------------------------------*/
      /*----------------------------------------------------------------------------------------*/
      case ENGINE_CMD_EMEGENCY_STOP:
        starter.set( RELAY_OFF );
        fuel.pump.set( RELAY_OFF );
        fuel.pump.status = RELAY_OFF;
        fuel.booster.relay.set( RELAY_OFF );
        fuel.booster.relay.status = RELAY_OFF;
        coolant.cooler.relay.set( RELAY_OFF );
        coolant.cooler.relay.status = RELAY_OFF;
        coolant.heater.relay.set( RELAY_OFF );
        coolant.heater.relay.status = RELAY_OFF;
        idleRelay.set( RELAY_OFF );
        vRELAYdelayTrig( &stopSolenoid );
        preHeater.active    = 0U;
        preHeater.relay.set( RELAY_OFF );
        preHeater.relay.status = RELAY_OFF;
        vFPOsetGenReady( RELAY_OFF );
        vFPOsetReadyToStart( RELAY_OFF );
        engine.status = ENGINE_STATUS_EMERGENCY_STOP;
        engine.cmd = ENGINE_CMD_NONE;
        break;
      /*----------------------------------------------------------------------------------------*/
      /*------------------------------- ENGINE RESET TO IDLE -----------------------------------*/
      /*----------------------------------------------------------------------------------------*/
      case ENGINE_CMD_RESET_TO_IDLE:
        vENGINEresetAlarms();
        starter.status  = STARTER_IDLE;
        planStop.status = STOP_IDLE;
        engine.status   = ENGINE_STATUS_IDLE;
        idleStopError   = 0U;
        stopSolenoid.relay.set( RELAY_OFF );
        vFPOsetReadyToStart( RELAY_ON );
        vFPOsetGenReady( RELAY_OFF );
        engine.cmd = ENGINE_CMD_NONE;

        break;
      /*----------------------------------------------------------------------------------------*/
      /*----------------------------------------------------------------------------------------*/
      /*----------------------------------------------------------------------------------------*/
      default:
        engine.cmd = ENGINE_CMD_NONE;
        break;
    }
    /* Process outputs */
    vRELAYimpulseProcess( &preHeater, coolantVal );
    vRELAYdelayProcess( &stopSolenoid );
    /* Send status */

    /*---------------------*/
  }
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


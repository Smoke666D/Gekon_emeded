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
#include "alarm.h"
#include "debug.h"
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
static const fix16_t fix60                  = F16( 60U );
static const fix16_t dryContactTrigLevel    = F16( 0x7FFFU );
static const fix16_t chargerImpulseDuration = F16( CHARGER_IMPULSE_DURATION );
static const fix16_t oilTrashhold           = F16( 0.015 );

#if ( DEBUG_SERIAL_STATUS > 0U )
  static const char* cSensorTypes[5U] = { "NONE", "NORMAL_OPEN", "NORMAL_CLOSE", "RESISTIVE", "CURRENT" };
  static const char* engineCmdStr[8U] =
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
  static const char* engineStatusStr[9U] =
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
  static const char* starterStatusStr[11U] =
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
  static const char* planStopStatusStr[7U] =
  {
    "IDLE",
    "COOLDOWN",
    "WAIT_ELECTRO",
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
void vENGINEtask ( void* argument );
/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/
fix16_t getChargerInput ( void )
{
  fix16_t res = 0U;
  return res;
}
void setChargerOutput ( RELAY_STATUS status )
{
  return;
}
/*----------------------------------------------------------------------------*/
void vSENSORprocess ( SENSOR* sensor, fix16_t* value )
{
  eFunctionError funcStat = SENSOR_STATUS_NORMAL;

  *value = sensor->get();
  vALARMcheck( &sensor->cutout, *value );
  if ( sensor->cutout.error.status != ALARM_STATUS_IDLE )
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
  vALARMcheck( &oil.alarm, value );
  if ( oil.alarm.error.status == ALARM_STATUS_IDLE )
  {
    vALARMcheck( &oil.preAlarm, value );
  }
  return value;
}
/*----------------------------------------------------------------------------*/
fix16_t fCOOLANTprocess ( void )
{
  fix16_t value = 0U;
  vSENSORprocess( &coolant.temp, &value );
  vALARMcheck( &coolant.alarm, value );
  if ( coolant.alarm.error.status == ALARM_STATUS_IDLE )
  {
    vALARMcheck( &coolant.preAlarm, value );
  }
  vRELAYautoProces( &coolant.heater, value );
  vRELAYautoProces( &coolant.cooler, value );
  return value;
}
/*----------------------------------------------------------------------------*/
fix16_t fFUELprocess ( void )
{
  fix16_t value = 0U;
  vSENSORprocess( &fuel.level, &value );
  vALARMcheck( &fuel.lowAlarm, value );
  if ( fuel.lowAlarm.error.status == ALARM_STATUS_IDLE )
  {
    vALARMcheck( &fuel.lowPreAlarm, value );
  }
  vALARMcheck( &fuel.hightAlarm, value );
  if ( fuel.hightAlarm.error.status == ALARM_STATUS_IDLE )
  {
    vALARMcheck( &fuel.hightPreAlarm, value );
  }
  vRELAYautoProces( &fuel.booster, value );
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
    if ( speed.hightAlarm.error.status == ALARM_STATUS_IDLE )
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
  if ( battery.hightAlarm.error.status == ALARM_STATUS_IDLE )
  {
    vALARMcheck( &battery.lowAlarm, value );
  }
  return value;
}
/*----------------------------------------------------------------------------*/
fix16_t fCHARGERprocess ( PERMISSION startup )
{
  fix16_t value = 0U;
  if ( charger.enb == PERMISSION_ENABLE )
  {
    if ( ( startup == PERMISSION_ENABLE ) && ( charger.status != CHARGER_STATUS_STARTUP ) )
    {
      charger.status    = CHARGER_STATUS_STARTUP;
      charger.iteration = 0U;
    }
    value = charger.get();
    switch ( charger.status )
    {
      case CHARGER_STATUS_IDLE:
        if ( value < charger.setpoint )
        {
          charger.status = CHARGER_STATUS_IMPULSE;
        }
        break;
      case CHARGER_STATUS_STARTUP:
        if ( startup == PERMISSION_ENABLE )
        {
          if ( charger.relay.status == RELAY_OFF )
          {
            vRELAYset( &charger.relay, RELAY_ON );
          }
        }
        else
        {
          charger.status = CHARGER_STATUS_IDLE;
        }
        break;
      case CHARGER_STATUS_IMPULSE:
        if ( charger.iteration < charger.attempts )
        {
          vRELAYset( &charger.relay, RELAY_ON );
          charger.timer.delay  = chargerImpulseDuration;
          charger.iteration++;
          //vLOGICstartTimer( charger.timer );
          charger.status = CHARGER_STATUS_DELAY;
        }
        break;
      case CHARGER_STATUS_DELAY:
        if ( uLOGICisTimer( charger.timer ) > 0U )
        {

        }
        break;
      case CHARGER_STATUS_ERROR:
        break;
      default:
        charger.status = CHARGER_STATUS_IDLE;
        vRELAYset( &charger.relay, RELAY_OFF );
        break;
    }
  }
  return value;
}
/*----------------------------------------------------------------------------*/
void vENGINEmileageProcess ( void )
{
  uint16_t data     = 0U;
  uint16_t setPoint = 0U;

  if ( engine.status == ENGINE_STATUS_WORK )
  {
    switch ( maintence.status )
    {
      case MAINTENCE_STATUS_STOP:
        vLOGICstartTimer( &maintence.timer );
        maintence.status = MAINTENCE_STATUS_RUN;
        break;
      case MAINTENCE_STATUS_RUN:
        if ( uLOGICisTimer( maintence.timer ) > 0U )
        {
          eDATAAPIfreeData( DATA_API_CMD_INC,  ENGINE_WORK_MINUTES_ADR, &data );
          if ( data >= 60U )
          {
            eDATAAPIfreeData( DATA_API_CMD_ERASE, ENGINE_WORK_MINUTES_ADR, NULL );
            eDATAAPIfreeData( DATA_API_CMD_INC,   ENGINE_WORK_TIME_ADR,    &data );
            maintence.status = MAINTENCE_STATUS_CHECK;
          }
          else
          {
            vLOGICstartTimer( &maintence.timer );
          }
          eDATAAPIfreeData( DATA_API_CMD_SAVE, 0U, NULL );
        }
        break;
      case MAINTENCE_STATUS_CHECK:
        eDATAAPIfreeData( DATA_API_CMD_READ, ENGINE_WORK_TIME_ADR, &setPoint );
        if ( ( maintence.oil.error.enb > 0U ) && ( maintence.oil.error.active > 0U ) )
        {
          eDATAAPIfreeData( DATA_API_CMD_INC,  MAINTENANCE_ALARM_OIL_TIME_LEFT_ADR, &data );
          vALARMcheck( &maintence.oil,  fix16_from_int( setPoint ) );
        }
        if ( ( maintence.air.error.enb > 0U ) && ( maintence.air.error.active > 0U ) )
        {
          eDATAAPIfreeData( DATA_API_CMD_INC,  MAINTENANCE_ALARM_AIR_TIME_LEFT_ADR, &data );
          vALARMcheck( &maintence.air,  fix16_from_int( setPoint ) );
        }
        if ( ( maintence.fuel.error.enb > 0U ) && ( maintence.fuel.error.active > 0U ) )
        {
          eDATAAPIfreeData( DATA_API_CMD_INC,  MAINTENANCE_ALARM_FUEL_TIME_LEFT_ADR, &data );
          vALARMcheck( &maintence.fuel, fix16_from_int( setPoint ) );
        }
        eDATAAPIfreeData( DATA_API_CMD_SAVE, 0U, NULL );
        vLOGICstartTimer( &maintence.timer );
        maintence.status = MAINTENCE_STATUS_RUN;
        break;
      default:
        maintence.status = MAINTENCE_STATUS_STOP;
        vLOGICresetTimer( maintence.timer );
        break;
    }
  }
  else if ( maintence.status != MAINTENCE_STATUS_STOP )
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
      vSYSSerial( ">>Engine          : Started by generator frequency: " );
      fix16_to_str( freq, buffer, 2U );
      vSYSSerial( buffer );
      vSYSSerial( "\r\n" );
    #endif
  }
  if ( ( starter.startCrit.critOilPressEnb > 0U ) && ( pressure >= starter.startCrit.critOilPressLevel ) )
  {
    res = 1U;
    #if ( DEBUG_SERIAL_STATUS > 0U )
      vSYSSerial( ">>Engine          : Started by oil pressure: " );
      fix16_to_str( pressure, buffer, 2U );
      vSYSSerial( buffer );
      vSYSSerial( "\r\n" );
    #endif
  }
  if ( ( starter.startCrit.critChargeEnb > 0U ) && ( voltage >= starter.startCrit.critChargeLevel ) )
  {
    res = 1U;
    #if ( DEBUG_SERIAL_STATUS > 0U )
      vSYSSerial( ">>Engine          : Started by charger voltage: " );
      fix16_to_str( voltage, buffer, 2U );
      vSYSSerial( buffer );
      vSYSSerial( "\r\n" );
    #endif
  }
  if ( ( starter.startCrit.critSpeedEnb > 0U ) && ( speed >= starter.startCrit.critSpeedLevel ) )
  {
    res = 1U;
    #if ( DEBUG_SERIAL_STATUS > 0U )
      vSYSSerial( ">>Engine          : Started by engine speed: " );
      fix16_to_str( speed, buffer, 2U );
      vSYSSerial( buffer );
      vSYSSerial( "\r\n" );
    #endif
  }
  return res;
}
/*----------------------------------------------------------------------------*/
uint8_t uENGINEisStop ( fix16_t voltage, fix16_t freq, fix16_t pressure, fix16_t speed  )
{
  uint8_t res = 0U;
  if ( ( pressure <  oil.trashhold ) &&
       ( speed    == 0U                             ) &&
       ( freq     == 0U                             ) &&
       ( voltage  == 0U                             ) )
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
    vENGINEenbToStr( oil.alarm.error.enb, buf );
    vSYSSerial( buf );
    vSYSSerial( "\r\n" );
    vSYSSerial( "    Prealarm       : ");
    vENGINEenbToStr( oil.alarm.error.enb, buf );
    vSYSSerial( buf );
    vSYSSerial( "\r\n" );

    vSYSSerial( ">>Fuel level sensor \r\n" );
    vSYSSerial( "    Type           : ");
    vSYSSerial( cSensorTypes[fuel.level.type] );
    vSYSSerial( "\n\r" );
    vSYSSerial( "    Low alarm      : ");
    vENGINEenbToStr( fuel.lowAlarm.error.enb, buf );
    vSYSSerial( buf );
    vSYSSerial( "\r\n" );
    vSYSSerial( "    Low prealarm   : ");
    vENGINEenbToStr( fuel.lowPreAlarm.error.enb, buf );
    vSYSSerial( buf );
    vSYSSerial( "\r\n" );
    vSYSSerial( "    Hight alarm    : ");
    vENGINEenbToStr( fuel.hightPreAlarm.error.enb, buf );
    vSYSSerial( buf );
    vSYSSerial( "\r\n" );
    vSYSSerial( "    Hight prealarm : ");
    vENGINEenbToStr( fuel.hightPreAlarm.error.enb, buf );
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
    vENGINEenbToStr( speed.lowAlarm.error.enb, buf );
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
    vSYSSerial( ">>Engine command  : " );
    vSYSSerial( engineCmdStr[cmd] );
    vSYSSerial( "\r\n" );
  #endif
  return;
}
/*----------------------------------------------------------------------------*/
void vLOGICprintStarterStatus ( STARTER_STATUS status )
{
  #if ( DEBUG_SERIAL_STATUS > 0U )
    vSYSSerial( ">>Starter status  : " );
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
  oil.trashhold                          = fix16_mul( getMaxValue( &oilPressureAlarmLevel ), oilTrashhold );
  oil.pressure.type                      = getBitMap( &oilPressureSetup, OIL_PRESSURE_SENSOR_TYPE_ADR );
  oil.pressure.chart                     = &oilSensorChart;
  oil.pressure.get                       = xADCGetSOP;
  oil.pressure.cutout.error.enb          = getBitMap( &oilPressureSetup, OIL_PRESSURE_OPEN_CIRCUIT_ALARM_ENB_ADR );
  oil.pressure.cutout.error.active       = PERMISSION_ENABLE;
  oil.pressure.cutout.level              = SENSOR_CUTOUT_LEVEL;
  oil.pressure.cutout.error.status       = ALARM_STATUS_IDLE;
  oil.pressure.cutout.timer.delay        = 0U;
  oil.pressure.cutout.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  oil.pressure.cutout.error.ack          = PERMISSION_DISABLE;
  oil.pressure.cutout.error.event.action = ACTION_EMERGENCY_STOP;
  oil.pressure.cutout.error.event.type   = EVENT_OIL_SENSOR_ERROR;
  oil.pressure.cutout.error.id           = DEFINE_ERROR_LIST_ADR;
  oil.pressure.cutout.error.trig         = TRIGGER_IDLE;
  oil.pressure.cutout.type               = ALARM_LEVEL_HIGHT;
  oil.pressure.status                    = SENSOR_STATUS_NORMAL;
  if ( ( oil.pressure.type == SENSOR_TYPE_RESISTIVE ) || ( oil.pressure.type == SENSOR_TYPE_CURRENT ) )
  {
    oil.alarm.error.enb = getBitMap( &oilPressureSetup, OIL_PRESSURE_ALARM_ENB_ADR );
    oil.alarm.level     = getValue( &oilPressureAlarmLevel );
  }
  else
  {
    oil.alarm.error.enb = PERMISSION_ENABLE;
    oil.alarm.level     = dryContactTrigLevel;
  }
  oil.alarm.type               = ALARM_LEVEL_LOW;
  oil.alarm.timer.delay        = 0U;
  oil.alarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  oil.alarm.error.event.type   = EVENT_OIL_LOW_PRESSURE;
  oil.alarm.error.event.action = ACTION_EMERGENCY_STOP;
  oil.alarm.error.ack          = PERMISSION_DISABLE;
  oil.alarm.error.id           = DEFINE_ERROR_LIST_ADR;
  oil.alarm.error.trig         = TRIGGER_IDLE;
  oil.alarm.error.active       = PERMISSION_DISABLE;
  oil.alarm.error.status       = ALARM_STATUS_IDLE;

  if ( ( oil.pressure.type == SENSOR_TYPE_RESISTIVE ) || ( oil.pressure.type == SENSOR_TYPE_CURRENT ) )
  {
    oil.preAlarm.error.enb               = getBitMap( &oilPressureSetup, OIL_PRESSURE_PRE_ALARM_ENB_ADR );
    oil.preAlarm.error.active            = PERMISSION_DISABLE;
    oil.preAlarm.type                    = ALARM_LEVEL_LOW;
    oil.preAlarm.level                   = getValue( &oilPressurePreAlarmLevel );
    oil.preAlarm.timer.delay             = 0U;
    oil.preAlarm.timer.id                = LOGIC_DEFAULT_TIMER_ID;
    oil.preAlarm.error.event.type        = EVENT_OIL_LOW_PRESSURE;
    oil.preAlarm.error.event.action      = ACTION_WARNING;
    oil.preAlarm.error.ack               = PERMISSION_ENABLE;
    oil.preAlarm.error.id                = DEFINE_ERROR_LIST_ADR;
    oil.preAlarm.error.trig              = TRIGGER_IDLE;
    oil.preAlarm.error.status            = ALARM_STATUS_IDLE;
  }
  else
  {
    oil.preAlarm.error.enb = PERMISSION_DISABLE;
  }
  /*--------------------------------------------------------------*/
  coolant.temp.type                      = getBitMap( &coolantTempSetup, COOLANT_TEMP_SENSOR_TYPE_ADR );
  coolant.temp.chart                     = &coolantSensorChart;
  coolant.temp.get                       = xADCGetSCT;
  coolant.temp.cutout.error.enb          = getBitMap( &coolantTempSetup, COOLANT_TEMP_OPEN_CIRCUIT_ALARM_ENB_ADR );
  coolant.temp.cutout.error.active       = PERMISSION_ENABLE;
  coolant.temp.cutout.level              = SENSOR_CUTOUT_LEVEL;
  coolant.temp.cutout.timer.delay        = 0U;
  coolant.temp.cutout.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  coolant.temp.cutout.error.ack          = PERMISSION_DISABLE;
  coolant.temp.cutout.error.event.action = ACTION_EMERGENCY_STOP;
  coolant.temp.cutout.error.event.type   = EVENT_ENGINE_TEMP_SENSOR_ERROR;
  coolant.temp.cutout.error.id           = DEFINE_ERROR_LIST_ADR;
  coolant.temp.cutout.error.trig         = TRIGGER_IDLE;
  coolant.temp.cutout.type               = ALARM_LEVEL_HIGHT;
  coolant.temp.cutout.error.status       = ALARM_STATUS_IDLE;
  coolant.temp.status                    = SENSOR_STATUS_NORMAL;
  if ( ( coolant.temp.type == SENSOR_TYPE_RESISTIVE ) || ( coolant.temp.type == SENSOR_TYPE_CURRENT ) )
  {
    coolant.alarm.error.enb   = getBitMap( &coolantTempSetup, COOLANT_HIGHT_TEMP_ALARM_ENB_ADR );
    coolant.alarm.type  = ALARM_LEVEL_HIGHT;
    coolant.alarm.level = getValue( &coolantHightTempAlarmLevel );
  }
  else
  {
    coolant.alarm.error.enb   = PERMISSION_ENABLE;
    coolant.alarm.type  = ALARM_LEVEL_LOW;
    coolant.alarm.level = dryContactTrigLevel;
  }
  coolant.alarm.error.active            = PERMISSION_ENABLE;
  coolant.alarm.timer.delay             = 0U;
  coolant.alarm.timer.id                = LOGIC_DEFAULT_TIMER_ID;
  coolant.alarm.error.event.type        = EVENT_ENGINE_HIGHT_TEMP;
  coolant.alarm.error.event.action      = ACTION_EMERGENCY_STOP;
  coolant.alarm.error.ack               = PERMISSION_DISABLE;
  coolant.alarm.error.id                = DEFINE_ERROR_LIST_ADR;
  coolant.alarm.error.trig              = TRIGGER_IDLE;
  coolant.alarm.error.status            = ALARM_STATUS_IDLE;
  if ( ( coolant.temp.type == SENSOR_TYPE_RESISTIVE ) || ( coolant.temp.type == SENSOR_TYPE_CURRENT ) )
  {
    coolant.preAlarm.error.enb          = getBitMap( &coolantTempSetup, COOLANT_HIGHT_TEMP_PRE_ALARM_ENB_ADR );
    coolant.preAlarm.error.active       = PERMISSION_ENABLE;
    coolant.preAlarm.type               = ALARM_LEVEL_HIGHT;
    coolant.preAlarm.level              = getValue( &coolantHightTempPreAlarmLevel );
    coolant.preAlarm.timer.delay        = 0U;
    coolant.preAlarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
    coolant.preAlarm.error.ack          = PERMISSION_ENABLE;
    coolant.preAlarm.error.event.type   = EVENT_ENGINE_HIGHT_TEMP;
    coolant.preAlarm.error.event.action = ACTION_WARNING;
    coolant.preAlarm.error.id           = DEFINE_ERROR_LIST_ADR;
    coolant.preAlarm.error.trig         = TRIGGER_IDLE;
    coolant.preAlarm.error.status       = ALARM_STATUS_IDLE;
  }
  coolant.heater.relay.enb    = getBitMap( &coolantTempSetup, COOLANT_TEMP_HEATER_ENB_ADR );
  coolant.heater.onLevel      = getValue( &coolantTempHeaterOnLevel );
  coolant.heater.offLevel     = getValue( &coolantTempHeaterOffLevel );
  coolant.heater.relay.set    = vFPOsetHeater;
  coolant.heater.relay.status = RELAY_OFF;
  coolant.cooler.relay.enb    = getBitMap( &coolantTempSetup, COOLANT_TEMP_COOLER_ENB_ADR );
  coolant.cooler.onLevel      = getValue( &coolantTempCoolerOnLevel );
  coolant.cooler.offLevel     = getValue( &coolantTempCoolerOffLevel );
  coolant.cooler.relay.set    = vFPOsetCooler;
  coolant.cooler.relay.status = RELAY_OFF;
  /*--------------------------------------------------------------*/
  preHeater.relay.enb    = getBitMap( &engineSetup, ENGINE_PRE_HEAT_ENB_ADR );
  preHeater.active       = PERMISSION_DISABLE;
  preHeater.level        = getValue( &enginePreHeatLevel );
  preHeater.relay.set    = vFPOsetPreheater;
  preHeater.relay.status = RELAY_OFF;
  preHeater.status       = RELAY_DELAY_IDLE;
  preHeater.timer.delay  = getValue( &enginePreHeatDelay );
  /*--------------------------------------------------------------*/
  fuel.level.type                       = getBitMap( &fuelLevelSetup, FUEL_LEVEL_SENSOR_TYPE_ADR );
  fuel.level.chart                      = &fuelSensorChart;
  fuel.level.get                        = xADCGetSFL;
  fuel.level.cutout.error.enb           = getBitMap( &fuelLevelSetup, FUEL_LEVEL_OPEN_CIRCUIT_ALARM_ENB_ADR );
  fuel.level.cutout.error.active        = PERMISSION_ENABLE;
  fuel.level.cutout.level               = SENSOR_CUTOUT_LEVEL;
  fuel.level.cutout.error.status        = ALARM_STATUS_IDLE;
  fuel.level.cutout.timer.delay         = 0U;
  fuel.level.cutout.timer.id            = LOGIC_DEFAULT_TIMER_ID;
  fuel.level.cutout.error.ack           = PERMISSION_ENABLE;
  fuel.level.cutout.error.event.action  = ACTION_EMERGENCY_STOP;
  fuel.level.cutout.error.event.type    = EVENT_FUEL_LEVEL_SENSOR_ERROR;
  fuel.level.cutout.error.id            = DEFINE_ERROR_LIST_ADR;
  fuel.level.cutout.error.trig          = TRIGGER_IDLE;
  fuel.level.cutout.type                = ALARM_LEVEL_HIGHT;
  fuel.level.status                     = SENSOR_STATUS_NORMAL;

  fuel.lowAlarm.error.enb               = getBitMap( &fuelLevelSetup, FUEL_LEVEL_LOW_ALARM_ENB_ADR );
  fuel.lowAlarm.error.active            = PERMISSION_ENABLE;
  fuel.lowAlarm.type                    = ALARM_LEVEL_LOW;
  fuel.lowAlarm.level                   = getValue( &fuelLevelLowAlarmLevel );
  fuel.lowAlarm.timer.delay             = getValue( &fuelLevelLowAlarmDelay );
  fuel.lowAlarm.timer.id                = LOGIC_DEFAULT_TIMER_ID;
  fuel.lowAlarm.error.event.type        = EVENT_FUEL_LOW_LEVEL;
  fuel.lowAlarm.error.event.action      = ACTION_PLAN_STOP;
  fuel.lowAlarm.error.ack               = PERMISSION_ENABLE;
  fuel.lowAlarm.error.id                = DEFINE_ERROR_LIST_ADR;
  fuel.lowAlarm.error.trig              = TRIGGER_IDLE;
  fuel.lowAlarm.error.status            = ALARM_STATUS_IDLE;

  fuel.lowPreAlarm.error.enb            = getBitMap( &fuelLevelSetup, FUEL_LEVEL_LOW_PRE_ALARM_ENB_ADR );
  fuel.lowPreAlarm.error.active         = PERMISSION_ENABLE;
  fuel.lowPreAlarm.type                 = ALARM_LEVEL_LOW;
  fuel.lowPreAlarm.level                = getValue( &fuelLevelLowPreAlarmLevel );
  fuel.lowPreAlarm.timer.delay          = getValue( &fuelLevelLowPreAlarmDelay );
  fuel.lowPreAlarm.timer.id             = LOGIC_DEFAULT_TIMER_ID;
  fuel.lowPreAlarm.error.ack            = PERMISSION_ENABLE;
  fuel.lowPreAlarm.error.event.type     = EVENT_FUEL_LOW_LEVEL;
  fuel.lowPreAlarm.error.event.action   = ACTION_WARNING;
  fuel.lowPreAlarm.error.id             = DEFINE_ERROR_LIST_ADR;
  fuel.lowPreAlarm.error.trig           = TRIGGER_IDLE;
  fuel.lowPreAlarm.error.status         = ALARM_STATUS_IDLE;

  fuel.hightPreAlarm.error.enb          = getBitMap( &fuelLevelSetup, FUEL_LEVEL_HIGHT_PRE_ALARM_ENB_ADR );
  fuel.hightPreAlarm.error.active       = PERMISSION_ENABLE;
  fuel.hightPreAlarm.type               = ALARM_LEVEL_HIGHT;
  fuel.hightPreAlarm.level              = getValue( &fuelLevelHightPreAlarmLevel );
  fuel.hightPreAlarm.timer.delay        = getValue( &fuelLevelHightPreAlarmDelay );
  fuel.hightPreAlarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  fuel.hightPreAlarm.error.ack          = PERMISSION_ENABLE;
  fuel.hightPreAlarm.error.event.type   = EVENT_FUEL_HIGHT_LEVEL;
  fuel.hightPreAlarm.error.event.action = ACTION_WARNING;
  fuel.hightPreAlarm.error.id           = DEFINE_ERROR_LIST_ADR;
  fuel.hightPreAlarm.error.trig         = TRIGGER_IDLE;
  fuel.hightPreAlarm.error.status       = ALARM_STATUS_IDLE;

  fuel.hightAlarm.error.enb             = getBitMap( &fuelLevelSetup, FUEL_LEVEL_HIGHT_ALARM_ENB_ADR );
  fuel.hightAlarm.error.active          = PERMISSION_ENABLE;
  fuel.hightAlarm.type                  = ALARM_LEVEL_HIGHT;
  fuel.hightAlarm.level                 = getValue( &fuelLevelHightAlarmLevel );
  fuel.hightAlarm.timer.delay           = getValue( &fuelLevelHightAlarmDelay );
  fuel.hightAlarm.timer.id              = LOGIC_DEFAULT_TIMER_ID;
  fuel.hightAlarm.error.event.type      = EVENT_FUEL_HIGHT_LEVEL;
  fuel.hightAlarm.error.event.action    = ACTION_PLAN_STOP;
  fuel.hightAlarm.error.ack             = PERMISSION_ENABLE;
  fuel.hightAlarm.error.id              = DEFINE_ERROR_LIST_ADR;
  fuel.hightAlarm.error.trig            = TRIGGER_IDLE;
  fuel.hightAlarm.error.status          = ALARM_STATUS_IDLE;

  fuel.booster.relay.enb    = getBitMap( &fuelLevelSetup, FUEL_PUMP_ENB_ADR );
  fuel.booster.onLevel      = getValue( &fuelPumpOnLevel );
  fuel.booster.offLevel     = getValue( &fuelPumpOffLevel );
  fuel.booster.relay.set    = vFPOsetBooster;
  fuel.booster.relay.status = RELAY_OFF;

  fuel.pump.enb    = PERMISSION_ENABLE;
  fuel.pump.set    = vFPOsetPump;
  fuel.pump.status = RELAY_OFF;
  /*--------------------------------------------------------------*/
  battery.get                           = xADCGetVDD;
  battery.lowAlarm.error.enb            = getBitMap( &batteryAlarms, BATTERY_UNDER_VOLTAGE_ENB_ADR );
  battery.lowAlarm.error.active         = PERMISSION_DISABLE;
  battery.lowAlarm.type                 = ALARM_LEVEL_LOW;
  battery.lowAlarm.level                = getValue( &batteryUnderVoltageLevel );
  battery.lowAlarm.timer.delay          = getValue( &batteryUnderVoltageDelay );
  battery.lowAlarm.timer.id             = LOGIC_DEFAULT_TIMER_ID;
  battery.lowAlarm.error.ack            = PERMISSION_ENABLE;
  battery.lowAlarm.error.event.type     = EVENT_BATTERY_LOW;
  battery.lowAlarm.error.event.action   = ACTION_WARNING;
  battery.lowAlarm.error.id             = DEFINE_ERROR_LIST_ADR;
  battery.lowAlarm.error.trig           = TRIGGER_IDLE;
  battery.lowAlarm.error.status         = ALARM_STATUS_IDLE;

  battery.hightAlarm.error.enb          = getBitMap( &batteryAlarms, BATTERY_OVER_VOLTAGE_ENB_ADR );
  battery.hightAlarm.error.active       = PERMISSION_DISABLE;
  battery.hightAlarm.type               = ALARM_LEVEL_HIGHT;
  battery.hightAlarm.level              = getValue( &batteryOverVoltageLevel );
  battery.hightAlarm.timer.delay        = getValue( &batteryOverVoltageDelay );
  battery.hightAlarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  battery.hightAlarm.error.ack          = PERMISSION_ENABLE;
  battery.hightAlarm.error.event.type   = EVENT_BATTERY_HIGHT;
  battery.hightAlarm.error.event.action = ACTION_WARNING;
  battery.hightAlarm.error.id           = DEFINE_ERROR_LIST_ADR;
  battery.hightAlarm.error.trig         = TRIGGER_IDLE;
  battery.hightAlarm.error.status       = ALARM_STATUS_IDLE;
  /*--------------------------------------------------------------*/
  charger.enb                              = PERMISSION_ENABLE;
  charger.status                           = CHARGER_STATUS_IDLE;
  charger.attempts                         = CHARGER_ATTEMPTS_NUMBER;
  charger.iteration                        = 0U;
  charger.get                              = getChargerInput;
  /*
  charger.set                              = setChargerOutput;
  charger.timer.delay                      = chargerImpulseDuration;
  charger.timer.id                         = LOGIC_DEFAULT_TIMER_ID;
  charger.error.enb             = getBitMap( &batteryAlarms, BATTERY_CHARGE_SHUTDOWN_ENB_ADR );
  charger.error.active          = PERMISSION_DISABLE;
  charger.error.type                  = ALARM_LEVEL_HIGHT;
  charger.error.level                 = getValue( &batteryChargeShutdownLevel );
  charger.error.timer.delay           = getValue( &batteryChargeShutdownDelay );
  charger.error.timer.id              = LOGIC_DEFAULT_TIMER_ID;
  charger.error.error.ack             = PERMISSION_DISABLE;
  charger.error.error.event.type      = EVENT_CHARGER_FAIL;
  charger.error.error.event.action    = ACTION_WARNING;
  charger.error.error.id              = DEFINE_ERROR_LIST_ADR;
  charger.error.error.trig            = TRIGGER_IDLE;
  charger.error.error.status          = ALARM_STATUS_IDLE;
  charger.hightPreAlarm.error.enb          = getBitMap( &batteryAlarms, BATTERY_CHARGE_WARNING_ENB_ADR );
  charger.hightPreAlarm.error.active       = PERMISSION_DISABLE;
  charger.hightPreAlarm.type               = ALARM_LEVEL_HIGHT;
  charger.hightPreAlarm.level              = getValue( &batteryChargeWarningLevel );
  charger.hightPreAlarm.timer.delay        = getValue( &batteryChargeWarningDelay );
  charger.hightPreAlarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  charger.hightPreAlarm.error.ack          = PERMISSION_ENABLE;
  charger.hightPreAlarm.error.event.type   = EVENT_CHARGER_FAIL;
  charger.hightPreAlarm.error.event.action = ACTION_WARNING;
  charger.hightPreAlarm.error.id           = DEFINE_ERROR_LIST_ADR;
  charger.hightPreAlarm.error.trig         = TRIGGER_IDLE;
  charger.hightPreAlarm.error.status       = ALARM_STATUS_IDLE;
  */
  /*--------------------------------------------------------------*/
  starter.set            = vFPOsetStarter;
  starter.attempts  = getBitMap( &engineSetup, ENGINE_START_ATTEMPTS_ADR );
  starter.startDelay     = getValue( &timerStartDelay );
  starter.crankingDelay  = getValue( &timerCranking );
  starter.crankDelay     = getValue( &timerCrankDelay );
  starter.blockDelay     = getValue( &timerSafetyOnDelay );
  starter.idlingDelay    = getValue( &timerStartupIdleTime );
  starter.nominalDelay   = getValue( &timerNominalRPMDelay );
  starter.warmingDelay   = getValue( &timerWarming );
  starter.iteration = 0U;
  starter.status         = STARTER_IDLE;

  starter.startCrit.critGenFreqEnb    = getBitMap( &genSetup, GEN_POWER_GENERATOR_CONTROL_ENB_ADR );
  starter.startCrit.critGenFreqLevel  = getValue( &starterStopGenFreqLevel );
  starter.startCrit.critOilPressEnb   = getBitMap( &starterStopSetup, STARTER_STOP_OIL_PRESSURE_ENB_ADR );
  starter.startCrit.critOilPressLevel = getValue( &starterStopOilPressureLevel );
  starter.startCrit.critChargeEnb     = getBitMap( &starterStopSetup, STARTER_STOP_CHARGE_ALTERNATOR_ENB_ADR );
  starter.startCrit.critChargeLevel   = getValue( &starterStopChargeAlternatorLevel );
  starter.startCrit.critSpeedEnb      = getBitMap( &starterStopSetup, STARTER_STOP_SPEED_ENB_ADR );
  starter.startCrit.critSpeedLevel    = getValue( &starterStopSpeedLevel );
  /*--------------------------------------------------------------*/
  planStop.status           = STOP_IDLE;
  planStop.coolingDelay     = getValue( &timerCooling );
  planStop.coolingIdleDelay = getValue( &timerCoolingIdle );
  planStop.processDelay     = getValue( &timerFailStopDelay );
  vSYSprintFix16(planStop.processDelay);
  /*--------------------------------------------------------------*/
  engine.startCheckOil           = getBitMap( &starterStopSetup, STARTER_OIL_PRESSURE_CHECK_ON_START_ENB_ADR );
  engine.status                  = ENGINE_STATUS_IDLE;
  engine.banStart                = PERMISSION_DISABLE;
  engine.stopError.enb           = PERMISSION_ENABLE;
  engine.stopError.active        = PERMISSION_ENABLE;
  engine.stopError.ack           = PERMISSION_DISABLE;
  engine.stopError.event.action  = ACTION_EMERGENCY_STOP;
  engine.stopError.event.type    = EVENT_STOP_FAIL;
  engine.stopError.id            = DEFINE_ERROR_LIST_ADR;
  engine.stopError.status        = ALARM_STATUS_IDLE;
  engine.stopError.trig          = TRIGGER_IDLE;

  engine.startError.enb          = PERMISSION_ENABLE;
  engine.startError.active       = PERMISSION_DISABLE;
  engine.startError.ack          = PERMISSION_DISABLE;
  engine.startError.event.action = ACTION_EMERGENCY_STOP;
  engine.startError.event.type   = EVENT_START_FAIL;
  engine.startError.id           = DEFINE_ERROR_LIST_ADR;
  engine.startError.status       = ALARM_STATUS_IDLE;
  engine.startError.trig         = TRIGGER_IDLE;
  /*--------------------------------------------------------------*/
  speed.enb    = getBitMap( &speedSetup, SPEED_ENB_ADR );
  speed.status = SENSOR_STATUS_NORMAL;
  speed.get    = fVRgetSpeed;

  speed.lowAlarm.error.enb            = getBitMap( &speedSetup, SPEED_LOW_ALARM_ENB_ADR );
  speed.lowAlarm.error.active         = PERMISSION_DISABLE;
  speed.lowAlarm.type                 = ALARM_LEVEL_LOW;
  speed.lowAlarm.level                = getValue( &speedLowAlarmLevel );
  speed.lowAlarm.timer.delay          = 0U;
  speed.lowAlarm.timer.id             = LOGIC_DEFAULT_TIMER_ID;
  speed.lowAlarm.error.ack            = PERMISSION_DISABLE;
  speed.lowAlarm.error.event.type     = EVENT_SPEED_LOW;
  speed.lowAlarm.error.event.action   = ACTION_EMERGENCY_STOP;
  speed.lowAlarm.error.id             = DEFINE_ERROR_LIST_ADR;
  speed.lowAlarm.error.trig           = TRIGGER_IDLE;
  speed.lowAlarm.error.status         = ALARM_STATUS_IDLE;

  speed.hightAlarm.error.enb          = PERMISSION_ENABLE;
  speed.hightAlarm.error.active       = PERMISSION_ENABLE;
  speed.hightAlarm.type               = ALARM_LEVEL_HIGHT;
  speed.hightAlarm.level              = getValue( &speedHightAlarmLevel );
  speed.hightAlarm.timer.delay        = 0U;
  speed.hightAlarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  speed.hightAlarm.error.ack          = PERMISSION_DISABLE;
  speed.hightAlarm.error.event.type   = EVENT_SPEED_HIGHT;
  speed.hightAlarm.error.event.action = ACTION_EMERGENCY_STOP;
  speed.hightAlarm.error.id           = DEFINE_ERROR_LIST_ADR;
  speed.hightAlarm.error.trig         = TRIGGER_IDLE;
  speed.hightAlarm.error.status       = ALARM_STATUS_IDLE;
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
  maintence.timer.delay          = fix60;
  maintence.timer.id             = LOGIC_DEFAULT_TIMER_ID;
  maintence.oil.error.enb        = getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_OIL_ENB_ADR );
  maintence.oil.error.active     = PERMISSION_DISABLE;
  maintence.oil.type             = ALARM_LEVEL_HIGHT;
  maintence.oil.level            = getValue( &maintenanceAlarmOilTime );
  maintence.oil.timer.delay      = 0U;
  maintence.oil.timer.id         = LOGIC_DEFAULT_TIMER_ID;
  maintence.oil.error.event.type = EVENT_MAINTENANCE_OIL;
  maintence.oil.error.id         = DEFINE_ERROR_LIST_ADR;
  if ( getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_OIL_ACTION_ADR ) == 0U )
  {
    maintence.oil.error.event.action = ACTION_BAN_START;
    maintence.oil.error.ack          = PERMISSION_DISABLE;
  }
  else
  {
    maintence.oil.error.event.action = ACTION_WARNING;
    maintence.oil.error.ack          = PERMISSION_ENABLE;
  }
  maintence.oil.error.trig   = TRIGGER_IDLE;
  maintence.oil.error.status = ALARM_STATUS_IDLE;

  maintence.air.error.enb        = getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_AIR_ENB_ADR );
  maintence.air.error.active     = PERMISSION_DISABLE;
  maintence.air.type             = ALARM_LEVEL_HIGHT;
  maintence.air.level            = getValue( &maintenanceAlarmAirTime );
  maintence.air.timer.delay      = 0U;
  maintence.air.timer.id         = LOGIC_DEFAULT_TIMER_ID;
  maintence.air.error.event.type = EVENT_MAINTENANCE_AIR;
  maintence.air.error.id         = DEFINE_ERROR_LIST_ADR;
  if ( getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_AIR_ACTION_ADR ) == 0U )
  {
    maintence.air.error.event.action = ACTION_BAN_START;
    maintence.air.error.ack          = PERMISSION_DISABLE;
  }
  else
  {
    maintence.air.error.event.action = ACTION_WARNING;
    maintence.air.error.ack          = PERMISSION_ENABLE;
  }
  maintence.air.error.trig   = TRIGGER_IDLE;
  maintence.air.error.status = ALARM_STATUS_IDLE;

  maintence.fuel.error.enb        = getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_FUEL_ENB_ADR );
  maintence.fuel.error.active     = PERMISSION_DISABLE;
  maintence.fuel.type             = ALARM_LEVEL_HIGHT;
  maintence.fuel.level            = getValue( &maintenanceAlarmFuelTime );
  maintence.fuel.timer.delay      = 0U;
  maintence.fuel.timer.id         = LOGIC_DEFAULT_TIMER_ID;
  maintence.fuel.error.event.type = EVENT_MAINTENANCE_FUEL;
  if ( getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_FUEL_ACTION_ADR ) == 0U )
  {
    maintence.fuel.error.event.action = ACTION_BAN_START;
    maintence.fuel.error.ack          = PERMISSION_DISABLE;
  }
  else
  {
    maintence.fuel.error.event.action = ACTION_WARNING;
    maintence.fuel.error.ack          = PERMISSION_ENABLE;
  }
  maintence.fuel.error.trig   = TRIGGER_IDLE;
  maintence.fuel.error.status = ALARM_STATUS_IDLE;

  return;
}

void vENGINEdataReInit ( void )
{
  oil.pressure.type                      = getBitMap( &oilPressureSetup, OIL_PRESSURE_SENSOR_TYPE_ADR );
  oil.pressure.cutout.error.enb          = getBitMap( &oilPressureSetup, OIL_PRESSURE_OPEN_CIRCUIT_ALARM_ENB_ADR );
  if ( ( oil.pressure.type == SENSOR_TYPE_RESISTIVE ) || ( oil.pressure.type == SENSOR_TYPE_CURRENT ) )
  {
    oil.alarm.error.enb = getBitMap( &oilPressureSetup, OIL_PRESSURE_ALARM_ENB_ADR );
    oil.alarm.level     = getValue( &oilPressureAlarmLevel );
    oil.preAlarm.error.enb               = getBitMap( &oilPressureSetup, OIL_PRESSURE_PRE_ALARM_ENB_ADR );
    oil.preAlarm.level                   = getValue( &oilPressurePreAlarmLevel );
  }
  else
  {
    oil.alarm.error.enb = PERMISSION_ENABLE;
    oil.alarm.level     = dryContactTrigLevel;
    oil.preAlarm.error.enb = PERMISSION_DISABLE;
  }
  /*--------------------------------------------------------------*/
  coolant.temp.type                      = getBitMap( &coolantTempSetup, COOLANT_TEMP_SENSOR_TYPE_ADR );
  coolant.temp.cutout.error.enb          = getBitMap( &coolantTempSetup, COOLANT_TEMP_OPEN_CIRCUIT_ALARM_ENB_ADR );
  if ( ( coolant.temp.type == SENSOR_TYPE_RESISTIVE ) || ( coolant.temp.type == SENSOR_TYPE_CURRENT ) )
  {
    coolant.alarm.error.enb   = getBitMap( &coolantTempSetup, COOLANT_HIGHT_TEMP_ALARM_ENB_ADR );
    coolant.alarm.type  = ALARM_LEVEL_HIGHT;
    coolant.alarm.level = getValue( &coolantHightTempAlarmLevel );
    coolant.preAlarm.error.enb          = getBitMap( &coolantTempSetup, COOLANT_HIGHT_TEMP_PRE_ALARM_ENB_ADR );
    coolant.preAlarm.level              = getValue( &coolantHightTempPreAlarmLevel );
  }
  else
  {
    coolant.alarm.error.enb   = PERMISSION_ENABLE;
    coolant.alarm.type  = ALARM_LEVEL_LOW;
    coolant.alarm.level = dryContactTrigLevel;
  }
  coolant.heater.relay.enb    = getBitMap( &coolantTempSetup, COOLANT_TEMP_HEATER_ENB_ADR );
  coolant.heater.onLevel      = getValue( &coolantTempHeaterOnLevel );
  coolant.heater.offLevel     = getValue( &coolantTempHeaterOffLevel );
  coolant.cooler.relay.enb    = getBitMap( &coolantTempSetup, COOLANT_TEMP_COOLER_ENB_ADR );
  coolant.cooler.onLevel      = getValue( &coolantTempCoolerOnLevel );
  coolant.cooler.offLevel     = getValue( &coolantTempCoolerOffLevel );
  /*--------------------------------------------------------------*/
  preHeater.relay.enb    = getBitMap( &engineSetup, ENGINE_PRE_HEAT_ENB_ADR );
  preHeater.level        = getValue( &enginePreHeatLevel );
  preHeater.relay.set    = vFPOsetPreheater;
  preHeater.timer.delay  = getValue( &enginePreHeatDelay );
  /*--------------------------------------------------------------*/
  fuel.level.type                       = getBitMap( &fuelLevelSetup, FUEL_LEVEL_SENSOR_TYPE_ADR );
  fuel.level.cutout.error.enb           = getBitMap( &fuelLevelSetup, FUEL_LEVEL_OPEN_CIRCUIT_ALARM_ENB_ADR );
  fuel.lowAlarm.error.enb               = getBitMap( &fuelLevelSetup, FUEL_LEVEL_LOW_ALARM_ENB_ADR );
  fuel.lowAlarm.level                   = getValue( &fuelLevelLowAlarmLevel );
  fuel.lowAlarm.timer.delay             = getValue( &fuelLevelLowAlarmDelay );
  fuel.lowPreAlarm.error.enb            = getBitMap( &fuelLevelSetup, FUEL_LEVEL_LOW_PRE_ALARM_ENB_ADR );
  fuel.lowPreAlarm.level                = getValue( &fuelLevelLowPreAlarmLevel );
  fuel.lowPreAlarm.timer.delay          = getValue( &fuelLevelLowPreAlarmDelay );
  fuel.hightPreAlarm.error.enb          = getBitMap( &fuelLevelSetup, FUEL_LEVEL_HIGHT_PRE_ALARM_ENB_ADR );
  fuel.hightPreAlarm.level              = getValue( &fuelLevelHightPreAlarmLevel );
  fuel.hightPreAlarm.timer.delay        = getValue( &fuelLevelHightPreAlarmDelay );
  fuel.hightAlarm.error.enb             = getBitMap( &fuelLevelSetup, FUEL_LEVEL_HIGHT_ALARM_ENB_ADR );
  fuel.hightAlarm.level                 = getValue( &fuelLevelHightAlarmLevel );
  fuel.hightAlarm.timer.delay           = getValue( &fuelLevelHightAlarmDelay );
  fuel.booster.relay.enb    = getBitMap( &fuelLevelSetup, FUEL_PUMP_ENB_ADR );
  fuel.booster.onLevel      = getValue( &fuelPumpOnLevel );
  fuel.booster.offLevel     = getValue( &fuelPumpOffLevel );
  /*--------------------------------------------------------------*/
  battery.lowAlarm.error.enb            = getBitMap( &batteryAlarms, BATTERY_UNDER_VOLTAGE_ENB_ADR );
  battery.lowAlarm.level                = getValue( &batteryUnderVoltageLevel );
  battery.lowAlarm.timer.delay          = getValue( &batteryUnderVoltageDelay );
  battery.hightAlarm.error.enb          = getBitMap( &batteryAlarms, BATTERY_OVER_VOLTAGE_ENB_ADR );
  battery.hightAlarm.level              = getValue( &batteryOverVoltageLevel );
  battery.hightAlarm.timer.delay        = getValue( &batteryOverVoltageDelay );
  /*--------------------------------------------------------------*/
  starter.set            = vFPOsetStarter;
  starter.attempts       = getBitMap( &engineSetup, ENGINE_START_ATTEMPTS_ADR );
  starter.startDelay     = getValue( &timerStartDelay );
  starter.crankingDelay  = getValue( &timerCranking );
  starter.crankDelay     = getValue( &timerCrankDelay );
  starter.blockDelay     = getValue( &timerSafetyOnDelay );
  starter.idlingDelay    = getValue( &timerStartupIdleTime );
  starter.nominalDelay   = getValue( &timerNominalRPMDelay );
  starter.warmingDelay   = getValue( &timerWarming );

  starter.startCrit.critGenFreqEnb    = getBitMap( &genSetup, GEN_POWER_GENERATOR_CONTROL_ENB_ADR );
  starter.startCrit.critGenFreqLevel  = getValue( &starterStopGenFreqLevel );
  starter.startCrit.critOilPressEnb   = getBitMap( &starterStopSetup, STARTER_STOP_OIL_PRESSURE_ENB_ADR );
  starter.startCrit.critOilPressLevel = getValue( &starterStopOilPressureLevel );
  starter.startCrit.critChargeEnb     = getBitMap( &starterStopSetup, STARTER_STOP_CHARGE_ALTERNATOR_ENB_ADR );
  starter.startCrit.critChargeLevel   = getValue( &starterStopChargeAlternatorLevel );
  starter.startCrit.critSpeedEnb      = getBitMap( &starterStopSetup, STARTER_STOP_SPEED_ENB_ADR );
  starter.startCrit.critSpeedLevel    = getValue( &starterStopSpeedLevel );
  /*--------------------------------------------------------------*/
  planStop.coolingDelay     = getValue( &timerCooling );
  planStop.coolingIdleDelay = getValue( &timerCoolingIdle );
  planStop.processDelay     = getValue( &timerFailStopDelay );
  /*--------------------------------------------------------------*/
  engine.startCheckOil           = getBitMap( &starterStopSetup, STARTER_OIL_PRESSURE_CHECK_ON_START_ENB_ADR );
  /*--------------------------------------------------------------*/
  speed.enb    = getBitMap( &speedSetup, SPEED_ENB_ADR );
  speed.lowAlarm.error.enb            = getBitMap( &speedSetup, SPEED_LOW_ALARM_ENB_ADR );
  speed.lowAlarm.level                = getValue( &speedLowAlarmLevel );
  speed.hightAlarm.level              = getValue( &speedHightAlarmLevel );
  /*--------------------------------------------------------------*/
  stopSolenoid.relay.enb    = uFPOisEnable( FPO_FUN_STOP_SOLENOID );
  stopSolenoid.timer.delay  = getValue( configReg[TIMER_SOLENOID_HOLD_ADR] );
  /*--------------------------------------------------------------*/
  idleRelay.enb = uFPOisEnable( FPO_FUN_IDLING );
  /*--------------------------------------------------------------*/
  maintence.oil.error.enb        = getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_OIL_ENB_ADR );
  maintence.oil.level            = getValue( &maintenanceAlarmOilTime );
  if ( getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_OIL_ACTION_ADR ) == 0U )
  {
    maintence.oil.error.event.action = ACTION_BAN_START;
    maintence.oil.error.ack          = PERMISSION_DISABLE;
  }
  else
  {
    maintence.oil.error.event.action = ACTION_WARNING;
    maintence.oil.error.ack          = PERMISSION_ENABLE;
  }
  maintence.air.error.enb        = getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_AIR_ENB_ADR );
  maintence.air.level            = getValue( &maintenanceAlarmAirTime );
  if ( getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_AIR_ACTION_ADR ) == 0U )
  {
    maintence.air.error.event.action = ACTION_BAN_START;
    maintence.air.error.ack          = PERMISSION_DISABLE;
  }
  else
  {
    maintence.air.error.event.action = ACTION_WARNING;
    maintence.air.error.ack          = PERMISSION_ENABLE;
  }
  maintence.fuel.error.enb        = getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_FUEL_ENB_ADR );
  maintence.fuel.level            = getValue( &maintenanceAlarmFuelTime );
  if ( getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_FUEL_ACTION_ADR ) == 0U )
  {
    maintence.fuel.error.event.action = ACTION_BAN_START;
    maintence.fuel.error.ack          = PERMISSION_DISABLE;
  }
  else
  {
    maintence.fuel.error.event.action = ACTION_WARNING;
    maintence.fuel.error.ack          = PERMISSION_ENABLE;
  }
  return;
}



/*----------------------------------------------------------------------------*/
void vENGINEresetAlarms ( void )
{
  vERRORreset( &speed.hightAlarm.error);
  vERRORreset( &speed.lowAlarm.error );
  //vERRORreset( &charger.error.error );
  //vERRORreset( &charger.hightPreAlarm.error );
  vERRORreset( &battery.hightAlarm.error );
  vERRORreset( &battery.lowAlarm.error );
  vERRORreset( &fuel.hightAlarm.error );
  vERRORreset( &fuel.hightPreAlarm.error );
  vERRORreset( &fuel.lowAlarm.error );
  vERRORreset( &fuel.lowPreAlarm.error );
  vERRORreset( &coolant.alarm.error );
  vERRORreset( &coolant.preAlarm.error );
  vERRORreset( &oil.alarm.error );
  vERRORreset( &oil.preAlarm.error );
  vERRORreset( &engine.stopError );
  vERRORreset( &engine.startError );
  engine.banStart = PERMISSION_DISABLE;
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
/*----------------------------------------------------------------------------*/
void vENGINEsendCmd ( ENGINE_COMMAND cmd )
{
  ENGINE_COMMAND engineCmd = cmd;
  xQueueSend( pEngineCommandQueue, &engineCmd, portMAX_DELAY );
  return;
}
/*----------------------------------------------------------------------------*/
QueueHandle_t pENGINEgetCommandQueue ( void )
{
  return pEngineCommandQueue;
}
/*----------------------------------------------------------------------------*/
uint8_t uENGINEisStarterScrollFinish ( void )
{
  return starterFinish;
}
/*----------------------------------------------------------------------------*/
uint8_t uENGINEisBlockTimerFinish ( void )
{
  return blockTimerFinish;
}
/*----------------------------------------------------------------------------*/
ENGINE_STATUS eENGINEgetEngineStatus ( void )
{
  return engine.status;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------- TASK -----------------------------------*/
/*----------------------------------------------------------------------------*/
void vENGINEtask ( void* argument )
{
  fix16_t         oilVal      = 0U;
  fix16_t         coolantVal  = 0U;
  fix16_t         speedVal    = 0U;
  fix16_t         chargerVal  = 0U;
  SYSTEM_TIMER    commonTimer = { 0U };
  ENGINE_COMMAND  inputCmd    = ENGINE_CMD_NONE;
  SYSTEM_EVENT    event       = { 0U };
  for (;;)
  {
    /*------------------------------------------------------------------*/
    /*-------------------- Read system notification --------------------*/
    /*------------------------------------------------------------------*/
    if ( ( xEventGroupGetBits( xDATAAPIgetEventGroup() ) & DATA_API_FLAG_ENGINE_TASK_CONFIG_REINIT ) > 0U )
    {
      vENGINEdataReInit();
      xEventGroupClearBits( xDATAAPIgetEventGroup(), DATA_API_FLAG_ENGINE_TASK_CONFIG_REINIT );
    }
    /*------------------------------------------------------------------*/
    /*----------------------- Read input command -----------------------*/
    /*------------------------------------------------------------------*/
    if ( xQueueReceive( pENGINEgetCommandQueue(), &inputCmd, 0U ) == pdPASS )
    {
      if ( engine.cmd != inputCmd )
      {
        //-------------------- One action commands ---------------------*/
        if ( inputCmd == ENGINE_CMD_BAN_START )
        {
          engine.banStart = PERMISSION_ENABLE;
        }
        else if ( inputCmd == ENGINE_CMD_ALLOW_START )
        {
          engine.banStart = PERMISSION_DISABLE;
        }
        //------------------- Long actions command ---------------------*/
        else
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
    }
    /*------------------------------------------------------------------*/
    /*------------------------- Process inputs -------------------------*/
    /*------------------------------------------------------------------*/
    oilVal     = fOILprocess();
    coolantVal = fCOOLANTprocess();
    fFUELprocess();
    speedVal   = fSPEEDprocess();
    fBATTERYprocess();
    //chargerVal = fCHARGERprocess( PERMISSION_DISABLE );
    /*------------------------------------------------------------------*/
    /*--------------------- Static condition check ---------------------*/
    /*------------------------------------------------------------------*/
    vERRORcheck( &engine.stopError, !( uENGINEisStop( fELECTROgetMaxGenVoltage(), xADCGetGENLFreq(), oilVal, speedVal )) );
    vERRORcheck( &engine.startError, 1U );
    /*------------------------------------------------------------------*/
    /*--------------------- Statistic calculation ----------------------*/
    /*------------------------------------------------------------------*/
    vENGINEmileageProcess();
    /*------------------------------------------------------------------*/
    /*------------------------- Input commands -------------------------*/
    /*------------------------------------------------------------------*/
    switch ( engine.cmd )
    {
      case ENGINE_CMD_NONE:
        if ( ( engine.stopError.active != PERMISSION_ENABLE ) && ( engine.status == ENGINE_STATUS_IDLE ) )
        {
          engine.stopError.active = PERMISSION_ENABLE;
        }
        break;
      /*----------------------------------------------------------------------------------------*/
      /*-------------------------------------- ENGINE START ------------------------------------*/
      /*----------------------------------------------------------------------------------------*/
      /*|--startDelay--|--crankingDelay--|--crankDelay--|--...--|--blockDelay--|--idlingDelay--|--nominalDelay--|--warmingDelay--> */
      case ENGINE_CMD_START:
        if ( ( ( engine.status == ENGINE_STATUS_IDLE ) || ( engine.status == ENGINE_STATUS_BUSY_STARTING ) ) &&
             ( engine.banStart == PERMISSION_DISABLE ) )
        {
          switch ( starter.status )
          {
            case STARTER_IDLE:
              if ( ( engine.startCheckOil == PERMISSION_ENABLE ) && ( oilVal > ENGINE_OIL_PRESSURE_TRESH_HOLD ) )
              {
                starter.status = STARTER_FAIL;
                vLOGICprintStarterStatus( starter.status );
              }
              else
              {
                vFPOsetReadyToStart( RELAY_OFF );
                vRELAYimpulseReset( &preHeater );
                engine.stopError.active            = PERMISSION_DISABLE;
                preHeater.active                   = PERMISSION_DISABLE;
                starterFinish                      = 0U;
                speed.lowAlarm.error.active        = PERMISSION_DISABLE;
                speed.hightAlarm.error.active      = PERMISSION_DISABLE;
                oil.alarm.error.active             = PERMISSION_DISABLE;
                oil.preAlarm.error.active          = PERMISSION_DISABLE;
                battery.hightAlarm.error.active    = PERMISSION_DISABLE;
                battery.lowAlarm.error.active      = PERMISSION_DISABLE;
                //charger.error.error.active    = PERMISSION_DISABLE;
                //charger.hightPreAlarm.error.active = PERMISSION_DISABLE;
                engine.status                      = ENGINE_STATUS_BUSY_STARTING;
                commonTimer.delay                  = starter.startDelay;
                starter.status                     = STARTER_START_DELAY;
                vELECTROsendCmd( ELECTRO_CMD_DISABLE_START_ALARMS );
                vRELAYset( &fuel.pump, RELAY_ON );
                if ( starter.idlingDelay != 0U )
                {
                  vRELAYset( &idleRelay, RELAY_ON );
                }
                vLOGICstartTimer( &commonTimer );
                vLOGICprintStarterStatus( starter.status );
              }
              break;
            case STARTER_START_DELAY:
              if ( uLOGICisTimer( commonTimer ) > 0U )
              {
                starter.status   = STARTER_READY;
                preHeater.active = PERMISSION_ENABLE;
                vLOGICprintStarterStatus( starter.status );
              }
              break;
            case STARTER_READY:
              starter.iteration++;
              starter.status = STARTER_CRANKING;
              starter.set( RELAY_ON );
              commonTimer.delay = starter.crankingDelay;
              vLOGICstartTimer( &commonTimer );
              vLOGICprintStarterStatus( starter.status );
              break;
            case STARTER_CRANKING:
              if ( uENGINEisWork( xADCGetGENLFreq(), oilVal, chargerVal, speedVal ) > 0U )
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
                if ( starter.iteration < starter.attempts )
                {
                  starter.status = STARTER_CRANK_DELAY;
                  commonTimer.delay = starter.crankDelay;
                  vLOGICstartTimer( &commonTimer );
                  vLOGICprintStarterStatus( starter.status );
                }
                else
                {
                  vRELAYset( &fuel.pump, RELAY_OFF );
                  vRELAYdelayTrig( &stopSolenoid );
                  preHeater.active = PERMISSION_DISABLE;
                  starter.status   = STARTER_FAIL;
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
                commonTimer.delay = starter.idlingDelay;
                vLOGICstartTimer( &commonTimer );
                starter.status                     = STARTER_IDLE_WORK;
                blockTimerFinish                   = 1U;
                speed.hightAlarm.error.active      = PERMISSION_ENABLE;
                oil.alarm.error.active             = PERMISSION_ENABLE;
                oil.preAlarm.error.active          = PERMISSION_ENABLE;
                battery.hightAlarm.error.active    = PERMISSION_ENABLE;
                battery.lowAlarm.error.active      = PERMISSION_ENABLE;
                //charger.error.error.active    = PERMISSION_ENABLE;
                //charger.hightPreAlarm.error.active = PERMISSION_ENABLE;
                vELECTROsendCmd( ELECTRO_CMD_ENABLE_START_TO_IDLE_ALARMS );
                vLOGICprintStarterStatus( starter.status );
              }
              break;
            case STARTER_IDLE_WORK:
              if ( uLOGICisTimer( commonTimer ) > 0U )
              {
                vRELAYset( &idleRelay, RELAY_OFF );
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
                starter.status              = STARTER_WARMING;
                speed.lowAlarm.error.active = PERMISSION_ENABLE;
                vELECTROsendCmd( ELECTRO_CMD_ENABLE_IDLE_ALARMS );
                vLOGICprintStarterStatus( starter.status );
              }
              break;
            case STARTER_WARMING:
              if ( uLOGICisTimer( commonTimer ) > 0U )
              {
                starter.status   = STARTER_OK;
                preHeater.active = PERMISSION_DISABLE;
                vRELAYset( &preHeater.relay, RELAY_OFF );
                vFPOsetGenReady( RELAY_ON );
                vLOGICprintStarterStatus( starter.status );
              }
              break;
            case STARTER_FAIL:
              engine.status            = ENGINE_STATUS_FAIL_STARTING;
              engine.cmd               = ENGINE_CMD_NONE;
              starter.status           = STARTER_IDLE;
              engine.cmd               = ENGINE_CMD_NONE;
              engine.startError.active = PERMISSION_ENABLE;
              starter.iteration   = 0U;
              vLOGICprintStarterStatus( starter.status );
              break;
            case STARTER_OK:
              engine.status          = ENGINE_STATUS_WORK;
              engine.cmd             = ENGINE_CMD_NONE;
              starter.status         = STARTER_IDLE;
              starter.iteration = 0U;
              eDATAAPIfreeData( DATA_API_CMD_INC,  ENGINE_STARTS_NUMBER_ADR, NULL );
              eDATAAPIfreeData( DATA_API_CMD_SAVE, ENGINE_STARTS_NUMBER_ADR, NULL );
              maintence.oil.error.active  = PERMISSION_ENABLE;
              maintence.air.error.active  = PERMISSION_ENABLE;
              maintence.fuel.error.active = PERMISSION_ENABLE;
              vLOGICprintStarterStatus( starter.status );
              event.action = ACTION_NONE;
              event.type   = EVENT_ENGINE_START;
              vSYSeventSend( event, NULL );
              break;
            default:
              vLOGICresetTimer( commonTimer );
              engine.status  = ENGINE_STATUS_FAIL_STARTING;
              engine.cmd     = ENGINE_CMD_NONE;
              starter.status = STARTER_FAIL;
              starter.set( RELAY_OFF );
              vRELAYset( &preHeater.relay, RELAY_OFF );
              vRELAYset( &idleRelay, RELAY_OFF );
              vRELAYset( &fuel.pump, RELAY_OFF );
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
                planStop.status = STOP_WAIT_ELECTRO;
                vELECTROsendCmd( ELECTRO_CMD_DISABLE_IDLE_ALARMS );
                vLOGICprintPlanStopStatus( planStop.status );
              }
              break;
            case STOP_WAIT_ELECTRO:
              if ( eELECTROgetAlarmStatus() == ELECTRO_ALARM_STATUS_WORK_ON_IDLE )
              {
                if ( planStop.coolingIdleDelay != 0U )
                {
                  vRELAYset( &idleRelay, RELAY_ON );
                }
                commonTimer.delay           = planStop.coolingIdleDelay;
                speed.lowAlarm.error.active = PERMISSION_DISABLE;
                planStop.status             = STOP_IDLE_COOLDOWN;
                vLOGICstartTimer( &commonTimer );
                vLOGICprintPlanStopStatus( planStop.status );
              }
              break;
            case STOP_IDLE_COOLDOWN:
              if ( uLOGICisTimer( commonTimer ) > 0U )
              {
                oil.alarm.error.active    = PERMISSION_DISABLE;
                oil.preAlarm.error.active = PERMISSION_DISABLE;
                vRELAYset( &idleRelay, RELAY_OFF );
                vRELAYset( &fuel.pump, RELAY_OFF );
                vRELAYdelayTrig( &stopSolenoid );
                commonTimer.delay = planStop.processDelay;
                planStop.status   = STOP_PROCESSING;
                vELECTROsendCmd( ELECTRO_CMD_DISABLE_START_ALARMS );
                vLOGICstartTimer( &commonTimer );
                vLOGICprintPlanStopStatus( planStop.status );
              }
              break;
            case STOP_PROCESSING:
              if ( uLOGICisTimer( commonTimer ) > 0U )
              {
                planStop.status = STOP_FAIL;
                vLOGICprintPlanStopStatus( planStop.status );
                vELECTROsendCmd( ELECTRO_CMD_ENABLE_STOP_ALARMS );
              }
              if ( uENGINEisStop( fELECTROgetMaxGenVoltage(), xADCGetGENLFreq(), oilVal, speedVal ) > 0U )
              {
                vLOGICresetTimer( commonTimer );
                planStop.status = STOP_OK;
                vLOGICprintPlanStopStatus( planStop.status );
                vELECTROsendCmd( ELECTRO_CMD_ENABLE_STOP_ALARMS );
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
              engine.status               = ENGINE_STATUS_IDLE;
              engine.cmd                  = ENGINE_CMD_NONE;
              planStop.status             = STOP_IDLE;
              maintence.oil.error.active  = 0U;
              maintence.air.error.active  = 0U;
              maintence.fuel.error.active = 0U;
              vFPOsetReadyToStart( RELAY_ON );
              vLOGICprintPlanStopStatus( planStop.status );
              event.action = ACTION_NONE;
              event.type   = EVENT_ENGINE_STOP;
              vSYSeventSend( event, NULL );
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
        switch ( engine.status )
        {
          case ENGINE_STATUS_WORK:
            vELECTROsendCmd( ELECTRO_CMD_DISABLE_IDLE_ALARMS );
            engine.status = ENGINE_STATUS_WORK_WAIT_ELECTRO;
            break;
          case ENGINE_STATUS_WORK_WAIT_ELECTRO:
            if ( eELECTROgetAlarmStatus() == ELECTRO_ALARM_STATUS_WORK_ON_IDLE )
            {
              speed.lowAlarm.error.active = 0U;
              vRELAYset( &idleRelay, RELAY_ON );
              vFPOsetGenReady( RELAY_OFF );
              vLOGICprintEngineStatus( engine.status );
              engine.cmd    = ENGINE_CMD_NONE;
              engine.status = ENGINE_STATUS_WORK_ON_IDLE;
            }
            break;
          default:
            engine.cmd = ENGINE_CMD_NONE;
            break;
        }
        break;
      /*----------------------------------------------------------------------------------------*/
      /*--------------------------- ENGINE GO TO NOMINAL FROM IDLE -----------------------------*/
      /*----------------------------------------------------------------------------------------*/
      case ENGINE_CMD_GOTO_NORMAL:
        switch ( engine.status )
        {
          case ENGINE_STATUS_WORK_ON_IDLE:
            vRELAYset( &idleRelay, RELAY_OFF );
            vLOGICprintEngineStatus( engine.status );
            commonTimer.delay = starter.nominalDelay;
            vLOGICstartTimer( &commonTimer );
            engine.status = ENGINE_STATUS_WORK_GOTO_NOMINAL;
            break;
          case ENGINE_STATUS_WORK_GOTO_NOMINAL:
            if ( uLOGICisTimer( commonTimer ) > 0U )
            {
              engine.status = ENGINE_STATUS_WORK;
              vLOGICprintEngineStatus( engine.status );
              speed.lowAlarm.error.active = PERMISSION_ENABLE;
              vFPOsetGenReady( RELAY_ON );
              vELECTROsendCmd( ELECTRO_CMD_ENABLE_IDLE_ALARMS );
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
        vRELAYset( &fuel.pump, RELAY_OFF );
        vRELAYset( &fuel.booster.relay, RELAY_OFF );
        vRELAYset( &coolant.cooler.relay, RELAY_OFF );
        vRELAYset( &coolant.heater.relay, RELAY_OFF );
        vRELAYset( &idleRelay, RELAY_OFF );
        vRELAYdelayTrig( &stopSolenoid );
        preHeater.active = 0U;
        vRELAYset( &preHeater.relay, RELAY_OFF );
        vFPOsetGenReady( RELAY_OFF );
        vFPOsetReadyToStart( RELAY_OFF );
        engine.status             = ENGINE_STATUS_EMERGENCY_STOP;
        engine.cmd                = ENGINE_CMD_NONE;
        engine.startError.active  = PERMISSION_DISABLE;
        engine.stopError.active   = PERMISSION_ENABLE;
        oil.alarm.error.active    = PERMISSION_DISABLE;
        oil.preAlarm.error.active = PERMISSION_DISABLE;

        vLOGICresetTimer( commonTimer );
        maintence.timer.id        = LOGIC_DEFAULT_TIMER_ID;
        commonTimer.id            = LOGIC_DEFAULT_TIMER_ID;
        break;
      /*----------------------------------------------------------------------------------------*/
      /*------------------------------- ENGINE RESET TO IDLE -----------------------------------*/
      /*----------------------------------------------------------------------------------------*/
      case ENGINE_CMD_RESET_TO_IDLE:
        vENGINEresetAlarms();
        starter.status  = STARTER_IDLE;
        planStop.status = STOP_IDLE;
        engine.status   = ENGINE_STATUS_IDLE;
        vRELAYset( &stopSolenoid.relay, RELAY_OFF );
        speed.lowAlarm.error.active = PERMISSION_DISABLE;
        vFPOsetReadyToStart( RELAY_ON );
        vFPOsetGenReady( RELAY_OFF );
        engine.cmd               = ENGINE_CMD_NONE;
        engine.startError.active = PERMISSION_DISABLE;
        engine.stopError.active  = PERMISSION_ENABLE;
        break;
      /*----------------------------------------------------------------------------------------*/
      /*--------------------------------- ENGINE BAN START -------------------------------------*/
      /*----------------------------------------------------------------------------------------*/
      case ENGINE_CMD_BAN_START:
        engine.banStart = PERMISSION_ENABLE;
        break;
      case ENGINE_CMD_ALLOW_START:
        engine.banStart = PERMISSION_DISABLE;
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
    /*------------------------------------------------------------------------------------------*/
  }
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


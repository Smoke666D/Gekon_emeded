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
#include "lcd.h"
#include "charger.h"
/*-------------------------------- Structures --------------------------------*/
static ENGINE_TYPE        engine              = { 0U };
static OIL_TYPE           oil                 = { 0U };
static COOLANT_TYPE       coolant             = { 0U };
static FUEL_TYPE          fuel                = { 0U };
static SPEED_TYPE         speed               = { 0U };
static BATTERY_TYPE       battery             = { 0U };
static CHARGER_TYPE       charger             = { 0U };
static STARTER_TYPE       starter             = { 0U };
static PLAN_STOP_TYPE     planStop            = { 0U };
static MAINTENCE_TYPE     maintence           = { 0U };
static RELAY_DELAY_DEVICE stopSolenoid        = { 0U };
static RELAY_DEVICE       idleRelay           = { 0U };
static PREHEATER_TYPE     preHeater           = { 0U };
static StaticQueue_t      xEngineCommandQueue = { 0U };
static QueueHandle_t      pEngineCommandQueue = NULL;
static EMERGENCY_STATUS   emgencyStopStatus   = EMERGENCY_STATUS_IDLE;
/*--------------------------------- Constant ---------------------------------*/
static const fix16_t fix60                  = F16( 60U );                      /* --- */
static const fix16_t chargerImpulseDuration = F16( CHARGER_IMPULSE_DURATION ); /* sec */
static const fix16_t oilTrashhold           = F16( 0.015 );                    /* Bar */
static const fix16_t sensorCutoutLevel      = F16( SENSOR_CUTOUT_LEVEL );      /* Ohm */
static const fix16_t fuelPrePumpingDelay    = F16( 1 );                        /* sec */
#if ( DEBUG_SERIAL_STATUS > 0U )
  static const char* cSensorTypes[5U] =
  {
    "NONE",
    "NORMAL_OPEN",
    "NORMAL_CLOSE",
    "RESISTIVE",
    "CURRENT"
  };
  static const char* engineCmdStr[8U] =
  {
    "NONE",
    "START",
    "STOP",
    "GOTO_IDLE",
    "GOTO_NORMAL",
    "EMEGENCY_STOP",
    "RESET_TO_IDLE",
    "BAN_START"
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
  static const char* starterStatusStr[13U] =
  {
    "IDLE",
    "PREHEATING",
    "FUEL_PREPUMPING",
    "START_PREPARATION",
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
  static const char* emergencyStopStatusStr[3U] =
  {
    "START",
    "PROCESSING",
    "END"
  };
#endif
/*-------------------------------- Variables ---------------------------------*/
static ENGINE_COMMAND engineCommandBuffer[ENGINE_COMMAND_QUEUE_LENGTH] = { 0U };
static TRIGGER_STATE  starterFinish                                    = TRIGGER_IDLE;
static TRIGGER_STATE  blockTimerFinish                                 = TRIGGER_IDLE;
static fix16_t        currentSpeed                                     = 0U;
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
TRIGGER_STATE uENGINEisSensorCutout ( fix16_t value )
{
  TRIGGER_STATE res = TRIGGER_IDLE;
  if ( value >= sensorCutoutLevel )
  {
    res = TRIGGER_SET;
  }
  return res;
}
/*----------------------------------------------------------------------------*/
uint8_t eSENSORcheckCutout ( SENSOR_CHANNEL source )
{
  uint8_t res = 0U;

  switch ( source )
  {
    case SENSOR_CHANNEL_OIL:
      res = uADCGetDCChError( OP_CHANEL_ERROR );
      break;
    case SENSOR_CHANNEL_COOLANT:
      res = uADCGetDCChError( CT_CHANEL_ERROR );
      break;
    case SENSOR_CHANNEL_FUEL:
      res = uADCGetDCChError( FL_CHANEL_ERROR );
      break;
    case SENSOR_CHANNEL_COMMON:
      res = uADCGetDCChError( COMMON_ERROR );
      break;
    default:
      break;
  }

  return res;
}
/*----------------------------------------------------------------------------*/
uint8_t uSENSORisAnalog ( SENSOR sensor )
{
  uint8_t res = 0U;
  if ( ( sensor.type == SENSOR_TYPE_RESISTIVE ) || ( sensor.type == SENSOR_TYPE_CURRENT ) )
  {
    res = 1U;
  }
  return res;
}
/*----------------------------------------------------------------------------*/
uint8_t uSENSORisDiscret ( SENSOR sensor )
{
  uint8_t res = 0U;
  if ( ( sensor.type == SENSOR_TYPE_NORMAL_OPEN ) || ( sensor.type == SENSOR_TYPE_NORMAL_CLOSE ) )
  {
    res = 1U;
  }
  return res;
}
/*----------------------------------------------------------------------------*/
uint8_t uSENSORisDisable ( SENSOR sensor )
{
  uint8_t res = 0U;
  if ( sensor.type == SENSOR_TYPE_NONE )
  {
    res = 1U;
  }
  return res;
}
/*----------------------------------------------------------------------------*/
void vSENSORprocess ( SENSOR* sensor, fix16_t* value )
{
  eFunctionError funcStat = SENSOR_STATUS_NORMAL;

  *value = sensor->get();
  //vERRORcheck( &sensor->cutout, eSENSORcheckCutout( sensor->channel ) );
  if ( sensor->cutout.status != ALARM_STATUS_IDLE )
  {
    sensor->status = SENSOR_STATUS_LINE_ERROR;
  }
  else
  {
    if ( uSENSORisAnalog( *sensor ) > 0U )
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
    else if ( sensor->type == SENSOR_TYPE_NORMAL_OPEN )
    {
      sensor->trig   = !uENGINEisSensorCutout( *value );
      sensor->status = SENSOR_STATUS_NORMAL;
    }
    else if ( sensor->type == SENSOR_TYPE_NORMAL_CLOSE )
    {
      sensor->trig   = uENGINEisSensorCutout( *value );
      sensor->status = SENSOR_STATUS_NORMAL;
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
  if ( uSENSORisAnalog( oil.pressure ) > 0U )
  {
    vALARMcheck( &oil.alarm, value );
    vALARMcheck( &oil.preAlarm, value );
  }
  else if ( uSENSORisDiscret( oil.pressure ) > 0U )
  {
    vERRORcheck( &oil.alarm.error, oil.pressure.trig );
  }
  else
  {

  }
  return value;
}
/*----------------------------------------------------------------------------*/
fix16_t fCOOLANTprocess ( void )
{
  fix16_t value = 0U;
  vSENSORprocess( &coolant.temp, &value );
  if ( uSENSORisAnalog( coolant.temp ) > 0U )
  {
    vALARMcheck( &coolant.alarm, value );
    vALARMcheck( &coolant.electroAlarm, value );
    vALARMcheck( &coolant.preAlarm, value );
    vRELAYautoProces( &coolant.heater, value );
    vRELAYautoProces( &coolant.cooler, value );
  }
  else if ( uSENSORisDiscret( coolant.temp ) > 0U)
  {
    vERRORcheck( &coolant.alarm.error, coolant.temp.trig );
  }
  else
  {

  }
  return value;
}
/*----------------------------------------------------------------------------*/
fix16_t fFUELprocess ( void )
{
  fix16_t value = 0U;
  vSENSORprocess( &fuel.level, &value );
  if ( uSENSORisAnalog( fuel.level ) > 0U)
  {
    vALARMcheck( &fuel.lowAlarm, value );
    vALARMcheck( &fuel.lowPreAlarm, value );
    vALARMcheck( &fuel.hightAlarm, value );
    vALARMcheck( &fuel.hightPreAlarm, value );
    vRELAYautoProces( &fuel.booster, value );
  }
  else if ( uSENSORisDiscret( fuel.level ) > 0U )
  {
    vERRORcheck( &fuel.lowAlarm.error, fuel.level.trig );
  }
  else
  {

  }
  return value;
}
/*----------------------------------------------------------------------------*/
fix16_t fSPEEDfromFreq ( fix16_t freq )
{
  fix16_t sub = fix16_div( freq, speed.polePairs );
  fix16_t res = fix16_mul( sub, fix60 );
  float a = fix16_to_float( freq );
  float b = fix16_to_float( sub );
  float c = fix16_to_float( res );
  float d = fix16_to_float( speed.polePairs );
  return fix16_mul( fix16_div( freq, speed.polePairs ), fix60 ); /* RPM */
}
/*----------------------------------------------------------------------------*/
fix16_t fSPEEDprocess ( void )
{
  fix16_t value = 0U;
  value = speed.get();
  vALARMcheck( &speed.hightAlarm, value );
  if ( speed.hightAlarm.error.status == ALARM_STATUS_IDLE )
  {
    vALARMcheck( &speed.lowAlarm, value );
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
fix16_t fCHARGERprocess ( void )
{
  fix16_t value = 0U;
  if ( charger.enb == PERMISSION_ENABLE )
  {
    if ( charger.start == PERMISSION_ENABLE )
    {
      switch ( charger.status )
      {
        case CHARGER_STATUS_IDLE:
          if ( charger.get() < charger.alarm.level )
          {
            charger.relay.set( RELAY_ON );
            vLOGICstartTimer( &charger.timer, "Charger timer       " );
            charger.status = CHARGER_STATUS_DELAY;
          }
          else
          {
            charger.status = CHARGER_STATUS_MEASURING;
          }
          break;
        case CHARGER_STATUS_DELAY:
          if ( uLOGICisTimer( &charger.timer ) > 0U )
          {
            charger.relay.set( RELAY_OFF );
            charger.status = CHARGER_STATUS_MEASURING;
          }
          break;
        case CHARGER_STATUS_MEASURING:
          value = charger.get();
          vALARMcheck( &charger.alarm, value );
          break;
        default:
          charger.relay.set( RELAY_OFF );
          charger.status = CHARGER_STATUS_IDLE;
          break;
      }
    }
    else
    {
      if ( charger.status != CHARGER_STATUS_IDLE )
      {
        charger.relay.set( RELAY_OFF );
        vERRORrelax( &charger.alarm.error );
        charger.status = CHARGER_STATUS_IDLE;
      }
      else
      {
        value = charger.get();
      }
    }
  }
  return value;
}
/*----------------------------------------------------------------------------*/
void vENGINEmileageProcess ( void )
{
  uint16_t data     = 0U;
  uint8_t  changeFl = 0U;

  vALARMcheck( &maintence.oil.alarm,  fix16_from_int( maintence.oil.data  ) );
  vALARMcheck( &maintence.air.alarm,  fix16_from_int( maintence.air.data  ) );
  vALARMcheck( &maintence.fuel.alarm, fix16_from_int( maintence.fuel.data ) );
  if ( engine.status == ENGINE_STATUS_WORK )
  {
    switch ( maintence.status )
    {
      case MAINTENANCE_STATUS_STOP:
        vLOGICstartTimer( &maintence.timer, "Maintence timer     " );
        maintence.status = MAINTENANCE_STATUS_RUN;
        break;
      case MAINTENANCE_STATUS_RUN:
        if ( uLOGICisTimer( &maintence.timer ) > 0U )
        {
          eDATAAPIfreeData( DATA_API_CMD_INC,  ENGINE_WORK_MINUTES_ADR, &data );
          if ( data >= 60U )
          {
            eDATAAPIfreeData( DATA_API_CMD_ERASE, ENGINE_WORK_MINUTES_ADR, NULL );
            eDATAAPIfreeData( DATA_API_CMD_INC,   ENGINE_WORK_TIME_ADR,    &data );
            maintence.status = MAINTENANCE_STATUS_CHECK;
          }
          else
          {
            vLOGICstartTimer( &maintence.timer, "Maintence timer     " );
          }
          eDATAAPIfreeData( DATA_API_CMD_SAVE, 0U, NULL );
        }
        break;
      case MAINTENANCE_STATUS_CHECK:
        changeFl = 0U;
        if ( ( maintence.oil.alarm.error.enb    == PERMISSION_ENABLE ) &&
             ( maintence.oil.alarm.error.active == PERMISSION_ENABLE ) &&
             ( maintence.oil.alarm.error.status == ALARM_STATUS_IDLE ) )
        {
          eDATAAPIfreeData( DATA_API_CMD_INC, MAINTENANCE_ALARM_OIL_TIME_LEFT_ADR, &data );
          maintence.oil.data++;
          changeFl = 1U;
        }
        if ( ( maintence.air.alarm.error.enb    == PERMISSION_ENABLE ) &&
             ( maintence.air.alarm.error.active == PERMISSION_ENABLE ) &&
             ( maintence.air.alarm.error.status == ALARM_STATUS_IDLE ) )
        {
          eDATAAPIfreeData( DATA_API_CMD_INC, MAINTENANCE_ALARM_AIR_TIME_LEFT_ADR, &data );
          maintence.air.data++;
          changeFl = 1U;
        }
        if ( ( maintence.fuel.alarm.error.enb    == PERMISSION_ENABLE ) &&
             ( maintence.fuel.alarm.error.active == PERMISSION_ENABLE ) &&
             ( maintence.fuel.alarm.error.status == ALARM_STATUS_IDLE ) )
        {
          eDATAAPIfreeData( DATA_API_CMD_INC, MAINTENANCE_ALARM_FUEL_TIME_LEFT_ADR, &data );
          maintence.fuel.data++;
          changeFl = 1U;
        }
        if ( changeFl > 0U )
        {
          eDATAAPIfreeData( DATA_API_CMD_SAVE, 0U, NULL );
        }
        vLOGICstartTimer( &maintence.timer, "Maintence timer     " );
        maintence.status = MAINTENANCE_STATUS_RUN;
        break;
      default:
        maintence.status = MAINTENANCE_STATUS_STOP;
        vLOGICresetTimer( &maintence.timer );
        break;
    }
  }
  else if ( maintence.status != MAINTENANCE_STATUS_STOP )
  {
    maintence.status = MAINTENANCE_STATUS_STOP;
    vLOGICresetTimer( &maintence.timer );
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
  if ( starter.startCrit.critGenFreqEnb == PERMISSION_ENABLE )
  {
    if ( vADCGetGenFreqPres() == FREQ_DETECTED )
    {
      res = 1U;
      #if ( DEBUG_SERIAL_STATUS > 0U )
        vSYSSerial( ">>Engine          : Started by generator frequency: LOW\r\n" );
      #endif
    }
    if ( ( freq >= starter.startCrit.critGenFreqLevel ) && ( res == 0U ) )
    {
      res = 1U;
      #if ( DEBUG_SERIAL_STATUS > 0U )
        vSYSSerial( ">>Engine          : Started by generator frequency: " );
        fix16_to_str( freq, buffer, 2U );
        vSYSSerial( buffer );
        vSYSSerial( "\r\n" );
      #endif
    }
  }
  if ( ( starter.startCrit.critOilPressEnb == PERMISSION_ENABLE ) && ( pressure >= starter.startCrit.critOilPressLevel ) )
  {
    res = 1U;
    #if ( DEBUG_SERIAL_STATUS > 0U )
      vSYSSerial( ">>Engine          : Started by oil pressure: " );
      fix16_to_str( pressure, buffer, 2U );
      vSYSSerial( buffer );
      vSYSSerial( "\r\n" );
    #endif
  }
  if ( ( starter.startCrit.critChargeEnb == PERMISSION_ENABLE ) && ( voltage >= starter.startCrit.critChargeLevel ) )
  {
    res = 1U;
    #if ( DEBUG_SERIAL_STATUS > 0U )
      vSYSSerial( ">>Engine          : Started by charger voltage: " );
      fix16_to_str( voltage, buffer, 2U );
      vSYSSerial( buffer );
      vSYSSerial( "\r\n" );
    #endif
  }
  if ( ( starter.startCrit.critSpeedEnb == PERMISSION_ENABLE ) && ( speed >= starter.startCrit.critSpeedLevel ) )
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
uint8_t uENGINEisStop ( fix16_t voltage, fix16_t freq, fix16_t pressure, TRIGGER_STATE oilTrig, fix16_t speed  )
{
  uint8_t oilRes = 0U;
  uint8_t res    = 0U;

  if ( engine.startCheckOil == PERMISSION_ENABLE )
  {
    switch ( oil.pressure.type )
    {
      case SENSOR_TYPE_NONE:
        oilRes = 1U;
        break;
      case SENSOR_TYPE_NORMAL_OPEN:
        if ( oilTrig == TRIGGER_SET )
        {
          oilRes = 1U;
        }
        break;
     case SENSOR_TYPE_NORMAL_CLOSE:
        if ( TRIGGER_SET == TRIGGER_SET )
        {
          oilRes = 1U;
        }
        break;
      case SENSOR_TYPE_RESISTIVE:
        if ( pressure < oil.trashhold )
        {
          oilRes = 1U;
        }
        break;
      case SENSOR_TYPE_CURRENT:
        if ( pressure < oil.trashhold )
        {
          oilRes = 1U;
        }
        break;
      default:
        oilRes = 0U;
        break;
    }
  }
  else
  {
    oilRes = 1U;
  }
  if ( ( oilRes   == 1U ) &&
       ( speed    == 0U ) &&
       ( freq     == 0U ) &&
       ( voltage  == 0U ) )
  {
    res = 1U;
  }
  return res;
}
/*----------------------------------------------------------------------------*/
uint8_t uENGINEisUnban( void )
{
  uint8_t         res    = 1U;
  uint8_t         size   = 0U;
  uint8_t         i      = 0U;
  LOG_RECORD_TYPE record = { 0U };
  eLOGICERactiveErrorList( ERROR_LIST_CMD_COUNTER, NULL, &size );
  for ( i=0U; i<size; i++ )
  {
    eLOGICERactiveErrorList( ERROR_LIST_CMD_READ, &record, &i );
    if ( record.event.action == ACTION_BAN_START )
    {
      res = 0U;
    }
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
    str[6U] = 0U;
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
    str[7U] = 0U;
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
void vLOGICprintPlanStopStatus ( STOP_STATUS status )
{
  #if ( DEBUG_SERIAL_STATUS > 0U )
    vSYSSerial( ">>Plan stop status: " );
    vSYSSerial( planStopStatusStr[status] );
    vSYSSerial( "\r\n" );
  #endif
  return;
}
/*----------------------------------------------------------------------------*/
void vLOGICprintEmergencyStopStatus ( EMERGENCY_STATUS status )
{
  #ifdef DEBUG
    vSYSSerial( ">>Emergency status: " );
    vSYSSerial( emergencyStopStatusStr[status] );
    vSYSSerial( "\r\n" );
  #endif
  return;
}
/*----------------------------------------------------------------------------*/
void vENGINEdataInit ( void )
{
  oil.trashhold                    = fix16_mul( getMaxValue( &oilPressureAlarmLevel ), oilTrashhold );
  oil.pressure.type                = getBitMap( &oilPressureSetup, OIL_PRESSURE_SENSOR_TYPE_ADR );
  oil.pressure.chart               = charts[OIL_CHART_ADR];
  oil.pressure.get                 = OIL_SENSOR_SOURCE;
  oil.pressure.channel             = SENSOR_CHANNEL_OIL;
  oil.pressure.trig                = TRIGGER_IDLE;
  oil.pressure.cutout.active       = PERMISSION_ENABLE;
  oil.pressure.cutout.status       = ALARM_STATUS_IDLE;
  oil.pressure.cutout.ack          = PERMISSION_DISABLE;
  oil.pressure.cutout.event.action = ACTION_EMERGENCY_STOP;
  oil.pressure.cutout.event.type   = EVENT_OIL_SENSOR_ERROR;
  oil.pressure.cutout.trig         = TRIGGER_IDLE;
  oil.pressure.status              = SENSOR_STATUS_NORMAL;
  if ( oil.pressure.type == SENSOR_TYPE_NONE )
  {
    oil.alarm.error.enb     = PERMISSION_DISABLE;
    oil.preAlarm.error.enb  = PERMISSION_DISABLE;
    oil.pressure.cutout.enb = PERMISSION_DISABLE;
  }
  else if ( uSENSORisAnalog( oil.pressure ) > 0U )
  {
    oil.pressure.cutout.enb = getBitMap( &oilPressureSetup, OIL_PRESSURE_OPEN_CIRCUIT_ALARM_ENB_ADR );
    oil.alarm.error.enb     = getBitMap( &oilPressureSetup, OIL_PRESSURE_ALARM_ENB_ADR              );
    oil.preAlarm.error.enb  = getBitMap( &oilPressureSetup, OIL_PRESSURE_PRE_ALARM_ENB_ADR          );
  }
  else
  {
    oil.alarm.error.enb     = PERMISSION_ENABLE;
    oil.preAlarm.error.enb  = PERMISSION_DISABLE;
    oil.pressure.cutout.enb = PERMISSION_DISABLE;
  }
  oil.alarm.type                  = ALARM_LEVEL_LOW;
  oil.alarm.level                 = getValue( &oilPressureAlarmLevel );
  oil.alarm.timer.delay           = 0U;
  oil.alarm.timer.id              = LOGIC_DEFAULT_TIMER_ID;
  oil.alarm.error.active          = PERMISSION_DISABLE;
  oil.alarm.error.event.type      = EVENT_OIL_LOW_PRESSURE;
  oil.alarm.error.event.action    = ACTION_EMERGENCY_STOP;
  oil.alarm.error.ack             = PERMISSION_DISABLE;
  oil.alarm.error.trig            = TRIGGER_IDLE;
  oil.alarm.error.status          = ALARM_STATUS_IDLE;
  oil.preAlarm.type               = ALARM_LEVEL_LOW;
  oil.preAlarm.level              = getValue( &oilPressurePreAlarmLevel );
  oil.preAlarm.timer.delay        = 0U;
  oil.preAlarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  oil.preAlarm.error.active       = PERMISSION_DISABLE;
  oil.preAlarm.error.event.type   = EVENT_OIL_LOW_PRESSURE;
  oil.preAlarm.error.event.action = ACTION_WARNING;
  oil.preAlarm.error.ack          = PERMISSION_ENABLE;
  oil.preAlarm.error.trig         = TRIGGER_IDLE;
  oil.preAlarm.error.status       = ALARM_STATUS_IDLE;
  /*--------------------------------------------------------------*/
  coolant.temp.type                = getBitMap( &coolantTempSetup, COOLANT_TEMP_SENSOR_TYPE_ADR );
  coolant.temp.chart               = charts[COOLANT_CHART_ADR];
  coolant.temp.channel             = SENSOR_CHANNEL_COOLANT;
  coolant.temp.get                 = COOLANT_SENSOR_SOURCE;
  coolant.temp.trig                = TRIGGER_IDLE;
  coolant.temp.cutout.active       = PERMISSION_ENABLE;
  coolant.temp.cutout.ack          = PERMISSION_DISABLE;
  coolant.temp.cutout.event.action = ACTION_EMERGENCY_STOP;
  coolant.temp.cutout.event.type   = EVENT_ENGINE_TEMP_SENSOR_ERROR;
  coolant.temp.cutout.trig         = TRIGGER_IDLE;
  coolant.temp.cutout.status       = ALARM_STATUS_IDLE;
  coolant.temp.status              = SENSOR_STATUS_NORMAL;
  if ( coolant.temp.type == SENSOR_TYPE_NONE )
  {
    coolant.temp.cutout.enb        = PERMISSION_DISABLE;
    coolant.alarm.error.enb        = PERMISSION_DISABLE;
    coolant.electroAlarm.error.enb = PERMISSION_DISABLE;
    coolant.preAlarm.error.enb     = PERMISSION_DISABLE;
    coolant.heater.relay.enb       = PERMISSION_DISABLE;
    coolant.cooler.relay.enb       = PERMISSION_DISABLE;
  }
  else if ( uSENSORisAnalog( coolant.temp ) > 0U )
  {
    coolant.temp.cutout.enb        = getBitMap( &coolantTempSetup, FUEL_LEVEL_OPEN_CIRCUIT_ALARM_ENB_ADR    );
    coolant.alarm.error.enb        = getBitMap( &coolantTempSetup, COOLANT_HIGHT_TEMP_ALARM_ENB_ADR         );
    coolant.electroAlarm.error.enb = getBitMap( &coolantTempSetup, COOLANT_HIGHT_TEMP_ELECTRO_ALARM_ENB_ADR );
    coolant.preAlarm.error.enb     = getBitMap( &coolantTempSetup, COOLANT_HIGHT_TEMP_PRE_ALARM_ENB_ADR     );
    coolant.heater.relay.enb       = getBitMap( &coolantTempSetup, COOLANT_TEMP_HEATER_ENB_ADR              );
    coolant.cooler.relay.enb       = getBitMap( &coolantTempSetup, COOLANT_TEMP_COOLER_ENB_ADR              );
  }
  else
  {
    coolant.temp.cutout.enb        = PERMISSION_DISABLE;
    coolant.alarm.error.enb        = PERMISSION_ENABLE;
    coolant.electroAlarm.error.enb = PERMISSION_DISABLE;
    coolant.preAlarm.error.enb     = PERMISSION_DISABLE;
    coolant.heater.relay.enb       = PERMISSION_DISABLE;
    coolant.cooler.relay.enb       = PERMISSION_DISABLE;
  }
  coolant.alarm.error.active              = PERMISSION_ENABLE;
  coolant.alarm.type                      = ALARM_LEVEL_HIGHT;
  coolant.alarm.level                     = getValue( &coolantHightTempAlarmLevel );
  coolant.alarm.timer.delay               = 0U;
  coolant.alarm.timer.id                  = LOGIC_DEFAULT_TIMER_ID;
  coolant.alarm.error.event.type          = EVENT_ENGINE_HIGHT_TEMP;
  coolant.alarm.error.event.action        = ACTION_EMERGENCY_STOP;
  coolant.alarm.error.ack                 = PERMISSION_DISABLE;
  coolant.alarm.error.trig                = TRIGGER_IDLE;
  coolant.alarm.error.status              = ALARM_STATUS_IDLE;
  coolant.electroAlarm.error.active       = PERMISSION_ENABLE;
  coolant.electroAlarm.type               = ALARM_LEVEL_HIGHT;
  coolant.electroAlarm.level              = getValue( &coolantHightTempElectroAlarmLevel );
  coolant.electroAlarm.timer.delay        = 0U;
  coolant.electroAlarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  coolant.electroAlarm.error.ack          = PERMISSION_DISABLE;
  coolant.electroAlarm.error.event.type   = EVENT_ENGINE_HIGHT_TEMP;
  coolant.electroAlarm.error.event.action = ACTION_SHUTDOWN;
  coolant.electroAlarm.error.trig         = TRIGGER_IDLE;
  coolant.electroAlarm.error.status       = ALARM_STATUS_IDLE;
  coolant.preAlarm.error.active           = PERMISSION_ENABLE;
  coolant.preAlarm.type                   = ALARM_LEVEL_HIGHT;
  coolant.preAlarm.level                  = getValue( &coolantHightTempPreAlarmLevel );
  coolant.preAlarm.timer.delay            = 0U;
  coolant.preAlarm.timer.id               = LOGIC_DEFAULT_TIMER_ID;
  coolant.preAlarm.error.ack              = PERMISSION_ENABLE;
  coolant.preAlarm.error.event.type       = EVENT_ENGINE_HIGHT_TEMP;
  coolant.preAlarm.error.event.action     = ACTION_WARNING;
  coolant.preAlarm.error.trig             = TRIGGER_IDLE;
  coolant.preAlarm.error.status           = ALARM_STATUS_IDLE;
  coolant.heater.onLevel                  = getValue( &coolantTempHeaterOnLevel );
  coolant.heater.offLevel                 = getValue( &coolantTempHeaterOffLevel );
  coolant.heater.relay.set                = vFPOsetHeater;
  coolant.heater.relay.status             = RELAY_OFF;
  coolant.cooler.onLevel                  = getValue( &coolantTempCoolerOnLevel );
  coolant.cooler.offLevel                 = getValue( &coolantTempCoolerOffLevel );
  coolant.cooler.relay.set                = vFPOsetCooler;
  coolant.cooler.relay.status             = RELAY_OFF;
  /*--------------------------------------------------------------*/
  preHeater.relay.enb    = PERMISSION_ENABLE;
  preHeater.level        = getValue( &enginePreHeatLevel );
  preHeater.relay.set    = vFPOsetPreheater;
  preHeater.relay.status = RELAY_OFF;
  preHeater.delay        = getValue( &enginePreHeatDelay );
  /*--------------------------------------------------------------*/
  fuel.level.type                 = getBitMap( &fuelLevelSetup, FUEL_LEVEL_SENSOR_TYPE_ADR );
  fuel.level.channel              = SENSOR_CHANNEL_FUEL;
  fuel.level.chart                = charts[FUEL_CHART_ADR];
  fuel.level.get                  = FUEL_SENSOR_SOURCE;
  fuel.level.trig                 = TRIGGER_IDLE;
  fuel.level.cutout.active        = PERMISSION_ENABLE;
  fuel.level.cutout.status        = ALARM_STATUS_IDLE;
  fuel.level.cutout.ack           = PERMISSION_ENABLE;
  fuel.level.cutout.event.action  = ACTION_EMERGENCY_STOP;
  fuel.level.cutout.event.type    = EVENT_FUEL_LEVEL_SENSOR_ERROR;
  fuel.level.cutout.trig          = TRIGGER_IDLE;
  fuel.level.status               = SENSOR_STATUS_NORMAL;
  if ( fuel.level.type == SENSOR_TYPE_NONE )
  {
    fuel.level.cutout.enb        = PERMISSION_DISABLE;
    fuel.lowAlarm.error.enb      = PERMISSION_DISABLE;
    fuel.lowPreAlarm.error.enb   = PERMISSION_DISABLE;
    fuel.hightPreAlarm.error.enb = PERMISSION_DISABLE;
    fuel.hightAlarm.error.enb    = PERMISSION_DISABLE;
    fuel.booster.relay.enb       = PERMISSION_DISABLE;
  }
  else if ( uSENSORisAnalog( fuel.level ) > 0U )
  {
    fuel.level.cutout.enb        = getBitMap( &fuelLevelSetup, FUEL_LEVEL_OPEN_CIRCUIT_ALARM_ENB_ADR );
    fuel.lowAlarm.error.enb      = getBitMap( &fuelLevelSetup, FUEL_LEVEL_LOW_ALARM_ENB_ADR          );
    fuel.lowPreAlarm.error.enb   = getBitMap( &fuelLevelSetup, FUEL_LEVEL_LOW_PRE_ALARM_ENB_ADR      );
    fuel.hightPreAlarm.error.enb = getBitMap( &fuelLevelSetup, FUEL_LEVEL_HIGHT_PRE_ALARM_ENB_ADR    );
    fuel.hightAlarm.error.enb    = getBitMap( &fuelLevelSetup, FUEL_LEVEL_HIGHT_ALARM_ENB_ADR        );
    fuel.booster.relay.enb       = getBitMap( &fuelLevelSetup, FUEL_PUMP_ENB_ADR                     );
  }
  else
  {
    fuel.level.cutout.enb        = PERMISSION_DISABLE;
    fuel.lowAlarm.error.enb      = PERMISSION_ENABLE;
    fuel.lowPreAlarm.error.enb   = PERMISSION_DISABLE;
    fuel.hightPreAlarm.error.enb = PERMISSION_DISABLE;
    fuel.hightAlarm.error.enb    = PERMISSION_DISABLE;
    fuel.booster.relay.enb       = PERMISSION_DISABLE;
  }
  fuel.lowAlarm.error.active            = PERMISSION_ENABLE;
  fuel.lowAlarm.type                    = ALARM_LEVEL_LOW;
  fuel.lowAlarm.level                   = getValue( &fuelLevelLowAlarmLevel );
  fuel.lowAlarm.timer.delay             = getValue( &fuelLevelLowAlarmDelay );
  fuel.lowAlarm.timer.id                = LOGIC_DEFAULT_TIMER_ID;
  fuel.lowAlarm.error.event.type        = EVENT_FUEL_LOW_LEVEL;
  fuel.lowAlarm.error.event.action      = ACTION_SHUTDOWN;
  fuel.lowAlarm.error.ack               = PERMISSION_ENABLE;
  fuel.lowAlarm.error.trig              = TRIGGER_IDLE;
  fuel.lowAlarm.error.status            = ALARM_STATUS_IDLE;
  fuel.lowPreAlarm.error.active         = PERMISSION_ENABLE;
  fuel.lowPreAlarm.type                 = ALARM_LEVEL_LOW;
  fuel.lowPreAlarm.level                = getValue( &fuelLevelLowPreAlarmLevel );
  fuel.lowPreAlarm.timer.delay          = getValue( &fuelLevelLowPreAlarmDelay );
  fuel.lowPreAlarm.timer.id             = LOGIC_DEFAULT_TIMER_ID;
  fuel.lowPreAlarm.error.ack            = PERMISSION_ENABLE;
  fuel.lowPreAlarm.error.event.type     = EVENT_FUEL_LOW_LEVEL;
  fuel.lowPreAlarm.error.event.action   = ACTION_WARNING;
  fuel.lowPreAlarm.error.trig           = TRIGGER_IDLE;
  fuel.lowPreAlarm.error.status         = ALARM_STATUS_IDLE;
  fuel.hightPreAlarm.error.active       = PERMISSION_ENABLE;
  fuel.hightPreAlarm.type               = ALARM_LEVEL_HIGHT;
  fuel.hightPreAlarm.level              = getValue( &fuelLevelHightPreAlarmLevel );
  fuel.hightPreAlarm.timer.delay        = getValue( &fuelLevelHightPreAlarmDelay );
  fuel.hightPreAlarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  fuel.hightPreAlarm.error.ack          = PERMISSION_ENABLE;
  fuel.hightPreAlarm.error.event.type   = EVENT_FUEL_HIGHT_LEVEL;
  fuel.hightPreAlarm.error.event.action = ACTION_WARNING;
  fuel.hightPreAlarm.error.trig         = TRIGGER_IDLE;
  fuel.hightPreAlarm.error.status       = ALARM_STATUS_IDLE;
  fuel.hightAlarm.error.active          = PERMISSION_ENABLE;
  fuel.hightAlarm.type                  = ALARM_LEVEL_HIGHT;
  fuel.hightAlarm.level                 = getValue( &fuelLevelHightAlarmLevel );
  fuel.hightAlarm.timer.delay           = getValue( &fuelLevelHightAlarmDelay );
  fuel.hightAlarm.timer.id              = LOGIC_DEFAULT_TIMER_ID;
  fuel.hightAlarm.error.event.type      = EVENT_FUEL_HIGHT_LEVEL;
  fuel.hightAlarm.error.event.action    = ACTION_PLAN_STOP;
  fuel.hightAlarm.error.ack             = PERMISSION_ENABLE;
  fuel.hightAlarm.error.trig            = TRIGGER_IDLE;
  fuel.hightAlarm.error.status          = ALARM_STATUS_IDLE;
  fuel.booster.onLevel                  = getValue( &fuelPumpOnLevel );
  fuel.booster.offLevel                 = getValue( &fuelPumpOffLevel );
  fuel.booster.relay.set                = vFPOsetBooster;
  fuel.booster.relay.status             = RELAY_OFF;
  fuel.pump.enb                         = PERMISSION_ENABLE;
  fuel.pump.set                         = vFPOsetPump;
  fuel.pump.status                      = RELAY_OFF;
  /*--------------------------------------------------------------*/
  battery.get                           = xADCGetVDD;
  battery.lowAlarm.error.enb            = getBitMap( &batteryAlarms, BATTERY_UNDER_VOLTAGE_ENB_ADR );
  battery.lowAlarm.error.active         = PERMISSION_ENABLE;
  battery.lowAlarm.type                 = ALARM_LEVEL_LOW;
  battery.lowAlarm.level                = getValue( &batteryUnderVoltageLevel );
  battery.lowAlarm.timer.delay          = getValue( &batteryUnderVoltageDelay );
  battery.lowAlarm.timer.id             = LOGIC_DEFAULT_TIMER_ID;
  battery.lowAlarm.error.ack            = PERMISSION_ENABLE;
  battery.lowAlarm.error.event.type     = EVENT_BATTERY_LOW;
  battery.lowAlarm.error.event.action   = ACTION_WARNING;
  battery.lowAlarm.error.trig           = TRIGGER_IDLE;
  battery.lowAlarm.error.status         = ALARM_STATUS_IDLE;
  battery.hightAlarm.error.enb          = getBitMap( &batteryAlarms, BATTERY_OVER_VOLTAGE_ENB_ADR );
  battery.hightAlarm.error.active       = PERMISSION_ENABLE;
  battery.hightAlarm.type               = ALARM_LEVEL_HIGHT;
  battery.hightAlarm.level              = getValue( &batteryOverVoltageLevel );
  battery.hightAlarm.timer.delay        = getValue( &batteryOverVoltageDelay );
  battery.hightAlarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  battery.hightAlarm.error.ack          = PERMISSION_ENABLE;
  battery.hightAlarm.error.event.type   = EVENT_BATTERY_HIGHT;
  battery.hightAlarm.error.event.action = ACTION_WARNING;
  battery.hightAlarm.error.trig         = TRIGGER_IDLE;
  battery.hightAlarm.error.status       = ALARM_STATUS_IDLE;
  /*--------------------------------------------------------------*/
  charger.enb                      = PERMISSION_ENABLE;
  charger.status                   = CHARGER_STATUS_IDLE;
  charger.get                      = xADCGetCAC;
  charger.relay.enb                = PERMISSION_ENABLE;
  charger.relay.set                = vCHARGERset;
  charger.relay.status             = RELAY_OFF;
  charger.timer.delay              = chargerImpulseDuration;
  charger.timer.id                 = LOGIC_DEFAULT_TIMER_ID;
  charger.alarm.error.enb          = getBitMap( &batteryAlarms, BATTERY_CHARGE_ALARM_ENB_ADR );
  charger.alarm.error.active       = PERMISSION_ENABLE;
  charger.alarm.type               = ALARM_LEVEL_LOW;
  charger.alarm.level              = getValue( &batteryChargeAlarmLevel );
  charger.alarm.timer.delay        = getValue( &batteryChargeAlarmDelay );
  charger.alarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  charger.alarm.error.ack          = PERMISSION_ENABLE;
  charger.alarm.error.event.type   = EVENT_CHARGER_FAIL;
  charger.alarm.error.event.action = ACTION_WARNING;
  charger.alarm.error.trig         = TRIGGER_IDLE;
  charger.alarm.error.status       = ALARM_STATUS_IDLE;
  /*--------------------------------------------------------------*/
  starter.set                         = vFPOsetStarter;
  starter.attempts                    = getBitMap( &engineSetup, ENGINE_START_ATTEMPTS_ADR );
  starter.crankingDelay               = getValue( &timerCranking );
  starter.crankDelay                  = getValue( &timerCrankDelay );
  starter.blockDelay                  = getValue( &timerSafetyOnDelay );
  starter.idlingDelay                 = getValue( &timerStartupIdleTime );
  starter.nominalDelay                = getValue( &timerNominalRPMDelay );
  starter.warmingDelay                = getValue( &timerWarming );
  starter.iteration                   = 0U;
  starter.status                      = STARTER_STATUS_IDLE;
  starter.startCrit.critGenFreqEnb    = getBitMap( &genSetup, GEN_POWER_GENERATOR_CONTROL_ENB_ADR );
  starter.startCrit.critGenFreqLevel  = getValue( &starterStopGenFreqLevel );
  starter.startCrit.critOilPressEnb   = getBitMap( &starterStopSetup, STARTER_STOP_OIL_PRESSURE_ENB_ADR );
  starter.startCrit.critOilPressLevel = getValue( &starterStopOilPressureLevel );
  starter.startCrit.critChargeEnb     = getBitMap( &starterStopSetup, STARTER_STOP_CHARGE_ALTERNATOR_ENB_ADR );
  starter.startCrit.critChargeLevel   = getValue( &starterStopChargeAlternatorLevel );
  starter.startCrit.critSpeedEnb      = getBitMap( &starterStopSetup, STARTER_STOP_SPEED_ENB_ADR );
  starter.startCrit.critSpeedLevel    = getValue( &starterStopSpeedLevel );
  /*--------------------------------------------------------------*/
  planStop.status           = STOP_STATUS_IDLE;
  planStop.coolingDelay     = getValue( &timerCooling );
  planStop.coolingIdleDelay = getValue( &timerCoolingIdle );
  planStop.processDelay     = getValue( &timerFailStopDelay );
  vSYSprintFix16(planStop.processDelay);
  /*--------------------------------------------------------------*/
  engine.startCheckOil                  = getBitMap( &starterStopSetup, STARTER_OIL_PRESSURE_CHECK_ON_START_ENB_ADR );
  engine.status                         = ENGINE_STATUS_IDLE;
  engine.banStart                       = PERMISSION_DISABLE;
  engine.stopError.enb                  = PERMISSION_ENABLE;
  engine.stopError.active               = PERMISSION_ENABLE;
  engine.stopError.ack                  = PERMISSION_DISABLE;
  engine.stopError.event.action         = ACTION_EMERGENCY_STOP;
  engine.stopError.event.type           = EVENT_STOP_FAIL;
  engine.stopError.status               = ALARM_STATUS_IDLE;
  engine.stopError.trig                 = TRIGGER_IDLE;
  engine.sensorCommonError.enb          = PERMISSION_ENABLE;
  engine.sensorCommonError.active       = PERMISSION_ENABLE;
  engine.sensorCommonError.ack          = PERMISSION_ENABLE;
  engine.sensorCommonError.event.action = ACTION_EMERGENCY_STOP;
  engine.sensorCommonError.event.type   = EVENT_SENSOR_COMMON_ERROR;
  engine.sensorCommonError.status       = ALARM_STATUS_IDLE;
  engine.sensorCommonError.trig         = TRIGGER_IDLE;
  engine.startError.enb                 = PERMISSION_ENABLE;
  engine.startError.active              = PERMISSION_DISABLE;
  engine.startError.ack                 = PERMISSION_DISABLE;
  engine.startError.event.action        = ACTION_EMERGENCY_STOP;
  engine.startError.event.type          = EVENT_START_FAIL;
  engine.startError.status              = ALARM_STATUS_IDLE;
  engine.startError.trig                = TRIGGER_IDLE;
  /*--------------------------------------------------------------*/
  speed.enb                           = getBitMap( &speedSetup, SPEED_ENB_ADR );
  speed.status                        = SENSOR_STATUS_NORMAL;
  speed.get                           = fVRgetSpeed;
  speed.polePairs                     = getValue( &genPoleQuantity );
  speed.lowAlarm.error.enb            = getBitMap( &speedSetup, SPEED_LOW_ALARM_ENB_ADR );
  speed.lowAlarm.error.active         = PERMISSION_DISABLE;
  speed.lowAlarm.type                 = ALARM_LEVEL_LOW;
  speed.lowAlarm.level                = getValue( &speedLowAlarmLevel );
  speed.lowAlarm.timer.delay          = 0U;
  speed.lowAlarm.timer.id             = LOGIC_DEFAULT_TIMER_ID;
  speed.lowAlarm.error.ack            = PERMISSION_DISABLE;
  speed.lowAlarm.error.event.type     = EVENT_SPEED_LOW;
  speed.lowAlarm.error.event.action   = ACTION_EMERGENCY_STOP;
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
  maintence.timer.delay                = fix60;
  maintence.timer.id                   = LOGIC_DEFAULT_TIMER_ID;
  maintence.oil.data                   = *freeDataArray[MAINTENANCE_ALARM_OIL_TIME_LEFT_ADR];
  maintence.oil.alarm.error.enb        = getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_OIL_ENB_ADR );
  maintence.oil.alarm.error.active     = PERMISSION_ENABLE;
  maintence.oil.alarm.type             = ALARM_LEVEL_HIGHT;
  maintence.oil.alarm.level            = getValue( &maintenanceAlarmOilTime );
  maintence.oil.alarm.timer.delay      = 0U;
  maintence.oil.alarm.timer.id         = LOGIC_DEFAULT_TIMER_ID;
  maintence.oil.alarm.error.event.type = EVENT_MAINTENANCE_OIL;
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

void vENGINEdataReInit ( void )
{
  oil.pressure.type       = getBitMap( &oilPressureSetup, OIL_PRESSURE_SENSOR_TYPE_ADR );
  oil.pressure.channel    = SENSOR_CHANNEL_OIL;
  oil.pressure.cutout.enb = getBitMap( &oilPressureSetup, OIL_PRESSURE_OPEN_CIRCUIT_ALARM_ENB_ADR );
  oil.alarm.level         = getValue( &oilPressureAlarmLevel );
  oil.preAlarm.level      = getValue( &oilPressurePreAlarmLevel );
  if ( oil.pressure.type == SENSOR_TYPE_NONE )
  {
    oil.alarm.error.enb    = PERMISSION_DISABLE;
    oil.preAlarm.error.enb = PERMISSION_DISABLE;
  }
  else if ( uSENSORisAnalog( oil.pressure ) > 0U )
  {
    oil.alarm.error.enb    = getBitMap( &oilPressureSetup, OIL_PRESSURE_ALARM_ENB_ADR );
    oil.preAlarm.error.enb = getBitMap( &oilPressureSetup, OIL_PRESSURE_PRE_ALARM_ENB_ADR );
  }
  else
  {
    oil.alarm.error.enb    = PERMISSION_ENABLE;
    oil.preAlarm.error.enb = PERMISSION_DISABLE;
  }
  /*--------------------------------------------------------------*/
  coolant.temp.type          = getBitMap( &coolantTempSetup, COOLANT_TEMP_SENSOR_TYPE_ADR );
  coolant.temp.cutout.enb    = getBitMap( &coolantTempSetup, COOLANT_TEMP_OPEN_CIRCUIT_ALARM_ENB_ADR );
  coolant.alarm.level        = getValue( &coolantHightTempAlarmLevel );
  coolant.electroAlarm.level = getValue( &coolantHightTempElectroAlarmLevel );
  coolant.preAlarm.level     = getValue( &coolantHightTempPreAlarmLevel );
  if ( coolant.temp.type == SENSOR_TYPE_NONE )
  {
    coolant.alarm.error.enb        = PERMISSION_DISABLE;
    coolant.electroAlarm.error.enb = PERMISSION_DISABLE;
    coolant.preAlarm.error.enb     = PERMISSION_DISABLE;
    coolant.heater.relay.enb       = PERMISSION_DISABLE;
    coolant.cooler.relay.enb       = PERMISSION_DISABLE;
  }
  else if ( uSENSORisAnalog( coolant.temp ) > 0U )
  {
    coolant.alarm.error.enb        = getBitMap( &coolantTempSetup, COOLANT_HIGHT_TEMP_ALARM_ENB_ADR );
    coolant.electroAlarm.error.enb = getBitMap( &coolantTempSetup, COOLANT_HIGHT_TEMP_ELECTRO_ALARM_ENB_ADR );
    coolant.preAlarm.error.enb     = getBitMap( &coolantTempSetup, COOLANT_HIGHT_TEMP_PRE_ALARM_ENB_ADR );
    coolant.heater.relay.enb       = getBitMap( &coolantTempSetup, COOLANT_TEMP_HEATER_ENB_ADR );
    coolant.cooler.relay.enb       = getBitMap( &coolantTempSetup, COOLANT_TEMP_COOLER_ENB_ADR );
  }
  else
  {
    coolant.alarm.error.enb        = PERMISSION_ENABLE;
    coolant.electroAlarm.error.enb = PERMISSION_DISABLE;
    coolant.preAlarm.error.enb     = PERMISSION_DISABLE;
    coolant.heater.relay.enb       = PERMISSION_DISABLE;
    coolant.cooler.relay.enb       = PERMISSION_DISABLE;
  }
  coolant.heater.onLevel    = getValue( &coolantTempHeaterOnLevel );
  coolant.heater.offLevel   = getValue( &coolantTempHeaterOffLevel );
  coolant.cooler.onLevel    = getValue( &coolantTempCoolerOnLevel );
  coolant.cooler.offLevel   = getValue( &coolantTempCoolerOffLevel );
  /*--------------------------------------------------------------*/
  preHeater.level     = getValue( &enginePreHeatLevel );
  preHeater.relay.set = vFPOsetPreheater;
  preHeater.delay     = getValue( &enginePreHeatDelay );
  /*--------------------------------------------------------------*/
  fuel.level.type                = getBitMap( &fuelLevelSetup, FUEL_LEVEL_SENSOR_TYPE_ADR );
  fuel.level.cutout.enb          = getBitMap( &fuelLevelSetup, FUEL_LEVEL_OPEN_CIRCUIT_ALARM_ENB_ADR );
  if ( fuel.level.type == SENSOR_TYPE_NONE )
  {
    fuel.lowAlarm.error.enb      = PERMISSION_DISABLE;
    fuel.lowPreAlarm.error.enb   = PERMISSION_DISABLE;
    fuel.hightPreAlarm.error.enb = PERMISSION_DISABLE;
    fuel.hightAlarm.error.enb    = PERMISSION_DISABLE;
    fuel.booster.relay.enb       = PERMISSION_DISABLE;
  }
  if ( uSENSORisAnalog( fuel.level ) > 0U )
  {
    fuel.lowAlarm.error.enb      = getBitMap( &fuelLevelSetup, FUEL_LEVEL_LOW_ALARM_ENB_ADR );
    fuel.lowPreAlarm.error.enb   = getBitMap( &fuelLevelSetup, FUEL_LEVEL_LOW_PRE_ALARM_ENB_ADR );
    fuel.hightPreAlarm.error.enb = getBitMap( &fuelLevelSetup, FUEL_LEVEL_HIGHT_PRE_ALARM_ENB_ADR );
    fuel.hightAlarm.error.enb    = getBitMap( &fuelLevelSetup, FUEL_LEVEL_HIGHT_ALARM_ENB_ADR );
    fuel.booster.relay.enb       = getBitMap( &fuelLevelSetup, FUEL_PUMP_ENB_ADR );
  }
  else
  {
    fuel.lowAlarm.error.enb      = PERMISSION_ENABLE;
    fuel.lowPreAlarm.error.enb   = PERMISSION_DISABLE;
    fuel.hightPreAlarm.error.enb = PERMISSION_DISABLE;
    fuel.hightAlarm.error.enb    = PERMISSION_DISABLE;
    fuel.booster.relay.enb       = PERMISSION_DISABLE;
  }

  fuel.lowAlarm.level            = getValue( &fuelLevelLowAlarmLevel );
  fuel.lowAlarm.timer.delay      = getValue( &fuelLevelLowAlarmDelay );
  fuel.lowPreAlarm.level         = getValue( &fuelLevelLowPreAlarmLevel );
  fuel.lowPreAlarm.timer.delay   = getValue( &fuelLevelLowPreAlarmDelay );
  fuel.hightPreAlarm.level       = getValue( &fuelLevelHightPreAlarmLevel );
  fuel.hightPreAlarm.timer.delay = getValue( &fuelLevelHightPreAlarmDelay );
  fuel.hightAlarm.level          = getValue( &fuelLevelHightAlarmLevel );
  fuel.hightAlarm.timer.delay    = getValue( &fuelLevelHightAlarmDelay );
  fuel.booster.onLevel           = getValue( &fuelPumpOnLevel );
  fuel.booster.offLevel          = getValue( &fuelPumpOffLevel );
  /*--------------------------------------------------------------*/
  battery.lowAlarm.error.enb     = getBitMap( &batteryAlarms, BATTERY_UNDER_VOLTAGE_ENB_ADR );
  battery.lowAlarm.level         = getValue( &batteryUnderVoltageLevel );
  battery.lowAlarm.timer.delay   = getValue( &batteryUnderVoltageDelay );
  battery.hightAlarm.error.enb   = getBitMap( &batteryAlarms, BATTERY_OVER_VOLTAGE_ENB_ADR );
  battery.hightAlarm.level       = getValue( &batteryOverVoltageLevel );
  battery.hightAlarm.timer.delay = getValue( &batteryOverVoltageDelay );
  /*--------------------------------------------------------------*/
  starter.set                         = vFPOsetStarter;
  starter.attempts                    = getBitMap( &engineSetup, ENGINE_START_ATTEMPTS_ADR );
  starter.crankingDelay               = getValue( &timerCranking );
  starter.crankDelay                  = getValue( &timerCrankDelay );
  starter.blockDelay                  = getValue( &timerSafetyOnDelay );
  starter.idlingDelay                 = getValue( &timerStartupIdleTime );
  starter.nominalDelay                = getValue( &timerNominalRPMDelay );
  starter.warmingDelay                = getValue( &timerWarming );
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
  engine.startCheckOil = getBitMap( &starterStopSetup, STARTER_OIL_PRESSURE_CHECK_ON_START_ENB_ADR );
  /*--------------------------------------------------------------*/
  speed.enb                = getBitMap( &speedSetup, SPEED_ENB_ADR );
  speed.lowAlarm.error.enb = getBitMap( &speedSetup, SPEED_LOW_ALARM_ENB_ADR );
  speed.lowAlarm.level     = getValue( &speedLowAlarmLevel );
  speed.hightAlarm.level   = getValue( &speedHightAlarmLevel );
  speed.polePairs          = getValue( &genPoleQuantity );
  /*--------------------------------------------------------------*/
  stopSolenoid.relay.enb   = uFPOisEnable( FPO_FUN_STOP_SOLENOID );
  stopSolenoid.timer.delay = getValue( configReg[TIMER_SOLENOID_HOLD_ADR] );
  /*--------------------------------------------------------------*/
  idleRelay.enb = uFPOisEnable( FPO_FUN_IDLING );
  /*--------------------------------------------------------------*/
  maintence.oil.data            = *freeDataArray[MAINTENANCE_ALARM_OIL_TIME_LEFT_ADR];
  maintence.oil.alarm.error.enb = getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_OIL_ENB_ADR );
  maintence.oil.alarm.level     = getValue( &maintenanceAlarmOilTime );
  if ( getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_OIL_ACTION_ADR ) == 0U )
  {
    maintence.oil.alarm.error.event.action = ACTION_BAN_START;
  }
  else
  {
    maintence.oil.alarm.error.event.action = ACTION_WARNING;
  }
  maintence.air.data            = *freeDataArray[MAINTENANCE_ALARM_AIR_TIME_LEFT_ADR];
  maintence.air.alarm.error.enb = getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_AIR_ENB_ADR );
  maintence.air.alarm.level     = getValue( &maintenanceAlarmAirTime );
  if ( getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_AIR_ACTION_ADR ) == 0U )
  {
    maintence.air.alarm.error.event.action = ACTION_BAN_START;
  }
  else
  {
    maintence.air.alarm.error.event.action = ACTION_WARNING;
  }
  maintence.fuel.data                   = *freeDataArray[MAINTENANCE_ALARM_FUEL_TIME_LEFT_ADR];
  maintence.fuel.alarm.error.enb        = getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_FUEL_ENB_ADR );
  maintence.fuel.alarm.level            = getValue( &maintenanceAlarmFuelTime );
  if ( getBitMap( &maintenanceAlarms, MAINTENANCE_ALARM_FUEL_ACTION_ADR ) == 0U )
  {
    maintence.fuel.alarm.error.event.action = ACTION_BAN_START;
  }
  else
  {
    maintence.fuel.alarm.error.event.action = ACTION_WARNING;
  }
  return;
}
/*----------------------------------------------------------------------------*/
void vENGINEresetAlarms ( void )
{
  vALARMreset( &charger.alarm            );
  vALARMreset( &speed.hightAlarm         );
  vALARMreset( &speed.lowAlarm           );
  vALARMreset( &battery.hightAlarm       );
  vALARMreset( &battery.lowAlarm         );
  vALARMreset( &fuel.hightAlarm          );
  vALARMreset( &fuel.hightPreAlarm       );
  vALARMreset( &fuel.lowAlarm            );
  vALARMreset( &fuel.lowPreAlarm         );
  vALARMreset( &coolant.alarm            );
  vALARMreset( &coolant.electroAlarm     );
  vALARMreset( &coolant.preAlarm         );
  vALARMreset( &oil.alarm                );
  vALARMreset( &oil.preAlarm             );
  vERRORreset( &engine.stopError         );
  vERRORreset( &engine.startError        );
  vERRORreset( &engine.sensorCommonError );
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
  vSTATUSsetup( DEVICE_STATUS_READY_TO_START, LOGIC_DEFAULT_TIMER_ID );
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
TRIGGER_STATE uENGINEisStarterScrollFinish ( void )
{
  return starterFinish;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE uENGINEisBlockTimerFinish ( void )
{
  return blockTimerFinish;
}
/*----------------------------------------------------------------------------*/
ENGINE_STATUS eENGINEgetEngineStatus ( void )
{
  return engine.status;
}
/*----------------------------------------------------------------------------*/
STARTER_STATUS eENGINEgetStarterStatus ( void )
{
  return starter.status;
}
EMERGENCY_STATUS eENGINEgetEmergencyStatus ( void )
{
  return emgencyStopStatus;
}
/*----------------------------------------------------------------------------*/
STOP_STATUS eENGINEgetPlanStopStatus ( void )
{
  return planStop.status;
}
/*----------------------------------------------------------------------------*/
PERMISSION eENGINEisStartBan ( void )
{
  return engine.banStart;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eENGINEgetOilSensorState ( void )
{
  return oil.pressure.trig;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eENGINEgetCoolantSensorState ( void )
{
  return coolant.temp.trig;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eENGINEgetFuelSensorState ( void )
{
  return fuel.level.trig;
}
/*----------------------------------------------------------------------------*/
fix16_t fENGINEspeedGet ( void )
{
  return currentSpeed;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------- TASK -----------------------------------*/
/*----------------------------------------------------------------------------*/
void vENGINEtask ( void* argument )
{
  fix16_t         oilVal      = 0U;
  fix16_t         coolantVal  = 0U;
  fix16_t         chargerVal  = 0U;
  fix16_t         genFreqVal  = 0U;
  SYSTEM_TIMER    commonTimer = { 0U };
  ENGINE_COMMAND  inputCmd    = ENGINE_CMD_NONE;
  SYSTEM_EVENT    event       = { 0U };
  commonTimer.id = LOGIC_DEFAULT_TIMER_ID;
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
        /*-------------------- One action commands ---------------------*/
        if ( inputCmd == ENGINE_CMD_BAN_START )
        {
          engine.banStart = PERMISSION_ENABLE;
          if ( engine.status == ENGINE_STATUS_IDLE )
          {
            vSTATUSsetup( DEVICE_STATUS_BAN_START, LOGIC_DEFAULT_TIMER_ID );
          }
        }
        /*------------------- Long actions command ---------------------*/
        else
        {
          switch ( engine.status )
          {
            case ENGINE_STATUS_IDLE:
              if ( eSTATUSgetStatus() != DEVICE_STATUS_ERROR )
              {
                engine.cmd = inputCmd;
                vLOGICprintEngineCmd( engine.cmd );
              }
              break;
            case ENGINE_STATUS_ERROR:
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
    oilVal       = fOILprocess();
    coolantVal   = fCOOLANTprocess();
    fFUELprocess();
    chargerVal   = fCHARGERprocess();
    fBATTERYprocess();
    genFreqVal   = xADCGetGENLFreq();

    if ( speed.enb == PERMISSION_ENABLE )
    {
      currentSpeed = fSPEEDprocess();
    }
    else
    {
      currentSpeed = fSPEEDfromFreq( genFreqVal );
    }

    //vERRORcheck( &engine.sensorCommonError, eSENSORcheckCutout( SENSOR_CHANNEL_COMMON ) );
    /*------------------------------------------------------------------*/
    /*--------------------- Static condition check ---------------------*/
    /*------------------------------------------------------------------*/
    vERRORcheck( &engine.stopError, !( uENGINEisStop( fELECTROgetMaxGenVoltage(), genFreqVal, oilVal, oil.pressure.trig, currentSpeed ) ) );
    vERRORcheck( &engine.startError, 1U );
    /*------------------------------------------------------------------*/
    /*--------------------- Statistic calculation ----------------------*/
    /*------------------------------------------------------------------*/
    vENGINEmileageProcess();
    /*------------------------------------------------------------------*/
    /*-------------------- Engine ban start check ----------------------*/
    /*------------------------------------------------------------------*/
    if ( engine.banStart == PERMISSION_ENABLE )
    {
      if ( uENGINEisUnban() > 0U )
      {
        engine.banStart = PERMISSION_DISABLE;
        vSTATUSsetup( DEVICE_STATUS_READY_TO_START, LOGIC_DEFAULT_TIMER_ID );
      }
    }
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
      case ENGINE_CMD_START:
        if ( ( ( engine.status   == ENGINE_STATUS_IDLE ) || ( engine.status      == ENGINE_STATUS_BUSY_STARTING ) ) &&
               ( engine.banStart == PERMISSION_DISABLE ) )
        {
          switch ( starter.status )
          {
            case STARTER_STATUS_IDLE:
              vFPOsetReadyToStart( RELAY_OFF );
              engine.stopError.active            = PERMISSION_DISABLE;
              speed.lowAlarm.error.active        = PERMISSION_DISABLE;
              speed.hightAlarm.error.active      = PERMISSION_DISABLE;
              oil.alarm.error.active             = PERMISSION_DISABLE;
              oil.preAlarm.error.active          = PERMISSION_DISABLE;
              battery.hightAlarm.error.active    = PERMISSION_DISABLE;
              battery.lowAlarm.error.active      = PERMISSION_DISABLE;
              engine.status                      = ENGINE_STATUS_BUSY_STARTING;
              vELECTROsendCmd( ELECTRO_CMD_DISABLE_START_ALARMS );
              if ( starter.idlingDelay != 0U )
              {
                vRELAYset( &idleRelay, RELAY_ON );
              }
              if ( ( preHeater.delay > 0U ) &&
                   ( ( ( uSENSORisAnalog( coolant.temp ) > 0U ) && ( coolantVal <= preHeater.level ) ) ||
                     ( uSENSORisAnalog( coolant.temp ) == 0U ) ) )
              {
                commonTimer.delay = preHeater.delay;
                vLOGICstartTimer( &commonTimer, "Common engine timer " );
                vSTATUSsetup( DEVICE_STATUS_PREHEATING, commonTimer.id );
                vRELAYset( &preHeater.relay, RELAY_ON );
                starter.status = STARTER_STATUS_PREHEATING;
                vLOGICprintStarterStatus( starter.status );
              }
              else
              {
                starter.status = STARTER_STATUS_FUEL_PREPUMPING;
              }
              break;
            case STARTER_STATUS_PREHEATING:
              if ( uLOGICisTimer( &commonTimer ) > 0U )
              {
                vRELAYset( &preHeater.relay, RELAY_OFF );
                vLOGICprintStarterStatus( starter.status );
                starter.status = STARTER_STATUS_FUEL_PREPUMPING;
              }
              break;
            case STARTER_STATUS_FUEL_PREPUMPING:
              vRELAYset( &fuel.pump, RELAY_ON  );
              commonTimer.delay = fuelPrePumpingDelay;
              vLOGICstartTimer( &commonTimer, "Common engine timer " );
              vLOGICprintStarterStatus( starter.status );
              starter.status = STARTER_STATUS_START_PREPARATION;
              break;
            case STARTER_STATUS_START_PREPARATION:
              if ( ( eELECTROgetAlarmStatus()      == ELECTRO_ALARM_STATUS_START ) &&
                   ( uLOGICisTimer( &commonTimer ) >  0U                         ) )
              {
                starter.status = STARTER_STATUS_READY;
                vLOGICprintStarterStatus( starter.status );
              }
              break;
            case STARTER_STATUS_READY:
              if ( uENGINEisWork( genFreqVal, oilVal, chargerVal, currentSpeed ) > 0U )
              {
                vLCD_BrigthOn();
                starterFinish     = TRIGGER_SET;
                starter.status    = STARTER_STATUS_CONTROL_BLOCK;
                commonTimer.delay = starter.blockDelay;
                vLOGICstartTimer( &commonTimer, "Common engine timer " );
                vSTATUSsetup( DEVICE_STATUS_CONTROL_BLOCK, commonTimer.id );
              }
              else
              {
                starter.iteration++;
                starter.status    = STARTER_STATUS_CRANKING;
                commonTimer.delay = starter.crankingDelay;
                vLCD_BrigthOFF();
                starter.set( RELAY_ON );
                vLOGICstartTimer( &commonTimer, "Common engine timer " );
                vSTATUSsetup( DEVICE_STATUS_CRANKING, commonTimer.id );
              }
              vLOGICprintStarterStatus( starter.status );
              break;
            case STARTER_STATUS_CRANKING:
              if ( uENGINEisWork( genFreqVal, oilVal, chargerVal, currentSpeed ) > 0U )
              {
                starter.set( RELAY_OFF );
                starterFinish  = 1U;
                starter.status = STARTER_STATUS_CONTROL_BLOCK;
                vLCD_BrigthOn();
                vLOGICresetTimer( &commonTimer );
                commonTimer.delay = starter.blockDelay;
                vLOGICstartTimer( &commonTimer, "Common engine timer " );
                vLOGICprintStarterStatus( starter.status );
                vSTATUSsetup( DEVICE_STATUS_CONTROL_BLOCK, commonTimer.id );
              }
              if ( uLOGICisTimer( &commonTimer ) > 0U )
              {
                starter.set( RELAY_OFF );
                vLCD_BrigthOn();
                if ( starter.iteration < starter.attempts )
                {
                  starter.status = STARTER_STATUS_CRANK_DELAY;
                  commonTimer.delay = starter.crankDelay;
                  vLOGICstartTimer( &commonTimer, "Common engine timer " );
                  vLOGICprintStarterStatus( starter.status );
                  vSTATUSsetup( DEVICE_STATUS_CRANK_DELAY, commonTimer.id );
                }
                else
                {
                  vRELAYset( &fuel.pump, RELAY_OFF );
                  vRELAYdelayTrig( &stopSolenoid );
                  starter.status   = STARTER_STATUS_FAIL;
                  vLOGICprintStarterStatus( starter.status );
                }
              }
              break;
            case STARTER_STATUS_CRANK_DELAY:
              if ( uLOGICisTimer( &commonTimer ) > 0U )
              {
                starter.status = STARTER_STATUS_READY;
                vLOGICprintStarterStatus( starter.status );
              }
              break;
            case STARTER_STATUS_CONTROL_BLOCK:
              if ( uLOGICisTimer( &commonTimer ) > 0U )
              {
                commonTimer.delay = starter.idlingDelay;
                vLOGICstartTimer( &commonTimer, "Common engine timer " );
                starter.status                     = STARTER_STATUS_IDLE_WORK;
                blockTimerFinish                   = TRIGGER_SET;
                speed.hightAlarm.error.active      = PERMISSION_ENABLE;
                oil.alarm.error.active             = PERMISSION_ENABLE;
                oil.preAlarm.error.active          = PERMISSION_ENABLE;
                battery.hightAlarm.error.active    = PERMISSION_ENABLE;
                battery.lowAlarm.error.active      = PERMISSION_ENABLE;
                charger.start                      = PERMISSION_ENABLE;
                vELECTROsendCmd( ELECTRO_CMD_ENABLE_START_TO_IDLE_ALARMS );
                vLOGICprintStarterStatus( starter.status );
                vSTATUSsetup( DEVICE_STATUS_IDLE_WORK, commonTimer.id );
              }
              break;
            case STARTER_STATUS_IDLE_WORK:
              if ( uLOGICisTimer( &commonTimer ) > 0U )
              {
                vRELAYset( &idleRelay, RELAY_OFF );
                commonTimer.delay = starter.nominalDelay;
                vLOGICstartTimer( &commonTimer, "Common engine timer " );
                starter.status = STARTER_STATUS_MOVE_TO_NOMINAL;
                vLOGICprintStarterStatus( starter.status );
              }
              break;
            case STARTER_STATUS_MOVE_TO_NOMINAL:
              if ( uLOGICisTimer( &commonTimer ) > 0U )
              {
                commonTimer.delay = starter.warmingDelay;
                vLOGICstartTimer( &commonTimer, "Common engine timer " );
                starter.status              = STARTER_STATUS_WARMING;
                speed.lowAlarm.error.active = PERMISSION_ENABLE;
                vELECTROsendCmd( ELECTRO_CMD_ENABLE_IDLE_ALARMS );
                vLOGICprintStarterStatus( starter.status );
                vSTATUSsetup( DEVICE_STATUS_WARMING, commonTimer.id );
              }
              break;
            case STARTER_STATUS_WARMING:
              if ( uLOGICisTimer( &commonTimer ) > 0U )
              {
                starter.status = STARTER_STATUS_OK;
                vFPOsetGenReady( RELAY_ON );
                vLOGICprintStarterStatus( starter.status );
              }
              break;
            case STARTER_STATUS_FAIL:
              engine.status            = ENGINE_STATUS_ERROR;
              engine.cmd               = ENGINE_CMD_NONE;
              starter.status           = STARTER_STATUS_IDLE;
              engine.cmd               = ENGINE_CMD_NONE;
              engine.startError.active = PERMISSION_ENABLE;
              charger.start            = PERMISSION_DISABLE;
              starter.iteration        = 0U;
              blockTimerFinish         = TRIGGER_IDLE;
              starterFinish            = TRIGGER_IDLE;
              vLOGICprintStarterStatus( starter.status );
              break;
            case STARTER_STATUS_OK:
              engine.status     = ENGINE_STATUS_WORK;
              engine.cmd        = ENGINE_CMD_NONE;
              starter.status    = STARTER_STATUS_IDLE;
              starter.iteration = 0U;
              eDATAAPIfreeData( DATA_API_CMD_INC,  ENGINE_STARTS_NUMBER_ADR, NULL );
              eDATAAPIfreeData( DATA_API_CMD_SAVE, 0U, NULL );
              vLOGICprintStarterStatus( starter.status );
              event.action = ACTION_NONE;
              event.type   = EVENT_ENGINE_START;
              vSYSeventSend( event, NULL );
              vSTATUSsetup( DEVICE_STATUS_WORKING, LOGIC_DEFAULT_TIMER_ID );
              break;
            default:
              vLOGICresetTimer( &commonTimer );
              engine.status  = ENGINE_STATUS_ERROR;
              engine.cmd     = ENGINE_CMD_NONE;
              starter.status = STARTER_STATUS_FAIL;
              charger.start  = PERMISSION_DISABLE;
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
            case STOP_STATUS_IDLE:
              planStop.status = STOP_STATUS_COOLDOWN;
              charger.start   = PERMISSION_DISABLE;
              vFPOsetGenReady( RELAY_OFF );
              commonTimer.delay = planStop.coolingDelay;
              vLOGICstartTimer( &commonTimer, "Common engine timer " );
              vLOGICprintPlanStopStatus( planStop.status );
              vSTATUSsetup( DEVICE_STATUS_COOLDOWN, commonTimer.id );
              break;
            case STOP_STATUS_COOLDOWN:
              if ( uLOGICisTimer( &commonTimer ) > 0U )
              {
                planStop.status = STOP_STATUS_WAIT_ELECTRO;
                vELECTROsendCmd( ELECTRO_CMD_DISABLE_IDLE_ALARMS );
                vLOGICprintPlanStopStatus( planStop.status );
              }
              break;
            case STOP_STATUS_WAIT_ELECTRO:
              if ( eELECTROgetAlarmStatus() == ELECTRO_ALARM_STATUS_WORK_ON_IDLE )
              {
                if ( planStop.coolingIdleDelay != 0U )
                {
                  vRELAYset( &idleRelay, RELAY_ON );
                }
                commonTimer.delay           = planStop.coolingIdleDelay;
                speed.lowAlarm.error.active = PERMISSION_DISABLE;
                planStop.status             = STOP_STATUS_IDLE_COOLDOWN;
                vLOGICstartTimer( &commonTimer, "Common engine timer " );
                vLOGICprintPlanStopStatus( planStop.status );
                vSTATUSsetup( DEVICE_STATUS_IDLE_COOLDOWN, commonTimer.id );
              }
              break;
            case STOP_STATUS_IDLE_COOLDOWN:
              if ( uLOGICisTimer( &commonTimer ) > 0U )
              {
                oil.alarm.error.active    = PERMISSION_DISABLE;
                oil.preAlarm.error.active = PERMISSION_DISABLE;
                vRELAYset( &idleRelay, RELAY_OFF );
                vRELAYset( &fuel.pump, RELAY_OFF );
                vRELAYdelayTrig( &stopSolenoid );
                commonTimer.delay = planStop.processDelay;
                planStop.status   = STOP_STATUS_PROCESSING;
                vELECTROsendCmd( ELECTRO_CMD_DISABLE_START_ALARMS );
                vLOGICstartTimer( &commonTimer, "Common engine timer " );
                vLOGICprintPlanStopStatus( planStop.status );
                vSTATUSsetup( DEVICE_STATUS_STOP_PROCESSING, commonTimer.id );
              }
              break;
            case STOP_STATUS_PROCESSING:
              if ( uLOGICisTimer( &commonTimer ) > 0U )
              {
                planStop.status = STOP_STATUS_FAIL;
                vLOGICprintPlanStopStatus( planStop.status );
                vELECTROsendCmd( ELECTRO_CMD_ENABLE_STOP_ALARMS );
              }
              if ( uENGINEisStop( fELECTROgetMaxGenVoltage(), genFreqVal, oilVal, oil.pressure.trig, currentSpeed ) > 0U )
              {
                vLOGICresetTimer( &commonTimer );
                planStop.status = STOP_STATUS_OK;
                vLOGICprintPlanStopStatus( planStop.status );
                vELECTROsendCmd( ELECTRO_CMD_ENABLE_STOP_ALARMS );
              }
              break;
            case STOP_STATUS_FAIL:
              engine.status    = ENGINE_STATUS_ERROR;
              engine.cmd       = ENGINE_CMD_NONE;
              planStop.status  = STOP_STATUS_IDLE;
              event.action     = ACTION_EMERGENCY_STOP;
              event.type       = EVENT_STOP_FAIL;
              blockTimerFinish = TRIGGER_IDLE;
              starterFinish    = TRIGGER_IDLE;
              vSYSeventSend( event, NULL );
              vLOGICprintPlanStopStatus( planStop.status );
              break;
            case STOP_STATUS_OK:
              engine.status    = ENGINE_STATUS_IDLE;
              engine.cmd       = ENGINE_CMD_NONE;
              planStop.status  = STOP_STATUS_IDLE;
              blockTimerFinish = TRIGGER_IDLE;
              starterFinish    = TRIGGER_IDLE;
              vFPOsetReadyToStart( RELAY_ON );
              vLOGICprintPlanStopStatus( planStop.status );
              event.action = ACTION_NONE;
              event.type   = EVENT_ENGINE_STOP;
              vSYSeventSend( event, NULL );
              break;
            default:
              vLOGICresetTimer( &commonTimer );
              planStop.status = STOP_STATUS_FAIL;
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
            engine.status = ENGINE_STATUS_WORK_GOTO_IDLE;
            break;
          case ENGINE_STATUS_WORK_GOTO_IDLE:
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
            vLOGICstartTimer( &commonTimer, "Common engine timer " );
            engine.status = ENGINE_STATUS_WORK_GOTO_NOMINAL;
            break;
          case ENGINE_STATUS_WORK_GOTO_NOMINAL:
            if ( uLOGICisTimer( &commonTimer ) > 0U )
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
        switch ( emgencyStopStatus )
        {
          case EMERGENCY_STATUS_IDLE:
            vLOGICprintEmergencyStopStatus( emgencyStopStatus );
            vELECTROsendCmd( ELECTRO_CMD_ENABLE_STOP_ALARMS );
            vLCD_BrigthOn();
            starter.set( RELAY_OFF );
            vRELAYset( &fuel.pump,            RELAY_OFF );
            vRELAYset( &fuel.booster.relay,   RELAY_OFF );
            vRELAYset( &coolant.cooler.relay, RELAY_OFF );
            vRELAYset( &coolant.heater.relay, RELAY_OFF );
            vRELAYset( &idleRelay,            RELAY_OFF );
            vRELAYset( &preHeater.relay,      RELAY_OFF );
            vRELAYdelayTrig( &stopSolenoid );
            vFPOsetGenReady( RELAY_OFF );
            vFPOsetReadyToStart( RELAY_OFF );
            engine.status             = ENGINE_STATUS_ERROR;
            planStop.status           = STOP_STATUS_IDLE;
            engine.startError.active  = PERMISSION_DISABLE;
            oil.alarm.error.active    = PERMISSION_DISABLE;
            oil.preAlarm.error.active = PERMISSION_DISABLE;
            charger.start             = PERMISSION_DISABLE;
            blockTimerFinish          = TRIGGER_IDLE;
            starterFinish             = TRIGGER_IDLE;
            emgencyStopStatus         = EMERGENCY_STATUS_PROCESSING;
            vLOGICresetTimer( &commonTimer );
            commonTimer.delay = planStop.processDelay;
            vLOGICstartTimer( &commonTimer, "Common engine timer " );
            vLOGICprintEmergencyStopStatus( emgencyStopStatus );
            vSTATUSsetup( DEVICE_STATUS_STOP_PROCESSING, commonTimer.id );
            break;
          case EMERGENCY_STATUS_PROCESSING:
            if ( uLOGICisTimer( &commonTimer ) > 0U )
            {
              emgencyStopStatus = EMERGENCY_STATUS_END;
            }
            if ( uENGINEisStop( fELECTROgetMaxGenVoltage(), genFreqVal, oilVal, oil.pressure.trig, currentSpeed ) > 0U )
            {
              vLOGICresetTimer( &commonTimer );
              emgencyStopStatus = EMERGENCY_STATUS_END;
            }
            break;
          case EMERGENCY_STATUS_END:
            vSTATUSsetup( DEVICE_STATUS_ERROR, LOGIC_DEFAULT_TIMER_ID );
            vLOGICprintEmergencyStopStatus( emgencyStopStatus );
            emgencyStopStatus       = EMERGENCY_STATUS_IDLE;
            engine.cmd              = ENGINE_CMD_NONE;
            engine.stopError.active = PERMISSION_ENABLE;
            break;
          default:
            emgencyStopStatus = EMERGENCY_STATUS_IDLE;
            break;
        }
        break;
      /*----------------------------------------------------------------------------------------*/
      /*------------------------------- ENGINE RESET TO IDLE -----------------------------------*/
      /*----------------------------------------------------------------------------------------*/
      case ENGINE_CMD_RESET_TO_IDLE:
        vENGINEresetAlarms();
        starter.status  = STARTER_STATUS_IDLE;
        planStop.status = STOP_STATUS_IDLE;
        engine.status   = ENGINE_STATUS_IDLE;
        charger.start   = PERMISSION_DISABLE;
        vRELAYset( &stopSolenoid.relay, RELAY_OFF );
        speed.lowAlarm.error.active = PERMISSION_DISABLE;
        vFPOsetReadyToStart( RELAY_ON );
        vFPOsetGenReady( RELAY_OFF );
        vLOGICresetTimer( &commonTimer        );
        vLOGICresetTimer( &maintence.timer    );
        vLOGICresetTimer( &charger.timer      );
        vLOGICresetTimer( &stopSolenoid.timer );
        engine.cmd               = ENGINE_CMD_NONE;
        engine.startError.active = PERMISSION_DISABLE;
        engine.stopError.active  = PERMISSION_ENABLE;
        blockTimerFinish         = TRIGGER_IDLE;
        starterFinish            = TRIGGER_IDLE;
        break;
      /*----------------------------------------------------------------------------------------*/
      /*--------------------------------- ENGINE BAN START -------------------------------------*/
      /*----------------------------------------------------------------------------------------*/
      case ENGINE_CMD_BAN_START:
        engine.banStart = PERMISSION_ENABLE;
        vSTATUSsetup( DEVICE_STATUS_BAN_START, LOGIC_DEFAULT_TIMER_ID );
        break;
      /*----------------------------------------------------------------------------------------*/
      /*----------------------------------------------------------------------------------------*/
      /*----------------------------------------------------------------------------------------*/
      default:
        engine.cmd = ENGINE_CMD_NONE;
        break;
    }
    /* Process outputs */
    vRELAYdelayProcess( &stopSolenoid );
    /*------------------------------------------------------------------------------------------*/
  }
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


/*
 * generator.c
 *
 *  Created on: 24 июн. 2020 г.
 *      Author: photo_Mickey
 */
/*--------------------------------- Includes ---------------------------------*/
#include "electro.h"
#include "config.h"
#include "dataProces.h"
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "fpo.h"
#include "adc.h"
#include "alarm.h"
/*---------------------------------- Define ----------------------------------*/

/*-------------------------------- Structures --------------------------------*/
static GENERATOR_TYPE      generator            = { 0U };
static MAINS_TYPE          mains                = { 0U } ;
static ELECTRO_SYSTEM_TYPE electro              = { 0U };
static StaticQueue_t       xElectroCommandQueue = { 0U };
static QueueHandle_t       pElectroCommandQueue = NULL;
/*--------------------------------- Constant ---------------------------------*/
static const fix16_t tempProtectionTimeMult    = F16( TEMP_PROTECTION_TIME_MULTIPLIER ); /* Temperature protection constant */
static const fix16_t shortCircuitTrippingCurve = F16( CUTOUT_PROTECTION_TRIPPING_CURVE );
static const fix16_t shortCircuitConstant      = F16( SHORT_CIRCUIT_CONSTANT );          /* Short circuit protection constant */
static const fix16_t shortCircuitCutoutPower   = F16( CUTOUT_POWER );
/*-------------------------------- Variables ---------------------------------*/
static ELECTRO_COMMAND electroCommandBuffer[ELECTRO_COMMAND_QUEUE_LENGTH] = { 0U };
static fix16_t         maxGeneratorVolage                                 = 0U;
static fix16_t         shortCircuitDividend                               = 0U;
/*-------------------------------- External ----------------------------------*/
osThreadId_t electroHandle = NULL;
/*-------------------------------- Functions ---------------------------------*/
void vELECTROtask ( void* argument );
/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/
/* Calculation of disconnection time in temperature protection
 * input:  input   - input current from most loaded line
 *         setting - current of disconnection
 * output: Time of disconnection
 */
fix16_t fIDMTcalcTemp ( fix16_t input, fix16_t setting )
{
  return fix16_div( tempProtectionTimeMult, fix16_sq( fix16_sub( fix16_div( input, setting ), fix16_one ) ) );
}
/*---------------------------------------------------------------------------------------------------*/
/* Calculation of disconnection time in short circuit protection
 * input:  input   - input current from most loaded line
 *         setting - current of disconnection
 * output: Time of disconnection
 */
fix16_t fIDMTcalcCutout ( fix16_t input, fix16_t setting )
{
  return fix16_div( shortCircuitDividend, fix16_sub( fix16_pow( fix16_div( input, setting ), shortCircuitCutoutPower ), fix16_one ) );
}
/*---------------------------------------------------------------------------------------------------*/
fix16_t fELECTROgetMin ( fix16_t* value, uint8_t length )
{
  fix16_t res = value[0U];
  uint8_t i   = 0U;

  for ( i=1U; i<length; i++ )
  {
    res = fix16_min( res, value[i] );
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
fix16_t fELECTROgetMax ( fix16_t* value, uint8_t length )
{
  fix16_t res = value[0U];
  uint8_t i   = 0U;

  for ( i=1U; i<length; i++ )
  {
    res = fix16_max( res, value[i] );
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
void vELECTROalarmCheck ( ALARM_TYPE* alarm, fix16_t* value, uint8_t length )
{
  if ( electro.scheme == ELECTRO_SCHEME_SINGLE_PHASE )
  {
    vALARMcheck( alarm, value[0U] );
  }
  else
  {
    if ( alarm->type == ALARM_LEVEL_LOW )
    {
      vALARMcheck( alarm, fELECTROgetMin( value, length ) );
    }
    else
    {
      vALARMcheck( alarm, fELECTROgetMax( value, length ) );
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uSecToTic ( fix16_t input )
{
  uint32_t res = 0U;
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
void vELECTROcurrentAlarmProcess ( fix16_t current, CURRENT_ALARM_TYPE* alarm )
{
  switch ( alarm->state )
  {
    case ELECTRO_CURRENT_STATUS_IDLE:
      if ( current >= alarm->cutout.current )
      {
        alarm->cutout.delay       = fIDMTcalcCutout( current, alarm->cutout.current );
        alarm->state              = ELECTRO_CURRENT_STATUS_CUTOUT_TRIG;
        alarm->tim->Instance->CNT = 0U;
        HAL_TIM_Base_Start( alarm->tim );
      }
      else if ( current >= alarm->over.current )
      {
        alarm->over.delay         = fIDMTcalcTemp( current, alarm->over.current );
        alarm->state              = ELECTRO_CURRENT_STATUS_OVER_TRIG;
        alarm->tim->Instance->CNT = 0U;
        HAL_TIM_Base_Start( alarm->tim );
      }
      else
      {

      }
      break;
    /*--------------------------------------------------------------------------------*/
    case ELECTRO_CURRENT_STATUS_OVER_TRIG:
      if ( current >= alarm->cutout.current )
      {
        alarm->over.delay = fIDMTcalcTemp( current, alarm->over.current );
        if ( alarm->tim->Instance->CNT >= uSecToTic( alarm->over.delay ) )
        {
          vSYSeventSend( alarm->over.event, NULL );
          alarm->state = ELECTRO_CURRENT_STATUS_ALARM;
        }
      }
      else
      {
        alarm->state = ELECTRO_CURRENT_STATUS_OVER_COOLDOWN;
      }
      break;
    /*--------------------------------------------------------------------------------*/
    case ELECTRO_CURRENT_STATUS_OVER_COOLDOWN:
      if ( current >= alarm->cutout.current )
      {
        alarm->state = ELECTRO_CURRENT_STATUS_OVER_TRIG;
      }
      else
      {
        if ( alarm->tim->Instance->CNT >= alarm->over.delay )
        {
          alarm->state = ELECTRO_CURRENT_STATUS_IDLE;
        }
      }
      alarm->state = ELECTRO_CURRENT_STATUS_IDLE;
      HAL_TIM_Base_Stop( alarm->tim );
      break;
    /*--------------------------------------------------------------------------------*/
    case ELECTRO_CURRENT_STATUS_CUTOUT_TRIG:
      if ( current >= alarm->cutout.current )
      {
        alarm->cutout.delay = fIDMTcalcCutout( current, alarm->cutout.current );
        if ( alarm->tim->Instance->CNT >= uSecToTic( alarm->cutout.delay ) )
        {
          vSYSeventSend( alarm->cutout.event, NULL );
          alarm->state = ELECTRO_CURRENT_STATUS_ALARM;
        }
      }
      else
      {
        alarm->state = ELECTRO_CURRENT_STATUS_OVER_TRIG;
      }
      break;
    /*--------------------------------------------------------------------------------*/
    case ELECTRO_CURRENT_STATUS_ALARM:
      HAL_TIM_Base_Stop( alarm->tim );
      break;
    /*--------------------------------------------------------------------------------*/
    default:
      alarm->state = ELECTRO_CURRENT_STATUS_IDLE;
      break;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
fix16_t fELECTROcalcPhaseImbalance ( fix16_t* value )
{
  return fix16_abs( fix16_sub( fELECTROgetMax( value, 3U ), fELECTROgetMax( value, 3U ) ) );
}
/*---------------------------------------------------------------------------------------------------*/
void vMAINSprocess ( void )
{
  fix16_t      voltage[MAINS_LINE_NUMBER] = { 0U };
  fix16_t      freq                       = 0U;
  uint8_t      i                          = 0U;
  uint8_t      mainsFlag                  = 0U;
  /*--------------------------- Read inputs ---------------------------*/
  for ( i=0U; i<MAINS_LINE_NUMBER; i++ )
  {
    voltage[i] = mains.line[i].getVoltage();
  }
  freq = mains.getFreq();
  /*------------------------- Alarm ignoring --------------------------*/
  if ( ( generator.state == ELECTRO_STATUS_LOAD ) && ( mains.alarmsIgnor == PERMISSION_DISABLE ) )
  {
    mains.hightFreqAlarm.error.ignor    = PERMISSION_ENABLE;
    mains.lowFreqAlarm.error.ignor      = PERMISSION_ENABLE;
    mains.hightVoltageAlarm.error.ignor = PERMISSION_ENABLE;
    mains.lowVoltageAlarm.error.ignor   = PERMISSION_ENABLE;
    eLOGICERactiveErrorList( ERROR_LIST_CMD_ACK, NULL, &mains.hightFreqAlarm.error.id    );
    eLOGICERactiveErrorList( ERROR_LIST_CMD_ACK, NULL, &mains.lowFreqAlarm.error.id      );
    eLOGICERactiveErrorList( ERROR_LIST_CMD_ACK, NULL, &mains.hightVoltageAlarm.error.id );
    eLOGICERactiveErrorList( ERROR_LIST_CMD_ACK, NULL, &mains.lowVoltageAlarm.error.id   );
  }
  if ( ( generator.state != ELECTRO_STATUS_LOAD ) && ( mains.alarmsIgnor == PERMISSION_ENABLE ) )
  {
    mains.hightFreqAlarm.error.ignor    = PERMISSION_DISABLE;
    mains.lowFreqAlarm.error.ignor      = PERMISSION_DISABLE;
    mains.hightVoltageAlarm.error.ignor = PERMISSION_DISABLE;
    mains.lowVoltageAlarm.error.ignor   = PERMISSION_DISABLE;
  }
  /*------------------------- Voltage alarms --------------------------*/
  vELECTROalarmCheck( &mains.lowVoltageAlarm, voltage, MAINS_LINE_NUMBER );
  if ( mains.lowVoltageAlarm.error.status == ALARM_STATUS_IDLE )
  {
    vELECTROalarmCheck( &mains.hightVoltageAlarm, voltage, MAINS_LINE_NUMBER );
  }
  /*------------------------ Frequency alarms --------------------------*/
  vALARMcheck( &mains.lowFreqAlarm, freq );
  if ( mains.lowFreqAlarm.error.status == ALARM_STATUS_IDLE )
  {
    vALARMcheck( &mains.hightFreqAlarm, freq );
  }
  /*--------------------------- Events ---------------------------------*/
  if ( ( eERRORisActive( &mains.lowVoltageAlarm.error   ) == TRIGGER_IDLE ) &&
       ( eERRORisActive( &mains.hightVoltageAlarm.error ) == TRIGGER_IDLE ) &&
       ( eERRORisActive( &mains.lowFreqAlarm.error      ) == TRIGGER_IDLE ) &&
       ( eERRORisActive( &mains.hightFreqAlarm.error    ) == TRIGGER_IDLE ) )
  {
    // Сеть востноволена, логировать позитивное событие, событие возврата сети
    mainsFlag = 1U;
    vFPOsetMainsFail( RELAY_OFF );
  }
  else
  {
    // Ошибка сети, не логировать (избыточно), событие переключения на генератор
    mainsFlag   = 0U;
    vFPOsetMainsFail( RELAY_ON );
  }
  vERRORcheck( &mains.autoStart, !mainsFlag );
  vERRORcheck( &mains.autoStop,  mainsFlag );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/* Check generator
 * Input:  None
 * Output: Maximum of voltage
 */
fix16_t fGENERATORprocess ( void )
{
  fix16_t voltage[MAINS_LINE_NUMBER] = { 0U };
  fix16_t freq                       = 0U;
  fix16_t current[MAINS_LINE_NUMBER] = { 0U };
  fix16_t power[MAINS_LINE_NUMBER]   = { 0U };
  fix16_t maxCurrent                 = 0U;
  uint8_t i                          = 0U;

  for ( i=0U; i<GENERATOR_LINE_NUMBER; i++ )
  {
    voltage[i] = generator.line[i].getVoltage();
    current[i] = generator.line[i].getCurrent();
    //power[i]   = fix16_mul( fix16_mul( voltage[i], current[i] ), generator.rating.cosFi );
  }
  freq = generator.getFreq();
  maxCurrent = fELECTROgetMax( current, GENERATOR_LINE_NUMBER );
  vELECTROalarmCheck( &generator.lowVoltageAlarm, voltage, MAINS_LINE_NUMBER );
  if ( mains.lowVoltageAlarm.error.status == ALARM_STATUS_IDLE )
  {
    vELECTROalarmCheck( &generator.hightVoltageAlarm, voltage, MAINS_LINE_NUMBER );
  }
  vALARMcheck( &generator.lowFreqAlarm, freq );
  if ( mains.lowFreqAlarm.error.status == ALARM_STATUS_IDLE )
  {
    vALARMcheck( &generator.hightFreqAlarm, freq );
  }
  if ( electro.scheme != ELECTRO_SCHEME_SINGLE_PHASE )
  {
    vALARMcheck( &generator.phaseImbalanceAlarm, fELECTROcalcPhaseImbalance( current ) );
  }
  vELECTROalarmCheck( &generator.overloadAlarm, power, MAINS_LINE_NUMBER );
  vELECTROcurrentAlarmProcess( maxCurrent, &generator.currentAlarm );

  return fELECTROgetMax( voltage, MAINS_LINE_NUMBER );
}
/*---------------------------------------------------------------------------------------------------*/
void vELECTROdataInit ( /*TIM_HandleTypeDef* currentTIM*/ void )
{
  shortCircuitDividend = fix16_mul( shortCircuitTrippingCurve, shortCircuitConstant );

  electro.scheme      = getBitMap( &genSetup, GEN_AC_SYS_ADR );
  electro.state       = ELECTRO_PROC_STATUS_IDLE;
  electro.cmd         = ELECTRO_CMD_NONE;
  electro.timer.id    = 0U;
  electro.timer.delay = getValue( &timerTransferDelay );

  generator.enb                    = getBitMap( &genSetup, GEN_POWER_GENERATOR_CONTROL_ENB_ADR );
  generator.rating.power.active    = getValue( &genRatedActivePowerLevel );
  generator.rating.power.reactive  = getValue( &genRatedReactivePowerLevel );
  generator.rating.power.apparent  = getValue( &genRatedApparentPowerLevel );
  generator.rating.cosFi           = fix16_div( generator.rating.power.active, generator.rating.power.apparent );
  generator.rating.freq            = getValue( &genRatedFrequencyLevel );
  generator.rating.current.primary = getValue( &genCurrentPrimaryLevel );
  generator.rating.current.nominal = getValue( &genCurrentFullLoadRatingLevel );
  /*----------------------------------------------------------------------------*/
  generator.lowVoltageAlarm.error.enb          = getBitMap( &genAlarms, GEN_UNDER_VOLTAGE_ALARM_ENB_ADR );
  generator.lowVoltageAlarm.error.active       = 0U;
  generator.lowVoltageAlarm.type               = ALARM_LEVEL_LOW;
  generator.lowVoltageAlarm.level              = getValue( &genUnderVoltageAlarmLevel );
  generator.lowVoltageAlarm.timer.delay        = 0U;
  generator.lowVoltageAlarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  generator.lowVoltageAlarm.error.event.type   = EVENT_GENERATOR_LOW_VOLTAGE;
  generator.lowVoltageAlarm.error.event.action = ACTION_EMERGENCY_STOP;
  generator.lowVoltageAlarm.error.status       = ALARM_STATUS_IDLE;
  generator.lowVoltageAlarm.error.id           = DEFINE_ERROR_LIST_ADR;
  /*----------------------------------------------------------------------------*/
  generator.lowVoltagePreAlarm.error.enb          = getBitMap( &genAlarms, GEN_UNDER_VOLTAGE_PRE_ALARM_ENB_ADR );
  generator.lowVoltagePreAlarm.error.active       = 0U;
  generator.lowVoltagePreAlarm.type               = ALARM_LEVEL_LOW;
  generator.lowVoltagePreAlarm.level              = getValue( &genUnderVoltagePreAlarmLevel );
  generator.lowVoltagePreAlarm.timer.delay        = 0U;
  generator.lowVoltagePreAlarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  generator.lowVoltagePreAlarm.error.event.type   = EVENT_GENERATOR_LOW_VOLTAGE;
  generator.lowVoltagePreAlarm.error.event.action = ACTION_WARNING;
  generator.lowVoltagePreAlarm.error.status       = ALARM_STATUS_IDLE;
  generator.lowVoltagePreAlarm.error.id           = DEFINE_ERROR_LIST_ADR;
  /*----------------------------------------------------------------------------*/
  generator.hightVoltagePreAlarm.error.enb          = getBitMap( &genAlarms, GEN_OVER_VOLTAGE_PRE_ALARM_ENB_ADR );
  generator.hightVoltagePreAlarm.error.active       = 0U;
  generator.hightVoltagePreAlarm.type               = ALARM_LEVEL_HIGHT;
  generator.hightVoltagePreAlarm.level              = getValue( &genOverVoltagePreAlarmLevel );
  generator.hightVoltagePreAlarm.timer.delay        = 0U;
  generator.hightVoltagePreAlarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  generator.hightVoltagePreAlarm.error.event.type   = EVENT_GENERATOR_HIGHT_VOLTAGE;
  generator.hightVoltagePreAlarm.error.event.action = ACTION_WARNING;
  generator.hightVoltagePreAlarm.error.status       = ALARM_STATUS_IDLE;
  generator.hightVoltagePreAlarm.error.id           = DEFINE_ERROR_LIST_ADR;
  /*----------------------------------------------------------------------------*/
  generator.hightVoltageAlarm.error.enb          = 1U;
  generator.hightVoltageAlarm.error.active       = 0U;
  generator.hightVoltageAlarm.type               = ALARM_LEVEL_HIGHT;
  generator.hightVoltageAlarm.level              = getValue( &genOverVoltageAlarmLevel );
  generator.hightVoltageAlarm.timer.delay        = 0U;
  generator.hightVoltageAlarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  generator.hightVoltageAlarm.error.event.type   = EVENT_GENERATOR_HIGHT_VOLTAGE;
  generator.hightVoltageAlarm.error.event.action = ACTION_EMERGENCY_STOP;
  generator.hightVoltageAlarm.error.status       = ALARM_STATUS_IDLE;
  generator.hightVoltageAlarm.error.id           = DEFINE_ERROR_LIST_ADR;
  /*----------------------------------------------------------------------------*/
  generator.lowFreqAlarm.error.enb          = getBitMap( &genAlarms, GEN_UNDER_FREQUENCY_ALARM_ENB_ADR );
  generator.lowFreqAlarm.error.active       = 0U;
  generator.lowFreqAlarm.type               = ALARM_LEVEL_LOW;
  generator.lowFreqAlarm.level              = getValue( &genUnderFrequencyAlarmLevel );
  generator.lowFreqAlarm.timer.delay        = 0U;
  generator.lowFreqAlarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  generator.lowFreqAlarm.error.event.type   = EVENT_GENERATOR_LOW_FREQUENCY;
  generator.lowFreqAlarm.error.event.action = ACTION_EMERGENCY_STOP;
  generator.lowFreqAlarm.error.status       = ALARM_STATUS_IDLE;
  generator.lowFreqAlarm.error.id           = DEFINE_ERROR_LIST_ADR;
  /*----------------------------------------------------------------------------*/
  generator.lowFreqPreAlarm.error.enb          = getBitMap( &genAlarms, GEN_UNDER_FREQUENCY_PRE_ALARM_ENB_ADR );
  generator.lowFreqPreAlarm.error.active       = 0U;
  generator.lowFreqPreAlarm.type               = ALARM_LEVEL_LOW;
  generator.lowFreqPreAlarm.level              = getValue( &genUnderFrequencyPreAlarmLevel );
  generator.lowFreqPreAlarm.timer.delay        = 0U;
  generator.lowFreqPreAlarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  generator.lowFreqPreAlarm.error.event.type   = EVENT_GENERATOR_LOW_FREQUENCY;
  generator.lowFreqPreAlarm.error.event.action = ACTION_WARNING;
  generator.lowFreqPreAlarm.error.status       = ALARM_STATUS_IDLE;
  generator.lowFreqPreAlarm.error.id           = DEFINE_ERROR_LIST_ADR;
  /*----------------------------------------------------------------------------*/
  generator.hightFreqPreAlarm.error.enb          = getBitMap( &genAlarms, GEN_OVER_FREQUENCY_PRE_ALARM_ENB_ADR );
  generator.hightFreqPreAlarm.error.active       = 0U;
  generator.hightFreqPreAlarm.type               = ALARM_LEVEL_HIGHT;
  generator.hightFreqPreAlarm.level              = getValue( &genOverFrequencyPreAlarmLevel );
  generator.hightFreqPreAlarm.timer.delay        = 0U;
  generator.hightFreqPreAlarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  generator.hightFreqPreAlarm.error.event.type   = EVENT_GENERATOR_HIGHT_FREQUENCY;
  generator.hightFreqPreAlarm.error.event.action = ACTION_WARNING;
  generator.hightFreqPreAlarm.error.status       = ALARM_STATUS_IDLE;
  generator.hightFreqPreAlarm.error.id           = DEFINE_ERROR_LIST_ADR;
  /*----------------------------------------------------------------------------*/
  generator.hightFreqAlarm.error.enb          = getBitMap( &genAlarms, GEN_OVER_FREQUENCY_ALARM_ENB_ADR );
  generator.hightFreqAlarm.error.active       = 0U;
  generator.hightFreqAlarm.type               = ALARM_LEVEL_HIGHT;
  generator.hightFreqAlarm.level              = getValue( &genOverFrequencyAlarmLevel );
  generator.hightFreqAlarm.timer.delay        = 0U;
  generator.hightFreqAlarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  generator.hightFreqAlarm.error.event.type   = EVENT_GENERATOR_HIGHT_FREQUENCY;;
  generator.hightFreqAlarm.error.event.action = ACTION_EMERGENCY_STOP;
  generator.hightFreqAlarm.error.status       = ALARM_STATUS_IDLE;
  generator.hightFreqAlarm.error.id           = DEFINE_ERROR_LIST_ADR;
  /*----------------------------------------------------------------------------*/
  generator.phaseImbalanceAlarm.error.enb        = getBitMap( &genAlarms, GEN_CURRENT_OVER_PHASE_IMBALANCE_ENB_ADR );
  generator.phaseImbalanceAlarm.error.active     = 0U;
  generator.phaseImbalanceAlarm.type             = ALARM_LEVEL_HIGHT;
  generator.phaseImbalanceAlarm.level            = getValue( &genCurrentOverPhaseImbalanceLevel );
  generator.phaseImbalanceAlarm.timer.delay      = getValue( &genCurrentOverPhaseImbalanceDelay );
  generator.phaseImbalanceAlarm.timer.id         = 0U;
  generator.phaseImbalanceAlarm.error.event.type = EVENT_PHASE_IMBALANCE;
  if ( getBitMap( &genAlarms, GEN_CURRENT_OVER_PHASE_IMBALANCE_ACTION_ADR ) == 0U )
  {
    generator.phaseImbalanceAlarm.error.event.action = ACTION_EMERGENCY_STOP;
  }
  else
  {
    generator.phaseImbalanceAlarm.error.event.action = ACTION_WARNING;
  }
  generator.phaseImbalanceAlarm.error.status = ALARM_STATUS_IDLE;
  generator.phaseImbalanceAlarm.error.id     = DEFINE_ERROR_LIST_ADR;
  /*----------------------------------------------------------------------------*/
  generator.overloadAlarm.error.enb        = getBitMap( &genAlarms, GEN_CURRENT_OVERLOAD_PROTECTION_ENB_ADR );
  generator.overloadAlarm.error.active     = 0U;
  generator.overloadAlarm.type             = ALARM_LEVEL_HIGHT;
  generator.overloadAlarm.level            = getValue( &genCurrentOverloadProtectionLevel );
  generator.overloadAlarm.timer.delay      = getValue( &genCurrentOverloadProtectionDelay );
  generator.overloadAlarm.timer.id         = LOGIC_DEFAULT_TIMER_ID;
  generator.overloadAlarm.error.event.type = EVENT_OVER_POWER;
  if ( getBitMap( &genAlarms, GEN_CURRENT_OVERLOAD_PROTECTION_ACTION_ADR ) == 0U )
  {
    generator.overloadAlarm.error.event.action = ACTION_PLAN_STOP;
  }
  else
  {
    generator.overloadAlarm.error.event.action = ACTION_WARNING;
  }
  generator.overloadAlarm.error.status = ALARM_STATUS_IDLE;
  generator.overloadAlarm.error.id     = DEFINE_ERROR_LIST_ADR;
  /*----------------------------------------------------------------------------*/
  generator.currentAlarm.state               = ELECTRO_CURRENT_STATUS_IDLE;
  generator.currentAlarm.over.current        = fix16_mul( generator.rating.current.nominal,
                                                          fix16_div( getValue( &genOverCurrentThermalProtectionLevel ), F16( 100U ) ) );
  generator.currentAlarm.over.delay          = 0U;
  generator.currentAlarm.over.event.type     = EVENT_OVER_CURRENT;
  generator.currentAlarm.over.event.action   = ACTION_PLAN_STOP;
  generator.currentAlarm.cutout.current      = fix16_mul( generator.rating.current.nominal,
                                                          fix16_div( getValue( &genOverCurrentCutoffLevel ), F16( 100U ) ) );
  generator.currentAlarm.cutout.delay        = 0U;
  generator.currentAlarm.cutout.event.type   = EVENT_SHORT_CIRCUIT;
  generator.currentAlarm.cutout.event.action = ACTION_PLAN_STOP;
  //generator.currentAlarm.tim                 = currentTIM;
  /*----------------------------------------------------------------------------*/
  generator.relay.enb    = uFPOisEnable( FPO_FUN_TURN_ON_GEN );
  generator.relay.status = RELAY_OFF;
  generator.relay.set    = vFPOsetGenSw;
  /*----------------------------------------------------------------------------*/
  generator.relayOn.relay.enb    = uFPOisEnable( FPO_FUN_TURN_ON_GEN_IMPULSE );
  generator.relayOn.relay.status = RELAY_OFF;
  generator.relayOn.relay.set    = vFPOsetGenOnImp;
  generator.relayOn.timer.delay  = getValue( &timerGenBreakerClosePulse );
  generator.relayOn.timer.id     = LOGIC_DEFAULT_TIMER_ID;
  generator.relayOn.status       = RELAY_DELAY_IDLE;
  /*----------------------------------------------------------------------------*/
  generator.relayOff.relay.enb    = uFPOisEnable( FPO_FUN_TURN_OFF_GEN_IMPULSE );
  generator.relayOff.relay.status = RELAY_OFF;
  generator.relayOff.relay.set    = vFPOsetGenOffImp;
  generator.relayOff.timer.delay  = getValue( &timerGenBreakerTripPulse );
  generator.relayOff.timer.id     = LOGIC_DEFAULT_TIMER_ID;
  generator.relayOff.status       = RELAY_DELAY_IDLE;
  /*----------------------------------------------------------------------------*/
  generator.getFreq             = xADCGetGENLFreq;
  generator.line[0U].getVoltage = xADCGetGENL1;
  generator.line[0U].getCurrent = xADCGetGENL1Cur;
  generator.line[1U].getVoltage = xADCGetGENL2;
  generator.line[1U].getCurrent = xADCGetGENL2Cur;
  generator.line[2U].getVoltage = xADCGetGENL3;
  generator.line[2U].getCurrent = xADCGetGENL3Cur;
  /*----------------------------------------------------------------------------*/
  /*----------------------------------------------------------------------------*/
  /*----------------------------------------------------------------------------*/
  mains.enb = getBitMap( &mainsSetup, MAINS_CONTROL_ENB_ADR );
  /*----------------------------------------------------------------------------*/
  mains.autoStart.ack          = PERMISSION_ENABLE;
  mains.autoStart.active       = PERMISSION_ENABLE;
  mains.autoStart.enb          = PERMISSION_ENABLE;
  mains.autoStart.event.action = ACTION_AUTO_START;
  mains.autoStart.event.type   = EVENT_MAINS_FAIL;
  mains.autoStart.id           = DEFINE_ERROR_LIST_ADR;
  mains.autoStart.ignor        = PERMISSION_DISABLE;
  mains.autoStart.status       = ALARM_STATUS_IDLE;
  mains.autoStart.trig         = TRIGGER_IDLE;
  /*----------------------------------------------------------------------------*/
  mains.autoStop.ack          = PERMISSION_ENABLE;
  mains.autoStop.active       = PERMISSION_ENABLE;
  mains.autoStop.enb          = PERMISSION_ENABLE;
  mains.autoStop.event.action = ACTION_AUTO_STOP;
  mains.autoStop.event.type   = EVENT_MAINS_OK;
  mains.autoStop.id           = DEFINE_ERROR_LIST_ADR;
  mains.autoStop.ignor        = PERMISSION_DISABLE;
  mains.autoStop.status       = ALARM_STATUS_IDLE;
  mains.autoStop.trig         = TRIGGER_IDLE;
  /*----------------------------------------------------------------------------*/
  mains.lowVoltageAlarm.error.enb          = getBitMap( &mainsAlarms, MAINS_UNDER_VOLTAGE_ALARM_ENB_ADR );
  mains.lowVoltageAlarm.error.active       = PERMISSION_ENABLE;
  mains.lowVoltageAlarm.type               = ALARM_LEVEL_LOW;
  mains.lowVoltageAlarm.level              = getValue( &mainsUnderVoltageAlarmLevel );
  mains.lowVoltageAlarm.timer.delay        = 0U;
  mains.lowVoltageAlarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  mains.lowVoltageAlarm.error.event.type   = EVENT_MAINS_LOW_VOLTAGE;
  mains.lowVoltageAlarm.error.event.action = ACTION_WARNING;
  mains.lowVoltageAlarm.error.ack          = PERMISSION_ENABLE;
  mains.lowVoltageAlarm.error.status       = ALARM_STATUS_IDLE;
  mains.lowVoltageAlarm.error.id           = DEFINE_ERROR_LIST_ADR;
  /*----------------------------------------------------------------------------*/
  mains.hightVoltageAlarm.error.enb          = getBitMap( &mainsAlarms, MAINS_OVER_VOLTAGE_ALARM_ENB_ADR );
  mains.hightVoltageAlarm.error.active       = PERMISSION_ENABLE;
  mains.hightVoltageAlarm.type               = ALARM_LEVEL_HIGHT;
  mains.hightVoltageAlarm.level              = getValue( &mainsOverVoltageAlarmLevel );
  mains.hightVoltageAlarm.timer.delay        = 0U;
  mains.hightVoltageAlarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  mains.hightVoltageAlarm.error.event.type   = EVENT_MAINS_HIGHT_VOLTAGE;
  mains.hightVoltageAlarm.error.event.action = ACTION_WARNING;
  mains.hightVoltageAlarm.error.ack          = PERMISSION_ENABLE;
  mains.hightVoltageAlarm.error.status       = ALARM_STATUS_IDLE;
  mains.hightVoltageAlarm.error.id           = DEFINE_ERROR_LIST_ADR;
  /*----------------------------------------------------------------------------*/
  mains.lowFreqAlarm.error.enb          = getBitMap( &mainsAlarms, MAINS_UNDER_FREQUENCY_ALARM_ENB_ADR );
  mains.lowFreqAlarm.error.active       = PERMISSION_ENABLE;
  mains.lowFreqAlarm.type               = ALARM_LEVEL_LOW;
  mains.lowFreqAlarm.level              = getValue( &mainsUnderFrequencyAlarmLevel );
  mains.lowFreqAlarm.timer.delay        = 0U;
  mains.lowFreqAlarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  mains.lowFreqAlarm.error.event.type   = EVENT_MAINS_LOW_FREQUENCY;
  mains.lowFreqAlarm.error.event.action = ACTION_WARNING;
  mains.lowFreqAlarm.error.ack          = PERMISSION_ENABLE;
  mains.lowFreqAlarm.error.status       = ALARM_STATUS_IDLE;
  mains.lowFreqAlarm.error.id           = DEFINE_ERROR_LIST_ADR;
  /*----------------------------------------------------------------------------*/
  mains.hightFreqAlarm.error.enb          = getBitMap( &mainsAlarms, MAINS_OVER_FREQUENCY_ALARM_ENB_ADR );
  mains.hightFreqAlarm.error.active       = PERMISSION_ENABLE;
  mains.hightFreqAlarm.type               = ALARM_LEVEL_HIGHT;
  mains.hightFreqAlarm.level              = getValue( &mainsOverFrequencyAlarmLevel );
  mains.hightFreqAlarm.timer.delay        = 0U;
  mains.hightFreqAlarm.timer.id           = LOGIC_DEFAULT_TIMER_ID;
  mains.hightFreqAlarm.error.event.type   = EVENT_MAINS_HIGHT_FREQUENCY;
  mains.hightFreqAlarm.error.event.action = ACTION_WARNING;
  mains.hightFreqAlarm.error.ack          = PERMISSION_ENABLE;
  mains.hightFreqAlarm.error.status       = ALARM_STATUS_IDLE;
  mains.hightFreqAlarm.error.id           = DEFINE_ERROR_LIST_ADR;
  /*----------------------------------------------------------------------------*/
  mains.relay.enb    = uFPOisEnable( FPO_FUN_TURN_ON_MAINS );
  mains.relay.status = RELAY_OFF;
  mains.relay.set    = vFPOsetMainsSw;
  /*----------------------------------------------------------------------------*/
  mains.relayOn.relay.enb    = uFPOisEnable( FPO_FUN_TURN_ON_MAINS_IMPULSE );
  mains.relayOn.relay.status = RELAY_OFF;
  mains.relayOn.relay.set    = vFPOsetMainsOnImp;
  mains.relayOn.timer.delay  = getValue( &timerMainsBreakerClosePulse );
  mains.relayOn.timer.id     = LOGIC_DEFAULT_TIMER_ID;
  mains.relayOn.status       = RELAY_DELAY_IDLE;
  /*----------------------------------------------------------------------------*/
  mains.relayOff.relay.enb    = uFPOisEnable( FPO_FUN_TURN_OFF_MAINS_IMPULSE );
  mains.relayOff.relay.status = RELAY_OFF;
  mains.relayOff.relay.set    = vFPOsetMainsOffImp;
  mains.relayOff.timer.delay  = getValue( &timerMainsBreakerTripPulse );
  mains.relayOff.timer.id     = LOGIC_DEFAULT_TIMER_ID;
  mains.relayOff.status       = RELAY_DELAY_IDLE;
  /*----------------------------------------------------------------------------*/
  mains.getFreq = xADCGetNETLFreq;
  mains.line[0U].getVoltage = xADCGetNETL1;
  mains.line[0U].getCurrent = NULL;
  mains.line[1U].getVoltage = xADCGetNETL2;
  mains.line[1U].getCurrent = NULL;
  mains.line[2U].getVoltage = xADCGetNETL3;
  mains.line[2U].getCurrent = NULL;
  /*----------------------------------------------------------------------------*/
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vELECTROinit ( void )
{
  /* Data initialization */
  vELECTROdataInit();
  /* OS initialization */
  pElectroCommandQueue = xQueueCreateStatic( ELECTRO_COMMAND_QUEUE_LENGTH, sizeof( ELECTRO_COMMAND ), electroCommandBuffer, &xElectroCommandQueue );
  const osThreadAttr_t electroTask_attributes = {
    .name       = "electroTask",
    .priority   = ( osPriority_t ) ELECTRO_TASK_PRIORITY,
    .stack_size = ELECTRO_TASK_STACK_SIZE
  };
  electroHandle = osThreadNew( vELECTROtask, NULL, &electroTask_attributes );
  /* Default condition initialization */
  mains.relay.set( RELAY_ON );
  mains.state = ELECTRO_STATUS_LOAD;
  generator.relay.set( RELAY_OFF );
  generator.state = ELECTRO_STATUS_IDLE;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
QueueHandle_t pELECTROgetCommandQueue ( void )
{
  return pElectroCommandQueue;
}
/*---------------------------------------------------------------------------------------------------*/
ELECTRO_STATUS eELECTROgetGeneratorStatus ( void )
{
  return generator.state;
}
/*---------------------------------------------------------------------------------------------------*/
ELECTRO_STATUS eELECTROgetMainsStatus ( void )
{
  return mains.state;
}
/*---------------------------------------------------------------------------------------------------*/
ELECTRO_ALARM_STATUS eELECTROgetAlarmStatus ( void )
{
  return electro.alarmState;
}
/*---------------------------------------------------------------------------------------------------*/
void vELECTROsendCmd ( ELECTRO_COMMAND cmd )
{
  ELECTRO_COMMAND electroCmd = cmd;
  xQueueSend( pElectroCommandQueue, &electroCmd, portMAX_DELAY );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
fix16_t fELECTROgetMaxGenVoltage ( void )
{
  return maxGeneratorVolage;
}
/*---------------------------------------------------------------------------------------------------*/
TRIGGER_STATE eELECTROgetMainsErrorFlag ( void )
{
  return eERRORisActive( &mains.autoStart );
}
/*---------------------------------------------------------------------------------------------------*/
void vELECTROtask ( void* argument )
{
  ELECTRO_COMMAND inputCmd    = ELECTRO_CMD_NONE;
  uint32_t        inputNotifi = 0U;
  for(;;)
  {
    /*-------------------- Read system notification --------------------*/
    if ( xTaskNotifyWait( 0U, 0xFFFFFFFFU, &inputNotifi, TASK_NOTIFY_WAIT_DELAY ) == pdPASS )
    {
      if ( ( inputNotifi & DATA_API_MESSAGE_REINIT ) > 0U )
      {
        vELECTROdataInit();
      }
    }
    /*---------------------- Data input processing ---------------------*/
    maxGeneratorVolage = fGENERATORprocess();
    vMAINSprocess();
    /*-------------------- Input commands processing -------------------*/
    if ( electro.state == ELECTRO_PROC_STATUS_IDLE )
    {
      if ( xQueueReceive( pElectroCommandQueue, &inputCmd, 0U ) == pdPASS )
      {
        electro.cmd = inputCmd;
      }
    }
    /*----------------------- Command processing -----------------------*/
    switch( electro.cmd )
    {
      case ELECTRO_CMD_NONE:
        break;
      case ELECTRO_CMD_LOAD_MAINS:
        if ( mains.state == ELECTRO_STATUS_IDLE )
        {
          switch ( electro.state )
          {
            case ELECTRO_PROC_STATUS_IDLE:
              generator.relay.set( RELAY_OFF );
              vRELAYdelayTrig( &generator.relayOff );
              electro.state = ELECTRO_PROC_STATUS_DISCONNECT;
              vLOGICprintDebug( ">>Electro         : Disconnect Generator\r\n" );
              break;
            case ELECTRO_PROC_STATUS_DISCONNECT:
              if ( generator.relayOff.status == RELAY_DELAY_IDLE )
              {
                vLOGICstartTimer( &electro.timer );
                electro.state   = ELECTRO_PROC_STATUS_CONNECT;
                vLOGICprintDebug( ">>Electro         : Start delay\r\n" );
              }
              break;
            case ELECTRO_PROC_STATUS_CONNECT:
              if ( uLOGICisTimer( electro.timer ) > 0U )
              {
                generator.state = ELECTRO_STATUS_IDLE;
                mains.relay.set( RELAY_ON );
                vRELAYdelayTrig( &mains.relayOn );
                electro.state = ELECTRO_PROC_STATUS_DONE;
                vLOGICprintDebug( ">>Electro         : Connect mains\r\n" );
              }
              break;
            case ELECTRO_PROC_STATUS_DONE:
              if ( mains.relayOn.status == RELAY_DELAY_IDLE )
              {
                electro.state = ELECTRO_STATUS_IDLE;
                mains.state   = ELECTRO_STATUS_LOAD;
                electro.cmd   = ELECTRO_CMD_NONE;
                vLOGICprintDebug( ">>Electro         : Mains connection done\r\n" );
              }
              break;
            default:
              electro.cmd   = ELECTRO_CMD_NONE;
              electro.state = ELECTRO_STATUS_IDLE;
              break;
          }
        }
        break;
      case ELECTRO_CMD_LOAD_GENERATOR:
        if ( generator.state == ELECTRO_STATUS_IDLE )
        {
          switch ( electro.state )
          {
            case ELECTRO_PROC_STATUS_IDLE:
              mains.relay.set( RELAY_OFF );
              vRELAYdelayTrig( &mains.relayOff );
              vLOGICstartTimer( &electro.timer );
              electro.state = ELECTRO_PROC_STATUS_DISCONNECT;
              vLOGICprintDebug( ">>Electro         : Disconnect mains\r\n" );
              break;
            case ELECTRO_PROC_STATUS_DISCONNECT:
              if ( mains.relayOff.status == RELAY_DELAY_IDLE )
              {
                vLOGICstartTimer( &electro.timer );
                electro.state = ELECTRO_PROC_STATUS_CONNECT;
                vLOGICprintDebug( ">>Electro         : Start delay\r\n" );
              }
              break;
            case ELECTRO_PROC_STATUS_CONNECT:
              if ( uLOGICisTimer( electro.timer ) > 0U )
              {
                mains.state   = ELECTRO_STATUS_IDLE;
                generator.relay.set( RELAY_ON );
                vRELAYdelayTrig( &generator.relayOn );
                electro.state = ELECTRO_PROC_STATUS_DONE;
                vLOGICprintDebug( ">>Electro         : Connect generator\r\n" );
              }
              break;
            case ELECTRO_PROC_STATUS_DONE:
              if ( generator.relayOn.status == RELAY_DELAY_IDLE )
              {
                generator.state = ELECTRO_STATUS_LOAD;
                electro.state   = ELECTRO_STATUS_IDLE;
                electro.cmd     = ELECTRO_CMD_NONE;
                vLOGICprintDebug( ">>Electro         : Generator connection done\r\n" );
              }
              break;
            default:
              electro.cmd   = ELECTRO_CMD_NONE;
              electro.state = ELECTRO_STATUS_IDLE;
              break;
          }
        }
        break;
      case ELECTRO_CMD_ENABLE_STOP_ALARMS:
        mains.lowVoltageAlarm.error.active          = PERMISSION_ENABLE;
        mains.hightVoltageAlarm.error.active        = PERMISSION_ENABLE;
        mains.lowFreqAlarm.error.active             = PERMISSION_ENABLE;
        mains.hightFreqAlarm.error.active           = PERMISSION_ENABLE;
        generator.lowVoltageAlarm.error.active      = PERMISSION_DISABLE;
        generator.lowVoltagePreAlarm.error.active   = PERMISSION_DISABLE;
        generator.hightVoltagePreAlarm.error.active = PERMISSION_DISABLE;
        generator.hightVoltageAlarm.error.active    = PERMISSION_DISABLE;
        generator.lowFreqAlarm.error.active         = PERMISSION_DISABLE;
        generator.lowFreqPreAlarm.error.active      = PERMISSION_DISABLE;
        generator.hightFreqPreAlarm.error.active    = PERMISSION_DISABLE;
        generator.hightFreqAlarm.error.active       = PERMISSION_DISABLE;
        electro.alarmState                          = ELECTRO_ALARM_STATUS_STOP;
        electro.cmd                                 = ELECTRO_CMD_NONE;
        vLOGICprintDebug( ">>Electro         : Alarms set as stop state\r\n" );
        break;
      case ELECTRO_CMD_DISABLE_START_ALARMS:
        mains.lowVoltageAlarm.error.active          = PERMISSION_DISABLE;
        mains.hightVoltageAlarm.error.active        = PERMISSION_DISABLE;
        mains.lowFreqAlarm.error.active             = PERMISSION_DISABLE;
        mains.hightFreqAlarm.error.active           = PERMISSION_DISABLE;
        generator.lowVoltageAlarm.error.active      = PERMISSION_DISABLE;
        generator.lowVoltagePreAlarm.error.active   = PERMISSION_DISABLE;
        generator.hightVoltagePreAlarm.error.active = PERMISSION_DISABLE;
        generator.hightVoltageAlarm.error.active    = PERMISSION_DISABLE;
        generator.lowFreqAlarm.error.active         = PERMISSION_DISABLE;
        generator.lowFreqPreAlarm.error.active      = PERMISSION_DISABLE;
        generator.hightFreqPreAlarm.error.active    = PERMISSION_DISABLE;
        generator.hightFreqAlarm.error.active       = PERMISSION_DISABLE;
        electro.alarmState                          = ELECTRO_ALARM_STATUS_START;
        electro.cmd                                 = ELECTRO_CMD_NONE;
        vLOGICprintDebug( ">>Electro         : Alarms set as start state\r\n" );
        break;
      case ELECTRO_CMD_ENABLE_START_TO_IDLE_ALARMS:
        mains.lowVoltageAlarm.error.active          = PERMISSION_ENABLE;
        mains.hightVoltageAlarm.error.active        = PERMISSION_ENABLE;
        mains.lowFreqAlarm.error.active             = PERMISSION_ENABLE;
        mains.hightFreqAlarm.error.active           = PERMISSION_ENABLE;
        generator.lowVoltageAlarm.error.active      = PERMISSION_ENABLE;
        generator.lowVoltagePreAlarm.error.active   = PERMISSION_ENABLE;
        generator.hightVoltagePreAlarm.error.active = PERMISSION_ENABLE;
        generator.hightVoltageAlarm.error.active    = PERMISSION_ENABLE;
        generator.hightFreqPreAlarm.error.active    = PERMISSION_ENABLE;
        generator.hightFreqAlarm.error.active       = PERMISSION_ENABLE;
        electro.alarmState                          = ELECTRO_ALARM_STATUS_START_ON_IDLE;
        electro.cmd                                 = ELECTRO_CMD_NONE;
        vLOGICprintDebug( ">>Electro         : Alarms set as start on idle\r\n" );
        break;
      case ELECTRO_CMD_DISABLE_IDLE_ALARMS:
        generator.lowFreqAlarm.error.active    = PERMISSION_DISABLE;
        generator.lowFreqPreAlarm.error.active = PERMISSION_DISABLE;
        electro.alarmState                     = ELECTRO_ALARM_STATUS_WORK_ON_IDLE;
        electro.cmd                            = ELECTRO_CMD_NONE;
        vLOGICprintDebug( ">>Electro         : Alarms set as work on idle\r\n" );
        break;
      case ELECTRO_CMD_ENABLE_IDLE_ALARMS:
        generator.lowFreqAlarm.error.active    = PERMISSION_ENABLE;
        generator.lowFreqPreAlarm.error.active = PERMISSION_ENABLE;
        electro.alarmState                     = ELECTRO_ALARM_STATUS_WORK;
        electro.cmd                            = ELECTRO_CMD_NONE;
        vLOGICprintDebug( ">>Electro         : Alarms set as normal work\r\n" );
        break;
      case ELECTRO_CMD_RESET_TO_IDLE:
        vERRORreset( &mains.lowVoltageAlarm.error );
        vERRORreset( &mains.hightVoltageAlarm.error );
        vERRORreset( &mains.lowFreqAlarm.error );
        vERRORreset( &mains.hightFreqAlarm.error );
        vERRORreset( &generator.hightFreqAlarm.error );
        vERRORreset( &generator.hightFreqPreAlarm.error );
        vERRORreset( &generator.hightVoltageAlarm.error );
        vERRORreset( &generator.hightVoltagePreAlarm.error );
        vERRORreset( &generator.lowFreqAlarm.error );
        vERRORreset( &generator.lowFreqPreAlarm.error );
        vERRORreset( &generator.lowVoltageAlarm.error );
        vERRORreset( &generator.lowVoltagePreAlarm.error );
        vERRORreset( &generator.overloadAlarm.error );
        vERRORreset( &generator.phaseImbalanceAlarm.error );
        mains.lowVoltageAlarm.error.active          = PERMISSION_ENABLE;
        mains.hightVoltageAlarm.error.active        = PERMISSION_ENABLE;
        mains.lowFreqAlarm.error.active             = PERMISSION_ENABLE;
        mains.hightFreqAlarm.error.active           = PERMISSION_ENABLE;
        vLOGICprintDebug( ">>Electro         : Set to idle \r\n" );
        break;
      default:
        vLOGICprintDebug( ">>Electro         : Error command\r\n" );
        break;
    }
    /*----------------------- Output processing ------------------------*/
    vRELAYdelayProcess( &generator.relayOff );
    vRELAYdelayProcess( &mains.relayOn );
    vRELAYdelayProcess( &generator.relayOn );
    vRELAYdelayProcess( &mains.relayOff );
  }
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

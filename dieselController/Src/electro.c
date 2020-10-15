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
/*---------------------------------- Define ----------------------------------*/

/*-------------------------------- Structures --------------------------------*/
static GENERATOR_TYPE      generator            = { 0U };
static MAINS_TYPE          mains                = { 0U } ;
static ELECTRO_SYSTEM_TYPE electro              = { 0U };
static StaticQueue_t       xElectroCommandQueue = { 0U };
static QueueHandle_t       pElectroCommandQueue = NULL;
/*--------------------------------- Constant ---------------------------------*/
/*-------------------------------- Variables ---------------------------------*/
static ELECTRO_COMMAND electroCommandBuffer[ELECTRO_COMMAND_QUEUE_LENGTH] = { 0U };
/*-------------------------------- External ----------------------------------*/
osThreadId_t electroHandle = NULL;
/*-------------------------------- Functions ---------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/
fix16_t getCallback( void )
{
  return 0;
}
/*---------------------------------------------------------------------------------------------------*/
/* Calculation of disconnection time in temperature protection
 * input:  input   - input current from most loaded line
 *         setting - current of disconnection
 * output: Time of disconnection
 */
fix16_t fIDMTcalcTemp ( fix16_t input, fix16_t setting )
{
  return fix16_div( TEMP_PROTECTION_TIME_MULTIPLIER, fix16_sq( fix16_sub( fix16_div( input, setting ), F16( 1U ) ) ) );
}
/*---------------------------------------------------------------------------------------------------*/
/* Calculation of disconnection time in short sircit protection
 * input:  input   - input current from most loaded line
 *         setting - current of disconnection
 * output: Time of disconnection
 */
fix16_t fIDMTcalcCutout ( fix16_t input, fix16_t setting )
{
  //return fix16_div( fix16_mul( CUTOUT_PROTECTION_TRIPPING_CURVE, F16( 0.14f ) ), fix16_sub( fix16_pow( fix16_div( input, setting ), CUTOUT_POWER ), F16( 1U ) ) );
  return 0;
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
        //alarm->cutout.delay       = fIDMTcalcCutout( current, alarm->cutout.current );
        alarm->state              = ELECTRO_CURRENT_STATUS_CUTOUT_TRIG;
        alarm->tim->Instance->CNT = 0U;
        HAL_TIM_Base_Start( alarm->tim );
      }
      else if ( current >= alarm->over.current )
      {
        //alarm->over.delay         = fIDMTcalcTemp( current, alarm->over.current );
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
  fix16_t voltage[MAINS_LINE_NUMBER] = { 0U };
  fix16_t freq[MAINS_LINE_NUMBER]    = { 0U };
  uint8_t i                          = 0U;

  for ( i=0U; i<MAINS_LINE_NUMBER; i++ )
  {
    voltage[i] = mains.line[i].getVoltage();
    freq[i]    = mains.line[i].getFreq();
  }
  vELECTROalarmCheck( &mains.lowVoltageAlarm, voltage, MAINS_LINE_NUMBER );
  if ( mains.lowVoltageAlarm.status == ALARM_STATUS_IDLE )
  {
    vELECTROalarmCheck( &mains.hightVoltageAlarm, voltage, MAINS_LINE_NUMBER );
  }
  vELECTROalarmCheck( &mains.lowFreqAlarm, freq, MAINS_LINE_NUMBER );
  if ( mains.lowFreqAlarm.status == ALARM_STATUS_IDLE )
  {
    vELECTROalarmCheck( &mains.hightFreqAlarm, freq, MAINS_LINE_NUMBER );
  }
  if ( mains.state == ELECTRO_STATUS_ALARM )
  {
    vFPOsetNetFault( RELAY_ON );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vGENERATORprocess ( void )
{
  fix16_t voltage[MAINS_LINE_NUMBER] = { 0U };
  fix16_t freq[MAINS_LINE_NUMBER]    = { 0U };
  fix16_t current[MAINS_LINE_NUMBER] = { 0U };
  fix16_t power[MAINS_LINE_NUMBER]   = { 0U };
  fix16_t maxCurrent                 = 0U;
  uint8_t i                          = 0U;

  for ( i=0U; i<GENERATOR_LINE_NUMBER; i++ )
  {
    voltage[i] = generator.line[i].getVoltage();
    freq[i]    = generator.line[i].getFreq();
    current[i] = generator.line[i].getCurrent();
    power[i]   = fix16_mul( fix16_mul( voltage[i], current[i] ), generator.rating.cosFi );
  }
  maxCurrent = fELECTROgetMax( current, GENERATOR_LINE_NUMBER );
  vELECTROalarmCheck( &generator.lowVoltageAlarm, voltage, MAINS_LINE_NUMBER );
  if ( mains.lowVoltageAlarm.status == ALARM_STATUS_IDLE )
  {
    vELECTROalarmCheck( &generator.hightVoltageAlarm, voltage, MAINS_LINE_NUMBER );
  }
  vELECTROalarmCheck( &generator.lowFreqAlarm, freq, MAINS_LINE_NUMBER );
  if ( mains.lowFreqAlarm.status == ALARM_STATUS_IDLE )
  {
    vELECTROalarmCheck( &generator.hightFreqAlarm, freq, MAINS_LINE_NUMBER );
  }
  if ( electro.scheme != ELECTRO_SCHEME_SINGLE_PHASE )
  {
    vALARMcheck( &generator.phaseImbalanceAlarm, fELECTROcalcPhaseImbalance( current ) );
  }
  vELECTROalarmCheck( &generator.overloadAlarm, power, MAINS_LINE_NUMBER );
  vELECTROcurrentAlarmProcess( maxCurrent, &generator.currentAlarm );

  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vELECTROdataInit ( /*TIM_HandleTypeDef* currentTIM*/ void )
{
  uint8_t i = 0U;

  electro.scheme      = getBitMap( &genSetup, 1U );
  electro.state       = ELECTRO_PROC_STATUS_IDLE;
  electro.cmd         = ELECTRO_CMD_NONE;
  electro.timer.id    = 0U;
  electro.timer.delay = getValue( &timerTransferDelay );

  generator.enb                    = getBitMap( &genSetup, 0U );
  generator.rating.power.active    = getValue( &genRatedActivePowerLevel );
  generator.rating.power.reactive  = getValue( &genRatedReactivePowerLevel );
  generator.rating.power.apparent  = getValue( &genRatedApparentPowerLevel );
  generator.rating.cosFi           = fix16_div( generator.rating.power.active, generator.rating.power.apparent );
  generator.rating.freq            = getValue( &genRatedFrequencyLevel );
  generator.rating.current.primary = getValue( &genCurrentPrimaryLevel );
  generator.rating.current.nominal = getValue( &genCurrentFullLoadRatingLevel );
  /*----------------------------------------------------------------------------*/
  generator.lowVoltageAlarm.enb          = getBitMap( &genAlarms, 0U );
  generator.lowVoltageAlarm.active       = 0U;
  generator.lowVoltageAlarm.type         = ALARM_LEVEL_LOW;
  generator.lowVoltageAlarm.level        = getValue( &genUnderVoltageAlarmLevel );
  generator.lowVoltageAlarm.timer.delay  = 0U;
  generator.lowVoltageAlarm.timer.id     = 0U;
  generator.lowVoltageAlarm.track.event.type   = EVENT_GENERATOR_LOW_VOLTAGE;
  generator.lowVoltageAlarm.track.event.action = ACTION_EMERGENCY_STOP;
  generator.lowVoltageAlarm.track.relax.enb    = 0U;
  generator.lowVoltageAlarm.status       = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  generator.lowVoltagePreAlarm.enb          = getBitMap( &genAlarms, 1U );
  generator.lowVoltagePreAlarm.active       = 0U;
  generator.lowVoltagePreAlarm.type         = ALARM_LEVEL_LOW;
  generator.lowVoltagePreAlarm.level        = getValue( &genUnderVoltagePreAlarmLevel );
  generator.lowVoltagePreAlarm.timer.delay  = 0U;
  generator.lowVoltagePreAlarm.timer.id     = 0U;
  generator.lowVoltagePreAlarm.track.event.type   = EVENT_GENERATOR_LOW_VOLTAGE;
  generator.lowVoltagePreAlarm.track.event.action = ACTION_WARNING;
  generator.lowVoltagePreAlarm.track.relax.enb    = 0U;
  generator.lowVoltagePreAlarm.status       = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  generator.hightVoltagePreAlarm.enb          = getBitMap( &genAlarms, 2U );
  generator.hightVoltagePreAlarm.active       = 0U;
  generator.hightVoltagePreAlarm.type         = ALARM_LEVEL_HIGHT;
  generator.hightVoltagePreAlarm.level        = getValue( &genOverVoltagePreAlarmLevel );
  generator.hightVoltagePreAlarm.timer.delay  = 0U;
  generator.hightVoltagePreAlarm.timer.id     = 0U;
  generator.hightVoltagePreAlarm.track.event.type   = EVENT_GENERATOR_HIGHT_VOLTAGE;
  generator.hightVoltagePreAlarm.track.event.action = ACTION_WARNING;
  generator.hightVoltagePreAlarm.track.relax.enb    = 0U;
  generator.hightVoltagePreAlarm.status       = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  generator.hightVoltageAlarm.enb          = 1U;
  generator.hightVoltageAlarm.active       = 0U;
  generator.hightVoltageAlarm.type         = ALARM_LEVEL_HIGHT;
  generator.hightVoltageAlarm.level        = getValue( &genOverVoltageAlarmLevel );
  generator.hightVoltageAlarm.timer.delay  = 0U;
  generator.hightVoltageAlarm.timer.id     = 0U;
  generator.hightVoltageAlarm.track.event.type   = EVENT_GENERATOR_HIGHT_VOLTAGE;
  generator.hightVoltageAlarm.track.event.action = ACTION_EMERGENCY_STOP;
  generator.hightVoltageAlarm.track.relax.enb    = 0U;
  generator.hightVoltageAlarm.status       = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  generator.lowFreqAlarm.enb          = getBitMap( &genAlarms, 3U );
  generator.lowFreqAlarm.active       = 0U;
  generator.lowFreqAlarm.type         = ALARM_LEVEL_LOW;
  generator.lowFreqAlarm.level        = getValue( &genUnderFrequencyAlarmLevel );
  generator.lowFreqAlarm.timer.delay  = 0U;
  generator.lowFreqAlarm.timer.id     = 0U;
  generator.lowFreqAlarm.track.event.type   = EVENT_GENERATOR_LOW_FREQUENCY;
  generator.lowFreqAlarm.track.event.action = ACTION_EMERGENCY_STOP;
  generator.lowFreqAlarm.track.relax.enb    = 0U;
  generator.lowFreqAlarm.status       = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  generator.lowFreqPreAlarm.enb          = getBitMap( &genAlarms, 4U );
  generator.lowFreqPreAlarm.active       = 0U;
  generator.lowFreqPreAlarm.type         = ALARM_LEVEL_LOW;
  generator.lowFreqPreAlarm.level        = getValue( &genUnderFrequencyPreAlarmLevel );
  generator.lowFreqPreAlarm.timer.delay  = 0U;
  generator.lowFreqPreAlarm.timer.id     = 0U;
  generator.lowFreqPreAlarm.track.event.type   = EVENT_GENERATOR_LOW_FREQUENCY;
  generator.lowFreqPreAlarm.track.event.action = ACTION_WARNING;
  generator.lowFreqPreAlarm.track.relax.enb    = 0U;
  generator.lowFreqPreAlarm.status       = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  generator.hightFreqPreAlarm.enb          = getBitMap( &genAlarms, 5U );
  generator.hightFreqPreAlarm.active       = 0U;
  generator.hightFreqPreAlarm.type         = ALARM_LEVEL_HIGHT;
  generator.hightFreqPreAlarm.level        = getValue( &genOverFrequencyPreAlarmLevel );
  generator.hightFreqPreAlarm.timer.delay  = 0U;
  generator.hightFreqPreAlarm.timer.id     = 0U;
  generator.hightFreqPreAlarm.track.event.type   = EVENT_GENERATOR_HIGHT_FREQUENCY;
  generator.hightFreqPreAlarm.track.event.action = ACTION_WARNING;
  generator.hightFreqPreAlarm.track.relax.enb    = 0U;
  generator.hightFreqPreAlarm.status       = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  generator.hightFreqAlarm.enb          = getBitMap( &genAlarms, 6U );
  generator.hightFreqAlarm.active       = 0U;
  generator.hightFreqAlarm.type         = ALARM_LEVEL_HIGHT;
  generator.hightFreqAlarm.level        = getValue( &genOverFrequencyAlarmLevel );
  generator.hightFreqAlarm.timer.delay  = 0U;
  generator.hightFreqAlarm.timer.id     = 0U;
  generator.hightFreqAlarm.track.event.type   = EVENT_GENERATOR_HIGHT_FREQUENCY;;
  generator.hightFreqAlarm.track.event.action = ACTION_EMERGENCY_STOP;
  generator.hightFreqAlarm.track.relax.enb    = 0U;
  generator.hightFreqAlarm.status       = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  generator.phaseImbalanceAlarm.enb          = getBitMap( &genAlarms, 8U );
  generator.phaseImbalanceAlarm.active       = 0U;
  generator.phaseImbalanceAlarm.type         = ALARM_LEVEL_HIGHT;
  generator.phaseImbalanceAlarm.level        = getValue( &genCurrentOverPhaseImbalanceLevel );
  generator.phaseImbalanceAlarm.timer.delay  = getValue( &genCurrentOverPhaseImbalanceDelay );
  generator.phaseImbalanceAlarm.timer.id     = 0U;
  generator.phaseImbalanceAlarm.track.event.type   = EVENT_PHASE_IMBALANCE;
  if ( getBitMap( &genAlarms, 11U ) == 0U )
  {
    generator.phaseImbalanceAlarm.track.event.action = ACTION_LOAD_SHUTDOWN;
  }
  else
  {
    generator.phaseImbalanceAlarm.track.event.action = ACTION_WARNING;
  }
  generator.phaseImbalanceAlarm.track.relax.enb    = 0U;
  generator.phaseImbalanceAlarm.status       = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  generator.overloadAlarm.enb         = getBitMap( &genAlarms, 7U );
  generator.overloadAlarm.active      = 0U;
  generator.overloadAlarm.type        = ALARM_LEVEL_HIGHT;
  generator.overloadAlarm.level       = getValue( &genCurrentOverloadProtectionLevel );
  generator.overloadAlarm.timer.delay = getValue( &genCurrentOverloadProtectionDelay );
  generator.overloadAlarm.timer.id    = 0U;
  generator.overloadAlarm.track.event.type = EVENT_OVER_POWER;
  if ( getBitMap( &genAlarms, 10U ) == 0U )
  {
    generator.overloadAlarm.track.event.action = ACTION_LOAD_SHUTDOWN;
  }
  else
  {
    generator.overloadAlarm.track.event.action = ACTION_WARNING;
  }
  generator.overloadAlarm.track.relax.enb  = 0U;
  generator.overloadAlarm.status     = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  generator.currentAlarm.state               = ELECTRO_CURRENT_STATUS_IDLE;
  generator.currentAlarm.over.current        = fix16_mul( generator.rating.current.nominal,
                                                          fix16_div( getValue( &genOverCurrentThermalProtectionLevel ), F16( 100U ) ) );
  generator.currentAlarm.over.delay          = 0U;
  generator.currentAlarm.over.event.type     = EVENT_OVER_CURRENT;
  generator.currentAlarm.over.event.action   = ACTION_EMERGENCY_STOP;
  generator.currentAlarm.cutout.current      = fix16_mul( generator.rating.current.nominal,
                                                          fix16_div( getValue( &genOverCurrentCutoffLevel ), F16( 100U ) ) );
  generator.currentAlarm.cutout.delay        = 0U;
  generator.currentAlarm.cutout.event.type   = EVENT_SHORT_CIRCUIT;
  generator.currentAlarm.cutout.event.action = ACTION_LOAD_SHUTDOWN;
  //generator.currentAlarm.tim                 = currentTIM;
  /*----------------------------------------------------------------------------*/
  generator.relay.enb    = uFPOisEnable( FPO_FUN_TURN_ON_GEN );
  generator.relay.status = RELAY_OFF;
  generator.relay.set    = vFPOsetGenSw;
  /*----------------------------------------------------------------------------*/
  generator.relayOn.active       = 1U;
  generator.relayOn.relay.enb    = uFPOisEnable( FPO_FUN_TURN_ON_GEN_IMPULSE );
  generator.relayOn.relay.status = RELAY_OFF;
  generator.relayOn.relay.set    = vFPOsetGenOnImp;
  generator.relayOn.level        = 0U;
  generator.relayOn.timer.delay  = getValue( &timerGenBreakerClosePulse );
  generator.relayOn.timer.id     = 0U;
  generator.relayOn.status       = RELAY_DELAY_IDLE;
  /*----------------------------------------------------------------------------*/
  generator.relayOff.active       = 0U;
  generator.relayOff.relay.enb    = uFPOisEnable( FPO_FUN_TURN_OFF_GEN_IMPULSE );
  generator.relayOff.relay.status = RELAY_OFF;
  generator.relayOff.relay.set    = vFPOsetGenOffImp;
  generator.relayOff.level        = 0U;
  generator.relayOff.timer.delay  = getValue( &timerGenBreakerTripPulse );
  generator.relayOff.timer.id     = 0U;
  generator.relayOff.status       = RELAY_DELAY_IDLE;
  /*----------------------------------------------------------------------------*/
  for ( i=0U; i<GENERATOR_LINE_NUMBER; i++ )
  {
    generator.line[i].getVoltage = getCallback;
    generator.line[i].getFreq    = getCallback;
    generator.line[i].getCurrent = getCallback;
  }
  /*----------------------------------------------------------------------------*/
  /*----------------------------------------------------------------------------*/
  /*----------------------------------------------------------------------------*/
  mains.enb = getBitMap( &mainsSetup, 0U );

  mains.lowVoltageAlarm.enb                = getBitMap( &mainsAlarms, 0U );
  mains.lowVoltageAlarm.active             = 0U;
  mains.lowVoltageAlarm.type               = ALARM_LEVEL_LOW;
  mains.lowVoltageAlarm.level              = getValue( &mainsUnderVoltageAlarmLevel );
  mains.lowVoltageAlarm.timer.delay        = 0U;
  mains.lowVoltageAlarm.timer.id           = 0U;
  mains.lowVoltageAlarm.track.event.type         = EVENT_MAINS_LOW_VOLTAGE;
  mains.lowVoltageAlarm.track.event.action       = ACTION_LOAD_GENERATOR;
  mains.lowVoltageAlarm.track.relax.enb          = 1U;
  mains.lowVoltageAlarm.track.relax.event.type   = EVENT_MAINS_VOLTAGE_RELAX;
  mains.lowVoltageAlarm.track.relax.event.action = ACTION_LOAD_MAINS;
  mains.lowVoltageAlarm.status             = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  mains.hightVoltageAlarm.enb                = getBitMap( &mainsAlarms, 1U );
  mains.hightVoltageAlarm.active             = 0U;
  mains.hightVoltageAlarm.type               = ALARM_LEVEL_HIGHT;
  mains.hightVoltageAlarm.level              = getValue( &mainsOverVoltageAlarmLevel );
  mains.hightVoltageAlarm.timer.delay        = 0U;
  mains.hightVoltageAlarm.timer.id           = 0U;
  mains.hightVoltageAlarm.track.event.type         = EVENT_MAINS_HIGHT_VOLTAGE;
  mains.hightVoltageAlarm.track.event.action       = ACTION_LOAD_GENERATOR;
  mains.hightVoltageAlarm.track.relax.enb          = 1U;
  mains.hightVoltageAlarm.track.relax.event.type   = EVENT_MAINS_VOLTAGE_RELAX;
  mains.hightVoltageAlarm.track.relax.event.action = ACTION_LOAD_MAINS;
  mains.hightVoltageAlarm.status             = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  mains.lowFreqAlarm.enb                = getBitMap( &mainsAlarms, 2U );
  mains.lowFreqAlarm.active             = 0U;
  mains.lowFreqAlarm.type               = ALARM_LEVEL_LOW;
  mains.lowFreqAlarm.level              = getValue( &mainsUnderFrequencyAlarmLevel );
  mains.lowFreqAlarm.timer.delay        = 0U;
  mains.lowFreqAlarm.timer.id           = 0U;
  mains.lowFreqAlarm.track.event.type         = EVENT_MAINS_LOW_FREQUENCY;
  mains.lowFreqAlarm.track.event.action       = ACTION_LOAD_GENERATOR;
  mains.lowFreqAlarm.track.relax.enb          = 1U;
  mains.lowFreqAlarm.track.relax.event.type   = EVENT_MAINS_FREQUENCY_RELAX;
  mains.lowFreqAlarm.track.relax.event.action = ACTION_LOAD_MAINS;
  mains.lowFreqAlarm.status             = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  mains.hightFreqAlarm.enb                = getBitMap( &mainsAlarms, 3U );
  mains.hightFreqAlarm.active             = 0U;
  mains.hightFreqAlarm.type               = ALARM_LEVEL_HIGHT;
  mains.hightFreqAlarm.level              = getValue( &mainsOverFrequencyAlarmLevel );
  mains.hightFreqAlarm.timer.delay        = 0U;
  mains.hightFreqAlarm.timer.id           = 0U;
  mains.hightFreqAlarm.track.event.type         = EVENT_MAINS_HIGHT_FREQUENCY;
  mains.hightFreqAlarm.track.event.action       = ACTION_LOAD_GENERATOR;
  mains.hightFreqAlarm.track.relax.enb          = 1U;
  mains.hightFreqAlarm.track.relax.event.type   = EVENT_MAINS_FREQUENCY_RELAX;
  mains.hightFreqAlarm.track.relax.event.action = ACTION_LOAD_MAINS;
  mains.hightFreqAlarm.status             = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  mains.relay.enb    = uFPOisEnable( FPO_FUN_TURN_ON_MAINS );
  mains.relay.status = RELAY_OFF;
  mains.relay.set    = vFPOsetMainsSw;
  /*----------------------------------------------------------------------------*/
  mains.relayOn.relay.enb    = uFPOisEnable( FPO_FUN_TURN_ON_MAINS_IMPULSE );
  mains.relayOn.relay.status = RELAY_OFF;
  mains.relayOn.relay.set    = vFPOsetMainsOnImp;
  mains.relayOn.level        = 0U;
  mains.relayOn.timer.delay  = getValue( &timerMainsBreakerClosePulse );
  mains.relayOn.timer.id     = 0U;
  mains.relayOn.status       = RELAY_DELAY_IDLE;
  /*----------------------------------------------------------------------------*/
  mains.relayOff.relay.enb    = uFPOisEnable( FPO_FUN_TURN_OFF_MAINS_IMPULSE );
  mains.relayOff.relay.status = RELAY_OFF;
  mains.relayOff.relay.set    = vFPOsetMainsOffImp;
  mains.relayOff.level        = 0U;
  mains.relayOff.timer.delay  = getValue( &timerMainsBreakerTripPulse );
  mains.relayOff.timer.id     = 0U;
  mains.relayOff.status       = RELAY_DELAY_IDLE;
  /*----------------------------------------------------------------------------*/
  for ( i=0U; i<MAINS_LINE_NUMBER; i++ )
  {
    mains.line[i].getVoltage = getCallback;
    mains.line[i].getFreq    = getCallback;
    mains.line[i].getCurrent = getCallback;
  }
  /*----------------------------------------------------------------------------*/
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vELECTROinit ( void )
{
  vELECTROdataInit();
  pElectroCommandQueue = xQueueCreateStatic( ELECTRO_COMMAND_QUEUE_LENGTH, sizeof( ELECTRO_COMMAND ), electroCommandBuffer, &xElectroCommandQueue );
  const osThreadAttr_t electroTask_attributes = {
    .name       = "electroTask",
    .priority   = ( osPriority_t ) ELECTRO_TASK_PRIORITY,
    .stack_size = ELECTRO_TASK_STACK_SIZE
  };
  electroHandle = osThreadNew( vELECTROtask, NULL, &electroTask_attributes );
  /*----------------------------------------------------------------------------*/
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
void vELECTROalarmStartDisable ( void )
{
  mains.lowVoltageAlarm.active          = 0U;
  mains.hightVoltageAlarm.active        = 0U;
  mains.lowFreqAlarm.active             = 0U;
  mains.hightFreqAlarm.active           = 0U;
  generator.lowVoltageAlarm.active      = 0U;
  generator.lowVoltagePreAlarm.active   = 0U;
  generator.hightVoltagePreAlarm.active = 0U;
  generator.hightVoltageAlarm.active    = 0U;
  generator.lowFreqAlarm.active         = 0U;
  generator.lowFreqPreAlarm.active      = 0U;
  generator.hightFreqPreAlarm.active    = 0U;
  generator.hightFreqAlarm.active       = 0U;
  return;
}

void vELECTROalarmStartToIdle ( void )
{
  mains.lowVoltageAlarm.active          = 1U;
  mains.hightVoltageAlarm.active        = 1U;
  mains.lowFreqAlarm.active             = 1U;
  mains.hightFreqAlarm.active           = 1U;
  generator.lowVoltageAlarm.active      = 1U;
  generator.lowVoltagePreAlarm.active   = 1U;
  generator.hightVoltagePreAlarm.active = 1U;
  generator.hightVoltageAlarm.active    = 1U;
  generator.hightFreqPreAlarm.active    = 1U;
  generator.hightFreqAlarm.active       = 1U;
  return;
}

void vELECTROalarmIdleDisable ( void )
{
  generator.lowFreqAlarm.active    = 0U;
  generator.lowFreqPreAlarm.active = 0U;
  return;
}

void vELECTROalarmIdleEnable ( void )
{
  generator.lowFreqAlarm.active    = 1U;
  generator.lowFreqPreAlarm.active = 1U;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vELECTROtask ( void const* argument )
{
  fix16_t         valOff      = 0U;
  fix16_t         valOn       = 0U;
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
    /**/
    vGENERATORprocess();
    vMAINSprocess();
    if ( electro.state == ELECTRO_PROC_STATUS_IDLE )
    {
      if ( xQueueReceive( pElectroCommandQueue, &inputCmd, 0U ) == pdPASS )
      {
        electro.cmd = inputCmd;
      }
    }
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
              vRELAYimpulseReset( &generator.relayOff );
              vRELAYimpulseReset( &mains.relayOn );
              generator.relay.set( RELAY_OFF );
              valOff = F16( 1U );
              electro.state = ELECTRO_PROC_STATUS_DISCONNECT;
              break;
            case ELECTRO_PROC_STATUS_DISCONNECT:
              if ( ( generator.relayOff.status == RELAY_IMPULSE_DONE ) || ( generator.relayOff.relay.enb == 0U ) )
              {
                vLOGICstartTimer( &electro.timer );
                electro.state   = ELECTRO_PROC_STATUS_CONNECT;
              }
              break;
            case ELECTRO_PROC_STATUS_CONNECT:
              if ( uLOGICisTimer( electro.timer ) > 0U )
              {
                generator.state = ELECTRO_STATUS_IDLE;
                mains.relay.set( RELAY_ON );
                valOn = F16( 1U );
                electro.state = ELECTRO_PROC_STATUS_DONE;
              }
              break;
            case ELECTRO_PROC_STATUS_DONE:
              if ( ( mains.relayOn.status == RELAY_IMPULSE_DONE ) || ( mains.relayOn.relay.enb == 0U ) )
              {
                electro.state = ELECTRO_STATUS_IDLE;
                mains.state   = ELECTRO_STATUS_LOAD;
                valOff = 0U;
                valOn  = 0U;
              }
              break;
            default:
              electro.state = ELECTRO_STATUS_IDLE;
              vRELAYimpulseReset( &generator.relayOff );
              vRELAYimpulseReset( &mains.relayOn );
              break;
          }
        }
        break;
      case ELECTRO_CMD_LOAD_GENERATOR:
        if ( mains.state == ELECTRO_STATUS_IDLE )
        {
          switch ( electro.state )
          {
            case ELECTRO_PROC_STATUS_IDLE:
              vRELAYimpulseReset( &generator.relayOn );
              vRELAYimpulseReset( &mains.relayOff );
              mains.relay.set( RELAY_ON );
              valOff = F16( 1U );
              vLOGICstartTimer( &electro.timer );
              electro.state = ELECTRO_PROC_STATUS_DISCONNECT;
              break;
            case ELECTRO_PROC_STATUS_DISCONNECT:
              if ( ( mains.relayOff.status == RELAY_IMPULSE_DONE ) || ( mains.relayOff.relay.enb == 0U ) )
              {
                vLOGICstartTimer( &electro.timer );
                electro.state = ELECTRO_PROC_STATUS_CONNECT;
              }
              break;
            case ELECTRO_PROC_STATUS_CONNECT:
              if ( uLOGICisTimer( electro.timer ) > 0U )
              {
                mains.state   = ELECTRO_STATUS_IDLE;
                generator.relay.set( RELAY_ON );
                valOn = F16( 1U );
                electro.state = ELECTRO_PROC_STATUS_DONE;
              }
              break;
            case ELECTRO_PROC_STATUS_DONE:
              if ( ( generator.relayOn.status == RELAY_IMPULSE_DONE ) || ( generator.relayOn.relay.enb == 0U ) )
              {
                generator.state = ELECTRO_STATUS_LOAD;
                electro.state   = ELECTRO_STATUS_IDLE;
                valOff = 0U;
                valOn  = 0U;
              }
              break;
            default:
              electro.state = ELECTRO_STATUS_IDLE;
              vRELAYimpulseReset( &generator.relayOn );
              vRELAYimpulseReset( &mains.relayOff );
              break;
          }
        }
        break;
      default:
        break;
    }
    vRELAYimpulseProcess( &generator.relayOff, valOff );
    vRELAYimpulseProcess( &mains.relayOn, valOn );
    vRELAYimpulseProcess( &generator.relayOn, valOn );
    vRELAYimpulseProcess( &mains.relayOff, valOff );
  }
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

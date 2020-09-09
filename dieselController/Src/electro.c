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
/*-------------------------------- Structures --------------------------------*/
static GENERATOR_TYPE      generator;
static MAINS_TYPE          mains;
static ELECTRO_SYSTEM_TYPE electro;
static osThreadId_t        electroHandle;
static StaticQueue_t       xElectroCommandQueue;
static QueueHandle_t       pElectroCommandQueue;
/*--------------------------------- Constant ---------------------------------*/
/*-------------------------------- Variables ---------------------------------*/
static ELECTRO_COMMAND electroCommandBuffer[ELECTRO_COMMAND_QUEUE_LENGTH];
/*-------------------------------- Functions ---------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/

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
  return fix16_div( fix16_mul( CUTOUT_PROTECTION_TRIPPING_CURVE, F16( 0.14f ) ),
                    fix16_sub( fix16_pow( fix16_div( input, setting ), CUTOUT_POWER ), F16( 1U ) ) );
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
void vELECTROalarmCheck ( ALARM_TYPE* alarm, fix16_t* value, uint8_t length, QueueHandle_t queue )
{
  if ( electro.scheme == ELECTRO_SCHEME_SINGLE_PHASE )
  {
    vALARMcheck( alarm, value[0U], queue );
  }
  else
  {
    if ( alarm->type == ALARM_LEVEL_LOW )
    {
      vALARMcheck( alarm, fELECTROgetMin( value, length ), queue );
    }
    else
    {
      vALARMcheck( alarm, fELECTROgetMax( value, length ), queue );
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
void vELECTROcurrentAlarmProcess ( fix16_t current, CURRENT_ALARM_TYPE* alarm, QueueHandle_t queue )
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
      break;
    /*--------------------------------------------------------------------------------*/
    case ELECTRO_CURRENT_STATUS_OVER_TRIG:
      if ( current >= alarm->cutout.current )
      {
        alarm->over.delay = fIDMTcalcTemp( current, alarm->over.current );
        if ( alarm->tim->Instance->CNT >= uSecToTic( alarm->over.delay ) )
        {
          xQueueSend( queue, &alarm->over.event, portMAX_DELAY );
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
          xQueueSend( queue, &alarm->cutout.event, portMAX_DELAY );
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
  fix16_t voltage[MAINS_LINE_NUMBER];
  fix16_t freq[MAINS_LINE_NUMBER];
  uint8_t i = 0U;

  for ( i=0U; i<MAINS_LINE_NUMBER; i++ )
  {
    voltage[i] = mains.line[i].getVoltage();
    freq[i]    = mains.line[i].getFreq();
  }
  vELECTROalarmCheck( &mains.lowVoltageAlarm, voltage, MAINS_LINE_NUMBER, pLOGICgetEventQueue() );
  if ( mains.lowVoltageAlarm.status == ALARM_STATUS_IDLE )
  {
    vELECTROalarmCheck( &mains.hightVoltageAlarm, voltage, MAINS_LINE_NUMBER, pLOGICgetEventQueue() );
  }
  vELECTROalarmCheck( &mains.lowFreqAlarm, freq, MAINS_LINE_NUMBER, pLOGICgetEventQueue() );
  if ( mains.lowFreqAlarm.status == ALARM_STATUS_IDLE )
  {
    vELECTROalarmCheck( &mains.hightFreqAlarm, freq, MAINS_LINE_NUMBER, pLOGICgetEventQueue() );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vGENERATORprocess ( void )
{
  fix16_t voltage[MAINS_LINE_NUMBER];
  fix16_t freq[MAINS_LINE_NUMBER];
  fix16_t current[MAINS_LINE_NUMBER];
  fix16_t power[MAINS_LINE_NUMBER];
  fix16_t maxCurrent = 0U;
  uint8_t i          = 0U;

  for ( i=0U; i<GENERATOR_LINE_NUMBER; i++ )
  {
    voltage[i] = generator.line[i].getVoltage();
    freq[i]    = generator.line[i].getFreq();
    current[i] = generator.line[i].getCurrent();
    power[i]   = fix16_mul( fix16_mul( voltage[i], current[i] ), generator.rating.cosFi );
  }
  maxCurrent = fELECTROgetMax( current, GENERATOR_LINE_NUMBER );
  vELECTROalarmCheck( &generator.lowVoltageAlarm, voltage, MAINS_LINE_NUMBER, pLOGICgetEventQueue() );
  if ( mains.lowVoltageAlarm.status == ALARM_STATUS_IDLE )
  {
    vELECTROalarmCheck( &generator.hightVoltageAlarm, voltage, MAINS_LINE_NUMBER, pLOGICgetEventQueue() );
  }
  vELECTROalarmCheck( &generator.lowFreqAlarm, freq, MAINS_LINE_NUMBER, pLOGICgetEventQueue() );
  if ( mains.lowFreqAlarm.status == ALARM_STATUS_IDLE )
  {
    vELECTROalarmCheck( &generator.hightFreqAlarm, freq, MAINS_LINE_NUMBER, pLOGICgetEventQueue() );
  }

  if ( electro.scheme != ELECTRO_SCHEME_SINGLE_PHASE )
  {
    vALARMcheck( &generator.phaseImbalanceAlarm, fELECTROcalcPhaseImbalance( current ), pLOGICgetEventQueue() );
  }

  vELECTROalarmCheck( &generator.overloadAlarm, power, MAINS_LINE_NUMBER, pLOGICgetEventQueue() );
  vELECTROcurrentAlarmProcess( maxCurrent, &generator.currentAlarm, pLOGICgetEventQueue() );

  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vELECTROinit ( TIM_HandleTypeDef* currentTIM )
{
  electro.scheme      = getBitMap( &genSetup, 1U );
  electro.state       = ELECTRO_PROC_STATUS_IDLE;
  electro.cmd         = ELECTRO_CMD_NONE;
  electro.timerID     = 0U;
  electro.switchDelay = getValue( &timerTransferDelay );

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
  generator.lowVoltageAlarm.delay        = 0U;
  generator.lowVoltageAlarm.timerID      = 0U;
  generator.lowVoltageAlarm.event.type   = EVENT_GENERATOR_LOW_VOLTAGE;
  generator.lowVoltageAlarm.event.action = ACTION_EMERGENCY_STOP;
  generator.lowVoltageAlarm.relax.enb    = 0U;
  generator.lowVoltageAlarm.status       = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  generator.lowVoltagePreAlarm.enb          = getBitMap( &genAlarms, 1U );
  generator.lowVoltagePreAlarm.active       = 0U;
  generator.lowVoltagePreAlarm.type         = ALARM_LEVEL_LOW;
  generator.lowVoltagePreAlarm.level        = getValue( &genUnderVoltagePreAlarmLevel );
  generator.lowVoltagePreAlarm.delay        = 0U;
  generator.lowVoltagePreAlarm.timerID      = 0U;
  generator.lowVoltagePreAlarm.event.type   = EVENT_GENERATOR_LOW_VOLTAGE;
  generator.lowVoltagePreAlarm.event.action = ACTION_WARNING;
  generator.lowVoltagePreAlarm.relax.enb    = 0U;
  generator.lowVoltagePreAlarm.status       = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  generator.hightVoltagePreAlarm.enb          = getBitMap( &genAlarms, 2U );
  generator.hightVoltagePreAlarm.active       = 0U;
  generator.hightVoltagePreAlarm.type         = ALARM_LEVEL_HIGHT;
  generator.hightVoltagePreAlarm.level        = getValue( &genOverVoltagePreAlarmLevel );
  generator.hightVoltagePreAlarm.delay        = 0U;
  generator.hightVoltagePreAlarm.timerID      = 0U;
  generator.hightVoltagePreAlarm.event.type   = EVENT_GENERATOR_HIGHT_VOLTAGE;
  generator.hightVoltagePreAlarm.event.action = ACTION_WARNING;
  generator.hightVoltagePreAlarm.relax.enb    = 0U;
  generator.hightVoltagePreAlarm.status       = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  generator.hightVoltageAlarm.enb          = 1U;
  generator.hightVoltageAlarm.active       = 0U;
  generator.hightVoltageAlarm.type         = ALARM_LEVEL_HIGHT;
  generator.hightVoltageAlarm.level        = getValue( &genOverVoltageAlarmLevel );
  generator.hightVoltageAlarm.delay        = 0U;
  generator.hightVoltageAlarm.timerID      = 0U;
  generator.hightVoltageAlarm.event.type   = EVENT_GENERATOR_HIGHT_VOLTAGE;
  generator.hightVoltageAlarm.event.action = ACTION_EMERGENCY_STOP;
  generator.hightVoltageAlarm.relax.enb    = 0U;
  generator.hightVoltageAlarm.status       = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  generator.lowFreqAlarm.enb          = getBitMap( &genAlarms, 3U );
  generator.lowFreqAlarm.active       = 0U;
  generator.lowFreqAlarm.type         = ALARM_LEVEL_LOW;
  generator.lowFreqAlarm.level        = getValue( &genUnderFrequencyAlarmLevel );
  generator.lowFreqAlarm.delay        = 0U;
  generator.lowFreqAlarm.timerID      = 0U;
  generator.lowFreqAlarm.event.type   = EVENT_GENERATOR_LOW_FREQUENCY;
  generator.lowFreqAlarm.event.action = ACTION_EMERGENCY_STOP;
  generator.lowFreqAlarm.relax.enb    = 0U;
  generator.lowFreqAlarm.status       = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  generator.lowFreqPreAlarm.enb          = getBitMap( &genAlarms, 4U );
  generator.lowFreqPreAlarm.active       = 0U;
  generator.lowFreqPreAlarm.type         = ALARM_LEVEL_LOW;
  generator.lowFreqPreAlarm.level        = getValue( &genUnderFrequencyPreAlarmLevel );
  generator.lowFreqPreAlarm.delay        = 0U;
  generator.lowFreqPreAlarm.timerID      = 0U;
  generator.lowFreqPreAlarm.event.type   = EVENT_GENERATOR_LOW_FREQUENCY;
  generator.lowFreqPreAlarm.event.action = ACTION_WARNING;
  generator.lowFreqPreAlarm.relax.enb    = 0U;
  generator.lowFreqPreAlarm.status       = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  generator.hightFreqPreAlarm.enb          = getBitMap( &genAlarms, 5U );
  generator.hightFreqPreAlarm.active       = 0U;
  generator.hightFreqPreAlarm.type         = ALARM_LEVEL_HIGHT;
  generator.hightFreqPreAlarm.level        = getValue( &genOverFrequencyPreAlarmLevel );
  generator.hightFreqPreAlarm.delay        = 0U;
  generator.hightFreqPreAlarm.timerID      = 0U;
  generator.hightFreqPreAlarm.event.type   = EVENT_GENERATOR_HIGHT_FREQUENCY;
  generator.hightFreqPreAlarm.event.action = ACTION_WARNING;
  generator.hightFreqPreAlarm.relax.enb    = 0U;
  generator.hightFreqPreAlarm.status       = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  generator.hightFreqAlarm.enb          = getBitMap( &genAlarms, 6U );
  generator.hightFreqAlarm.active       = 0U;
  generator.hightFreqAlarm.type         = ALARM_LEVEL_HIGHT;
  generator.hightFreqAlarm.level        = getValue( &genOverFrequencyAlarmLevel );
  generator.hightFreqAlarm.delay        = 0U;
  generator.hightFreqAlarm.timerID      = 0U;
  generator.hightFreqAlarm.event.type   = EVENT_GENERATOR_HIGHT_FREQUENCY;;
  generator.hightFreqAlarm.event.action = ACTION_EMERGENCY_STOP;
  generator.hightFreqAlarm.relax.enb    = 0U;
  generator.hightFreqAlarm.status       = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  generator.phaseImbalanceAlarm.enb          = getBitMap( &genAlarms, 8U );
  generator.phaseImbalanceAlarm.active       = 0U;
  generator.phaseImbalanceAlarm.type         = ALARM_LEVEL_HIGHT;
  generator.phaseImbalanceAlarm.level        = getValue( &genCurrentOverPhaseImbalanceLevel );
  generator.phaseImbalanceAlarm.delay        = getValue( &genCurrentOverPhaseImbalanceDelay );
  generator.phaseImbalanceAlarm.timerID      = 0U;
  generator.phaseImbalanceAlarm.event.type   = EVENT_PHASE_IMBALANCE;
  if ( getBitMap( &genAlarms, 11U ) == 0U )
  {
    generator.phaseImbalanceAlarm.event.action = ACTION_LOAD_SHUTDOWN;
  }
  else
  {
    generator.phaseImbalanceAlarm.event.action = ACTION_WARNING;
  }
  generator.phaseImbalanceAlarm.relax.enb    = 0U;
  generator.phaseImbalanceAlarm.status       = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  generator.overloadAlarm.enb        = getBitMap( &genAlarms, 7U );
  generator.overloadAlarm.active     = 0U;
  generator.overloadAlarm.type       = ALARM_LEVEL_HIGHT;
  generator.overloadAlarm.level      = getValue( &genCurrentOverloadProtectionLevel );
  generator.overloadAlarm.delay      = getValue( &genCurrentOverloadProtectionDelay );
  generator.overloadAlarm.timerID    = 0U;
  generator.overloadAlarm.event.type = EVENT_OVER_POWER;
  if ( getBitMap( &genAlarms, 10U ) == 0U )
  {
    generator.overloadAlarm.event.action = ACTION_LOAD_SHUTDOWN;
  }
  else
  {
    generator.overloadAlarm.event.action = ACTION_WARNING;
  }
  generator.overloadAlarm.relax.enb  = 0U;
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
  generator.currentAlarm.tim                 = currentTIM;
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
  generator.relayOn.delay        = getValue( &timerGenBreakerClosePulse );
  generator.relayOn.timerID      = 0U;
  generator.relayOn.status       = RELAY_DELAY_IDLE;
  /*----------------------------------------------------------------------------*/
  generator.relayOff.active       = 0U;
  generator.relayOff.relay.enb    = uFPOisEnable( FPO_FUN_TURN_OFF_GEN_IMPULSE );
  generator.relayOff.relay.status = RELAY_OFF;
  generator.relayOff.relay.set    = vFPOsetGenOffImp;
  generator.relayOff.level        = 0U;
  generator.relayOff.delay        = getValue( &timerGenBreakerTripPulse );
  generator.relayOff.timerID      = 0U;
  generator.relayOff.status       = RELAY_DELAY_IDLE;
  /*----------------------------------------------------------------------------*/

  /*----------------------------------------------------------------------------*/
  /*----------------------------------------------------------------------------*/
  /*----------------------------------------------------------------------------*/
  mains.enb = getBitMap( &mainsSetup, 0U );

  mains.lowVoltageAlarm.enb                = getBitMap( &mainsAlarms, 0U );
  mains.lowVoltageAlarm.active             = 0U;
  mains.lowVoltageAlarm.type               = ALARM_LEVEL_LOW;
  mains.lowVoltageAlarm.level              = getValue( &mainsUnderVoltageAlarmLevel );
  mains.lowVoltageAlarm.delay              = 0U;
  mains.lowVoltageAlarm.timerID            = 0U;
  mains.lowVoltageAlarm.event.type         = EVENT_MAINS_LOW_VOLTAGE;
  mains.lowVoltageAlarm.event.action       = ACTION_LOAD_GENERATOR;
  mains.lowVoltageAlarm.relax.enb          = 1U;
  mains.lowVoltageAlarm.relax.event.type   = EVENT_MAINS_VOLTAGE_RELAX;
  mains.lowVoltageAlarm.relax.event.action = ACTION_LOAD_MAINS;
  mains.lowVoltageAlarm.status             = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  mains.hightVoltageAlarm.enb                = getBitMap( &mainsAlarms, 1U );
  mains.hightVoltageAlarm.active             = 0U;
  mains.hightVoltageAlarm.type               = ALARM_LEVEL_HIGHT;
  mains.hightVoltageAlarm.level              = getValue( &mainsOverVoltageAlarmLevel );
  mains.hightVoltageAlarm.delay              = 0U;
  mains.hightVoltageAlarm.timerID            = 0U;
  mains.hightVoltageAlarm.event.type         = EVENT_MAINS_HIGHT_VOLTAGE;
  mains.hightVoltageAlarm.event.action       = ACTION_LOAD_GENERATOR;
  mains.hightVoltageAlarm.relax.enb          = 1U;
  mains.hightVoltageAlarm.relax.event.type   = EVENT_MAINS_VOLTAGE_RELAX;
  mains.hightVoltageAlarm.relax.event.action = ACTION_LOAD_MAINS;
  mains.hightVoltageAlarm.status             = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  mains.lowFreqAlarm.enb                = getBitMap( &mainsAlarms, 2U );
  mains.lowFreqAlarm.active             = 0U;
  mains.lowFreqAlarm.type               = ALARM_LEVEL_LOW;
  mains.lowFreqAlarm.level              = getValue( &mainsUnderFrequencyAlarmLevel );
  mains.lowFreqAlarm.delay              = 0U;
  mains.lowFreqAlarm.timerID            = 0U;
  mains.lowFreqAlarm.event.type         = EVENT_MAINS_LOW_FREQUENCY;
  mains.lowFreqAlarm.event.action       = ACTION_LOAD_GENERATOR;
  mains.lowFreqAlarm.relax.enb          = 1U;
  mains.lowFreqAlarm.relax.event.type   = EVENT_MAINS_FREQUENCY_RELAX;
  mains.lowFreqAlarm.relax.event.action = ACTION_LOAD_MAINS;
  mains.lowFreqAlarm.status             = ALARM_STATUS_IDLE;
  /*----------------------------------------------------------------------------*/
  mains.hightFreqAlarm.enb                = getBitMap( &mainsAlarms, 3U );
  mains.hightFreqAlarm.active             = 0U;
  mains.hightFreqAlarm.type               = ALARM_LEVEL_HIGHT;
  mains.hightFreqAlarm.level              = getValue( &mainsOverFrequencyAlarmLevel );
  mains.hightFreqAlarm.delay              = 0U;
  mains.hightFreqAlarm.timerID            = 0U;
  mains.hightFreqAlarm.event.type         = EVENT_MAINS_HIGHT_FREQUENCY;
  mains.hightFreqAlarm.event.action       = ACTION_LOAD_GENERATOR;
  mains.hightFreqAlarm.relax.enb          = 1U;
  mains.hightFreqAlarm.relax.event.type   = EVENT_MAINS_FREQUENCY_RELAX;
  mains.hightFreqAlarm.relax.event.action = ACTION_LOAD_MAINS;
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
  mains.relayOn.delay        = getValue( &timerMainsBreakerClosePulse );
  mains.relayOn.timerID      = 0U;
  mains.relayOn.status       = RELAY_DELAY_IDLE;
  /*----------------------------------------------------------------------------*/
  mains.relayOff.relay.enb    = uFPOisEnable( FPO_FUN_TURN_OFF_MAINS_IMPULSE );
  mains.relayOff.relay.status = RELAY_OFF;
  mains.relayOff.relay.set    = vFPOsetMainsOffImp;
  mains.relayOff.level        = 0U;
  mains.relayOff.delay        = getValue( &timerMainsBreakerTripPulse );
  mains.relayOff.timerID      = 0U;
  mains.relayOff.status       = RELAY_DELAY_IDLE;
  /*----------------------------------------------------------------------------*/
  pElectroCommandQueue = xQueueCreateStatic( ELECTRO_COMMAND_QUEUE_LENGTH, sizeof( ELECTRO_COMMAND ), electroCommandBuffer, &xElectroCommandQueue );
  const osThreadAttr_t electroTask_attributes = {
    .name       = "electroTask",
    .priority   = ( osPriority_t ) osPriorityLow,
    .stack_size = 1024U
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
  fix16_t         valOff   = 0U;
  fix16_t         valOn    = 0U;
  ELECTRO_COMMAND inputCmd = ELECTRO_CMD_NONE;
  for(;;)
  {
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
              if ( ( generator.relayOff.status == RELAY_DELAY_DONE ) || ( generator.relayOff.relay.enb == 0U ) )
              {
                vLOGICstartTimer( electro.switchDelay, &electro.timerID );
                electro.state   = ELECTRO_PROC_STATUS_CONNECT;
              }
              break;
            case ELECTRO_PROC_STATUS_CONNECT:
              if ( uLOGICisTimer( electro.timerID ) )
              {
                generator.state = ELECTRO_STATUS_IDLE;
                mains.relay.set( RELAY_ON );
                valOn = F16( 1U );
                electro.state = ELECTRO_PROC_STATUS_DONE;
              }
              break;
            case ELECTRO_PROC_STATUS_DONE:
              if ( ( mains.relayOn.status == RELAY_DELAY_DONE ) || ( mains.relayOn.relay.enb == 0U ) )
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
              vLOGICstartTimer( electro.switchDelay, &electro.timerID );
              electro.state = ELECTRO_PROC_STATUS_DISCONNECT;
              break;
            case ELECTRO_PROC_STATUS_DISCONNECT:
              if ( ( mains.relayOff.status == RELAY_DELAY_DONE ) || ( mains.relayOff.relay.enb == 0U ) )
              {
                vLOGICstartTimer( electro.switchDelay, &electro.timerID );
                electro.state = ELECTRO_PROC_STATUS_CONNECT;
              }
              break;
            case ELECTRO_PROC_STATUS_CONNECT:
              if ( uLOGICisTimer( electro.timerID ) )
              {
                mains.state   = ELECTRO_STATUS_IDLE;
                generator.relay.set( RELAY_ON );
                valOn = F16( 1U );
                electro.state = ELECTRO_PROC_STATUS_DONE;
              }
              break;
            case ELECTRO_PROC_STATUS_DONE:
              if ( ( generator.relayOn.status == RELAY_DELAY_DONE ) || ( generator.relayOn.relay.enb == 0U ) )
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

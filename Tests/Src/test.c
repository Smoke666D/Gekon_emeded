/*
 * test.c
 *
 *  Created on: 23 мар. 2022 г.
 *      Author: 79110
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "test.h"
#include "string.h"
#include "stdlib.h"
#include "fix16.h"
#include "fpi.h"
#include "fpo.h"
#include "electro.h"
#include "journal.h"
#include "engine.h"
#include "RTC.h"
#include "keyboard.h"
/*------------------------- Define ------------------------------------------------------------------*/
/*----------------------- Structures ----------------------------------------------------------------*/
/*----------------------- Constant ------------------------------------------------------------------*/
static const char* commandStrings[TEST_COMMANDS_NUMBER] = {
  TEST_SET_COMMAND_STR,
  TEST_GET_COMMAND_STR
};
static const char* targetStrings[TEST_TARGETS_NUMBER] = {
  TEST_TARGET_DIN_STR,
  TEST_TARGET_DOUT_STR,
  TEST_TARGET_TIME_STR,
  TEST_TARGET_OIL_STR,
  TEST_TARGET_COOLANT_STR,
  TEST_TARGET_FUEL_STR,
  TEST_TARGET_BAT_STR,
  TEST_TARGET_CHARG_STR,
  TEST_TARGET_GEN_STR,
  TEST_TARGET_MAINS_STR,
  TEST_TARGET_CUR_STR,
  TEST_TARGET_FREQ_STR,
  TEST_TARGET_SPEED_STR,
  TEST_TARGET_SWITCH_STR,
  TEST_TARGET_LED_STR
};
/*----------------------- Variables -----------------------------------------------------------------*/
/*----------------------- Functions -----------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PRIVATE -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
void eTESTparseString ( const char* str, TEST_TYPE* message )
{
  uint8_t i   = 0U;
  char*   p   = NULL;
  char*   sub = NULL;
  message->cmd      = TEST_COMMAND_NO;
  message->target   = TEST_TARGET_NO;
  message->data     = 0U;
  message->dataFlag = 0U;
  for ( i=0U; i<TEST_COMMANDS_NUMBER; i++ )
  {
    p = strstr( str, commandStrings[i] );
    if ( ( p > 0U ) && ( ( ( p + 1U ) == ' ' ) || ( ( p + 1U ) == 0U ) ) )
    {
      message->cmd = i + 1U;
      break;
    }
  }
  if ( ( message->cmd != TEST_COMMAND_NO ) && ( ( p + 1U ) == ' ' ) )
  {
    sub = p + 1U;
    for ( i=0U; i<TEST_TARGETS_NUMBER; i++ )
    {
      p = strstr( sub, targetStrings[i] );
      if ( ( p != NULL ) && ( ( ( p + 1U ) == ' ' ) || ( ( p + 1U ) == 0U ) ) )
      {
        message->target = i + 1U;
        break;
      }
    }
    if ( ( message->target != TEST_TARGET_NO ) && ( ( p + 1U ) == ' ' ) )
    {
      message->dataFlag = 1U;
      message->data     = atoi( p + 1U );
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vTESTtimeToStr ( RTC_TIME* time, char* buf )
{
  char sub[5U] = { 0U };
  ( void )itoa( time->day, sub, 10U );
  ( void )strcat( buf, sub );
  ( void )strcat( buf, "." );
  ( void )itoa( time->month, sub, 10U );
  ( void )strcat( buf, sub );
  ( void )strcat( buf, "." );
  ( void )itoa( time->year, sub, 10U );
  ( void )strcat( buf, sub );
  ( void )strcat( buf, " " );
  ( void )itoa( time->hour, sub, 10U );
  ( void )strcat( buf, sub );
  ( void )strcat( buf, ":" );
  ( void )itoa( time->min, sub, 10U );
  ( void )strcat( buf, sub );
  ( void )strcat( buf, ":" );
  ( void )itoa( time->sec, sub, 10U );
  ( void )strcat( buf, sub );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vTESTstrToTime ( RTC_TIME* time, char* buf )
{
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void status_to_string ( TEST_STATUS status, char* buf )
{
  switch ( status )
  {
    case TEST_STATUS_OK:
      buf = TEST_ERROR_OK_STR;
      break;
    case TEST_STATUS_ERROR_COMMAND:
      buf = TEST_ERROR_COMMAND_STR;
      break;
    case TEST_STATUS_ERROR_TARGET:
      buf = TEST_ERROR_TARGET_STR;
      break;
    case TEST_STATUS_ERROR_DATA:
      buf = TEST_ERROR_DATA_STR;
      break;
    case TEST_STATUS_ERROR_EXECUTING:
      buf = TEST_ERROR_EXECUTING_STR;
      break;
    default:
      buf = TEST_ERROR_UNKNOWN;
      break;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void dio_to_str ( uint8_t state, char* buf )
{
  if ( state > 0U )
  {
    buf = TEST_DIO_ON_STR;
  }
  else
  {
    buf = TEST_DIO_OFF_STR;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLIC --------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
TEST_STATUS vTESTprocess ( const char* str )
{
  TEST_STATUS res     = TEST_STATUS_OK;
  TEST_TYPE   message = { 0U };
  RTC_TIME    time    = { 0U };
  eTESTparseString( str, &message );
  switch ( message.cmd )
  {
    case TEST_COMMAND_SET:
      switch ( message.target )
      {
        case TEST_TARGET_DOUT:
          if ( ( message.data < FPO_NUMBER ) && ( message.dataFlag > 0U ) )
          {
            status_to_string( res, message.out );
          }
          else
          {
            res = TEST_STATUS_ERROR_DATA;
          }
          break;
        case TEST_TARGET_TIME:
          // !!!!!!!!Encoding time
          if ( eRTCsetTime( &time ) != RTC_OK )
          {
            res = TEST_STATUS_ERROR_EXECUTING;
          }
          status_to_string( res, message.out );
          break;
        case TEST_TARGET_LED:
          break;
        default:
          res = TEST_STATUS_ERROR_TARGET;
          break;
      }
      break;
    case TEST_COMMAND_GET:
      switch ( message.target )
      {
        case TEST_TARGET_DIN:
          if ( ( message.data < FPI_NUMBER ) && ( message.dataFlag > 0U ) )
          {
            dio_to_str( ( ( uFPIgetData() >> message.data ) & 0x01U ), message.out );
          }
          else
          {
            res = TEST_STATUS_ERROR_DATA;
          }
          break;
        case TEST_TARGET_DOUT:
          if ( ( message.data < FPO_NUMBER ) && ( message.dataFlag > 0U ) )
          {
            dio_to_str( ( ( uFPOgetData() >> message.data ) & 0x01U ), message.out );
          }
          else
          {
            res = TEST_STATUS_ERROR_DATA;
          }
          break;
        case TEST_TARGET_TIME:
          if ( eRTCgetTime( &time ) == RTC_OK )
          {
            vTESTtimeToStr( &time, message.out );
          }
          else
          {
            res = TEST_STATUS_ERROR_EXECUTING;
          }
          break;
        case TEST_TARGET_OIL:
          fix16_to_str( fENGINEgetOilPressure(), message.out, TEST_FIX_DECIMALS );
          break;
        case TEST_TARGET_COOL:
          fix16_to_str( fENGINEgetCoolantTemp(), message.out, TEST_FIX_DECIMALS );
          break;
        case TEST_TARGET_FUEL:
          fix16_to_str( fENGINEgetFuelLevel(), message.out, TEST_FIX_DECIMALS );
          break;
        case TEST_TARGET_BAT:
          fix16_to_str( fENGINEgetBatteryVoltage(), message.out, TEST_FIX_DECIMALS );
          break;
        case TEST_TARGET_CHARG:
          fix16_to_str( fENGINEgetChargerVoltage(), message.out, TEST_FIX_DECIMALS );
          break;
        case TEST_TARGET_GENERATOR:
          if ( ( message.data < MAINS_LINE_NUMBER ) && ( message.dataFlag > 0U ) )
          {
            fix16_to_str( fELECTROgetGeneratorVoltage( ( uint8_t )message.data ), message.out, TEST_FIX_DECIMALS );
          }
          else
          {
            res = TEST_STATUS_ERROR_DATA;
          }
          break;
        case TEST_TARGET_MAINS:
          if ( ( message.data < MAINS_LINE_NUMBER ) && ( message.dataFlag > 0U ) )
          {
            fix16_to_str( fELECTROgetMainsVoltage( ( uint8_t )message.data ), message.out, TEST_FIX_DECIMALS );
          }
          else
          {
            res = TEST_STATUS_ERROR_DATA;
          }
          break;
        case TEST_TARGET_CURRENT:
          if ( ( message.data < MAINS_LINE_NUMBER ) && ( message.dataFlag > 0U ) )
          {
            fix16_to_str( fELECTROgetCurrent( ( uint8_t )message.data ), message.out, TEST_FIX_DECIMALS );
          }
          else
          {
            res = TEST_STATUS_ERROR_DATA;
          }
          break;
        case TEST_TARGET_FREQ:
          if ( message.dataFlag > 0U )
          {
            switch ( message.data )
            {
              case TEST_FREQ_CHANNELS_MAINS:
                fix16_to_str( fELECTROgetMainsFreq(), message.out, TEST_FIX_DECIMALS );
                break;
              case TEST_FREQ_CHANNELS_GENERATOR:
                fix16_to_str( fELECTROgetGeneratorFreq(), message.out, TEST_FIX_DECIMALS );
                break;
              default:
                res = TEST_STATUS_ERROR_DATA;
                break;
            }
          }
        case TEST_TARGET_SPEED:
          fix16_to_str( fENGINEgetSpeed(), message.out, TEST_FIX_DECIMALS );
          break;
        case TEST_TARGET_SW:
          if ( ( message.data < KEYBOARD_COUNT ) && ( message.dataFlag > 0U ) )
          {
            dio_to_str( uKEYgetState( message.data ), message.out );
          }
          else
          {
            res = TEST_STATUS_ERROR_DATA;
          }
          break;
        default:
          res = TEST_STATUS_ERROR_TARGET;
          break;
      }
      if ( res != TEST_STATUS_OK )
      {
        status_to_string( res, message.out );
      }
      break;
    default:
      res = TEST_STATUS_ERROR_COMMAND;
      status_to_string( res, message.out );
      break;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

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
#include "controller.h"
#include "server.h"
#include "config.h"
/*------------------------- Define ------------------------------------------------------------------*/
/*----------------------- Structures ----------------------------------------------------------------*/
/*----------------------- Constant ------------------------------------------------------------------*/
static const char* commandStrings[TEST_COMMANDS_NUMBER] = {
  TEST_SET_COMMAND_STR,
  TEST_RESET_COMMAND_STR,
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
  TEST_TARGET_LED_STR,
  TEST_TARGET_STORAGE_STR,
  TEST_TARGET_ID_STR,
  TEST_TARGET_IP_STR,
  TEST_TARGET_MAC_STR,
  TEST_TARGET_VERSION_STR
};
/*----------------------- Variables -----------------------------------------------------------------*/
static TEST_TYPE message = { 0U };
/*----------------------- Functions -----------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PRIVATE -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
void eTESTparseString ( const char* str, TEST_TYPE* message )
{
  uint8_t i      = 0U;
  char*   p      = NULL;
  char*   cmdEnd = NULL;
  char*   trgEnd = NULL;
  message->cmd      = TEST_COMMAND_NO;
  message->target   = TEST_TARGET_NO;
  message->data     = 0U;
  message->dataFlag = 0U;
  for ( i=0U; i<TEST_COMMANDS_NUMBER; i++ )
  {
    p = strstr( str, commandStrings[i] );
    if ( p > 0U )
    {
      cmdEnd = &p[strlen( commandStrings[i] ) + 1U];
      if ( ( cmdEnd[0U] == ' ' ) || ( cmdEnd[0U] == 0U ) )
      {
        message->cmd = i + 1U;
        break;
      }
    }
  }
  if ( ( message->cmd != TEST_COMMAND_NO ) && ( cmdEnd[0U] == ' ' ) )
  {
    for ( i=0U; i<TEST_TARGETS_NUMBER; i++ )
    {
      p = strstr( cmdEnd, targetStrings[i] );
      if ( p > 0U )
      {
        trgEnd = &p[strlen( targetStrings[i] ) + 1U];
        if  ( ( trgEnd[0U] == ' ' ) || ( trgEnd[0U] == 0U ) )
        {
          message->target = i + 1U;
          break;
        }
      }
    }
    if ( ( message->target != TEST_TARGET_NO ) && ( trgEnd[0U] == ' ' ) )
    {
      message->dataFlag = 1U;
      if ( ( message->cmd == TEST_COMMAND_SET ) && ( message->target == TEST_TARGET_TIME ) )
      {
        strcpy( message->out, &trgEnd[1U] );
      }
      else
      {
        message->data = atoi( &trgEnd[1U] );
      }
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
  ( void )strcat( buf, "." );
  ( void )itoa( time->hour, sub, 10U );
  ( void )strcat( buf, sub );
  ( void )strcat( buf, "." );
  ( void )itoa( time->min, sub, 10U );
  ( void )strcat( buf, sub );
  ( void )strcat( buf, "." );
  ( void )itoa( time->sec, sub, 10U );
  ( void )strcat( buf, sub );
  ( void )strcat( buf, "." );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uTESTparseTimeFild ( char* pStr, uint8_t* output )
{
  uint8_t res     = 1U;
  char    sub[5U] = { 0U };
  char*   pEnd = NULL;
  pEnd = strchr( pStr, '.' );
  if ( pEnd != NULL )
  {
    ( void )memcpy( sub, pStr, ( pEnd - pStr ) );
    *output = ( uint8_t )atoi( sub );
    pStr    = pEnd + 1U;
  }
  else
  {
    res = 0U;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
TEST_STATUS vTESTstrToTime ( RTC_TIME* time, char* buf )
{
  TEST_STATUS res  = TEST_STATUS_ERROR_DATA;
  char*       pStr = buf;
  if ( uTESTparseTimeFild( pStr, &time->day ) > 0U )
  {
    if ( uTESTparseTimeFild( pStr, &time->month ) > 0U )
    {
      if ( uTESTparseTimeFild( pStr, &time->year ) > 0U )
      {
        if ( uTESTparseTimeFild( pStr, &time->hour ) > 0U )
        {
          if ( uTESTparseTimeFild( pStr, &time->min ) > 0U )
          {
            if ( uTESTparseTimeFild( pStr, &time->sec ) > 0U )
            {
              res = TEST_STATUS_OK;
            }
          }
        }
      }
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
void vTESTstatusToString ( TEST_STATUS status, char* buf )
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
void vTESTdioToStr ( uint8_t state, char* buf )
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
void vTESThexToStr ( uint8_t* data, uint8_t length, char* buf )
{
  uint8_t i = 0U;
  for ( i=0U; i<length; i++ )
  {
    ( void )itoa( data[i], &buf[2U * i], 16U );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vTESTversionToStr ( const uint16_t* version, char* buf )
{
  char sub[5U] = { 0U };
  ( void )itoa( ( uint8_t )( version[0U] ), sub, 10U );
  ( void )strcat( buf, sub );
  ( void )strcat( buf, "." );
  ( void )itoa( ( uint8_t )( version[1U] ), sub, 10U );
  ( void )strcat( buf, sub );
  ( void )strcat( buf, "." );
  ( void )itoa( ( uint8_t )( version[2U] ), sub, 10U );
  ( void )strcat( buf, sub );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLIC --------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
TEST_STATUS vTESTprocess ( const char* str )
{
  TEST_STATUS res  = TEST_STATUS_OK;
  RTC_TIME    time = { 0U };
  uint8_t     buf  = 0U;
  uint16_t    id[UNIQUE_ID_LENGTH] = { 0U };
  eTESTparseString( str, &message );
  switch ( message.cmd )
  {
    case TEST_COMMAND_SET:
      switch ( message.target )
      {
        case TEST_TARGET_DOUT:
          if ( ( message.dataFlag > 0U ) && ( message.data < FPO_NUMBER ) )
          {
            vFPOtest( message.data, 1U );
          }
          else
          {
            res = TEST_STATUS_ERROR_DATA;
          }
          break;
        case TEST_TARGET_TIME:
          if ( message.dataFlag > 0U )
          {
            res = vTESTstrToTime( &time, message.out );
            if ( res == TEST_STATUS_OK )
            {
              if ( eRTCsetTime( &time ) != RTC_OK )
              {
                res = TEST_STATUS_ERROR_EXECUTING;
              }
            }
          }
          break;
        case TEST_TARGET_LED:
          if ( message.dataFlag )
          {
            vCONTROLLERsetLed( message.data, 1U );
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
      vTESTstatusToString( res, message.out );
      break;
    case TEST_COMMAND_RESET:
      switch ( message.target )
      {
        case TEST_TARGET_DOUT:
          if ( ( message.dataFlag > 0U ) && ( message.data < FPO_NUMBER ) )
          {
            vFPOtest( message.data, 0U );
          }
          else
          {
            res = TEST_STATUS_ERROR_DATA;
          }
          break;
        case TEST_TARGET_LED:
          if ( message.dataFlag )
          {
            vCONTROLLERsetLed( message.data, 0U );
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
      vTESTstatusToString( res, message.out );
      break;
    case TEST_COMMAND_GET:
      switch ( message.target )
      {
        case TEST_TARGET_DIN:
          if ( ( message.data < FPI_NUMBER ) && ( message.dataFlag > 0U ) )
          {
            vTESTdioToStr( ( ( uFPIgetData() >> message.data ) & 0x01U ), message.out );
          }
          else
          {
            res = TEST_STATUS_ERROR_DATA;
          }
          break;
        case TEST_TARGET_DOUT:
          if ( ( message.data < FPO_NUMBER ) && ( message.dataFlag > 0U ) )
          {
            vTESTdioToStr( ( ( uFPOgetData() >> message.data ) & 0x01U ), message.out );
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
            vTESTdioToStr( uKEYgetState( message.data ), message.out );
          }
          else
          {
            res = TEST_STATUS_ERROR_DATA;
          }
          break;
        case TEST_TARGET_STORAGE:
          if ( eSTORAGEreadSR( &buf ) != EEPROM_OK )
          {
            res = TEST_STATUS_ERROR_EXECUTING;
          }
          break;
        case TEST_TARGET_ID:
          vSYSgetUniqueID16( id );
          vTESThexToStr( ( uint8_t* )id, UNIQUE_ID_LENGTH, message.out );
          break;
        case TEST_TARGET_IP:
          vSERVERgetStrIP( message.out );
          break;
        case TEST_TARGET_MAC:
          vTESThexToStr( pSERVERgetMAC(), MAC_ADR_LENGTH, message.out );
          break;
        case TEST_TARGET_VERSION:
          if ( message.dataFlag > 0U )
          {
            switch ( message.data )
            {
              case TEST_VERSION_BOOTLOADER:
                vTESTversionToStr( ( const uint16_t* )( versionController.value ), message.out );
                break;
              case TEST_VERSION_FIRMWARE:
                vTESTversionToStr( ( const uint16_t* )( versionFirmware.value ),   message.out );
                break;
              case TEST_VERSION_HARDWARE:
                vTESTversionToStr( ( const uint16_t* )( versionBootloader.value ), message.out );
                break;
              default:
                res = TEST_STATUS_ERROR_DATA;
                break;
            }
          }
          break;
        default:
          res = TEST_STATUS_ERROR_TARGET;
          break;
      }
      if ( res != TEST_STATUS_OK )
      {
        vTESTstatusToString( res, message.out );
      }
      break;
    default:
      res = TEST_STATUS_ERROR_COMMAND;
      vTESTstatusToString( res, message.out );
      break;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
char* cTESTgetOutput ( void )
{
  return message.out;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

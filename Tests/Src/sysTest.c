/*
 * test.c
 *
 *  Created on: 23 мар. 2022 г.
 *      Author: 79110
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "sysTest.h"
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
uint8_t uTESTparsingFilds ( const char* str, char** filds )
{
  uint8_t res = 0U;
  uint8_t i   = 0U;
  char*   p   = NULL;
  if ( str[0U] != 0U )
  {
    filds[0U] = ( char* )str;
    res++;
    for ( i=1U; i<TEST_FILDS_NUMBER; i++ )
    {
      p = strstr( filds[i - 1U], TEST_FILD_SEPORATOR );
      if ( p == 0U )
      {
        break;
      }
      else
      {
        filds[i] = p + 1U;
        res++;
      }
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uTESTparse ( const char* str, const char** data, uint8_t length )
{
  uint8_t res = 0U;
  uint8_t i   = 0U;
  for ( i=0U; i<length; i++ )
  {
    if ( strstr( str, data[i] ) == str )
    {
      res = i + 1U;
      break;
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
TEST_COMMAND eTESTparseCmd ( const char* str )
{
  return ( TEST_COMMAND )( uTESTparse( str, commandStrings, TEST_COMMANDS_NUMBER ) );
}
/*---------------------------------------------------------------------------------------------------*/
TEST_TARGET eTESTparseTarget ( const char* str )
{
  return ( TEST_TARGET )( uTESTparse( str, targetStrings, TEST_TARGETS_NUMBER ) );
}
/*---------------------------------------------------------------------------------------------------*/
void eTESTparseString ( const char* str, TEST_TYPE* message )
{
  char*   filds[TEST_FILDS_NUMBER] = { 0U };
  uint8_t counter = 0U;
  message->cmd      = TEST_COMMAND_NO;
  message->target   = TEST_TARGET_NO;
  message->data     = 0U;
  message->dataFlag = 0U;
  message->out[0U]  = 0U;
  counter = uTESTparsingFilds( str, filds );
  if ( counter > 0U )
  {
    message->cmd = eTESTparseCmd( ( const char* )filds[0U] );
    if ( ( counter > 1U ) && ( message->cmd != TEST_COMMAND_NO ) )
    {
      message->target = eTESTparseTarget( ( const char* )filds[1U] );
      if ( ( counter > 2U ) && ( message->target != TEST_TARGET_NO ) )
      {
        message->dataFlag = 1U;
        if ( ( message->cmd == TEST_COMMAND_SET ) && ( message->target == TEST_TARGET_TIME ) )
        {
          strcpy( message->out, filds[2U] );
        }
        else
        {
          message->data = atoi( filds[2U] );
        }
      }
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uTESTtimeToStr ( RTC_TIME* time, char* buf )
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
  ( void )strcat( buf, ".\n" );
  return strlen( buf );
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
uint8_t uTESTstatusToString ( TEST_STATUS status, char* buf )
{
  uint8_t res = 0U;
  switch ( status )
  {
    case TEST_STATUS_OK:
      ( void )strcpy( buf, TEST_ERROR_OK_STR );
      ( void )strcat( buf, "\n" );
      res = strlen( TEST_ERROR_OK_STR ) + 1U;
      break;
    case TEST_STATUS_ERROR_COMMAND:
      ( void )strcpy( buf, TEST_ERROR_COMMAND_STR );
      ( void )strcat( buf, "\n" );
      res = strlen( TEST_ERROR_COMMAND_STR ) + 1U;
      break;
    case TEST_STATUS_ERROR_TARGET:
      ( void )strcpy( buf, TEST_ERROR_TARGET_STR );
      ( void )strcat( buf, "\n" );
      res = strlen( TEST_ERROR_TARGET_STR ) + 1U;
      break;
    case TEST_STATUS_ERROR_DATA:
      ( void )strcpy( buf, TEST_ERROR_DATA_STR );
      ( void )strcat( buf, "\n" );
      res = strlen( TEST_ERROR_DATA_STR ) + 1U;
      break;
    case TEST_STATUS_ERROR_EXECUTING:
      ( void )strcpy( buf, TEST_ERROR_EXECUTING_STR );
      ( void )strcat( buf, "\n" );
      res = strlen( TEST_ERROR_EXECUTING_STR ) + 1U;
      break;
    default:
      ( void )strcpy( buf, TEST_ERROR_UNKNOWN );
      ( void )strcat( buf, "\n" );
      res = strlen( TEST_ERROR_UNKNOWN ) + 1U;
      break;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uTESTdioToStr ( uint8_t state, char* buf )
{
  uint8_t res = 0U;
  if ( state > 0U )
  {
    ( void )strcpy( buf, TEST_DIO_ON_STR );
    ( void )strcat( buf, "\n" );
    res = strlen( TEST_DIO_ON_STR ) + 1U;
  }
  else
  {
    ( void )strcpy( buf, TEST_DIO_OFF_STR);
    ( void )strcat( buf, "\n" );
    res = strlen( TEST_DIO_OFF_STR ) + 1U;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uTESThexToStr ( uint8_t* data, uint8_t length, char* buf )
{
  uint8_t i       = 0U;
  char    sub[3U] = { 0U };
  for ( i=0U; i<length; i++ )
  {
    ( void )itoa( data[i], sub, 16U );
   if ( sub[1U] == 0U )
   {
     sub[1U] = '0';
   }
   ( void )strcat( buf, sub );
   if ( i != ( length - 1U ) )
   {
     ( void )strcat( buf, "-" );
   }
  }
  ( void )strcat( buf, "\n" );
  return ( length * 2U + length );
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uTESTversionToStr ( const uint16_t* version, char* buf )
{
  char    sub[5U] = { 0U };
  ( void )itoa( ( uint8_t )( version[0U] ), sub, 10U );
  ( void )strcat( buf, sub );
  ( void )strcat( buf, "." );
  ( void )itoa( ( uint8_t )( version[1U] ), sub, 10U );
  ( void )strcat( buf, sub );
  ( void )strcat( buf, "." );
  ( void )itoa( ( uint8_t )( version[2U] ), sub, 10U );
  ( void )strcat( buf, sub );
  ( void )strcat( buf, "\n" );
  return strlen( buf );
}
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLIC --------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
TEST_STATUS vTESTprocess ( const char* str, uint8_t length )
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
      message.length = uTESTstatusToString( res, message.out );
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
      message.length = uTESTstatusToString( res, message.out );
      break;
    case TEST_COMMAND_GET:
      switch ( message.target )
      {
        case TEST_TARGET_DIN:
          if ( ( message.data < FPI_NUMBER ) && ( message.dataFlag > 0U ) )
          {
            message.length = uTESTdioToStr( ( ( uFPIgetData() >> message.data ) & 0x01U ), message.out );
          }
          else
          {
            res = TEST_STATUS_ERROR_DATA;
          }
          break;
        case TEST_TARGET_DOUT:
          if ( ( message.data < FPO_NUMBER ) && ( message.dataFlag > 0U ) )
          {
            message.length = uTESTdioToStr( ( ( uFPOgetData() >> message.data ) & 0x01U ), message.out );
          }
          else
          {
            res = TEST_STATUS_ERROR_DATA;
          }
          break;
        case TEST_TARGET_TIME:
          if ( eRTCgetTime( &time ) == RTC_OK )
          {
            message.length = uTESTtimeToStr( &time, message.out );
          }
          else
          {
            res = TEST_STATUS_ERROR_EXECUTING;
          }
          break;
        case TEST_TARGET_OIL:
          fix16_to_str( fENGINEgetOilPressure(), message.out, TEST_FIX_DECIMALS );
          ( void )strcat( message.out, "\n" );
          message.length = strlen( message.out );
          break;
        case TEST_TARGET_COOL:
          fix16_to_str( fENGINEgetCoolantTemp(), message.out, TEST_FIX_DECIMALS );
          ( void )strcat( message.out, "\n" );
          message.length = strlen( message.out );
          break;
        case TEST_TARGET_FUEL:
          fix16_to_str( fENGINEgetFuelLevel(), message.out, TEST_FIX_DECIMALS );
          ( void )strcat( message.out, "\n" );
          message.length = strlen( message.out );
          break;
        case TEST_TARGET_BAT:
          fix16_to_str( fENGINEgetBatteryVoltage(), message.out, TEST_FIX_DECIMALS );
          ( void )strcat( message.out, "\n" );
          message.length = strlen( message.out );
          break;
        case TEST_TARGET_CHARG:
          fix16_to_str( fENGINEgetChargerVoltage(), message.out, TEST_FIX_DECIMALS );
          ( void )strcat( message.out, "\n" );
          message.length = strlen( message.out );
          break;
        case TEST_TARGET_GENERATOR:
          if ( ( message.data < MAINS_LINE_NUMBER ) && ( message.dataFlag > 0U ) )
          {
            fix16_to_str( fELECTROgetGeneratorVoltage( ( uint8_t )message.data ), message.out, TEST_FIX_DECIMALS );
            ( void )strcat( message.out, "\n" );
            message.length = strlen( message.out );
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
            ( void )strcat( message.out, "\n" );
            message.length = strlen( message.out );
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
            ( void )strcat( message.out, "\n" );
            message.length = strlen( message.out );
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
                ( void )strcat( message.out, "\n" );
                message.length = strlen( message.out );
                break;
              case TEST_FREQ_CHANNELS_GENERATOR:
                fix16_to_str( fELECTROgetGeneratorFreq(), message.out, TEST_FIX_DECIMALS );
                ( void )strcat( message.out, "\n" );
                message.length = strlen( message.out );
                break;
              default:
                res = TEST_STATUS_ERROR_DATA;
                break;
            }
          }
          else
          {
            res = TEST_STATUS_ERROR_DATA;
          }
          break;
        case TEST_TARGET_SPEED:
          fix16_to_str( fENGINEgetSpeed(), message.out, TEST_FIX_DECIMALS );
          ( void )strcat( message.out, "\n" );
          message.length = strlen( message.out );
          break;
        case TEST_TARGET_SW:
          if ( ( message.data < KEYBOARD_COUNT ) && ( message.dataFlag > 0U ) )
          {
            message.length = uTESTdioToStr( uKEYgetState( message.data ), message.out );
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
          message.length = uTESTstatusToString( res, message.out );
          break;
        case TEST_TARGET_ID:
          vSYSgetUniqueID16( id );
          message.length = uTESThexToStr( ( uint8_t* )id, ( UNIQUE_ID_LENGTH * 2U ), message.out );
          break;
        case TEST_TARGET_IP:
          message.length = uSERVERgetStrIP( message.out );
          message.out[message.length++] = '\n';
          break;
        case TEST_TARGET_MAC:
          message.length = uTESThexToStr( pSERVERgetMAC(), MAC_ADR_LENGTH, message.out );
          break;
        case TEST_TARGET_VERSION:
          if ( message.dataFlag > 0U )
          {
            switch ( message.data )
            {
              case TEST_VERSION_BOOTLOADER:
                message.length = uTESTversionToStr( ( const uint16_t* )( versionController.value ), message.out );
                break;
              case TEST_VERSION_FIRMWARE:
                message.length = uTESTversionToStr( ( const uint16_t* )( versionFirmware.value ),   message.out );
                break;
              case TEST_VERSION_HARDWARE:
                message.length = uTESTversionToStr( ( const uint16_t* )( versionBootloader.value ), message.out );
                break;
              default:
                res = TEST_STATUS_ERROR_DATA;
                break;
            }
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
        message.length = uTESTstatusToString( res, message.out );
      }
      break;
    default:
      res = TEST_STATUS_ERROR_COMMAND;
      message.length = uTESTstatusToString( res, message.out );
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
uint8_t uTESTgetOutLength ( void )
{
  return message.length;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

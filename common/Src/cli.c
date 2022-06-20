/*
 * test.c
 *
 *  Created on: 23 мар. 2022 г.
 *      Author: 79110
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include <cli.h>
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
#include "dataAPI.h"
#include "dataProces.h"
#include "mbuart.h"
#include "fatsd.h"
/*------------------------- Define ------------------------------------------------------------------*/
/*----------------------- Structures ----------------------------------------------------------------*/
/*----------------------- Constant ------------------------------------------------------------------*/
static const char* commandStrings[CLI_COMMANDS_NUMBER] = {
  CLI_SET_COMMAND_STR,
  CLI_RESET_COMMAND_STR,
  CLI_GET_COMMAND_STR
};
static const char* targetStrings[CLI_TARGETS_NUMBER] = {
  CLI_TARGET_DIN_STR,
  CLI_TARGET_DOUT_STR,
  CLI_TARGET_TIME_STR,
  CLI_TARGET_OIL_STR,
  CLI_TARGET_COOLANT_STR,
  CLI_TARGET_FUEL_STR,
  CLI_TARGET_BAT_STR,
  CLI_TARGET_CHARG_STR,
  CLI_TARGET_GEN_STR,
  CLI_TARGET_MAINS_STR,
  CLI_TARGET_CUR_STR,
  CLI_TARGET_FREQ_STR,
  CLI_TARGET_SPEED_STR,
  CLI_TARGET_SWITCH_STR,
  CLI_TARGET_LED_STR,
  CLI_TARGET_STORAGE_STR,
  CLI_TARGET_UNIQUE_STR,
  CLI_TARGET_RELEASE_STR,
  CLI_TARGER_SERIAL_STR,
  CLI_TARGET_IP_STR,
  CLI_TARGET_MAC_STR,
  CLI_TARGET_VERSION_STR,
  CLI_TARGET_MODBUS_ADR_STR,
  CLI_TARGET_MODBUS_BR_STR,
  CLI_TARGET_SD_STR
};
/*----------------------- Variables -----------------------------------------------------------------*/
static TEST_TYPE message = { 0U };
/*----------------------- Functions -----------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PRIVATE -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
uint8_t uCLIparsingFields ( const char* str, char** fields )
{
  uint8_t res = 0U;
  char*   p   = NULL;
  if ( str[0U] != 0U )
  {
    fields[0U] = ( char* )str;
    res++;
    for ( uint8_t i=1U; i<CLI_FILDS_NUMBER; i++ )
    {
      p = strstr( fields[i - 1U], CLI_FILD_SEPORATOR );
      if ( p == 0U )
      {
        break;
      }
      else
      {
        fields[i] = p + 1U;
        res++;
      }
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uCLIparse ( const char* str, const char** dictionary, uint8_t length )
{
  uint8_t res = 0U;
  for ( uint8_t i=0U; i<length; i++ )
  {
    if ( strstr( str, dictionary[i] ) == str )
    {
      res = i + 1U;
      break;
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
void vCLIparseString ( const char* str, TEST_TYPE* message )
{
  char*   filds[CLI_FILDS_NUMBER] = { 0U };
  uint8_t fieldsCounter = 0U;
  message->cmd      = CLI_COMMAND_NO;
  message->target   = CLI_TARGET_NO;
  message->dataFlag = 0U;
  message->out[0U]  = 0U;
  for ( uint8_t i=0U; i<CLI_DATA_FILDS_NUMBER; i++ )
  {
    message->data[i] = 0U;
  }
  fieldsCounter     = uCLIparsingFields( str, filds );
  message->dataFlag = fieldsCounter - CLI_SYSTEM_FILDS_NUMBER;
  if ( fieldsCounter > 0U )
  {
    message->cmd = ( CLI_COMMAND )( uCLIparse( ( const char* )filds[0U], commandStrings, CLI_COMMANDS_NUMBER ) );
    if ( ( fieldsCounter > 1U ) && ( message->cmd != CLI_COMMAND_NO ) )
    {
      message->target = ( CLI_TARGET )( uCLIparse( ( const char* )filds[1U], targetStrings, CLI_TARGETS_NUMBER ) );
      if ( ( message->dataFlag > 0U ) && ( message->target != CLI_TARGET_NO ) )
      {
        if ( ( message->cmd == CLI_COMMAND_SET ) && ( message->target == CLI_TARGET_TIME ) )
        {
          strcpy( message->out, filds[CLI_SYSTEM_FILDS_NUMBER] );
        }
        else
        {
          for ( uint8_t i=0; i<message->dataFlag; i++ )
          {
            message->data[i] = atoi( filds[CLI_SYSTEM_FILDS_NUMBER + i] );
          }
        }
      }
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uCLItimeToStr ( RTC_TIME* time, char* buf )
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
  ( void )strcat( buf, CLI_LINE_END );
  return strlen( buf );
}
/*---------------------------------------------------------------------------------------------------*/
char* cCLIparseTimeFild ( char* pStr, uint8_t* output )
{
  char    sub[5U] = { 0U };
  char*   pEnd    = strchr( pStr, '.' );
  if ( pEnd != NULL )
  {
    ( void )memcpy( sub, pStr, ( pEnd - pStr ) );
    *output = ( uint8_t )atoi( sub );
    pStr    = pEnd + 1U;
  }
  return pStr;
}
/*---------------------------------------------------------------------------------------------------*/
CLI_STATUS eCLIstrToTime ( RTC_TIME* time, char* buf )
{
  CLI_STATUS res  = CLI_STATUS_ERROR_DATA;
  char*       pStr = buf;
  time->wday = TUESDAY;
  pStr = cCLIparseTimeFild( pStr, &time->day );
  if ( pStr > 0U )
  {
    pStr = cCLIparseTimeFild( pStr, &time->month );
    if ( pStr > 0U )
    {
      pStr = cCLIparseTimeFild( pStr, &time->year );
      if ( pStr > 0U )
      {
        pStr = cCLIparseTimeFild( pStr, &time->hour );
        if ( pStr > 0U )
        {
          pStr = cCLIparseTimeFild( pStr, &time->min );
          if ( pStr > 0U )
          {
            pStr = cCLIparseTimeFild( pStr, &time->sec );
            if ( pStr > 0U )
            {
              res = CLI_STATUS_OK;
            }
          }
        }
      }
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uCLIstatusToString ( CLI_STATUS status, char* buf )
{
  uint8_t res = 0U;
  switch ( status )
  {
    case CLI_STATUS_OK:
      ( void )strcpy( buf, CLI_ERROR_OK_STR );
      ( void )strcat( buf, CLI_LINE_END );
      res = strlen( CLI_ERROR_OK_STR ) + 1U;
      break;
    case CLI_STATUS_ERROR_COMMAND:
      ( void )strcpy( buf, CLI_ERROR_COMMAND_STR );
      ( void )strcat( buf, CLI_LINE_END );
      res = strlen( CLI_ERROR_COMMAND_STR ) + 1U;
      break;
    case CLI_STATUS_ERROR_TARGET:
      ( void )strcpy( buf, CLI_ERROR_TARGET_STR );
      ( void )strcat( buf, CLI_LINE_END );
      res = strlen( CLI_ERROR_TARGET_STR ) + 1U;
      break;
    case CLI_STATUS_ERROR_DATA:
      ( void )strcpy( buf, CLI_ERROR_DATA_STR );
      ( void )strcat( buf, CLI_LINE_END );
      res = strlen( CLI_ERROR_DATA_STR ) + 1U;
      break;
    case CLI_STATUS_ERROR_EXECUTING:
      ( void )strcpy( buf, CLI_ERROR_EXECUTING_STR );
      ( void )strcat( buf, CLI_LINE_END );
      res = strlen( CLI_ERROR_EXECUTING_STR ) + 1U;
      break;
    default:
      ( void )strcpy( buf, CLI_ERROR_UNKNOWN );
      ( void )strcat( buf, CLI_LINE_END );
      res = strlen( CLI_ERROR_UNKNOWN ) + 1U;
      break;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uCLIdioToStr ( uint8_t state, char* buf )
{
  uint8_t res = 0U;
  if ( state > 0U )
  {
    ( void )strcpy( buf, CLI_DIO_ON_STR );
    ( void )strcat( buf, CLI_LINE_END );
    res = strlen( CLI_DIO_ON_STR ) + 1U;
  }
  else
  {
    ( void )strcpy( buf, CLI_DIO_OFF_STR);
    ( void )strcat( buf, CLI_LINE_END );
    res = strlen( CLI_DIO_OFF_STR ) + 1U;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uCLIhexToStr ( uint8_t* data, uint8_t length, char* buf )
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
  ( void )strcat( buf, CLI_LINE_END );
  return ( length * 2U + length );
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uCLIversionToStr ( const uint16_t* version, char* buf )
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
  ( void )strcat( buf, CLI_LINE_END );
  return strlen( buf );
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uCLIreleasedToStr ( const uint16_t* data, uint8_t length, char* buf )
{
  char sub[3U] = { 0U };
  for ( uint8_t i=0; i<length; i++ )
  {
     ( void )itoa( data[i], sub, 10U );
     if ( data[i] < 10 )
     {
       ( void )strcat( buf, "0" );
     }
     ( void )strcat( buf, sub );
  }
  ( void )strcat( buf, CLI_LINE_END );
  return strlen( buf );
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uCLIserialToStr ( const uint16_t* data, char* buf )
{
  ( void )itoa( ( data[0] | ( ( uint32_t )( data[1U] ) << 16U ) ), buf, 10U );
  return strlen( buf );
}
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLIC --------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
CLI_STATUS vCLIprocess ( const char* str, uint8_t length )
{
  CLI_STATUS res  = CLI_STATUS_OK;
  RTC_TIME   time = { 0U };
  uint16_t   id[UNIQUE_ID_LENGTH] = { 0U };
  uint8_t    adr = 0U;
  vCLIparseString( str, &message );
  switch ( message.cmd )
  {
    case CLI_COMMAND_SET:
      switch ( message.target )
      {
        case CLI_TARGET_DOUT:
          if ( ( message.dataFlag > 0U ) && ( message.data[0U] < FPO_NUMBER ) )
          {
            vFPOtest( message.data[0U], 1U );
          }
          else
          {
            res = CLI_STATUS_ERROR_DATA;
          }
          break;
        case CLI_TARGET_TIME:
          if ( message.dataFlag > 0U )
          {
            res = eCLIstrToTime( &time, message.out );
            if ( res == CLI_STATUS_OK )
            {
              if ( eRTCsetTime( &time ) != RTC_OK )
              {
                res = CLI_STATUS_ERROR_EXECUTING;
              }
            }
          }
          break;
        case CLI_TARGET_LED:
          if ( message.dataFlag > 0U )
          {
            vCONTROLLERsetLed( message.data[0U], 1U );
          }
          else
          {
            res = CLI_STATUS_ERROR_DATA;
          }
          break;
        case CLI_TARGET_RELEASE:
          if ( message.dataFlag > 0U )
          {
            uint16_t buffer[CLI_RELEASED_SIZE] = { 0U };
            for ( uint8_t i=0U; i<CLI_RELEASED_SIZE; i++ )
            {
              buffer[i] = ( uint16_t )message.data[i];
            }
            vSTORAGEresetConfigWriteProtection();
            if ( eDATAAPIconfigValue( DATA_API_CMD_WRITE, RELEASE_DATE_ADR, buffer ) == DATA_API_STAT_OK )
            {
              if ( eDATAAPIconfigValue( DATA_API_CMD_SAVE, 0U, NULL ) != DATA_API_STAT_OK )
              {
                res = CLI_STATUS_ERROR_EXECUTING;
              }
            }
            else
            {
              res = CLI_STATUS_ERROR_EXECUTING;
            }
            vSTORAGEsetConfigWriteProtection();
          }
          else
          {
            res = CLI_STATUS_ERROR_DATA;
          }
          break;
        case CLI_TARGET_SERIAL:
          if ( message.dataFlag > 0U )
          {
            uint16_t buffer[2U] = { 0U };
            buffer[0U] = ( uint16_t )( message.data[0] );
            buffer[1U] = ( uint16_t )( message.data[0] >> 16U );
            vSTORAGEresetConfigWriteProtection();
            if ( eDATAAPIconfigValue( DATA_API_CMD_WRITE, SERIAL_NUMBER_ADR, buffer ) == DATA_API_STAT_OK )
            {
              if ( eDATAAPIconfigValue( DATA_API_CMD_SAVE, 0U, NULL ) != DATA_API_STAT_OK )
              {
                res = CLI_STATUS_ERROR_EXECUTING;
              }
            }
            else
            {
              res = CLI_STATUS_ERROR_EXECUTING;
            }
            vSTORAGEsetConfigWriteProtection();
          }
          else
          {
            res = CLI_STATUS_ERROR_DATA;
          }
          break;
        default:
          res = CLI_STATUS_ERROR_TARGET;
          break;
      }
      message.length = uCLIstatusToString( res, message.out );
      break;
    case CLI_COMMAND_RESET:
      switch ( message.target )
      {
        case CLI_TARGET_DOUT:
          if ( ( message.dataFlag > 0U ) && ( message.data[0U] < FPO_NUMBER ) )
          {
            vFPOtest( message.data[0U], 0U );
          }
          else
          {
            res = CLI_STATUS_ERROR_DATA;
          }
          break;
        case CLI_TARGET_LED:
          if ( message.dataFlag )
          {
            vCONTROLLERsetLed( message.data[0U], 0U );
          }
          else
          {
            res = CLI_STATUS_ERROR_DATA;
          }
          break;
        default:
          res = CLI_STATUS_ERROR_TARGET;
          break;
      }
      message.length = uCLIstatusToString( res, message.out );
      break;
    case CLI_COMMAND_GET:
      switch ( message.target )
      {
        case CLI_TARGET_DIN:
          if ( ( message.data[0U] < FPI_NUMBER ) && ( message.dataFlag > 0U ) )
          {
            message.length = uCLIdioToStr( ( ( uFPIgetRawData() >> message.data[0U] ) & 0x01U ), message.out );
          }
          else
          {
            res = CLI_STATUS_ERROR_DATA;
          }
          break;
        case CLI_TARGET_DOUT:
          if ( ( message.data[0U] < FPO_NUMBER ) && ( message.dataFlag > 0U ) )
          {
            message.length = uCLIdioToStr( ( ( uFPOgetData() >> message.data[0U] ) & 0x01U ), message.out );
          }
          else
          {
            res = CLI_STATUS_ERROR_DATA;
          }
          break;
        case CLI_TARGET_TIME:
          if ( eRTCgetTime( &time ) == RTC_OK )
          {
            message.length = uCLItimeToStr( &time, message.out );
          }
          else
          {
            res = CLI_STATUS_ERROR_EXECUTING;
          }
          break;
        case CLI_TARGET_OIL:
          fix16_to_str( fENGINEgetOilPressure(), message.out, CLI_FIX_DECIMALS );
          ( void )strcat( message.out, CLI_LINE_END );
          message.length = strlen( message.out );
          break;
        case CLI_TARGET_COOL:
          fix16_to_str( fENGINEgetCoolantTemp(), message.out, CLI_FIX_DECIMALS );
          ( void )strcat( message.out, CLI_LINE_END );
          message.length = strlen( message.out );
          break;
        case CLI_TARGET_FUEL:
          fix16_to_str( fENGINEgetFuelLevel(), message.out, CLI_FIX_DECIMALS );
          ( void )strcat( message.out, CLI_LINE_END );
          message.length = strlen( message.out );
          break;
        case CLI_TARGET_BAT:
          fix16_to_str( fENGINEgetBatteryVoltage(), message.out, CLI_FIX_DECIMALS );
          ( void )strcat( message.out, CLI_LINE_END );
          message.length = strlen( message.out );
          break;
        case CLI_TARGET_CHARG:
          fix16_to_str( fENGINEgetChargerVoltage(), message.out, CLI_FIX_DECIMALS );
          ( void )strcat( message.out, CLI_LINE_END );
          message.length = strlen( message.out );
          break;
        case CLI_TARGET_GENERATOR:
          if ( ( message.data[0U] < MAINS_LINE_NUMBER ) && ( message.dataFlag > 0U ) )
          {
            fix16_to_str( fELECTROgetGeneratorVoltage( ( uint8_t )message.data[0U] ), message.out, CLI_FIX_DECIMALS );
            ( void )strcat( message.out, CLI_LINE_END );
            message.length = strlen( message.out );
          }
          else
          {
            res = CLI_STATUS_ERROR_DATA;
          }
          break;
        case CLI_TARGET_MAINS:
          if ( ( message.data[0U] < MAINS_LINE_NUMBER ) && ( message.dataFlag > 0U ) )
          {
            fix16_to_str( fELECTROgetMainsVoltage( ( uint8_t )message.data[0U] ), message.out, CLI_FIX_DECIMALS );
            ( void )strcat( message.out, CLI_LINE_END );
            message.length = strlen( message.out );
          }
          else
          {
            res = CLI_STATUS_ERROR_DATA;
          }
          break;
        case CLI_TARGET_CURRENT:
          if ( ( message.data[0U] < MAINS_LINE_NUMBER ) && ( message.dataFlag > 0U ) )
          {
            fix16_to_str( fELECTROgetCurrent( ( uint8_t )message.data[0U] ), message.out, CLI_FIX_DECIMALS );
            ( void )strcat( message.out, CLI_LINE_END );
            message.length = strlen( message.out );
          }
          else
          {
            res = CLI_STATUS_ERROR_DATA;
          }
          break;
        case CLI_TARGET_FREQ:
          if ( message.dataFlag > 0U )
          {
            switch ( message.data[0U] )
            {
              case CLI_FREQ_CHANNELS_MAINS:
                fix16_to_str( fELECTROgetMainsFreq(), message.out, CLI_FIX_DECIMALS );
                ( void )strcat( message.out, CLI_LINE_END );
                message.length = strlen( message.out );
                break;
              case CLI_FREQ_CHANNELS_GENERATOR:
                fix16_to_str( fELECTROgetGeneratorFreq(), message.out, CLI_FIX_DECIMALS );
                ( void )strcat( message.out, CLI_LINE_END );
                message.length = strlen( message.out );
                break;
              default:
                res = CLI_STATUS_ERROR_DATA;
                break;
            }
          }
          else
          {
            res = CLI_STATUS_ERROR_DATA;
          }
          break;
        case CLI_TARGET_SPEED:
          fix16_to_str( fENGINEgetSpeed(), message.out, CLI_FIX_DECIMALS );
          ( void )strcat( message.out, CLI_LINE_END );
          message.length = strlen( message.out );
          break;
        case CLI_TARGET_SW:
          if ( ( message.data[0U] < KEYBOARD_COUNT ) && ( message.dataFlag > 0U ) )
          {
            adr = message.data[0U];
            if ( adr == 3U )
            {
              adr = 4U;
            }
            else if ( adr == 4U )
            {
              adr = 3U;
            }
            else {}
            message.length = uCLIdioToStr( uKEYgetState( adr ), message.out );
          }
          else
          {
            res = CLI_STATUS_ERROR_DATA;
          }
          break;
        case CLI_TARGET_STORAGE:
          if ( uDATAAPIisInitDone() == 0U )
          {
            res = CLI_STATUS_ERROR_EXECUTING;
          }
          message.length = uCLIstatusToString( res, message.out );
          break;
        case CLI_TARGET_UNIQUE:
          vSYSgetUniqueID16( id );
          message.length = uCLIhexToStr( ( uint8_t* )id, ( UNIQUE_ID_LENGTH * 2U ), message.out );
          break;
        case CLI_TARGET_RELEASE:
          message.length = uCLIreleasedToStr( releaseDate.value, releaseDate.atrib->len, message.out );
          break;
        case CLI_TARGET_SERIAL:
          message.length = uCLIserialToStr( serialNumber.value, message.out );
          break;
        case CLI_TARGET_IP:
          message.length = uSERVERgetStrIP( message.out );
          ( void )strcat( message.out, CLI_LINE_END );
          message.length++;
          break;
        case CLI_TARGET_MAC:
          message.length = uCLIhexToStr( pSERVERgetMAC(), MAC_ADR_LENGTH, message.out );
          break;
        case CLI_TARGET_VERSION:
          if ( message.dataFlag > 0U )
          {
            switch ( message.data[0U] )
            {
              case CLI_VERSION_BOOTLOADER:
                message.length = uCLIversionToStr( ( const uint16_t* )( versionController.value ), message.out );
                break;
              case CLI_VERSION_FIRMWARE:
                message.length = uCLIversionToStr( ( const uint16_t* )( versionFirmware.value ),   message.out );
                break;
              case CLI_VERSION_HARDWARE:
                message.length = uCLIversionToStr( ( const uint16_t* )( versionBootloader.value ), message.out );
                break;
              default:
                res = CLI_STATUS_ERROR_DATA;
                break;
            }
          }
          else
          {
            res = CLI_STATUS_ERROR_DATA;
          }
          break;
        case CLI_TARGET_MODBUS_ADR:
          ( void )utoa( getBitMap( &modbusSetup, MODBUS_ADR_ADR ), message.out, 10U );
          ( void )strcat( message.out, CLI_LINE_END );
          message.length = strlen( message.out );
          break;
        case CLI_TARGET_MODBUS_BR:
          ( void )utoa( uMBgetBaudrateValue( getBitMap( &modbusSetup, MODBUS_BAUDRATE_ADR ) ), message.out, 10U );
          ( void )strcat( message.out, CLI_LINE_END );
          message.length = strlen( message.out );
          break;
        case CLI_TARGET_SD:
          message.length = uCLIdioToStr( ( eFATSDgetStatus() == SD_STATUS_MOUNTED ? 1U : 0U ), message.out );
          break;
        default:
          res = CLI_STATUS_ERROR_TARGET;
          break;
      }
      if ( res != CLI_STATUS_OK )
      {
        message.length = uCLIstatusToString( res, message.out );
      }
      break;
    default:
      res = CLI_STATUS_ERROR_COMMAND;
      message.length = uCLIstatusToString( res, message.out );
      break;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
char* cCLIgetOutput ( void )
{
  return message.out;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uCLIgetOutLength ( void )
{
  return message.length;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

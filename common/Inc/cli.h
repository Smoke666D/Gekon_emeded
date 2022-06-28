/*
 * test.h
 *
 *  Created on: 23 мар. 2022 г.
 *      Author: 79110
 */

#ifndef INC_CLI_H_
#define INC_CLI_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#if defined ( UNIT_TEST )
#include "RTC.h"
#endif
/*------------------------ Define --------------------------------------*/
#define CLI_COMMANDS_NUMBER     3U
#define CLI_TARGETS_NUMBER      25U
#define CLI_MESSAGE_OUT_LENGTH  40U
#define CLI_FIX_DECIMALS        2U
#define CLI_SYSTEM_FILDS_NUMBER 2U
#define CLI_DATA_FILDS_NUMBER   3U
#define CLI_FILDS_NUMBER        ( CLI_SYSTEM_FILDS_NUMBER + CLI_DATA_FILDS_NUMBER )
#define CLI_RELEASED_SIZE       3U
#define CLI_COMMAND_STR_SIZE    5U
#define CLI_TARGET_STR_SIZE     8U

#define CLI_FILD_SEPORATOR      " "
#define CLI_LINE_END            "\n"

#define CLI_SET_COMMAND_STR     "set"
#define CLI_RESET_COMMAND_STR   "reset"
#define CLI_GET_COMMAND_STR     "get"

#define CLI_TARGET_DIN_STR        "din"
#define CLI_TARGET_DOUT_STR       "dout"
#define CLI_TARGET_TIME_STR       "time"
#define CLI_TARGET_OIL_STR        "oil"
#define CLI_TARGET_COOLANT_STR    "cool"
#define CLI_TARGET_FUEL_STR       "fuel"
#define CLI_TARGET_BAT_STR        "bat"
#define CLI_TARGET_CHARG_STR      "charg"
#define CLI_TARGET_GEN_STR        "gen"
#define CLI_TARGET_MAINS_STR      "mains"
#define CLI_TARGET_CUR_STR        "cur"
#define CLI_TARGET_FREQ_STR       "freq"
#define CLI_TARGET_SPEED_STR      "speed"
#define CLI_TARGET_SWITCH_STR     "sw"
#define CLI_TARGET_LED_STR        "led"
#define CLI_TARGET_STORAGE_STR    "storage"
#define CLI_TARGET_UNIQUE_STR     "unique"
#define CLI_TARGET_RELEASE_STR    "released"
#define CLI_TARGER_SERIAL_STR     "serial"
#define CLI_TARGET_IP_STR         "ip"
#define CLI_TARGET_MAC_STR        "mac"
#define CLI_TARGET_VERSION_STR    "version"
#define CLI_TARGET_MODBUS_ADR_STR "adr"
#define CLI_TARGET_MODBUS_BR_STR  "baudrate"
#define CLI_TARGET_SD_STR         "sd"

#define CLI_DIO_ON_STR          "on"
#define CLI_DIO_OFF_STR         "off"
#define CLI_ERROR_OK_STR        "Ok"
#define CLI_ERROR_COMMAND_STR   "Wrong command"
#define CLI_ERROR_TARGET_STR    "Wrong target"
#define CLI_ERROR_DATA_STR      "Wrong data"
#define CLI_ERROR_EXECUTING_STR "Executing error"
#define CLI_ERROR_UNKNOWN       "Unknown error"
/*------------------------- Macros -------------------------------------*/
/*-------------------------- ENUM --------------------------------------*/
typedef enum
{
  CLI_FREQ_CHANNELS_MAINS,
  CLI_FREQ_CHANNELS_GENERATOR
} CLI_FREQ_CHANNELS;

typedef enum
{
  CLI_VERSION_BOOTLOADER,
  CLI_VERSION_FIRMWARE,
  CLI_VERSION_HARDWARE
} CLI_VERSION;

typedef enum
{
  CLI_STATUS_OK,
  CLI_STATUS_ERROR_COMMAND,
  CLI_STATUS_ERROR_TARGET,
  CLI_STATUS_ERROR_DATA,
  CLI_STATUS_ERROR_EXECUTING
} CLI_STATUS;

typedef enum
{
  CLI_COMMAND_NO,
  CLI_COMMAND_SET,
  CLI_COMMAND_RESET,
  CLI_COMMAND_GET
} CLI_COMMAND;

typedef enum
{
  CLI_TARGET_NO,        /* 00 no */
  CLI_TARGET_DIN,       /* 01 get */
  CLI_TARGET_DOUT,      /* 02 set */
  CLI_TARGET_TIME,      /* 03 get & set */
  CLI_TARGET_OIL,       /* 04 get */
  CLI_TARGET_COOL,      /* 05 get */
  CLI_TARGET_FUEL,      /* 06 get */
  CLI_TARGET_BAT,       /* 07 get */
  CLI_TARGET_CHARG,     /* 08 get */
  CLI_TARGET_GENERATOR, /* 09 get */
  CLI_TARGET_MAINS,     /* 10 get */
  CLI_TARGET_CURRENT,   /* 11 get */
  CLI_TARGET_FREQ,      /* 12 get */
  CLI_TARGET_SPEED,     /* 13 get */
  CLI_TARGET_SW,        /* 14 get */
  CLI_TARGET_LED,       /* 15 set */
  CLI_TARGET_STORAGE,   /* 16 get */
  CLI_TARGET_UNIQUE,    /* 17 get */
  CLI_TARGET_RELEASE,   /* 18 get & set */
  CLI_TARGET_SERIAL,    /* 19 get & set */
  CLI_TARGET_IP,        /* 20 get */
  CLI_TARGET_MAC,       /* 21 get */
  CLI_TARGET_VERSION,   /* 22 get */
  CLI_TARGET_MODBUS_ADR,/* 23 get */
  CLI_TARGET_MODBUS_BR, /* 24 get */
  CLI_TARGET_SD         /* 25 get */
} CLI_TARGET;
/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  CLI_COMMAND cmd;
  CLI_TARGET  target;
  uint8_t     dataFlag;
  uint32_t    data[CLI_DATA_FILDS_NUMBER];
  uint8_t     length;
  char        out[CLI_MESSAGE_OUT_LENGTH];
} TEST_TYPE;
/*------------------------ Functions -----------------------------------*/
#if defined ( UNIT_TEST )
  uint8_t     uCLIparsingFields ( const char* str, char** filds );
  uint8_t     uCLIparse ( const char* str, const char* const* dictionary, uint8_t length );
  void        vCLIparseString ( const char* str, TEST_TYPE* message );
  uint8_t     uCLItimeToStr ( RTC_TIME* time, char* buf );
  char*       cCLIparseTimeFild ( char* pStr, uint8_t* output );
  CLI_STATUS  eCLIstrToTime ( RTC_TIME* time, char* buf );
  uint8_t     uCLIstatusToString ( CLI_STATUS status, char* buf );
  uint8_t     uCLIdioToStr ( uint8_t state, char* buf );
  uint8_t     uCLIhexToStr ( uint8_t* data, uint8_t length, char* buf );
  uint8_t     uCLIversionToStr ( const uint16_t* version, char* buf );
#endif
CLI_STATUS vCLIprocess ( const char* str, uint8_t length );
char*      cCLIgetOutput ( void );
uint8_t    uCLIgetOutLength ( void );
/*----------------------------------------------------------------------*/
#endif /* INC_CLI_H_ */

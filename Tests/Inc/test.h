/*
 * test.h
 *
 *  Created on: 23 мар. 2022 г.
 *      Author: 79110
 */

#ifndef INC_TEST_H_
#define INC_TEST_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#if defined ( UNIT_TEST )
#include "RTC.h"
#endif
/*------------------------ Define --------------------------------------*/
#define TEST_COMMANDS_NUMBER     3U
#define TEST_TARGETS_NUMBER      20U
#define TEST_MESSAGE_OUT_LENGTH  40U
#define TEST_FIX_DECIMALS        2U

#define TEST_SET_COMMAND_STR     "set"
#define TEST_RESET_COMMAND_STR   "reset"
#define TEST_GET_COMMAND_STR     "get"

#define TEST_TARGET_DIN_STR      "din"
#define TEST_TARGET_DOUT_STR     "dout"
#define TEST_TARGET_TIME_STR     "time"
#define TEST_TARGET_OIL_STR      "oil"
#define TEST_TARGET_COOLANT_STR  "cool"
#define TEST_TARGET_FUEL_STR     "fuel"
#define TEST_TARGET_BAT_STR      "bat"
#define TEST_TARGET_CHARG_STR    "charg"
#define TEST_TARGET_GEN_STR      "gen"
#define TEST_TARGET_MAINS_STR    "mains"
#define TEST_TARGET_CUR_STR      "cur"
#define TEST_TARGET_FREQ_STR     "freq"
#define TEST_TARGET_SPEED_STR    "speed"
#define TEST_TARGET_SWITCH_STR   "sw"
#define TEST_TARGET_LED_STR      "led"
#define TEST_TARGET_STORAGE_STR  "storage"
#define TEST_TARGET_ID_STR       "id"
#define TEST_TARGET_IP_STR       "ip"
#define TEST_TARGET_MAC_STR      "mac"
#define TEST_TARGET_VERSION_STR  "version"

#define TEST_DIO_ON_STR          "on"
#define TEST_DIO_OFF_STR         "off"
#define TEST_ERROR_OK_STR        "Ok"
#define TEST_ERROR_COMMAND_STR   "Wrong command"
#define TEST_ERROR_TARGET_STR    "Wrong target"
#define TEST_ERROR_DATA_STR      "Wrong data"
#define TEST_ERROR_EXECUTING_STR "Executing error"
#define TEST_ERROR_UNKNOWN       "Unknown error"
/*------------------------- Macros -------------------------------------*/
/*-------------------------- ENUM --------------------------------------*/
typedef enum
{
  TEST_FREQ_CHANNELS_MAINS,
  TEST_FREQ_CHANNELS_GENERATOR
} TEST_FREQ_CHANNELS;

typedef enum
{
  TEST_VERSION_BOOTLOADER,
  TEST_VERSION_FIRMWARE,
  TEST_VERSION_HARDWARE
} TEST_VERSION;

typedef enum
{
  TEST_STATUS_OK,
  TEST_STATUS_ERROR_COMMAND,
  TEST_STATUS_ERROR_TARGET,
  TEST_STATUS_ERROR_DATA,
  TEST_STATUS_ERROR_EXECUTING
} TEST_STATUS;

typedef enum
{
  TEST_COMMAND_NO,
  TEST_COMMAND_SET,
  TEST_COMMAND_RESET,
  TEST_COMMAND_GET
} TEST_COMMAND;

typedef enum
{
  TEST_TARGET_NO,        /* 00 no */
  TEST_TARGET_DIN,       /* 01 get */
  TEST_TARGET_DOUT,      /* 02 set */
  TEST_TARGET_TIME,      /* 03 get & set */
  TEST_TARGET_OIL,       /* 04 get */
  TEST_TARGET_COOL,      /* 05 get */
  TEST_TARGET_FUEL,      /* 06 get */
  TEST_TARGET_BAT,       /* 07 get */
  TEST_TARGET_CHARG,     /* 08 get */
  TEST_TARGET_GENERATOR, /* 09 get */
  TEST_TARGET_MAINS,     /* 10 get */
  TEST_TARGET_CURRENT,   /* 11 get */
  TEST_TARGET_FREQ,      /* 12 get */
  TEST_TARGET_SPEED,     /* 13 get */
  TEST_TARGET_SW,        /* 14 get */
  TEST_TARGET_LED,       /* 15 set */
  TEST_TARGET_STORAGE,   /* 16 get */
  TEST_TARGET_ID,        /* 17 get */
  TEST_TARGET_IP,        /* 18 get */
  TEST_TARGET_MAC,       /* 19 get & set */
  TEST_TARGET_VERSION    /* 20 get */
} TEST_TARGET;
/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  TEST_COMMAND cmd;
  TEST_TARGET  target;
  uint8_t      dataFlag : 1U;
  uint32_t     data;
  char         out[TEST_MESSAGE_OUT_LENGTH];
} TEST_TYPE;
/*------------------------ Functions -----------------------------------*/
#if defined ( UNIT_TEST )
void        eTESTparseString ( const char* str, TEST_TYPE* message );
void        vTESTtimeToStr ( RTC_TIME* time, char* buf );
uint8_t     uTESTparseTimeFild ( char* pStr, uint8_t* output );
TEST_STATUS vTESTstrToTime ( RTC_TIME* time, char* buf );
void        vTESTstatusToString ( TEST_STATUS status, char* buf );
void        vTESTdioToStr ( uint8_t state, char* buf );
#endif
TEST_STATUS vTESTprocess ( const char* str );
char*       cTESTgetOutput ( void );
/*----------------------------------------------------------------------*/
#endif /* INC_TEST_H_ */

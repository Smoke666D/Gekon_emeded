/*
 * common.h
 *
 *  Created on: 19 мар. 2020 г.
 *      Author: photo_Mickey
 */

#ifndef INC_COMMON_H_
#define INC_COMMON_H_
/*----------------------- Includes -------------------------------------*/
#include    "stm32f2xx_hal.h"
#include    "fix16.h"
/*------------------------ Define --------------------------------------*/
#define     UNIQUE_ID_ADR               0x1FFF7A10U
#define     UNIQUE_ID_LENGTH            6U
#define     SERIAL_QUEUE_SIZE           16U
#define     SERIAL_BUFFER_SIZE          25U
#define     SERIAL_END_CHAR_0           '\n'
#define     SERIAL_END_CHAR_1           '\r'
#define     SERIAL_OUTPUT_TIMEOUT       ( ( TickType_t )1000U  )
#define     SERIAL_PROTECT_TIMEOUT      ( ( TickType_t )100U )
#if defined ( UNIT_TEST )
  #define UNIT_TEST_BUFFER_SIZE   4096U
#endif
/*------------------------ Macros --------------------------------------*/
#define     REVERSE_BYTE( b )           ( ( ( b << 7U ) & 0x80U ) | ( ( b << 5U ) & 0x40U ) | ( ( b << 3U ) & 0x20U ) | ( ( b << 1U ) & 0x10U ) | ( ( b >> 1U ) & 0x08U ) | ( ( b >> 3U ) & 0x04U ) | ( ( b >> 5U ) & 0x02U ) | ( ( b >> 7U ) & 0x01U ) )
#define     GET_UNIQUE_ID0              ( *( uint32_t* )( UNIQUE_ID_ADR + 0x00U ) )
#define     GET_UNIQUE_ID1              ( *( uint32_t* )( UNIQUE_ID_ADR + 0x04U ) )
#define     GET_UNIQUE_ID2              ( *( uint32_t* )( UNIQUE_ID_ADR + 0x08U ) )
/*------------------------------ Enum ----------------------------------------*/
typedef enum
{
  INIT_OK,
  INIT_FAIL,
} INIT_STATE;

typedef enum
{
  SERIAL_STATE_IDLE,
  SERIAL_STATE_READING,
  SERIAL_STATE_WRITING
} SERIAL_STATE;
/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  GPIO_TypeDef* port;
  uint16_t      pin;
} GPIO_TYPE;

typedef struct __packed
{
  char*    data;
  uint16_t length;
} UART_MESSAGE;

typedef struct __packed
{
  UART_HandleTypeDef* uart;
  SERIAL_STATE        state : 2U;
  uint8_t             error : 1U;
  uint8_t             buffer;
  uint16_t            length;
  uint8_t             input[SERIAL_BUFFER_SIZE];
  UART_MESSAGE        output;
} SERIAL_TYPE;
/*----------------------- Callbacks ------------------------------------*/
typedef fix16_t  ( *getValueCallBack )( void );          /* Callback to get value */
typedef void     ( *setValueCallBack )( fix16_t );       /* Callback to set value */
/*----------------------- Functions ------------------------------------*/
void     vSERIALinit ( UART_HandleTypeDef* uart );
void     vSYSserial ( const char* data, uint16_t length );
void     vSYSprintFix16 ( fix16_t value );
void     vSYSgetUniqueID32 ( uint32_t* id );
void     vSYSgetUniqueID16 ( uint16_t* id );
uint8_t  uEncodeURI ( const uint16_t* input, uint8_t length, char* output );
void     vDecodeURI ( const char* input, uint16_t* output, uint8_t length );
uint8_t  uSYSisConst ( void* ptr );
uint32_t uSYSputChar ( char* str, uint32_t length, char ch );
uint32_t uSYSendString ( char* str, uint32_t length );
fix16_t  fSYSconstrain ( fix16_t in, fix16_t min, fix16_t max );
#if defined ( UNIT_TEST )
  void    vUNITputChar ( int data );
  void    vUNITwriteOutput ( void );
  void    vUNITresetOutput ( void );
  uint8_t eSERIALgetSerialState ( void );
#endif
/*----------------------------------------------------------------------*/
#endif /* INC_COMMON_H_ */

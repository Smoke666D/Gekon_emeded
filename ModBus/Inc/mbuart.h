/*
 * mbuart.h
 *
 *  Created on: 13.11.2019
 *      Author: Mikhail.Mikhailov
 */
#ifndef INC_MBUART_H_
#define INC_MBUART_H_
/*----------------------- Includes ------------------------------------------------------------------*/
#include "stm32f2xx_hal.h"
#include "mbport.h"
/*----------------------- Define --------------------------------------------------------------------*/
#define BUFFER_SIZE              260U
#define BOUDRATE_SIZE            12U
#define DEFINE_BAUD_RATE         UART_115200
#define DEFINE_PARITY            PARITY_EVEN
#define DEFINE_ADDRESS           MB_ADDRESS_MIN
#define MB_TIMER_MIN_TIC_PERIOD  ( 0.000025F )  /* sec, = 25  mks */
#define MB_TIMER_TARGET_INTERVAL ( 0.00025F )   /* sec, = 250 mks */
/*----------------------- Enums ---------------------------------------------------------------------*/
typedef enum
{
  STATE_RX_INIT,      /* Receiver is in initial state. */
  STATE_RX_IDLE,      /* Receiver is in idle state. */
  STATE_RX_RCV,       /* Frame is beeing received. */
  STATE_RX_ERROR,     /* If the frame is invalid. */
  STATE_M_RX_INIT,    /* Receiver is in initial state. */
  STATE_M_RX_IDLE,    /* Receiver is in idle state. */
  STATE_M_RX_RCV,     /* Frame is beeing received. */
  STATE_M_RX_ERROR    /* If the frame is invalid. */
} eMBRcvState;

typedef enum
{
  STATE_TX_IDLE,      /* Transmitter is in idle state. */
  STATE_TX_XMIT,      /* Transmitter is in transfer state. */
  STATE_M_TX_IDLE,    /* Transmitter is in idle state. */
  STATE_M_TX_XMIT,    /* Transmitter is in transfer state. */
  STATE_M_TX_XFWR
} eMBSndState;

typedef enum
{
  ADR_POINTER,        /* Use pointer *adr, as source of network address */
  ADR_REGISTER        /* Use data from ROM, as source of network address */
} eMBAdrType;

typedef enum
{
  UART_1200,     /* 0 */
  UART_2400,     /* 1 */
  UART_4800,     /* 2 */
  UART_9600,     /* 3 */
  UART_14400,    /* 4 */
  UART_19200,    /* 5 */
  UART_38400,    /* 6 */
  UART_56000,    /* 7 */
  UART_57600,    /* 8 */
  UART_115200,   /* 9 */
  UART_128000,   /* 10 */
  UART_256000    /* 11 */
} eMBUartBaudRate;

typedef enum
{
  STOP_BIT_2,
  PARITY_EVEN,
  PARITY_ODD
} eMBUartConfig;
/*----------------------- Structures ----------------------------------------------------------------*/
typedef struct __packed
{
  volatile uint8_t nok;
  volatile uint8_t half;
} MB_TIMER_COUNTERS;

typedef struct __packed
{
  TIM_HandleTypeDef* tim;      /* Hardware TIM structure */
  MB_TIMER_COUNTERS  counter;
} MB_TIMER_TYPE;

typedef struct __packed
{
  uint16_t read;
  uint16_t write;
} MB_BUFFER_INDEX;

typedef struct __packed
{
  uint8_t         buffer[BUFFER_SIZE];
  MB_BUFFER_INDEX index;
  uint16_t        counter;
} MB_BUFFER_TYPE;

typedef struct __packed
{
  eMBRcvState receive;
  eMBSndState send;
} MB_UART_STATE;

typedef struct __packed
{
  uint16_t byte;
  uint8_t  errRead;
} MB_UART_COUNTERS;

typedef struct __packed
{
  eMBUartBaudRate baudRate;
  eMBUartConfig   parity;
} MB_UART_SETTINGS;

typedef struct __packed
{
  GPIO_TypeDef* port;   /* Hardware GPIO port name of DE/RE pin  */
  uint16_t      pin;    /* Hardware GPIO  DE/RE pin number  */
} MB_DE_TYPE;

typedef struct __packed
{
  UART_HandleTypeDef* serial;     /* Hardware USART structure */
  MB_DE_TYPE          de;
  uint8_t             adr;
  MB_UART_SETTINGS    settings;
  MB_UART_STATE       state;
  MB_UART_COUNTERS    counter;
} MB_UART_TYPE;

typedef struct __packed
{
  UART_HandleTypeDef* uart;
  GPIO_TypeDef*       port;
  uint16_t            pin;
  TIM_HandleTypeDef*  tim;
} MB_INIT_TYPE;
/*----------------------- Macros --------------------------------------------------------------------*/
#define IS_MB_BAUD_RATE( INSTANCE )     ( ( ( INSTANCE ) == UART_1200 )   || \
                                          ( ( INSTANCE ) == UART_2400 )   || \
                                          ( ( INSTANCE ) == UART_4800 )   || \
                                          ( ( INSTANCE ) == UART_9600 )   || \
                                          ( ( INSTANCE ) == UART_14400 )  || \
                                          ( ( INSTANCE ) == UART_19200 )  || \
                                          ( ( INSTANCE ) == UART_38400 )  || \
                                          ( ( INSTANCE ) == UART_56000 )  || \
                                          ( ( INSTANCE ) == UART_57600 )  || \
                                          ( ( INSTANCE ) == UART_115200 ) || \
                                          ( ( INSTANCE ) == UART_128000 ) || \
                                          ( ( INSTANCE ) == UART_256000 ) )
#define IS_MB_PARITY( INSTANCE )         ( ( ( INSTANCE ) == STOP_BIT_2 )  || \
                                           ( ( INSTANCE ) == PARITY_EVEN ) || \
                                           ( ( INSTANCE ) == PARITY_ODD ) )
#define IS_MB_ADDRESS( INSTANCE )        ( ( ( INSTANCE ) <= MB_ADDRESS_MAX ) && ( ( INSTANCE ) != MB_ADDRESS_BROADCAST ) )
/*----------------------- Functions -----------------------------------------------------------------*/
MB_INIT_STATE     eMBhardwareInit ( MB_INIT_TYPE init );
MB_INIT_STATE     eMBreInit ( void );
void              vMBSetAdrReg ( uint8_t val );
uint8_t           uMBGetCurSlaveAdr ( void );
MB_ROM_INIT_STATE eMBGetROMInitResult ( void );
eMBSndState       eMBGetSendState ( void );
eMBRcvState       eMBGetRcvState ( void );
void              vMBstartHalfCharTimer ( void );
void              vMBgetRxBuffer ( volatile uint8_t* output );
void              vMBputStrPak ( uint8_t *data, uint16_t count );  /* Send data via UART */
void              vMBputStrPakMaster ( uint8_t *s, uint16_t c );
uint16_t          uMBGetATUByteCounter ( void );
void              vMBtimHandler ( void );
void              vMBuartHandler ( void );
/*---------------------------------------------------------------------------------------------------*/
#endif /* INC_MBUART_H_ */

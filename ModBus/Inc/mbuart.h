/*
 * mbuart.h
 *
 *  Created on: 13.11.2019
 *      Author: Mikhail.Mikhailov
 */
#ifndef INC_MBUART_H_
#define INC_MBUART_H_
/*----------------------- Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "mbport.h"
/*----------------------- Structures ----------------------------------------------------------------*/
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
	ADR_POINTER,				/* Use pointer *adr, as source of network address */
	ADR_REGISTER				/* Use data from ROM, as source of network address */
} eMBAdrType;

typedef enum
{
	UART_1200,		/* 0 */
	UART_2400,		/* 1 */
	UART_4800,		/* 2 */
	UART_9600,		/* 3 */
	UART_14400,		/* 4 */
	UART_19200,		/* 5 */
	UART_38400,		/* 6 */
	UART_56000,		/* 7 */
	UART_57600,		/* 8 */
	UART_115200,	/* 9 */
	UART_128000,	/* 10 */
	UART_256000		/* 11 */
} eMBUartBaudRate;

typedef enum
{
	STOP_BIT_2,
	PARITY_EVEN,
	PARITY_ODD
} eMBUartConfig;

typedef struct
{
		UART_HandleTypeDef 	*uart;		/* Hardware USART structure */
		TIM_HandleTypeDef 	*tim;			/* Hardware TIM structure */
		GPIO_TypeDef 				*DEport;	/* Hardware GPIO port name of DE/RE pin  */
		uint16_t 						DEpin;		/* Hardware GPIO  DE/RE pin number  */
		#if( IWDG_ON )
		IWDG_HandleTypeDef	*iwdg;		/* Hardware watch dog structure */
		#endif
		eMBAdrType					adrType;	/* Flag of network address: ADR_POINTER - use pointer *adr, ADR_REGISTER - use data from ROM */
		UCHAR								*adr;			/* Pointer to the network address */
		uint32_t						bank;			/* Number of flash bank for ModBus ROM */
		uint8_t							page;			/* Number of flash page for ModBus ROM */
} modbusStruct;
/*----------------------- Macros --------------------------------------------------------------------*/
#define IS_MB_BAUD_RATE( INSTANCE ) 		( ( ( INSTANCE ) == UART_1200 )   || \
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

#define IS_MB_PARITY( INSTANCE ) 				( ( ( INSTANCE ) == STOP_BIT_2 )  || \
																					( ( INSTANCE ) == PARITY_EVEN ) || \
																					( ( INSTANCE ) == PARITY_ODD ) )

#define	IS_MB_ADDRESS( INSTANCE )				( ( ( INSTANCE ) <= MB_ADDRESS_MAX ) && ( ( INSTANCE ) != MB_ADDRESS_BROADCAST ) )
/*----------------------- Define --------------------------------------------------------------------*/
#define RX_BUFFER_SIZE			100U
#define TX_BUFFER_SIZE 			260U
#define	DEFINE_BAUD_RATE		UART_115200
#define	DEFINE_PARITY				PARITY_EVEN
#define DEFINE_ADDRESS			MB_ADDRESS_MIN

/*----------------------- Functions -----------------------------------------------------------------*/
eMBInitState 		eMBInit( modbusStruct mbInit );
eMBInitState 		eMBReInit( void );
void 						vMBSetAdrReg( UCHAR val );
void 						vMBWatchDogReset( void );
UCHAR 					ucMBGetCurSlaveAdr( void );
eMBROMInitType 	eMBGetROMInitResult( void );

eMBSndState 		eMBGetSendState( void );
eMBRcvState 		eMBGetRcvState( void );

void 						vMBStartHalfCharTimer( void );
UCHAR*  				ucGetRxBuffer( void );
void 						vMBPutStrPak( UCHAR *data, USHORT count );	/* Send data via UART */
void 						vMBPutStrPakMaster( UCHAR *s, USHORT c );
USHORT 					usMBGetATUByteCounter( void );

void 						vMBtimHandler( void );
void 						vMBuartHandler( void );
/*---------------------------------------------------------------------------------------------------*/
#endif /* INC_MBUART_H_ */

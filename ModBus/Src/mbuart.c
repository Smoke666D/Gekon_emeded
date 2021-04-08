/*
 * mbuart.c
 *
 *  Created on: 13 нояб. 2019 г.
 *      Author: mikhail.mikhailov
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "mbuart.h"
#include "mbmemory.h"
#include "mbregister.h"
#include "cmsis_os.h"
#include "semphr.h"
#include "FreeRTOS.h"
#include "mbrtu.h"
/*----------------------- Structures ----------------------------------------------------------------*/
static 		modbusStruct		mb;
/*----------------------- Variables -----------------------------------------------------------------*/
static  	eMBRcvState 		eRcvState = STATE_RX_INIT;
static 		eMBSndState 		eSndState = STATE_TX_IDLE;

static 		uint8_t 				Count_errRead = 0U;
static		eMBROMInitType	ROMinitRes    = ROM_INIT_FAIL;
/* Data transfer */
static 		uint8_t					adrReg           = 0U;
static 		uint32_t 				BoundRateTable[] = { 1200U, 2400U, 4800U, 9600U, 14400U, 19200U, 38400U, 56000U, 57600U, 115200U, 128000U, 256000U };
volatile 	uint8_t  				timerNokCount    = 0U;
volatile 	uint8_t  				timerHalfCount   = 0U;
/* USART Receiver buffer */
static  	uint8_t 				rx_buffer[RX_BUFFER_SIZE];
volatile 	uint16_t				rx_wr_index  = 0U;
volatile 	uint16_t				rx_rd_index  = 0U;
volatile 	uint16_t 				rx_counter   = 0U;
static 		uint16_t 				Byte_Counter = 0U;
/* USART Transmitter buffer */
static		uint8_t  				tx_buffer[TX_BUFFER_SIZE];
static 		uint16_t 				tx_wr_index = 0U;
static 		uint16_t 				tx_rd_index = 0U;
static 		uint16_t 				tx_counter  = 0U;
/*----------------------- Functions -----------------------------------------------------------------*/
eMBInitState 	eMbuartInit( UART_HandleTypeDef *uart, eMBUartBaudRate baudRate, eMBUartConfig parity );
eMBInitState 	eMbtimInit( TIM_HandleTypeDef *tim, eMBUartBaudRate baudRate );
void 					vMBStartHalfCharTimer( void );
uint8_t 			ucMBReadUartInput( UART_HandleTypeDef *uart );
void 					vRS485WriteMode( modbusStruct m );
void 					vRS485ReadMode( modbusStruct m );
void 					vMBCleanUartInput( UART_HandleTypeDef *uart );
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief 	ModBus Re Initialization Function
  * @param 	none
  * @retval Status of result (EBInit_ERROR or EBInit_OK)
  */
eMBInitState eMBReInit( void )
{
	eMBUartBaudRate baudRate 	= UART_115200;
	eMBUartConfig 	parity   	= PARITY_EVEN;
	UCHAR						state			= 0U;
	eMBInitState		res				= EBInit_ERROR;

	__disable_irq ();
	/*---------------------- Memory -----------------------*/
	if ( ucMBSetMemory( mb.page, mb.bank ) == EBInit_OK )
	{
		/*---------------------- Register ---------------------*/
		ROMinitRes = vMBRegisterInit();
		baudRate 	 = usMBReadUserRegister( USER_REGISTER_BOUND_RATE_ADR );
		parity 		 = usMBReadUserRegister( USER_REGISTER_PARITY_ADR );
		adrReg 		 = usMBReadUserRegister( USER_REGISTER_ADDRESS_ADR );
		if ( !IS_MB_BAUD_RATE( baudRate ) )
		{
			baudRate = DEFINE_BAUD_RATE;
			vMBWriteShortToUserRegister( USER_REGISTER_BOUND_RATE_ADR, baudRate, RAM_NON_WR );
			state = 1U;
		}
		if ( !IS_MB_PARITY( parity ) )
		{
			parity 	 = DEFINE_PARITY;
			vMBWriteShortToUserRegister( USER_REGISTER_PARITY_ADR, parity, RAM_NON_WR );
			state = 1U;
		}
		if ( !IS_MB_ADDRESS( adrReg ) )
		{
			adrReg 	 = DEFINE_ADDRESS;
			vMBWriteShortToUserRegister( USER_REGISTER_ADDRESS_ADR, adrReg, RAM_NON_WR );
			state = 1U;
		}
		if ( state > 0U )
		{
			vMBWriteROMRegister();
		}
		/*---------------------- Address ----------------------*/
		if ( ( mb.adrType == ADR_REGISTER ) || ( ( mb.adrType == ADR_POINTER ) && ( mb.adr == NULL ) ) )
		{
			mb.adr = &adrReg;
		}
		/*---------------------- TIM -------------------------*/
		if( eMbtimInit( mb.tim, baudRate ) == EBInit_OK )
		{
			/*---------------------- USART ------------------------*/
			if( eMbuartInit( mb.uart, baudRate, parity ) == EBInit_OK )
			{
				vRS485ReadMode( mb );
				__enable_irq ();
				res = EBInit_OK;
			}
		}
	}
	return res;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief 	ModBus set new software network address
  * @param 	Value of new network address
  * @retval none
  */
void vMBSetAdrReg( UCHAR val )
{
	adrReg = val;
	vMBWriteShortToUserRegister( USER_REGISTER_ADDRESS_ADR, adrReg, ROM_WR );
	return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief 	ModBus Initialization Function
  * @param 	ModBus structure
  * @retval Status of result (EBInit_ERROR or EBInit_OK)
  */
eMBInitState eMBInit( modbusStruct mbInit )
{
	mb = mbInit;					/* Get hardware devices for RS485 ModBUS */
	return eMBReInit();		/* Devices initialization  */
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief 	Reset watch dog timer
  * @param 	none
  * @retval none
  */
void vMBWatchDogReset( void )
{
	#if ( IWDG_ON )
		HAL_IWDG_Refresh( mb.iwdg );
	#endif
	return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief 	Return send state of ModBus
  * @param 	none
  * @retval eMBSndState
  */
eMBSndState eMBGetSendState( void )
{
  return eSndState;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief 	Return receive state of ModBus
  * @param 	none
  * @retval eMBRcvState
  */
eMBRcvState eMBGetRcvState( void )
{
  return eRcvState;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief 	Return ModBus slave network address
  * @param 	none
  * @retval UCHAR
  */
UCHAR ucMBGetCurSlaveAdr( void )
{
	return *mb.adr;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief 	Return ModBus ROM initialization result
  * @param 	none
  * @retval eMBROMInitType (ROM_INIT_OK or ROM_INIT_FAIL)
  */
eMBROMInitType eMBGetROMInitResult( void )
{
	return ROMinitRes;
}

/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief 	Set RS485 to write mode
  * @param 	ModBus structure
  * @retval none
  */
void vRS485WriteMode( modbusStruct m )
{
	HAL_GPIO_WritePin( m.DEport, m.DEpin, GPIO_PIN_SET );
	return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief 	Set RS485 to read mode
  * @param 	ModBus structure
  * @retval none
  */
void vRS485ReadMode( modbusStruct m )
{
	HAL_GPIO_WritePin( m.DEport, m.DEpin, GPIO_PIN_RESET );
	return;
}
/*---------------------------------------------------------------------------------------------------*
 *---------------------------------------------------------------------------------------------------*
 *---------------------------------------------------------------------------------------------------*
 * 																	USART RS485
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------*/
/**
  * @brief ModBus UART Initialization Function
  * @param UART stucture
  * @retval Status of result (EBInit_ERROR or EBInit_OK)
  */
eMBInitState eMbuartInit( UART_HandleTypeDef *uart, eMBUartBaudRate baudRate, eMBUartConfig parity )
{
	uint16_t 			i 	= 0U;
	eMBInitState 	res = EBInit_ERROR;

	__HAL_UART_DISABLE( uart );
	if ( HAL_UART_DeInit( uart ) == HAL_OK )
	{
		uart->Init.HwFlowCtl    = UART_HWCONTROL_NONE;
		uart->Init.OverSampling = UART_OVERSAMPLING_16;
		uart->Init.Mode         = UART_MODE_TX_RX;
		uart->Init.BaudRate     = BoundRateTable[baudRate];
		switch ( parity )
		{
			case STOP_BIT_2:
				uart->Init.WordLength = UART_WORDLENGTH_8B;
				uart->Init.StopBits   = UART_STOPBITS_2;
				uart->Init.Parity     = UART_PARITY_NONE;
				break;
			case PARITY_EVEN:
				uart->Init.WordLength = UART_WORDLENGTH_9B;
				uart->Init.StopBits   = UART_STOPBITS_1;
				uart->Init.Parity     = UART_PARITY_EVEN;
				break;
			case PARITY_ODD:
				uart->Init.WordLength = UART_WORDLENGTH_9B;
				uart->Init.StopBits   = UART_STOPBITS_1;
				uart->Init.Parity     = UART_PARITY_ODD;
				break;
			default:
				uart->Init.WordLength = UART_WORDLENGTH_9B;
				uart->Init.StopBits   = UART_STOPBITS_1;
				uart->Init.Parity     = UART_PARITY_EVEN;
				break;
		}
		if ( HAL_UART_Init( uart ) == HAL_OK )
		{
			timerHalfCount 	= 0U;
			Byte_Counter 	 	= 0U;
			rx_counter   	 	= 0U;
			tx_counter   	 	= 0U;
			eRcvState 			= STATE_RX_INIT;
			eSndState 			= STATE_TX_IDLE;
			for ( i=0U; i < TX_BUFFER_SIZE; i++ )
			{
				tx_buffer[i] = 0x55U;
			}
			for ( i=0U; i < RX_BUFFER_SIZE; i++ )
			{
				rx_buffer[i] = 0U;
			}
			__HAL_UART_CLEAR_FLAG( uart, UART_FLAG_RXNE );
			__HAL_UART_CLEAR_FLAG( uart, UART_FLAG_TC );
			vMBCleanUartInput( uart );
			__HAL_UART_DISABLE_IT( uart, UART_IT_TC );			/* Transmission Complete Interrupt Disable  */
			__HAL_UART_ENABLE_IT( uart, UART_IT_RXNE );			/* UART Data Register not empty Interrupt Enable */
			//__HAL_UART_ENABLE( uart );
			res = EBInit_OK;
		}
	}
	return res;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief 	ModBus UART DR cleaner
  * @param 	UART stucture
  * @retval current DR register value
  */
uint8_t ucMBReadUartInput( UART_HandleTypeDef *uart )
{
	return ( uint8_t )( uart->Instance->DR & 0xFFU );
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief 	ModBus UART DR cleaner
  * @param 	UART stucture
  * @retval current DR register value
  */
void vMBCleanUartInput( UART_HandleTypeDef *uart )
{
	uint16_t buf = 0U;
	buf = uart->Instance->DR & 0xFFU;
	buf++;
	return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief 	Get current ModBus receive
  * 				byte counter value
  * @param 	none
  * @retval USHORT
  */
USHORT usMBGetATUByteCounter( void )
{
	USHORT CurrentByteCounter;

  CurrentByteCounter = Byte_Counter;
  return CurrentByteCounter;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief 	Get pointer to current ModBus receive buffer
  * @param 	none
  * @retval UCHAR
  */
UCHAR*  ucGetRxBuffer( void )
{
 return rx_buffer;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief 	Send unsigned char via RS485
  * @param 	data - output
  * @retval none
  */
void vMBPutChar( UCHAR data )
{
	vMBWatchDogReset();
	switch ( eSndState )
	{
		case STATE_TX_XMIT:
			tx_counter++;
			tx_wr_index++;
			tx_buffer[tx_wr_index] = data;
			__HAL_UART_ENABLE_IT( mb.uart, UART_IT_TC );			/* Transmission Complete Interrupt Enable */
			break;
		case STATE_TX_IDLE:
			vRS485WriteMode( mb );
			eSndState    = STATE_TX_XMIT;
			tx_rd_index  = 0U;
			tx_counter++;
			tx_wr_index  = 0U;
			mb.uart->Instance->DR = data;
			__HAL_UART_ENABLE_IT( mb.uart, UART_IT_TC );			/* Transmission Complete Interrupt Enable */
			__HAL_UART_DISABLE_IT( mb.uart, UART_IT_RXNE );		/* RXNE Interrupt Disable  */
			break;
		case STATE_M_TX_IDLE:
			break;
		case STATE_M_TX_XMIT:
			break;
		case STATE_M_TX_XFWR:
			break;
		default:
			break;
	}
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief	Start transmission via RS485
  * @param	data  - pointer to data array
  * 				count - length of data array
  * @retval none
  */
void vMBPutStrPak( UCHAR *data, USHORT count )
{
	USHORT i = 0U;

	vMBWatchDogReset();
	for ( i=1U; i < count; i++ )
	{
		tx_buffer[i - 1U] = data[i];
		tx_counter++;
	}
	tx_counter++;
	vRS485WriteMode( mb );
	tx_rd_index  = 0U;
	tx_wr_index  = 0U;
	eSndState    = STATE_TX_XMIT;
	__HAL_UART_ENABLE_IT( mb.uart, UART_IT_TC );			/* Transmission Complete Interrupt Enable */
	__HAL_UART_DISABLE_IT( mb.uart, UART_IT_RXNE );		/* RXNE Interrupt Disable  */
	mb.uart->Instance->DR = data[0U];
	return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief	Start transmission via RS485
  * @param	data  - pointer to data array
  * 				count - length of data array
  * @retval	none
  */
void vMBPutStrPakMaster( UCHAR *data, USHORT count )
{
	ULONG i = 0U;

	vMBWatchDogReset();
  tx_rd_index = 0U;
  tx_counter  = count - 2U;
  eSndState   = STATE_TX_XMIT;
  for ( i=0U; i < ( count - 1U ); i++)
  {
    tx_buffer[i] = data[i + 1U];
  }
  vRS485WriteMode( mb );
  __HAL_UART_CLEAR_FLAG( mb.uart, UART_FLAG_TC );		/* Clear Transmission Complete flag */
  USART1->DR = data[0U];														/* Write first byte to transmitter */
  __HAL_UART_ENABLE_IT( mb.uart, UART_IT_TC );			/* Transmission Complete Interrupt Enable */
  __HAL_UART_DISABLE_IT( mb.uart, UART_IT_RXNE );		/* RXNE Interrupt Disable  */
  return;
}
/*---------------------------------------------------------------------------------------------------*
 *---------------------------------------------------------------------------------------------------*
 *---------------------------------------------------------------------------------------------------*
 * 																		TIMER
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------*/
/**
  * @brief ModBus TIM Initialization Function
  * @param TIM structure
  * @retval Status of result (EBInit_ERROR or EBInit_OK)
  */
eMBInitState eMbtimInit( TIM_HandleTypeDef *tim, eMBUartBaudRate baudRate )
{
	/*--------------- Define --------------------------------------------*/
	#define  	MinTicPeriod			0.000025  // sec, = 25  mks
	#define		TargetInterval		0.00025		// sec, = 250 mks
	/*--------------- Structure -----------------------------------------*/
	TIM_ClockConfigTypeDef  sClockSourceConfig = { 0U };
	TIM_MasterConfigTypeDef sMasterConfig      = { 0U };
	eMBInitState						res 							 = EBInit_ERROR;
	/*--------------- Var -----------------------------------------------*/
	float 		halfCharInterval = 0U;
	/* ------------------------------------------------------------------*/
	__HAL_TIM_DISABLE( tim );
	__HAL_TIM_DISABLE_IT( tim, TIM_IT_UPDATE );
	if( baudRate > UART_14400 )
	{
		halfCharInterval = 0.00025;
	}
	else
	{
		halfCharInterval = ( 5U / ( float )BoundRateTable[baudRate] );
	}
	if ( HAL_TIM_Base_DeInit( tim ) == HAL_OK )
	{
		tim->Init.Prescaler         = ( ( HAL_RCC_GetPCLK2Freq() * MinTicPeriod ) - 1U );
		tim->Init.CounterMode       = TIM_COUNTERMODE_UP;
		tim->Init.Period            = halfCharInterval / MinTicPeriod;
		tim->Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
		tim->Init.RepetitionCounter = 0U;
		tim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
		if( HAL_TIM_Base_Init( tim ) == HAL_OK )
		{
			sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
			if( HAL_TIM_ConfigClockSource( tim, &sClockSourceConfig ) == HAL_OK )
			{
				sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
				sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
				if( HAL_TIMEx_MasterConfigSynchronization( tim, &sMasterConfig ) == HAL_OK )
				{
					__HAL_TIM_ENABLE_IT( tim, TIM_IT_UPDATE );
					res = EBInit_OK;
				}
			}
		}
	}
	return res;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief	Start timer of half char time
  * 				according the UART bound rate
  * @param	none
  * @retval	none
  */
void vMBStartHalfCharTimer( void )
{
	__HAL_TIM_SET_COUNTER( mb.tim, 0U );
	__HAL_TIM_ENABLE( mb.tim );
	timerHalfCount = 0U;
	return;
}
/*---------------------------------------------------------------------------------------------------*
 *---------------------------------------------------------------------------------------------------*
 *---------------------------------------------------------------------------------------------------*
 * 																		RTOS
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------*/
/**
  * @brief ModBus FreeRTOS task runner.
  * Put it to TIM interrupt handler
  * @param none
  * @retval none
  */
void vMBtimHandler( void )
{
	vMBWatchDogReset();
	timerHalfCount++;
	if( timerHalfCount >= 7U )
	{
  	timerHalfCount = 0U;
  	switch( eRcvState )
  	{
  		case STATE_RX_INIT:																				/* Receiver is in initial state. */
  			break;
  		case STATE_RX_IDLE:																				/* Receiver is in idle state. */
  		  break;
  		case STATE_RX_RCV:																				/* Frame is beeing received. */
  			ucMBParseATURecieverFrameBuffer( timerNokCount - 1U );
  		  break;
  		case STATE_RX_ERROR:																			/* If the frame is invalid. */
  			break;
  		case STATE_M_RX_INIT:																			/* Receiver is in initial state. */
  			vMBCleanUartInput( mb.uart );
  			break;
  		case STATE_M_RX_IDLE:																			/* Receiver is in idle state. */
  		  break;
  		case STATE_M_RX_RCV:																			/* Frame is beeing received. */
  		  break;
  		case STATE_M_RX_ERROR:																		/* If the frame is invalid. */
  		  break;
  		default:
  			break;
  	}
  	eRcvState = STATE_RX_IDLE;							/* Receiving end after 3.5 CHAR (7 tic) */
  	__HAL_TIM_DISABLE( mb.tim );						/* Turn off timer */
  	__HAL_TIM_SET_COUNTER( mb.tim, 0U );		/* Reset timer counter */
  	timerNokCount = 0U;											/* Reset NOK counter */
	}
	if( timerHalfCount == 3U )
	{
		timerNokCount++;
	}
	return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief 	ModBus FreeRTOS task runner.
  * 				Put it to UART interrupt handler
  * @param 	none
  * @retval none
  */
void vMBuartHandler( void )
{
	uint8_t data = 0U;

	vMBWatchDogReset();
	if ( __HAL_UART_GET_FLAG( mb.uart, UART_FLAG_RXNE ) > 0U ) /* Receive data register not empty flag */
	{
		data = ucMBReadUartInput( mb.uart );
		__HAL_UART_CLEAR_FLAG( mb.uart, UART_FLAG_RXNE );
		if ( __HAL_UART_GET_FLAG( mb.uart, ( UART_FLAG_PE | UART_FLAG_FE | UART_FLAG_NE | UART_FLAG_ORE ) ) == 0 )
		{
			switch ( eRcvState )
			{
				case STATE_RX_INIT:								/* Receiver is in initial state. */
					vMBStartHalfCharTimer();				/* */
					break;
				case STATE_RX_IDLE:								/* Receiver is in idle state. */
					eRcvState    = STATE_RX_RCV;		/* Move to receive mode */
					Byte_Counter = 1U;							/* Reset byte counter */
					rx_counter   = 1U;							/* Reset buffer counter */
					rx_buffer[0] = data;
					vMBStartHalfCharTimer();
					break;
				case STATE_RX_RCV:								/* Frame is beeing received. */
					Byte_Counter++;
					rx_buffer[rx_counter] = data;
					rx_counter++;
					vMBStartHalfCharTimer();
					break;
				case STATE_RX_ERROR:							/* If the frame is invalid. */
					Count_errRead++;
					break;
				case STATE_M_RX_INIT:							/* Receiver is in initial state. */
					eSndState = STATE_TX_IDLE;
					break;
				case STATE_M_RX_IDLE:							/* Receiver is in idle state. */
					break;
				case STATE_M_RX_RCV:							/* Frame is beeing received. */
					break;
				case STATE_M_RX_ERROR:						/* If the frame is invalid. */
					break;
				default:
					break;
			}
		}
		else
		{
			vMBCleanUartInput( mb.uart );
		}
	}
	if ( __HAL_UART_GET_FLAG( mb.uart, UART_FLAG_TC ) > 0U )				/* Transmission Complete flag */
	{
		__HAL_UART_CLEAR_FLAG( mb.uart, UART_FLAG_TC );
		switch ( eSndState )
		{
			case STATE_TX_IDLE:      															/* Transmitter is in idle state. */
				eRcvState = STATE_RX_IDLE;
				break;
			case STATE_TX_XMIT:      															/* Transmitter is in transfer state. */
				if ( --tx_counter > 0U )
				{
					mb.uart->Instance->DR = tx_buffer[tx_rd_index];
					tx_rd_index++;
					__HAL_UART_ENABLE_IT( mb.uart, UART_IT_TC );			/* Transmission Complete Interrupt Enable */
					__HAL_UART_DISABLE_IT( mb.uart, UART_IT_RXNE );		/* RXNE Interrupt Disable  */
				}
				else
				{
					tx_counter = 0U;
					eSndState  = STATE_TX_IDLE;
					vRS485ReadMode( mb ) ;                     				/* Switch RS485 to receiving */
					__HAL_UART_DISABLE_IT( mb.uart, UART_IT_TC );			/* Transmission Complete Interrupt Disable  */
					__HAL_UART_ENABLE_IT( mb.uart, UART_IT_RXNE );		/* RXNE Interrupt Enable */
					vMBStartHalfCharTimer();
				}
				break;
			case STATE_M_TX_IDLE:    			/* Transmitter is in idle state. */
				break;
			case STATE_M_TX_XMIT:    			/* Transmitter is in transfer state. */
				break;
			case STATE_M_TX_XFWR:
				break;
			default:
				break;
		}
	}
	return;
}
/*---------------------------------------------------------------------------------------------------*/

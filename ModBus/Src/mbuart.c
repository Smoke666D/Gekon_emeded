/*
 * mbuart.c
 *
 *  Created on: 13 нояб. 2019 г.
 *      Author: mikhail.mikhailov
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "mbuart.h"
#include "mbregister.h"
#include "cmsis_os.h"
#include "semphr.h"
#include "FreeRTOS.h"
#include "mbrtu.h"
#include "config.h"
#include "dataProces.h"
/*----------------------- Constatnts ----------------------------------------------------------------*/
static const uint32_t BoundRateTable[BOUDRATE_SIZE] = { 1200U, 2400U, 4800U, 9600U, 14400U, 19200U, 38400U, 56000U, 57600U, 115200U, 128000U, 256000U };
/*----------------------- Variables -----------------------------------------------------------------*/
/*----------------------- Structures ----------------------------------------------------------------*/
static          MB_UART_TYPE   mbUart  = { 0U };
static          MB_TIMER_TYPE  mbTimer = { 0U };
static volatile MB_BUFFER_TYPE rx      = { 0U };
static volatile MB_BUFFER_TYPE tx      = { 0U };
/*----------------------- Functions -----------------------------------------------------------------*/
MB_INIT_STATE eMBtimInit ( TIM_HandleTypeDef *tim, eMBUartBaudRate baudRate );
MB_INIT_STATE eMBuartInit ( UART_HandleTypeDef *uart, eMBUartBaudRate baudRate, eMBUartConfig parity );
void          vRS485setReadMode ( MB_DE_TYPE de );
void          vRS485setWriteMode ( MB_DE_TYPE de );
void          vMBcleanUartInput ( UART_HandleTypeDef *uart );
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief   ModBus Re Initialization Function
  * @param   none
  * @retval Status of result (EBInit_ERROR or EBInit_OK)
  */
MB_INIT_STATE eMBreInit ( void )
{
  MB_INIT_STATE res = EB_INIT_ERROR;
  /*---------------------- Memory -----------------------*/
  mbUart.settings.baudRate = getBitMap( &modbusSetup, MODBUS_BAUDRATE_ADR );
  mbUart.settings.parity   = DEFINE_PARITY;
  mbUart.adr               = getBitMap( &modbusSetup, MODBUS_ADR_ADR );
  if ( !IS_MB_BAUD_RATE( mbUart.settings.baudRate ) )
  {
    mbUart.settings.baudRate = DEFINE_BAUD_RATE;
  }
  if ( !IS_MB_PARITY( mbUart.settings.parity ) )
  {
    mbUart.settings.parity = DEFINE_PARITY;
  }
  if ( !IS_MB_ADDRESS( mbUart.adr ) )
  {
    mbUart.adr = DEFINE_ADDRESS;
  }
  /*---------------------- TIM -------------------------*/
  if ( eMBtimInit( mbTimer.tim, mbUart.settings.baudRate ) == EB_INIT_OK )
  {
    /*---------------------- USART ------------------------*/
    if ( eMBuartInit( mbUart.serial, mbUart.settings.baudRate, mbUart.settings.parity ) == EB_INIT_OK )
    {
      vRS485setReadMode( mbUart.de );
      res = EB_INIT_OK;
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief   ModBus Initialization Function
  * @param   ModBus structure
  * @retval Status of result (EBInit_ERROR or EBInit_OK)
  */
MB_INIT_STATE eMBhardwareInit ( MB_INIT_TYPE init )
{
  mbUart.state.send    = STATE_TX_IDLE;
  mbUart.state.receive = STATE_RX_INIT;
  mbUart.serial        = init.uart;
  mbUart.de.pin        = init.pin;
  mbUart.de.port       = init.port;
  mbTimer.tim          = init.tim;
  return eMBreInit();    /* Devices initialization  */
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief   Return send state of ModBus
  * @param   none
  * @retval eMBSndState
  */
eMBSndState eMBGetSendState( void )
{
  return mbUart.state.send;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief   Return receive state of ModBus
  * @param   none
  * @retval eMBRcvState
  */
eMBRcvState eMBGetRcvState ( void )
{
  return mbUart.state.receive;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief   Return ModBus slave network address
  * @param   none
  * @retval uint8_t
  */
uint8_t uMBGetCurSlaveAdr ( void )
{
  return mbUart.adr;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief   Set RS485 to write mode
  * @param   ModBus structure
  * @retval none
  */
void vRS485setWriteMode ( MB_DE_TYPE de )
{
  HAL_GPIO_WritePin( de.port, de.pin, GPIO_PIN_SET );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief   Set RS485 to read mode
  * @param   ModBus structure
  * @retval none
  */
void vRS485setReadMode ( MB_DE_TYPE de )
{
  HAL_GPIO_WritePin( de.port, de.pin, GPIO_PIN_RESET );
  return;
}
/*---------------------------------------------------------------------------------------------------*
 *---------------------------------------------------------------------------------------------------*
 *---------------------------------------------------------------------------------------------------*
 *                                   USART RS485
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------*/
/**
  * @brief ModBus UART Initialization Function
  * @param UART stucture
  * @retval Status of result (EBInit_ERROR or EBInit_OK)
  */
MB_INIT_STATE eMBuartInit ( UART_HandleTypeDef *uart, eMBUartBaudRate baudRate, eMBUartConfig parity )
{
  uint16_t      i   = 0U;
  MB_INIT_STATE res = EB_INIT_ERROR;
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
      mbTimer.counter.half = 0U;
      mbUart.counter.byte  = 0U;
      rx.counter           = 0U;
      tx.counter           = 0U;
      mbUart.state.receive = STATE_RX_INIT;
      mbUart.state.send    = STATE_TX_IDLE;
      for ( i=0U; i<BUFFER_SIZE; i++ )
      {
        tx.buffer[i] = 0x55U;
        rx.buffer[i] = 0U;
      }
      __HAL_UART_CLEAR_FLAG( uart, UART_FLAG_RXNE );
      __HAL_UART_CLEAR_FLAG( uart, UART_FLAG_TC );
      vMBcleanUartInput( uart );
      __HAL_UART_DISABLE_IT( uart, UART_IT_TC );      /* Transmission Complete Interrupt Disable  */
      __HAL_UART_ENABLE_IT( uart, UART_IT_RXNE );     /* UART Data Register not empty Interrupt Enable */
      //__HAL_UART_ENABLE( uart );
      res = EB_INIT_OK;
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief   ModBus UART DR cleaner
  * @param   UART stucture
  * @retval current DR register value
  */
uint8_t uMBreadUartInput ( UART_HandleTypeDef *uart )
{
  return ( uint8_t )( __HAL_UART_FLUSH_DRREGISTER( uart ) & 0x00FFU );
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief   ModBus UART DR cleaner
  * @param   UART stucture
  * @retval current DR register value
  */
void vMBcleanUartInput ( UART_HandleTypeDef *uart )
{
  uint16_t buf = __HAL_UART_FLUSH_DRREGISTER( uart ) & 0xFFU;
  buf++;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vMDcleanUartError ( UART_HandleTypeDef *uart )
{
  uint16_t data = 0U;
  data = uart->Instance->DR;
  data = uart->Instance->SR;
  data++;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief   Get current ModBus receive
  *         byte counter value
  * @param   none
  * @retval uint16_t
  */
uint16_t uMBgetByteCounter ( void )
{
  return mbUart.counter.byte;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief   Get pointer to current ModBus receive buffer
  * @param   none
  * @retval uint8_t
  */
volatile uint8_t* vMBgetRxBuffer ( void )
{
  return &rx.buffer[0U];
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief   Send unsigned char via RS485
  * @param   data - output
  * @retval none
  */
void vMBputChar ( uint8_t data )
{
  switch ( mbUart.state.send )
  {
    case STATE_TX_XMIT:
      tx.counter++;
      tx.index.write++;
      tx.buffer[tx.index.write] = data;
      __HAL_UART_ENABLE_IT( mbUart.serial, UART_IT_TC );      /* Transmission Complete Interrupt Enable */
      break;
    case STATE_TX_IDLE:
      vRS485setWriteMode( mbUart.de );
      mbUart.state.send = STATE_TX_XMIT;
      tx.index.read     = 0U;
      tx.counter++;
      tx.index.write    = 0U;
      __HAL_UART_FLUSH_DRREGISTER( mbUart.serial ) = data;
      __HAL_UART_ENABLE_IT( mbUart.serial, UART_IT_TC );      /* Transmission Complete Interrupt Enable */
      __HAL_UART_DISABLE_IT( mbUart.serial, UART_IT_RXNE );    /* RXNE Interrupt Disable  */
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
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief  Start transmission via RS485
  * @param  data  - pointer to data array
  *         count - length of data array
  * @retval none
  */
void vMBputStrPak ( uint8_t *data, uint16_t count )
{
  uint16_t i = 0U;
  for ( i=1U; i < count; i++ )
  {
    tx.buffer[i - 1U] = data[i];
    tx.counter++;
  }
  tx.counter++;
  vRS485setWriteMode( mbUart.de );
  tx.index.read     = 0U;
  tx.index.write    = 0U;
  mbUart.state.send = STATE_TX_XMIT;
  __HAL_UART_ENABLE_IT( mbUart.serial, UART_IT_TC );      /* Transmission Complete Interrupt Enable */
  __HAL_UART_DISABLE_IT( mbUart.serial, UART_IT_RXNE );    /* RXNE Interrupt Disable  */
  __HAL_UART_FLUSH_DRREGISTER( mbUart.serial ) = data[0U];
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief  Start transmission via RS485
  * @param  data  - pointer to data array
  *         count - length of data array
  * @retval  none
  */
void vMBputStrPakMaster ( uint8_t* data, uint16_t count )
{
  uint32_t i        = 0U;
  tx.index.read     = 0U;
  tx.counter        = count - 2U;
  mbUart.state.send = STATE_TX_XMIT;
  for ( i=0U; i<( count - 1U ); i++)
  {
    tx.buffer[i] = data[i + 1U];
  }
  vRS485setWriteMode( mbUart.de );
  __HAL_UART_CLEAR_FLAG( mbUart.serial, UART_FLAG_TC );    /* Clear Transmission Complete flag */
  __HAL_UART_FLUSH_DRREGISTER( mbUart.serial ) = data[0U]; /* Write first byte to transmitter */
  __HAL_UART_ENABLE_IT( mbUart.serial, UART_IT_TC );       /* Transmission Complete Interrupt Enable */
  __HAL_UART_DISABLE_IT( mbUart.serial, UART_IT_RXNE );    /* RXNE Interrupt Disable  */
  return;
}
/*---------------------------------------------------------------------------------------------------*
 *---------------------------------------------------------------------------------------------------*
 *---------------------------------------------------------------------------------------------------*
 *                                     TIMER
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------*/
/**
  * @brief ModBus TIM Initialization Function
  * @param TIM structure
  * @retval Status of result (EBInit_ERROR or EBInit_OK)
  */
MB_INIT_STATE eMBtimInit ( TIM_HandleTypeDef* tim, eMBUartBaudRate baudRate )
{
  TIM_ClockConfigTypeDef  sClockSourceConfig = { 0U };
  TIM_MasterConfigTypeDef sMasterConfig      = { 0U };
  MB_INIT_STATE           res                = EB_INIT_ERROR;
  float                   halfCharInterval   = MB_TIMER_TARGET_INTERVAL;
  /* ------------------------------------------------------------------*/
  __HAL_TIM_DISABLE( tim );
  __HAL_TIM_DISABLE_IT( tim, TIM_IT_UPDATE );
  if ( baudRate <= UART_14400 )
  {
    halfCharInterval = ( 5U / ( float )BoundRateTable[baudRate] );
  }
  if ( HAL_TIM_Base_DeInit( tim ) == HAL_OK )
  {
    tim->Init.Prescaler         = ( ( HAL_RCC_GetPCLK2Freq() * MB_TIMER_MIN_TIC_PERIOD ) - 1U );
    tim->Init.CounterMode       = TIM_COUNTERMODE_UP;
    tim->Init.Period            = halfCharInterval / MB_TIMER_MIN_TIC_PERIOD;
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
          res = EB_INIT_OK;
        }
      }
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief  Start timer of half char time
  *         according the UART bound rate
  * @param  none
  * @retval  none
  */
void vMBstartHalfCharTimer ( void )
{
  __HAL_TIM_SET_COUNTER( mbTimer.tim, 0U );
  __HAL_TIM_ENABLE( mbTimer.tim );
  mbTimer.counter.half = 0U;
  return;
}
/*---------------------------------------------------------------------------------------------------*
 *---------------------------------------------------------------------------------------------------*
 *---------------------------------------------------------------------------------------------------*
 *                                     RTOS
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
  mbTimer.counter.half++;
  if( mbTimer.counter.half >= 7U )
  {
    mbTimer.counter.half = 0U;
    switch( mbUart.state.receive )
    {
      case STATE_RX_INIT:                                        /* Receiver is in initial state. */
        break;
      case STATE_RX_IDLE:                                        /* Receiver is in idle state. */
        break;
      case STATE_RX_RCV:                                        /* Frame is beeing received. */
        uMBparseATURecieverFrameBuffer( mbTimer.counter.nok - 1U );
        break;
      case STATE_RX_ERROR:                                      /* If the frame is invalid. */
        break;
      case STATE_M_RX_INIT:                                      /* Receiver is in initial state. */
        vMBcleanUartInput( mbUart.serial );
        break;
      case STATE_M_RX_IDLE:                                      /* Receiver is in idle state. */
        break;
      case STATE_M_RX_RCV:                                      /* Frame is beeing received. */
        break;
      case STATE_M_RX_ERROR:                                    /* If the frame is invalid. */
        break;
      default:
        break;
    }
    mbUart.state.receive = STATE_RX_IDLE;              /* Receiving end after 3.5 CHAR (7 tic) */
    __HAL_TIM_DISABLE( mbTimer.tim );            /* Turn off timer */
    __HAL_TIM_SET_COUNTER( mbTimer.tim, 0U );    /* Reset timer counter */
    mbTimer.counter.nok = 0U;                      /* Reset NOK counter */
  }
  if ( mbTimer.counter.half == 3U )
  {
    mbTimer.counter.nok++;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief   ModBus FreeRTOS task runner.
  *         Put it to UART interrupt handler
  * @param   none
  * @retval none
  */
void vMBuartHandler ( void )
{
  uint8_t  data       = 0U;
  uint32_t isrflags   = READ_REG( mbUart.serial->Instance->SR  );
  uint32_t cr1its     = READ_REG( mbUart.serial->Instance->CR1 );
  //uint32_t cr3its     = READ_REG( mbUart.serial->Instance->CR3 );
  //uint32_t errorflags = ( isrflags & ( uint32_t )( USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE ) );
/*
  uint8_t srPE  = isrflags & ( uint32_t )( USART_SR_PE );
  uint8_t srFE  = isrflags & ( uint32_t )( USART_SR_FE  );
  uint8_t srORE = isrflags & ( uint32_t )( USART_SR_ORE  );
  uint8_t srNE  = isrflags & ( uint32_t )( USART_SR_NE  );
  data          = ( uint8_t )( mbUart.serial->Instance->DR );
*/
  //vMDcleanUartError( mbUart.serial );
  if ( ( ( isrflags & USART_SR_RXNE    ) != RESET ) &&
       ( ( cr1its   & USART_CR1_RXNEIE ) != RESET ) )
  {
    data = uMBreadUartInput( mbUart.serial );
    __HAL_UART_CLEAR_FLAG( mbUart.serial, UART_FLAG_RXNE );
    if ( __HAL_UART_GET_FLAG( mbUart.serial, ( UART_FLAG_PE | UART_FLAG_FE | UART_FLAG_NE | UART_FLAG_ORE ) ) == 0 )
    {
      switch ( mbUart.state.receive )
      {
        case STATE_RX_INIT:        /* Receiver is in initial state. */
          vMBstartHalfCharTimer(); /* */
          break;
        case STATE_RX_IDLE:                    /* Receiver is in idle state. */
          mbUart.state.receive = STATE_RX_RCV; /* Move to receive mode */
          mbUart.counter.byte  = 1U;           /* Reset byte counter */
          rx.counter           = 1U;           /* Reset buffer counter */
          rx.buffer[0U]        = data;
          vMBstartHalfCharTimer();
          break;
        case STATE_RX_RCV:                /* Frame is beeing received. */
          mbUart.counter.byte++;
          rx.buffer[rx.counter] = data;
          rx.counter++;
          vMBstartHalfCharTimer();
          break;
        case STATE_RX_ERROR:              /* If the frame is invalid. */
          mbUart.counter.errRead++;
          break;
        case STATE_M_RX_INIT:              /* Receiver is in initial state. */
          mbUart.state.send = STATE_TX_IDLE;
          break;
        case STATE_M_RX_IDLE:              /* Receiver is in idle state. */
          break;
        case STATE_M_RX_RCV:              /* Frame is beeing received. */
          break;
        case STATE_M_RX_ERROR:            /* If the frame is invalid. */
          break;
        default:
          break;
      }
    }
  }
  if ( __HAL_UART_GET_FLAG( mbUart.serial, UART_FLAG_TC ) > 0U )        /* Transmission Complete flag */
  {
    __HAL_UART_CLEAR_FLAG( mbUart.serial, UART_FLAG_TC );
    switch ( mbUart.state.send )
    {
      case STATE_TX_IDLE:                                    /* Transmitter is in idle state. */
        mbUart.state.receive = STATE_RX_IDLE;
        break;
      case STATE_TX_XMIT:                                    /* Transmitter is in transfer state. */
        if ( --tx.counter > 0U )
        {
          mbUart.serial->Instance->DR = tx.buffer[tx.index.read];
          tx.index.read++;
          __HAL_UART_ENABLE_IT( mbUart.serial, UART_IT_TC );    /* Transmission Complete Interrupt Enable */
          __HAL_UART_DISABLE_IT( mbUart.serial, UART_IT_RXNE ); /* RXNE Interrupt Disable  */
        }
        else
        {
          tx.counter        = 0U;
          mbUart.state.send = STATE_TX_IDLE;
          vRS485setReadMode( mbUart.de );                         /* Switch RS485 to receiving */
          __HAL_UART_DISABLE_IT( mbUart.serial, UART_IT_TC );  /* Transmission Complete Interrupt Disable  */
          __HAL_UART_ENABLE_IT( mbUart.serial, UART_IT_RXNE ); /* RXNE Interrupt Enable */
          vMBstartHalfCharTimer();
        }
        break;
      case STATE_M_TX_IDLE:          /* Transmitter is in idle state. */
        break;
      case STATE_M_TX_XMIT:          /* Transmitter is in transfer state. */
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

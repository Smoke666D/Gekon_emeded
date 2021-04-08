/*
 * mbrtu.c
 *
 *  Created on: 13 нояб. 2019 г.
 *      Author: mikhail.mikhailov
 */
/*----------------------- Includes ------------------------------------------------------------------*/
/*----- ModBus -----*/
#include "mbrtu.h"
#include "mb.h"
#include "mbuart.h"
#include "mbcrc.h"
/*------ RTOS ------*/
#include "FreeRTOS.h"
#include "task.h"						/* FreeRTOS ( critical section functions use ) */
#include "queue.h"
#include "stream_buffer.h"
#include "cmsis_os.h"
/*----------------------- Variables -----------------------------------------------------------------*/
static  	UCHAR										ATUSendFrameBuf[ MB_SER_PDU_SIZE_MAX ];
static		USHORT  								SndBufferCount = 0U;
static 		StreamBufferHandle_t  	xMessageRecevFrame;
const 		size_t 									xMessageRecevFrameSizeBytes = OS_MB_MESSAGE_SIZE;
const 		size_t									xTriggerLevel 							= OS_MB_MESSAGE_TIGGER_LEVEL;
const 		TickType_t 							xBlockTime 									= OS_MB_BLOCK_TIME;	/* Timeout for stream buffer receiver */
static		UCHAR										message[OS_MB_MESSAGE_SIZE];
static		MODBUSCOUNTERS 					ModBusCounters;
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Increment ModBus Exception error counter
* @param 	none
* @retval none
*/
void vMBCounterErrorInc( void )
{
	ModBusCounters.SlaveExceptionErrorCount++;
	return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	ModBus receive Message initialization
* @param 	none
* @retval none
*/
eMBInitState eMBReceiveMessageInit( void )
{
	eMBInitState res = EBInit_ERROR;
	xMessageRecevFrame = xStreamBufferCreate( xMessageRecevFrameSizeBytes, xTriggerLevel );
	if ( xMessageRecevFrame != NULL )
	{
		res = EBInit_OK;
	}
	return res;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	ModBus check Message initialization
* @param 	none
* @retval none
*/
UCHAR ucMBCheckMessage( void )
{
	UCHAR res = 0U;
	if ( xMessageRecevFrame > 0U )
	{
		res = 1U;
	}
	else
	{
		res = 0U;
	}
	return res;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	ModBus check Message initialization
* @param 	none
* @retval none
*/
size_t vMBReceiveMessage( UCHAR* input, ULONG size )
{
	return xStreamBufferReceive( xMessageRecevFrame, input, size, xBlockTime );
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Parsing received ModBus frame
* @param 	NOK_counter
* @retval none
*/
UCHAR ucMBParseATURecieverFrameBuffer( UCHAR NOK_counter )
{
	BaseType_t 	xHigherPriorityTaskWoken;
	size_t 			xBytesSent  = 0U;
	USHORT 			FrameLength = 0U;
  USHORT 			FrameCRC    = 0U;
  USHORT 			CalcCRC     = 0U;
  ULONG				i           = 0U;
  UCHAR				res					= 0U;
  UCHAR * 		pUARTBuffer;

  vMBWatchDogReset();
  FrameLength = usMBGetATUByteCounter();
  if ( !NOK_counter )
  {
  	if ( FrameLength > MB_SER_PDU_SIZE_MIN )
  	{
  		pUARTBuffer = ucGetRxBuffer(); 	/* Copy received buffer */
  		FrameCRC    = ( ( ( ( USHORT ) pUARTBuffer[FrameLength - 1U] ) ) << 8U ) |
  									( USHORT )pUARTBuffer[FrameLength - 2U]; 		/* Get CRC from received frame*/
  		CalcCRC     = usMBCRC16( pUARTBuffer, ( FrameLength - 2U ) );
  		if ( CalcCRC == FrameCRC )                                								/* Compare calculation CRC and received CRC */
  		{
  			for ( i=0U; i < ( FrameLength - 2U ); i++ )
  			{
  				message[i + 1U] = pUARTBuffer[i];
  			}
  			message[0U] = FrameLength - 2U;
  			ModBusCounters.BusMessageCount++;
  			if ( xMessageRecevFrame != NULL )
  			{
  				xBytesSent = xStreamBufferSendFromISR( xMessageRecevFrame,
																									( void * ) &message,
																									( FrameLength + 1U ),
																									&xHigherPriorityTaskWoken );
  				configASSERT( xBytesSent );
  				portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
  				res = 1U;
  			}
  		}
  	}
	}
  ModBusCounters.BusCommunicationErrorCount++;
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief	Check network address from ModBus frame and
  * 				copy frame to buffer
  * @param 	pucRcvAddress - frame buffer address
  * 				pucFrame      - frame buffer data
  * 				pusLength     - frame buffer length
  * @retval MBErrorCode
  */
MBErrorCode eMBRTUReceive( UCHAR* frame, UCHAR *pucRcvAddress, UCHAR *pucFrame, UCHAR *pusLength )
{
  MBErrorCode	eStatus = NO_VALID_FRAME;
  ULONG				i       = 0U;

  vMBWatchDogReset();
  if ( ( frame[1U] == ucMBGetCurSlaveAdr() ) ||
  		 ( frame[1U] == MB_ADDRESS_BROADCAST ) )
  {
    *pusLength     = frame[0U] - 1U; 		/* Write frame length to buffer */
    *pucRcvAddress = frame[1U];    					/* Write device address to buffer */
    for( i=0U; i < *pusLength; i++ )
    {
      pucFrame[i] = frame[i + 2U]; 			/* Write frame to buffer */
    }
    ModBusCounters.SlaveMessageCount++;
    if( *pucRcvAddress == MB_ADDRESS_BROADCAST )
    {
    	ModBusCounters.SlaveNoResponseCount++;
    }
    eStatus = VALID_FRAME;
  }
  return eStatus ;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief 	Make answer ModBus frame and send it
  * 				via RS485 as slave
  * @param 	pucRcvAddress - frame buffer address
  * 				pucFrame      - frame buffer data
  * 				pusLength     - frame buffer length
  * @retval none
  */
void vMBRTUSend( UCHAR* pucRcvAddress, UCHAR* pucFrame, UCHAR* pusLength )
{
  USHORT	usCRC16 = 0U;
  UCHAR 	i       = 0U;

  /* Transmitted frame is make in buffer ATUSendFrameBuf */
  ATUSendFrameBuf[0U] = *pucRcvAddress;
  SndBufferCount      = *pusLength + 1U;
  for ( i=1U; i < SndBufferCount; i++ )
  {
    ATUSendFrameBuf[i] = pucFrame[i - 1U];
  }
  /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
  usCRC16 = usMBCRC16( ATUSendFrameBuf, SndBufferCount );
  ATUSendFrameBuf[SndBufferCount] = ( UCHAR )( usCRC16 & 0x00FFU );
  SndBufferCount++;
  ATUSendFrameBuf[SndBufferCount] = ( UCHAR )( usCRC16 >> 8U );
  SndBufferCount++;
  vMBPutStrPak( ATUSendFrameBuf, SndBufferCount );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief 	Make answer ModBus frame and send it
  * 				via RS485 as master
  * @param 	pucRcvAddress - frame buffer address
  * 				pucFrame      - frame buffer data
  * 				pusLength     - frame buffer length
  * @retval MBErrorCode
  */
MBErrorCode eMBMasterRTUSend( UCHAR* pucRcvAddress, UCHAR* pucFrame, UCHAR* pusLength )
{
  USHORT      usCRC16 = 0U;
  UCHAR       i       = 0U;
  MBErrorCode	eStatus = MB_ENOERR;

  if ( ( eMBGetRcvState() == STATE_M_RX_IDLE ) && ( eMBGetSendState() == STATE_TX_IDLE ) )
  {
  	/* Transmitted frame is make in buffer ATUSendFrameBuf */
    ATUSendFrameBuf[0U] = *pucRcvAddress;
    SndBufferCount      = ( ( USHORT ) *pusLength ) + 1U;
    for ( i=1U; i < SndBufferCount; i++ )
    {
      ATUSendFrameBuf[i] = pucFrame[i - 1U];
    }
    /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
    usCRC16 = usMBCRC16(  ATUSendFrameBuf, SndBufferCount );
    ATUSendFrameBuf[SndBufferCount] = ( UCHAR )( usCRC16 & 0x00FFU );
    SndBufferCount++;
    ATUSendFrameBuf[SndBufferCount] = ( UCHAR )( usCRC16 >> 8U );
    SndBufferCount++;
    vMBPutStrPakMaster( ATUSendFrameBuf, ( SndBufferCount + 2U ) );
  }
  else
  {
    eStatus = MB_EIO;
  }
  return eStatus;
}
/*---------------------------------------------------------------------------------------------------*/

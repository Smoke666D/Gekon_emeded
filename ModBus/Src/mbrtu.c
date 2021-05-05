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
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"            /* FreeRTOS ( critical section functions use ) */
#include "queue.h"
#include "stream_buffer.h"
/*---------------------- Constant -------------------------------------------------------------------*/
const  size_t     xMessageRecevFrameSizeBytes = OS_MB_MESSAGE_SIZE;
const  size_t     xTriggerLevel               = OS_MB_MESSAGE_TIGGER_LEVEL;
const  TickType_t xBlockTime                  = OS_MB_BLOCK_TIME;           /* Timeout for stream buffer receiver */
/*----------------------- Variables -----------------------------------------------------------------*/
static uint8_t              ATUSendFrameBuf[MB_SER_PDU_SIZE_MAX] = { 0U };
static uint16_t             SndBufferCount                       = 0U;
static StreamBufferHandle_t xMessageRecevFrame                   = NULL;
static uint8_t              message[OS_MB_MESSAGE_SIZE]          = { 0U };
static MODBUSCOUNTERS       ModBusCounters                       = { 0U };
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Increment ModBus Exception error counter
* @param   none
* @retval none
*/
void vMBcounterErrorInc ( void )
{
  ModBusCounters.SlaveExceptionErrorCount++;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   ModBus receive Message initialization
* @param   none
* @retval none
*/
MB_INIT_STATE eMBreceiveMessageInit ( void )
{
  MB_INIT_STATE res   = EB_INIT_ERROR;
  xMessageRecevFrame = xStreamBufferCreate( xMessageRecevFrameSizeBytes, xTriggerLevel );
  if ( xMessageRecevFrame != NULL )
  {
    res = EB_INIT_OK;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   ModBus check Message initialization
* @param   none
* @retval none
*/
uint8_t uMBcheckMessage ( void )
{
  uint8_t res = 0U;
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
* @brief   ModBus check Message initialization
* @param   none
* @retval none
*/
size_t uMBreceiveMessage ( uint8_t* input, uint32_t size )
{
  return xStreamBufferReceive( xMessageRecevFrame, input, size, xBlockTime );
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Parsing received ModBus frame
* @param   NOK_counter
* @retval none
*/
uint8_t uMBparseATURecieverFrameBuffer ( uint8_t NOK_counter )
{
  BaseType_t   priority    = 0U;
  size_t       xBytesSent  = 0U;
  uint16_t     frameLength = 0U;
  uint16_t     frameCRC    = 0U;
  uint16_t     calcCRC     = 0U;
  uint32_t     i           = 0U;
  uint8_t      res         = 0U;
  volatile uint8_t* pUARTBuffer = NULL;

  frameLength = uMBGetATUByteCounter();
  if ( !NOK_counter )
  {
    if ( frameLength > MB_SER_PDU_SIZE_MIN )
    {
      vMBgetRxBuffer( pUARTBuffer );
      frameCRC    = ( ( ( ( uint16_t ) pUARTBuffer[frameLength - 1U] ) ) << 8U ) |
                          ( uint16_t ) pUARTBuffer[frameLength - 2U];     /* Get CRC from received frame*/
      calcCRC     = usMBCRC16( pUARTBuffer, ( frameLength - 2U ) );
      if ( calcCRC == frameCRC )                                                /* Compare calculation CRC and received CRC */
      {
        for ( i=0U; i<( frameLength - 2U ); i++ )
        {
          message[i + 1U] = pUARTBuffer[i];
        }
        message[0U] = frameLength - 2U;
        ModBusCounters.BusMessageCount++;
        if ( xMessageRecevFrame != NULL )
        {
          xBytesSent = xStreamBufferSendFromISR( xMessageRecevFrame,
                                                  ( void * ) &message,
                                                  ( frameLength + 1U ),
                                                  &priority );
          configASSERT( xBytesSent );
          portYIELD_FROM_ISR( priority );
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
  * @brief  Check network address from ModBus frame and
  *         copy frame to buffer
  * @param   pucRcvAddress - frame buffer address
  *         pucFrame      - frame buffer data
  *         pusLength     - frame buffer length
  * @retval MBErrorCode
  */
MBErrorCode eMBRTUreceive ( uint8_t* blob, MB_FRAME_TYPE* frame )
{
  MBErrorCode  status = NO_VALID_FRAME;
  uint32_t     i      = 0U;
  if ( ( blob[1U] == uMBGetCurSlaveAdr()  ) ||
       ( blob[1U] == MB_ADDRESS_BROADCAST ) )
  {
    frame->length = blob[0U] - 1U;     /* Write frame length to buffer */
    frame->adr    = blob[1U];          /* Write device address to buffer */
    for ( i=0U; i<frame->length; i++ )
    {
      frame->pdu.data[i] = blob[i + 2U];       /* Write frame to buffer */
    }
    frame->pdu.func = frame->pdu.data[MB_PDU_FUNC_OFF];
    ModBusCounters.SlaveMessageCount++;
    if ( frame->adr == MB_ADDRESS_BROADCAST )
    {
      ModBusCounters.SlaveNoResponseCount++;
    }
    status = VALID_FRAME;
  }
  return status ;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief   Make answer ModBus frame and send it
  *         via RS485 as slave
  * @param   pucRcvAddress - frame buffer address
  *         pucFrame      - frame buffer data
  *         pusLength     - frame buffer length
  * @retval none
  */
void vMBRTUsend ( MB_FRAME_TYPE* frame )
{
  uint16_t  usCRC16 = 0U;
  uint8_t   i       = 0U;

  /* Transmitted frame is make in buffer ATUSendFrameBuf */
  ATUSendFrameBuf[0U] = frame->adr;
  SndBufferCount      = frame->length + 1U;
  for ( i=1U; i<SndBufferCount; i++ )
  {
    ATUSendFrameBuf[i] = frame->pdu.data[i - 1U];
  }
  /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
  usCRC16 = usMBCRC16( ATUSendFrameBuf, SndBufferCount );
  ATUSendFrameBuf[SndBufferCount] = ( uint8_t )( usCRC16 & 0x00FFU );
  SndBufferCount++;
  ATUSendFrameBuf[SndBufferCount] = ( uint8_t )( usCRC16 >> 8U );
  SndBufferCount++;
  vMBputStrPak( ATUSendFrameBuf, SndBufferCount );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
  * @brief   Make answer ModBus frame and send it
  *         via RS485 as master
  * @param   pucRcvAddress - frame buffer address
  *         pucFrame      - frame buffer data
  *         pusLength     - frame buffer length
  * @retval MBErrorCode
  */
MBErrorCode eMBmasterRTUSend ( MB_FRAME_TYPE* frame )
{
  uint16_t     crc    = 0U;
  uint8_t      i      = 0U;
  MBErrorCode  status = MB_ENOERR;

  if ( ( eMBGetRcvState() == STATE_M_RX_IDLE ) && ( eMBGetSendState() == STATE_TX_IDLE ) )
  {
    /* Transmitted frame is make in buffer ATUSendFrameBuf */
    ATUSendFrameBuf[0U] = frame->adr;
    SndBufferCount      = ( ( uint16_t ) frame->length ) + 1U;
    for ( i=1U; i<SndBufferCount; i++ )
    {
      ATUSendFrameBuf[i] = frame->pdu.data[i - 1U];
    }
    /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
    crc = usMBCRC16(  ATUSendFrameBuf, SndBufferCount );
    ATUSendFrameBuf[SndBufferCount] = ( uint8_t )( crc & 0x00FFU );
    SndBufferCount++;
    ATUSendFrameBuf[SndBufferCount] = ( uint8_t )( crc >> 8U );
    SndBufferCount++;
    vMBputStrPakMaster( ATUSendFrameBuf, ( SndBufferCount + 2U ) );
  }
  else
  {
    status = MB_EIO;
  }
  return status;
}
/*---------------------------------------------------------------------------------------------------*/

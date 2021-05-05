/*
 * mb.c
 *
 *  Created on: 13 нояб. 2019 г.
 *      Author: mikhail.mikhailov
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "mb.h"
#include "mbcrc.h"
#include "mbfunc.h"
#include "mbregister.h"
#include "mbrtu.h"
#include "mbuart.h"
#include "mbport.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include "stream_buffer.h"
#include "string.h"
/*----------------------- Structures ----------------------------------------------------------------*/
static osThreadId_t   MBpollTaskHandle = NULL;             /* eMBPoll task */
static MB_EXCEPTION   eException       = MB_EX_NONE;      /* ModBus states */
static MB_FRAME_TYPE  mbFrame          = { 0U };
static MB_STATES_TYPE mbState          = { 0U };
/*----------------------- Variables -----------------------------------------------------------------*/
static uint8_t  inputMessage[OS_MB_MESSAGE_SIZE] = { 0U }; /* Input frame buffer */
static size_t   xReceivedBytes                   = 0U;     /* Number of received bytes from stream buffer */
/*----------------------- Functions -----------------------------------------------------------------*/
void eMBpoll ( void *argument );
/*----------------------- Structures ----------------------------------------------------------------*/
static const MBFunctionHandlerTable eMBFuncHandlers[MAX_FUN_NUM + 1U] =
{
  /*------------------- 00 -------------------*/
  { eMBNoFunction },
  /*------------------- 01 -------------------*/
  #if ( MB_FUNC_READ_COILS_ENABLED )
    { eMBFuncReadCoils },
  #else
    { eMBNoFunction },
  #endif
  /*------------------- 02 -------------------*/
  #if ( MB_FUNC_READ_DISCRETE_INPUTS_ENABLED )
    { eMBFuncReadDiscreteInputs },
  #else
    { eMBNoFunction },
  #endif
  /*------------------- 03 -------------------*/
  #if ( MB_FUNC_READ_HOLDING_ENABLED )
    { eMBFuncReadHoldingRegister },
  #else
    { eMBNoFunction },
  #endif
  /*------------------- 04 -------------------*/
  #if ( MB_FUNC_READ_INPUT_ENABLED )
    { eMBFuncReadInputRegister },
  #else
    { eMBNoFunction },
  #endif
  /*------------------- 05 -------------------*/
  #if ( MB_FUNC_WRITE_COIL_ENABLED )
    { eMBFuncWriteCoil },
  #else
    { eMBNoFunction },
  #endif
  /*------------------- 06 -------------------*/
  #if ( MB_FUNC_WRITE_HOLDING_ENABLED )
    { eMBFuncWriteHoldingRegister },
  #else
    { eMBNoFunction },
  #endif
  /*------------------- 07 -------------------*/
  { eMBNoFunction },        // Read Exception Status
  /*------------------- 08 -------------------*/
  #if ( MB_FUNC_DIAGNOSTICS_ENEBLED )
    { eMBNoFunction },        // Read Diagnostic
  #else
    { eMBNoFunction },
  #endif
  /*------------------- 09 -------------------*/
  { eMBNoFunction },
  /*------------------- 10 -------------------*/
  { eMBNoFunction },
  /*------------------- 11 -------------------*/
  { eMBNoFunction },        // Get CommEvernCounter
  /*------------------- 12 -------------------*/
  { eMBNoFunction },        // Get CommEvernLog
  /*------------------- 13 -------------------*/
  { eMBNoFunction },
  /*------------------- 14 -------------------*/
  { eMBNoFunction },
  /*------------------- 15 -------------------*/
  #if ( MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED )
    { eMBFuncWriteMultipleCoils },
  #else
    { eMBNoFunction },
  #endif
  /*------------------- 16 -------------------*/
  #if ( MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED )
    { eMBFuncWriteMultipleHoldingRegister },
  #else
    { eMBNoFunction },
  #endif
  /*------------------- 17 -------------------*/
  { eMBNoFunction },
  /*------------------- 18 -------------------*/
  { eMBNoFunction },
  /*------------------- 19 -------------------*/
  { eMBNoFunction },
  /*------------------- 20 -------------------*/
  { eMBNoFunction },
  /*------------------- 21 -------------------*/
  { eMBNoFunction },
  /*------------------- 22 -------------------*/
  #if ( MB_FUNC_MASK_WRITE_REGISTER_ENABLED )
    { eMBFuncMaskWriteRegister },
  #else
    { eMBNoFunction },
  #endif
  /*------------------- 23 -------------------*/
  #if ( MB_FUNC_READWRITE_HOLDING_ENABLED )
    { eMBFuncReadWriteMultipleHoldingRegister },
  #else
    { eMBNoFunction },
  #endif
  /*------------------- 24 -------------------*/
  { eMBNoFunction },
  /*------------------- 25 -------------------*/
  { eMBNoFunction },
  /*------------------- 26 -------------------*/
  { eMBNoFunction },
  /*------------------- 27 -------------------*/
  { eMBNoFunction },
  /*------------------- 28 -------------------*/
  { eMBNoFunction },
  /*------------------- 29 -------------------*/
  { eMBNoFunction },
  /*------------------- 30 -------------------*/
  { eMBNoFunction },
  /*------------------- 31 -------------------*/
  { eMBNoFunction },
  /*------------------- 32 -------------------*/
  { eMBNoFunction },
  /*------------------- 33 -------------------*/
  { eMBNoFunction },
  /*------------------- 34 -------------------*/
  { eMBNoFunction },
  /*------------------- 35 -------------------*/
  { eMBNoFunction },
  /*------------------- 36 -------------------*/
  { eMBNoFunction },
  /*------------------- 37 -------------------*/
  { eMBNoFunction },
  /*------------------- 38 -------------------*/
  { eMBNoFunction },
  /*------------------- 39 -------------------*/
  { eMBNoFunction },
  /*------------------- 40 -------------------*/
  { eMBNoFunction },
  /*------------------- 41 -------------------*/
  { eMBNoFunction },
  /*------------------- 42 -------------------*/
  { eMBNoFunction },
  /*------------------- 43 -------------------*/
  { eMBNoFunction }, // Read device Identification
  /*------------------- 44 -------------------*/
  #if ( MB_FUNC_SET_RS_PARAMETERS_ENEBLED )
    { eMBFuncSetRSParameters },
  #else
    { eMBNoFunction },
  #endif
};
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Enable listen only mode
* @param   none
* @retval none
*/
void vMBlistenOnlyModeEnablae( void )
{
  mbState.ListenOnlyMode = 1U;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Disable listen only mode
* @param   none
* @retval none
*/
void vMBListenOnlyModeDisable( void )
{
  mbState.ListenOnlyMode = 0U;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Initialization of ModBus
* @param   none
* @retval none
*/
void vMBstartUpMB( void )
{
  mbState.ListenOnlyMode = 0U;
  vMBstartHalfCharTimer();
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Set ModBus to Master mode
* @param   none
* @retval none
*/
void vMBsetMasterMode( void )
{
  mbState.MasterMode = 1U;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Return master/slave state of ModBus
* @param   none
* @retval state of ModBus
*/
uint8_t uMBifMasterMode ( void )
{
  return mbState.MasterMode;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   No ModBus function
* @param   pucFrame - pointer to static frame buffer
*         usLen    - pointer to static frame length buffer
* @retval function execution result
*/
MB_EXCEPTION eMBNoFunction ( uint8_t* pucFrame, uint8_t* len )
{
  return MB_EX_ILLEGAL_FUNCTION;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Get static buffer data of
*         frame address
* @param   none
* @retval frame address
*/
uint8_t uMBgetRequestAdress ( void )
{
  return mbFrame.adr;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   ModBus Queues initialization
* @param   none
* @retval none
*/
MB_INIT_STATE eMBQueueInit ( void )
{
  MB_INIT_STATE res = EB_INIT_OK;

  res = eMBreceiveMessageInit();
  if ( res == EB_INIT_OK )
  {
    res = eMBeventGroupInit();
  }
  return res;
}
/**
* @brief   ModBus main task initialization
* @param   none
* @retval none
*/

void vMBTaskInit ( void )
{
  /* definition and creation of MBModBusTask */
  const osThreadAttr_t MBpollTask_attributes = {
    .name       = "MBpollTask",
    .priority   = OS_MB_TASK_PRIORITY,
    .stack_size = OS_MB_TASK_STACK_SIZE
  };
  MBpollTaskHandle = osThreadNew( eMBpoll, NULL, &MBpollTask_attributes );
  vMBstartUpMB();
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vMBinit ( MB_INIT_TYPE init )
{
  if ( eMBhardwareInit( init ) == EB_INIT_OK )
  {
    if ( eMBQueueInit() == EB_INIT_OK )
    {
      vMBTaskInit();
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   ModBus main task function
* @param   none
* @retval none
*/
void eMBpoll ( void *argument )
{
  for(;;)
  {
    if ( uMBcheckMessage() > 0U )
    {
      xReceivedBytes = uMBreceiveMessage( inputMessage, sizeof( inputMessage ) );
      if ( xReceivedBytes > 0U )
      {
        if ( eMBRTUreceive( inputMessage, &mbFrame ) == VALID_FRAME )    /* Frame valid checking and copy frame to buffer */
        {
          /* Standard ModBus functions */
          if ( mbFrame.pdu.func < MAX_FUN_NUM )                                                       /* Function number validation */
          {
            eException = eMBFuncHandlers[mbFrame.pdu.func].pxHandler( mbFrame.pdu.data, &mbFrame.length );        /* Run command */
          }
          /* Custom ModBus functions */
          else if ( mbFrame.pdu.func >= CUSTOM_FUN_START )
          {
            switch ( mbFrame.pdu.func )
            {
              case MB_FUNC_SET_RS_PARAMETERS_CODE:
                eException = eMBFuncHandlers[MB_FUNC_SET_RS_PARAMETERS_NUMBER].pxHandler( mbFrame.pdu.data, &mbFrame.length );
                break;
              default:
                eException = MB_EX_ILLEGAL_FUNCTION;
                break;
            }
          }
          /* Invalid ModBus functions */
          else
          {
            eException = MB_EX_ILLEGAL_FUNCTION;                                              /* Illegal function number */
          }
          if ( eException != MB_EX_NONE )                                                      /* Error */
          {
            vMBcounterErrorInc();                                                              /* Increment error counter  */
          }
          if ( ( mbFrame.adr == MB_ADDRESS_BROADCAST ) || mbState.ListenOnlyMode )
          {
            if  ( eException == MB_EX_REINIT )
            {
              taskENTER_CRITICAL();
              eMBreInit();
              vMBstartUpMB();
              taskEXIT_CRITICAL();
            }
          }
          else
          {
            if ( ( eException != MB_EX_NONE ) && ( eException != MB_EX_REINIT ) )                /* An exception occured. Build an error frame. */
            {
              mbFrame.length = 1U;
              mbFrame.pdu.data[mbFrame.length] = ( uint8_t )( mbFrame.pdu.func | MB_FUNC_ERROR );
              mbFrame.length++;
              mbFrame.pdu.data[mbFrame.length] = eException;
            }
            vMBRTUsend( &mbFrame );
            if ( eException == MB_EX_REINIT )
            {
              taskENTER_CRITICAL();
              eMBreInit();
              vMBstartUpMB();
              taskEXIT_CRITICAL();
            }
          }
        }
      }
      else
      {
        xReceivedBytes = 1U;
      }
    }
  }
}
/*---------------------------------------------------------------------------------------------------*/

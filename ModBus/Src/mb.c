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
#include "mbmemory.h"
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
static 	osThreadId_t 						MBpollTaskHandle = NULL; 						/* eMBPoll task */
static 	eMBException 						eException       = MB_EX_NONE;			/* ModBus states */
/*----------------------- Variables -----------------------------------------------------------------*/
static 	uint8_t 								ListenOnlyMode 	 = 0U;							/* Listen ModBus mode flag */
static 	uint8_t 								MasterMode     	 = 0U;							/* Master ModBus mode flag */
static 	uint8_t 								ucRcvAddress   	 = 0U;							/* Static buffer for address of the frame */
/*---------- OS -------*/
static	UCHAR										inputMessage[OS_MB_MESSAGE_SIZE];		/* Input frame buffer */
static	size_t 									xReceivedBytes = 0U;								/* Number of received bytes from stream buffer */
static  uint32_t								taskTime       = 0U;								/* System variable for processor loading calculation */
/*--------- TASK VAR ------*/
static 	uint8_t 								ucMBFrame[MB_PDU_SIZE_MAX]; 				/* Static buffer for the frame */
static 	UCHAR 									PDULength      = 0U;								/* PDU frame length */
static 	UCHAR 									ucFunctionCode = 0U;								/* Function code from ModBus frame */
/*----------------------- Functions -----------------------------------------------------------------*/
void eMBPoll( void *argument );
/*----------------------- Structures ----------------------------------------------------------------*/
static MBFunctionHandlerTable eMBFuncHandlers[]=
{
  /*------------------- 00 -------------------*/
  eMBNoFunction,
  /*------------------- 01 -------------------*/
  #if ( MB_FUNC_READ_COILS_ENABLED )
    eMBFuncReadCoils,
  #else
    eMBNoFunction,
  #endif
  /*------------------- 02 -------------------*/
  #if ( MB_FUNC_READ_DISCRETE_INPUTS_ENABLED )
    eMBFuncReadDiscreteInputs,
  #else
    eMBNoFunction,
  #endif
  /*------------------- 03 -------------------*/
  #if ( MB_FUNC_READ_HOLDING_ENABLED )
    eMBFuncReadHoldingRegister,
  #else
    eMBNoFunction,
  #endif
  /*------------------- 04 -------------------*/
  #if ( MB_FUNC_READ_INPUT_ENABLED )
    eMBFuncReadInputRegister,
  #else
    eMBNoFunction,
  #endif
  /*------------------- 05 -------------------*/
  #if ( MB_FUNC_WRITE_COIL_ENABLED )
    eMBFuncWriteCoil,
  #else
    eMBNoFunction,
  #endif
  /*------------------- 06 -------------------*/
  #if ( MB_FUNC_WRITE_HOLDING_ENABLED )
    eMBFuncWriteHoldingRegister,
  #else
    eMBNoFunction,
  #endif
  /*------------------- 07 -------------------*/
  eMBNoFunction,        // Read Exception Status
  /*------------------- 08 -------------------*/
	#if ( MB_FUNC_DIAGNOSTICS_ENEBLED )
		eMBNoFunction,        // Read Diagnostic
	#else
		eMBNoFunction,
	#endif
  /*------------------- 09 -------------------*/
  eMBNoFunction,        // 09
  /*------------------- 10 -------------------*/
  eMBNoFunction,        // 10
  /*------------------- 11 -------------------*/
  eMBNoFunction,        // Get CommEvernCounter
  /*------------------- 12 -------------------*/
  eMBNoFunction,        // Get CommEvernLog
  /*------------------- 13 -------------------*/
  eMBNoFunction,        // 13
  /*------------------- 14 -------------------*/
  eMBNoFunction,        // 14
  /*------------------- 15 -------------------*/
  #if ( MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED )
    eMBFuncWriteMultipleCoils,
  #else
    eMBNoFunction,
  #endif
  /*------------------- 16 -------------------*/
  #if ( MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED )
    eMBFuncWriteMultipleHoldingRegister,
  #else
    eMBNoFunction,
  #endif
  /*------------------- 17 -------------------*/
  eMBNoFunction, // 17
  /*------------------- 18 -------------------*/
  eMBNoFunction, // 18
  /*------------------- 19 -------------------*/
  eMBNoFunction, // 19
  /*------------------- 20 -------------------*/
  eMBNoFunction, // 20
  /*------------------- 21 -------------------*/
  eMBNoFunction, // 21
  /*------------------- 22 -------------------*/
  #if ( MB_FUNC_MASK_WRITE_REGISTER_ENABLED )
    eMBFuncMaskWriteRegister,
  #else
    eMBNoFunction,
  #endif
  /*------------------- 23 -------------------*/
  #if ( MB_FUNC_READWRITE_HOLDING_ENABLED )
    eMBFuncReadWriteMultipleHoldingRegister,
  #else
    eMBNoFunction,
  #endif
  /*------------------- 24 -------------------*/
  eMBNoFunction, //24
  /*------------------- 25 -------------------*/
  eMBNoFunction, //25
  /*------------------- 26 -------------------*/
  eMBNoFunction, //26
  /*------------------- 27 -------------------*/
  eMBNoFunction, //27
  /*------------------- 28 -------------------*/
  eMBNoFunction, //28
  /*------------------- 29 -------------------*/
  eMBNoFunction, //29
  /*------------------- 30 -------------------*/
  eMBNoFunction, //30
  /*------------------- 31 -------------------*/
  eMBNoFunction, //31
  /*------------------- 32 -------------------*/
  eMBNoFunction, //32
  /*------------------- 33 -------------------*/
  eMBNoFunction, //33
  /*------------------- 34 -------------------*/
  eMBNoFunction, //34
  /*------------------- 35 -------------------*/
  eMBNoFunction, //35
  /*------------------- 36 -------------------*/
  eMBNoFunction, //36
  /*------------------- 37 -------------------*/
  eMBNoFunction, //37
  /*------------------- 38 -------------------*/
  eMBNoFunction, //38
  /*------------------- 39 -------------------*/
  eMBNoFunction, //39
  /*------------------- 40 -------------------*/
  eMBNoFunction, //40
  /*------------------- 41 -------------------*/
  eMBNoFunction, //41
  /*------------------- 42 -------------------*/
  eMBNoFunction, //42
  /*------------------- 43 -------------------*/
  eMBNoFunction, // Read device Identification
  /*------------------- 44 -------------------*/
	#if ( MB_FUNC_SET_RS_PARAMETERS_ENEBLED )
		eMBFuncSetRSParameters,
	#else
    eMBNoFunction,
  #endif
};
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Enable listen only mode
* @param 	none
* @retval none
*/
void vMBListenOnlyModeEnablae( void )
{
  ListenOnlyMode = 1U;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Disable listen only mode
* @param 	none
* @retval none
*/
void vMBListenOnlyModeDisable( void )
{
  ListenOnlyMode = 0U;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Initialization of ModBus
* @param 	none
* @retval none
*/
void vMBStartUpMB( void )
{
  ListenOnlyMode = 0U;
  vMBStartHalfCharTimer();
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Set ModBus to Master mode
* @param 	none
* @retval none
*/
void vMBSetMasterMode( void )
{
  MasterMode = 1U;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Return master/slave state of ModBus
* @param 	none
* @retval state of ModBus
*/
UCHAR ucMBIfMasterMode( void )
{
  return MasterMode;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	No ModBus function
* @param 	pucFrame - pointer to static frame buffer
* 				usLen    - pointer to static frame length buffer
* @retval function execution result
*/
eMBException eMBNoFunction( UCHAR * pucFrame, UCHAR * usLen )
{
  return MB_EX_ILLEGAL_FUNCTION;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Get static buffer data of
* 				frame address
* @param 	none
* @retval frame address
*/
UCHAR ucMBGetRequestAdress( void )
{
  return ucRcvAddress;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	ModBus Queues initialization
* @param 	none
* @retval none
*/
eMBInitState eMBQueueInit( void )
{
	eMBInitState 	res = EBInit_OK;

	res =  eMBReceiveMessageInit();
	if ( res == EBInit_OK )
	{
		res = eMBEventGroupInit();
	}
	return res;
}
/**
* @brief 	ModBus main task initialization
* @param 	none
* @retval none
*/

void vMBTaskInit( void )
{
  /* definition and creation of MBModBusTask */
  const osThreadAttr_t MBpollTask_attributes = {
    .name       = "MBpollTask",
    .priority   = OS_MB_TASK_PRIORITY,
    .stack_size = OS_MB_TASK_STACK_SIZE
  };
  MBpollTaskHandle = osThreadNew( eMBPoll, NULL, &MBpollTask_attributes );
  vMBStartUpMB();
	return;
}

/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Get system task time counter
* @param 	none
* @retval task counter
*/
uint32_t uMBGetTaskTime( void )
{
	return taskTime;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Increment system task counter
* @param 	none
* @retval none
*/
void vMBIncTaskTime( void )
{
	taskTime++;
	return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Reset system task counter
* @param 	none
* @retval none
*/
void vMBResetTaskTime( void )
{
	taskTime = 0U;
	return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	ModBus main task function
* @param 	none
* @retval none
*/
void eMBPoll( void *argument )
{
  for(;;)
  {
  	vMBWatchDogReset();
  	vMBIncTaskTime();
  	if ( ucMBCheckMessage() > 0U )
  	{
  		xReceivedBytes = vMBReceiveMessage( inputMessage, sizeof( inputMessage ) );
  		if ( xReceivedBytes > 0U )
  		{
  			if ( eMBRTUReceive( inputMessage, &ucRcvAddress, ucMBFrame, &PDULength ) == VALID_FRAME )  	/* Frame valid checking and copy frame to buffer */
  			{
  				ucFunctionCode = ucMBFrame[MB_PDU_FUNC_OFF];																							/* Get function code from the frame */
  				/* Standard ModBus functions */
  				if ( ucFunctionCode < MAX_FUN_NUM )                                         							/* Function number validation */
  				{
  					eException = eMBFuncHandlers[ucFunctionCode].pxHandler( ucMBFrame, &PDULength );				/* Run command */
  				}
  				/* Custom ModBus functions */
  				else if ( ucFunctionCode >= CUSTOM_FUN_START )
  				{
  					switch ( ucFunctionCode )
  					{
  						case MB_FUNC_SET_RS_PARAMETERS_CODE:
  							eException = eMBFuncHandlers[MB_FUNC_SET_RS_PARAMETERS_NUMBER].pxHandler( ucMBFrame, &PDULength );
  							break;
  						default:
  							eException = MB_EX_ILLEGAL_FUNCTION;
  							break;
  					}
  				}
  				/* Invalid ModBus functions */
  				else
  				{
  					eException = MB_EX_ILLEGAL_FUNCTION;                                    					/* Illegal function number */
  				}
  				if ( eException != MB_EX_NONE )                                            					/* Error */
  				{
  					vMBCounterErrorInc();																															/* Increment error counter  */
  				}
  				if ( ( ucRcvAddress == MB_ADDRESS_BROADCAST ) || ListenOnlyMode )
  				{
  					if  ( eException == MB_EX_REINIT )
  					{
  						taskENTER_CRITICAL();
  						eMBReInit();
  						vMBStartUpMB();
  						taskEXIT_CRITICAL();
  					}
  				}
  				else
  				{
  					if ( ( eException != MB_EX_NONE ) && ( eException != MB_EX_REINIT ) )								/* An exception occured. Build an error frame. */
  					{
  						PDULength = 0U;
  						PDULength++;
  						ucMBFrame[PDULength] = ( UCHAR )( ucFunctionCode | MB_FUNC_ERROR );
  						PDULength++;
  						ucMBFrame[PDULength] = eException;
  					}
  					vMBRTUSend( &ucRcvAddress, ucMBFrame, &PDULength );
  					if ( eException == MB_EX_REINIT )
  					{
  						taskENTER_CRITICAL();
  						eMBReInit();
  						vMBStartUpMB();
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

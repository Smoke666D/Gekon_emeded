/*
 * mbfunc.c
 *
 *  Created on: 14.11.2019
 *      Author: mikhail.mikhailov
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "mbfunc.h"
#include "mb.h"
#include "mbuart.h"
#include "mbregister.h"
#include "FreeRTOS.h"
#include "task.h"						/* FreeRTOS ( critical section functions use ) */
/*----------------------- Define --------------------------------------------------------------------*/
#define MB_HOLDING_REG_START  									0U
#define MB_COILS_REG_START    									0U
#define MB_INPUT_REG_START    									0U
#define MB_DI_REG_START       									0U
#define MB_PDU_FUNC_READ_WRITE_ADDR_OFF         0x01
#define MB_PDU_FUNC_READ_REGCNT_OFF             ( MB_PDU_DATA_OFF + 2U )
#define MB_PDU_FUNC_READ_SIZE                   0x05U
#define MB_PDU_FUNC_READ_REGCNT_MAX             0x007BU
#define MB_PDU_FUNC_READ_BITCNT_MAX             0x07B0U
#define MB_PDU_FUNC_WRITE_VALUE_OFF             0x03U
#define MB_PDU_FUNC_WRITE_SIZE                  0x05U
#define MB_PDU_FUNC_MASK_WRITE_SIZE             7U
#define MB_PDU_FUNC_WRITE_MUL_ADDR_OFF          ( MB_PDU_DATA_OFF + 0U )
#define MB_PDU_FUNC_WRITE_MUL_REGCNT_OFF        ( MB_PDU_DATA_OFF + 2U )
#define MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF       ( MB_PDU_DATA_OFF + 4U )
#define MB_PDU_FUNC_WRITE_MUL_VALUES_OFF        ( MB_PDU_DATA_OFF + 5U )
#define MB_PDU_FUNC_WRITE_MUL_SIZE_MIN          5U
#define MB_PDU_FUNC_WRITE_MUL_REGCNT_MAX        0x0078U
#define MB_PDU_FUNC_READWRITE_READ_ADDR_OFF     ( MB_PDU_DATA_OFF + 0U )
#define MB_PDU_FUNC_READWRITE_READ_REGCNT_OFF   ( MB_PDU_DATA_OFF + 2U )
#define MB_PDU_FUNC_READWRITE_WRITE_ADDR_OFF    ( MB_PDU_DATA_OFF + 4U )
#define MB_PDU_FUNC_READWRITE_WRITE_REGCNT_OFF  ( MB_PDU_DATA_OFF + 6U )
#define MB_PDU_FUNC_READWRITE_BYTECNT_OFF       ( MB_PDU_DATA_OFF + 8U )
#define MB_PDU_FUNC_READWRITE_WRITE_VALUES_OFF  ( MB_PDU_DATA_OFF + 9U )
#define MB_PDU_FUNC_READWRITE_SIZE_MIN          9U
/*---------------------------------------------------------------------------------------------------*/
#if ( MB_FUNC_MASK_WRITE_REGISTER_ENABLED )
/**
* @brief 	Masking function.
* @param 	pucFrame - pointer to static frame buffer
* 				usLen    - pointer to static frame length buffer
* @retval function execution result
*/
eMBException eMBFuncMaskWriteRegister( UCHAR* pucFrame, UCHAR* usLen )
{
	USHORT        usRegAddress = 0U;
	USHORT        usData       = 0U;
	USHORT        And_Mask     = 0U;
	USHORT        Or_Mask      = 0U;
	eMBException  eStatus      = MB_EX_NONE;
  /* Address validation */
  usRegAddress = ( USHORT )( pucFrame[STARTING_ADDRESS_HI] << 8U ) | ( USHORT )pucFrame[STARTING_ADDRESS_LO];
  if ( usRegAddress < HR_REGISTER_COUNT )
  {
    if ( *usLen == MB_PDU_FUNC_MASK_WRITE_SIZE )
    {
      And_Mask = ( ( ( USHORT ) pucFrame[AND_MASK_HI] ) << 8U ) |
      		       ( USHORT )pucFrame[AND_MASK_LO];
      Or_Mask  = ( ( ( USHORT ) pucFrame[OR_MASK_HI] ) << 8U ) |
      					 ( USHORT )pucFrame[OR_MASK_LO];
      usData   = ( usMBReadHolding ( usRegAddress ) & And_Mask )  |
      					 ( Or_Mask & ( USHORT )( ~And_Mask ) );
      usData = ( usData >> 8U ) | ( usData << 8U );
      //taskENTER_CRITICAL();
      vMBWriteHolding( usRegAddress, ( UCHAR* )&usData, 1U );
      //taskEXIT_CRITICAL();
    }
    else
    {
      eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    }
  }
  else
  {
    eStatus = MB_EX_ILLEGAL_DATA_ADDRESS;
  }
  return eStatus;
}
#endif
/*---------------------------------------------------------------------------------------------------*/
#if ( MB_FUNC_WRITE_HOLDING_ENABLED )
/**
* @brief 	Write holding register function.
* @param 	pucFrame - pointer to static frame buffer
* 				usLen    - pointer to static frame length buffer
* @retval function execution result
*/
eMBException eMBFuncWriteHoldingRegister( UCHAR* pucFrame, UCHAR* usLen )
{
	USHORT        usRegAddress = 0U;
	eMBException  eStatus 		 = MB_EX_NONE;

  if( *usLen == MB_PDU_FUNC_WRITE_SIZE )
  {
    usRegAddress = ( USHORT )( ( ( USHORT )pucFrame[STARTING_ADDRESS_HI] ) << 8 ) | ( USHORT )pucFrame[STARTING_ADDRESS_LO];
    if ( usRegAddress <  HR_REGISTER_COUNT )
    {
    	//taskENTER_CRITICAL();
      vMBWriteHolding( usRegAddress, &pucFrame[3U], 1U );
      //taskEXIT_CRITICAL();
    }
    else
    {
      eStatus = MB_EX_ILLEGAL_DATA_ADDRESS;
    }
  }
  else
  {
    eStatus = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return eStatus;
}
#endif
/*---------------------------------------------------------------------------------------------------*/
#if ( MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED )
/**
* @brief 	Write multiple holding registers
* @param 	pucFrame - pointer to static frame buffer
* 				usLen    - pointer to static frame length buffer
* @retval function execution result
*/
eMBException eMBFuncWriteMultipleHoldingRegister( UCHAR* pucFrame, UCHAR* usLen )
{
	USHORT        usRegAddress   = 0U;
	USHORT        usRegCount     = 0U;
	UCHAR         ucRegByteCount = 0U;
	eMBException  eStatus        = MB_EX_NONE;

  if ( *usLen >= ( MB_PDU_FUNC_WRITE_MUL_SIZE_MIN + MB_PDU_SIZE_MIN ) )
  {
  	/* Address validation */
    usRegAddress = ( ( ( USHORT )pucFrame[STARTING_ADDRESS_HI] ) << 8U ) |
    							 ( USHORT )pucFrame[STARTING_ADDRESS_LO];
    usRegCount   = ( ( ( USHORT )pucFrame[3U] ) << 8U ) |
    							 ( USHORT )pucFrame[4U];
    if ( ( usRegCount + usRegAddress) <= HR_REGISTER_COUNT )
    {
      ucRegByteCount = pucFrame[5U];
      if( ( usRegCount >= 1U ) &&
      		( usRegCount <= MB_PDU_FUNC_WRITE_MUL_REGCNT_MAX ) &&
					( ucRegByteCount == ( UCHAR ) ( 2U * usRegCount ) ) )
      {
      	//taskENTER_CRITICAL();
        vMBWriteHolding( usRegAddress, &pucFrame[6U], ( ucRegByteCount / 2U ) );
        //taskEXIT_CRITICAL();
        vMBWatchDogReset();
        *usLen = MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF;
      }
      else
      {
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
      }
    }
    else
    {
      eStatus = MB_EX_ILLEGAL_DATA_ADDRESS;
    }
  }
  else
  {
    eStatus = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return eStatus;
}

#endif
/*---------------------------------------------------------------------------------------------------*/
#if ( MB_FUNC_READ_HOLDING_ENABLED )
/**
* @brief 	Read holding register function.
* @param 	pucFrame - pointer to static frame buffer
* 				usLen    - pointer to static frame length buffer
* @retval function execution result
*/
eMBException eMBFuncReadHoldingRegister( UCHAR* pucFrame, UCHAR* usLen )
{
	USHORT          usRegAddress = 0U;					/* The Data Address of the first register requested */
	USHORT          usRegCount   = 0U;					/* The total number of registers requested */
	UCHAR           *pucFrameCur;								/* Response frame */
	UCHAR           i            = 0U;
	eMBException    eStatus      = MB_EX_NONE;

  if ( *usLen == MB_PDU_FUNC_READ_SIZE )
  {
    usRegAddress = ( ( ( USHORT )pucFrame[1U] ) << 8U ) | ( USHORT )pucFrame[2U];
    usRegCount   = ( ( ( USHORT )pucFrame[3U] ) << 8U ) | ( USHORT )pucFrame[4U];
    if ( ( usRegCount + usRegAddress ) <= HR_REGISTER_COUNT )                      	/* Register address validation */
    {
      if ( ( usRegCount >= 1U ) && ( usRegCount <= MB_PDU_FUNC_READ_REGCNT_MAX ) )
      {
        pucFrameCur    = &pucFrame[MB_PDU_FUNC_OFF];																/* Set the current PDU data pointer to the beginning. */
        pucFrameCur[0U] = MB_FUNC_READ_HOLDING_REGISTER;   													/* First byte contains the function code. */
        pucFrameCur[1U] = ( UCHAR )( usRegCount*2U );      													/* Second byte in the response contain the number of bytes. */
        for ( i=0; i < usRegCount; i++ )
        {
          pucFrameCur[2U + ( 2U * i )] = ( UCHAR )( usMBReadHolding( i + usRegAddress ) >> 8U );
          pucFrameCur[3U + ( 2U * i )] = ( UCHAR )( usMBReadHolding( i + usRegAddress ) & 0x00FFU );
        }
        *usLen = 2U + ( usRegCount * 2U );
      }
      else
      {
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
      }
    }
    else
    {
      eStatus = MB_EX_ILLEGAL_DATA_ADDRESS;
    }
  }
  else
  {
    eStatus = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return eStatus;
}
#endif
/*---------------------------------------------------------------------------------------------------*/
#if ( MB_FUNC_READ_INPUT_ENABLED )
/**
* @brief 	Read input register
* @param 	pucFrame - pointer to static frame buffer
* 				usLen    - pointer to static frame length buffer
* @retval function execution result
*/
eMBException    eMBFuncReadInputRegister( UCHAR* pucFrame, UCHAR* usLen )
{
	USHORT        usRegAddress = 0U;
	USHORT        usRegCount   = 0U;
	UCHAR         i            = 0U;
	eMBException  eStatus      = MB_EX_NONE;
	UCHAR         *pucFrameCur;

  if ( *usLen == MB_PDU_FUNC_READ_SIZE )
  {
    usRegAddress = ( ( ( USHORT )pucFrame[1U] ) << 8U ) | ( USHORT )pucFrame[2U];
    usRegCount   = ( ( ( USHORT )pucFrame[3U] ) << 8U ) | ( USHORT )pucFrame[4U];
    /* Address validation */
    if ( ( usRegAddress >= MB_INPUT_REG_START ) &&
    		 ( usRegAddress < ( MB_INPUT_REG_START + IR_REGISTER_COUNT ) ) &&
				 ( ( usRegCount + usRegAddress ) <= ( MB_INPUT_REG_START + IR_REGISTER_COUNT) ) )
    {
      if ( ( usRegCount >= 1U )  && ( usRegCount <= MB_PDU_FUNC_READ_REGCNT_MAX ) )
      {
        pucFrameCur    = &pucFrame[MB_PDU_FUNC_OFF];			/* Set the current PDU data pointer to the beginning. */
        pucFrameCur[0U] = MB_FUNC_READ_INPUT_REGISTER;     /* First byte contains the function code. */
        pucFrameCur[1U] = ( UCHAR ) ( usRegCount * 2U );    /* Second byte in the response contain the number of bytes. */
        for ( i=0U; i < usRegCount; i++ )
        {
          pucFrameCur[2U + ( 2U * i )] = ( UCHAR )( usMBReadInput( i + usRegAddress ) >> 8U );
          pucFrameCur[3U + ( 2U * i )] = ( UCHAR )( usMBReadInput( i + usRegAddress ) & 0x00FFU );
        }
        *usLen = 2U + ( usRegCount * 2U );
      }
      else
      {
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
      }
    }
    else
    {
      eStatus = MB_EX_ILLEGAL_DATA_ADDRESS;
    }
  }
  else
  {
    eStatus = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return eStatus;
}
#endif
/*---------------------------------------------------------------------------------------------------*/
#if ( MB_FUNC_READ_DISCRETE_INPUTS_ENABLED )
/**
* @brief 	Read bits from register
* @param 	pucFrame - pointer to static frame buffer
* 				usLen    - pointer to static frame length buffer
* @retval function execution result
*/
eMBException eMBFuncReadDiscreteInputs( UCHAR* pucFrame, UCHAR* usLen )
{
	USHORT          usRegAddress = 0U;
	USHORT          usRegCount   = 0U;
	UCHAR           *pucFrameCur;
	eMBException    eStatus      = MB_EX_NONE;

  if ( *usLen == MB_PDU_FUNC_READ_SIZE )
  {
    usRegAddress = ( ( ( USHORT )pucFrame[1U] ) << 8U) | ( USHORT )pucFrame[2U];
    usRegCount   = ( ( ( USHORT )pucFrame[3U] ) << 8U) | ( USHORT )pucFrame[4U];
    /* Address validation */
    if ( ( usRegAddress >= MB_DI_REG_START) &&
    		 ( usRegAddress < ( MB_DI_REG_START + DI_REG_COUNT ) ) &&
				 ( ( usRegCount + usRegAddress ) <= ( MB_DI_REG_START + DI_REG_COUNT ) ) )
    {
      if ( ( usRegCount >= 1U ) && ( usRegCount <= MB_PDU_FUNC_READ_BITCNT_MAX ) )
      {
        pucFrameCur    = &pucFrame[MB_PDU_FUNC_OFF];    	/* Set the current PDU data pointer to the beginning. */
        pucFrameCur[0U] = MB_FUNC_READ_DISCRETE_INPUTS;  	/* First byte contains the function code. */
        if ( ( usRegCount & 0x07U ) > 0U )                        	/* Second byte in the response contain the number of bytes. */
        {
          pucFrameCur[1U] = ( UCHAR )( ( usRegCount >> 3U ) + 1U );
          *usLen  = 2U + ( UCHAR )( ( usRegCount >> 3U ) + 1U );
        }
        else
        {
          pucFrameCur[1U] = ( UCHAR )( usRegCount >> 3U );
          *usLen  = 2U + ( UCHAR )( usRegCount >> 3U );
        }
        vMBReadBitData( usRegAddress, pucFrameCur , usRegCount, DISCRET_INPUT );
      }
      else
      {
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
      }
    }
    else
    {
      eStatus = MB_EX_ILLEGAL_DATA_ADDRESS;
    }
  }
  else
  {
    eStatus = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return eStatus;
}
#endif
/*---------------------------------------------------------------------------------------------------*/
#if ( MB_FUNC_READ_COILS_ENABLED )
/**
* @brief 	Read coil register function
* @param 	pucFrame - pointer to static frame buffer
* 				usLen    - pointer to static frame length buffer
* @retval function execution result
*/
eMBException    eMBFuncReadCoils( UCHAR* pucFrame, UCHAR* usLen )
{
	USHORT        usRegAddress = 0U;
	USHORT        usRegCount   = 0U;
	UCHAR         *pucFrameCur;
	eMBException  eStatus      = MB_EX_NONE;

  if ( *usLen == MB_PDU_FUNC_READ_SIZE )
  {
    usRegAddress = ( ( ( USHORT )pucFrame[1U] ) << 8U ) | ( USHORT )pucFrame[2U];
    usRegCount   = ( ( ( USHORT )pucFrame[3U] ) << 8U ) | ( USHORT )pucFrame[4U];
    /* Address validation */
    if ( ( usRegAddress >= MB_COILS_REG_START ) &&
    		 ( usRegAddress < ( MB_COILS_REG_START + CR_COILS_COUNT ) ) &&
				 ( ( usRegCount + usRegAddress ) <= ( MB_COILS_REG_START + CR_COILS_COUNT ) ) )
    {
      if ( ( usRegCount >= 1U ) && ( usRegCount <= MB_PDU_FUNC_READ_BITCNT_MAX ) )
      {
        pucFrameCur    = &pucFrame[MB_PDU_FUNC_OFF];    	/* Set the current PDU data pointer to the beginning. */
        pucFrameCur[0U] = MB_FUNC_READ_COILS;            	/* First byte contains the function code. */
        if ( ( usRegCount & 0x07U ) > 0U )               	/* Second byte in the response contain the number of bytes. */
        {
          pucFrameCur[1U] = ( UCHAR )( ( usRegCount >> 3U ) + 1U );
          *usLen  = 2U + ( UCHAR )( ( usRegCount >> 3U ) + 1U );
        }
        else
        {
          pucFrameCur[1U] = ( UCHAR )( usRegCount >> 3U );
          *usLen = 2U + ( UCHAR )( usRegCount >> 3U );
        }
        vMBReadBitData( usRegAddress, pucFrameCur , usRegCount, COIL ) ;
      }
      else
      {
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
      }
    }
    else
    {
      eStatus = MB_EX_ILLEGAL_DATA_ADDRESS;
    }
  }
  else
  {
    eStatus = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return eStatus;
}
#endif
/*---------------------------------------------------------------------------------------------------*/
#if ( MB_FUNC_WRITE_COIL_ENABLED )
/**
* @brief 	Write coil register function
* @param 	pucFrame - pointer to static frame buffer
* 				usLen    - pointer to static frame length buffer
* @retval function execution result
*/
eMBException    eMBFuncWriteCoil( UCHAR* pucFrame, UCHAR* usLen )
{
	USHORT        usRegAddress = 0U;
	USHORT        OutputValue  = 0U;
	eMBException  eStatus      = MB_EX_NONE;

  OutputValue = ( USHORT )( pucFrame[OUTPUT_VALUE_HI] << 8U ) | ( USHORT )( pucFrame[OUTPUT_VALUE_LO] );                          // Проверка валидности выходных данных
  if ( ( *usLen == MB_PDU_FUNC_WRITE_SIZE ) &&
  		 ( ( OutputValue == 0x0000U ) || ( OutputValue == 0xFF00U ) ) )
  {
    usRegAddress = ( USHORT )( pucFrame[STARTING_ADDRESS_HI] << 8U ) | ( USHORT )( pucFrame[STARTING_ADDRESS_LO] );              // Проверка валидности адресса
    if ( ( usRegAddress >= MB_COILS_REG_START ) &&
    		 ( usRegAddress < ( MB_COILS_REG_START + CR_COILS_COUNT ) ) )
    {
    	taskENTER_CRITICAL();
      vMBWriteCoil( usRegAddress, OutputValue );
      taskEXIT_CRITICAL();
    }
    else
    {
      eStatus = MB_EX_ILLEGAL_DATA_ADDRESS;
    }
  }
  else
  {
    eStatus = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return eStatus;
}
#endif
/*---------------------------------------------------------------------------------------------------*/
#if ( MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED )
/**
* @brief 	Write multiple coil registers function
* @param 	pucFrame - pointer to static frame buffer
* 				usLen    - pointer to static frame length buffer
* @retval function execution result
*/
eMBException    eMBFuncWriteMultipleCoils( UCHAR* pucFrame, UCHAR* usLen )
{
	USHORT        usRegAddress   = 0U;
	USHORT        usRegCount     = 0U;
	USHORT        ucRegByteCount = 0U;
	eMBException  eStatus        = MB_EX_NONE;

  usRegCount = ( ( ( USHORT )pucFrame[QUANTITY_OF_OUTPUTS_HI]) << 8U ) | ( USHORT )( pucFrame[QUANTITY_OF_OUTPUTS_LO] );
  /* Address validation */
  ucRegByteCount = usRegCount >> 3U;        // Согласно требования протокола, вычисляем необходимое кол-во байт для передчи
  if ( ( usRegCount & 0x0007U ) > 0U )
  {
    ucRegByteCount++; 											//Указанного в PDU количества регистров
  }
  if ( ( *usLen >= ( MB_PDU_FUNC_WRITE_MUL_SIZE_MIN + MB_PDU_SIZE_MIN ) ) &&
     ( ( usRegCount >= 1U ) &&
    	 ( usRegCount <= MB_PDU_FUNC_READ_BITCNT_MAX ) &&
       ( pucFrame[BYTE_COUNT] == ucRegByteCount) ) )
  {
  	/* Address validation */
    usRegAddress = ( USHORT )( pucFrame[STARTING_ADDRESS_HI] << 8U ) | ( USHORT )( pucFrame[STARTING_ADDRESS_LO] );
    if ( ( usRegAddress >= MB_COILS_REG_START ) &&
    		 ( usRegAddress < ( MB_COILS_REG_START + CR_COILS_COUNT ) ) &&
				 ( ( usRegCount + usRegAddress ) <= ( MB_COILS_REG_START + CR_COILS_COUNT ) ) )
    {
    	//taskENTER_CRITICAL();
      vMBWriteHolding( usRegAddress, &pucFrame[6U], usRegCount );
      //taskEXIT_CRITICAL();
      vMBWatchDogReset();
      *usLen = MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF;
    }
    else
    {
      eStatus = MB_EX_ILLEGAL_DATA_ADDRESS;
    }
  }
  else
  {
    eStatus = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return eStatus;
}
#endif
/*---------------------------------------------------------------------------------------------------*/
#if ( MB_FUNC_DIAGNOSTICS_ENEBLED )
/**
* @brief 	Diagnostics  function
* @param 	pucFrame - pointer to static frame buffer
* 				usLen    - pointer to static frame length buffer
* @retval function execution result
*/
eMBException eMBFuncDiagnostics( UCHAR* pucFrame, UCHAR* usLen )
{
	USHORT        SubFunctionCode = 0U;
	eMBException  eStatus         = MB_EX_NONE;

  SubFunctionCode = ( USHORT )( pucFrame[SUB_FUNCTION_HI] << 8U ) | ( USHORT )( pucFrame[SUB_FUNCTION_LO] );
  switch ( SubFunctionCode )
  {
    case  RETURN_QUERY_DATA:
      break;
    case  RESTART_COMUNICATION_OPTION:
      eStatus = MB_EX_REINIT;
      break;
    case  RETURN_DIAGNOSTIC_REGISTER:
      break;
    case  FORCE_LISTEN_ONLY_MODE:
    	vMBListenOnlyModeEnablae();
      break;
    default:
      eStatus = MB_EX_ILLEGAL_FUNCTION;
      break;
  }
  return eStatus;
}
#endif
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Change UART parameters function
*
* @param 	pucFrame - pointer to static frame buffer
* 				usLen    - pointer to static frame length buffer
* @retval function execution result
*/
eMBException eMBFuncSetRSParameters( UCHAR* pucFrame, UCHAR* usLen )
{
	eMBException  eStatus         = MB_EX_REINIT;
	USHORT        subFunctionCode = 0U;
	UCHAR					data						= 0U;

  subFunctionCode = ( USHORT )( pucFrame[SUB_FUNCTION_HI] << 8U ) | ( USHORT )( pucFrame[SUB_FUNCTION_LO] );
  data = pucFrame[SUB_FUNCTION_PARAM];
  switch ( subFunctionCode )
  {
    case SET_BOUNDRATE:
      // Проверяем, что заничение скорости предачи валидно, если да, то переписыаем новое значение в регистр инициализации и устанавливае флаг
      // по которому параметры канала будут измнены в оснвоном цикле
      if ( IS_MB_BAUD_RATE( subFunctionCode ) > 0U )
      {
      	vMBWriteShortToUserRegister( USER_REGISTER_BOUND_RATE_ADR, data, ROM_WR );
      	eStatus = MB_EX_REINIT;
      }
      else
      {
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
      }
      break;
    case  SET_PARITY:
      if ( IS_MB_PARITY( subFunctionCode ) > 0U )
      {
      	vMBWriteShortToUserRegister( USER_REGISTER_PARITY_ADR, data, ROM_WR );
      	eStatus = MB_EX_REINIT;
      }
      else
      {
        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
      }
      break;
    case SET_NET_ADR:
    	if ( IS_MB_ADDRESS( subFunctionCode ) > 0U )
    	{
    		vMBSetAdrReg( data );
    		eStatus = MB_EX_NONE;
    	}
    	else
    	{
    		eStatus = MB_EX_ILLEGAL_DATA_VALUE;
    	}
    	break;
    default:
      eStatus = MB_EX_ILLEGAL_FUNCTION;
      break;
  }
  return eStatus;
}

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
#include "task.h"            /* FreeRTOS ( critical section functions use ) */
/*----------------------- Define --------------------------------------------------------------------*/
#define MB_HOLDING_REG_START                    0U
#define MB_COILS_REG_START                      0U
#define MB_INPUT_REG_START                      0U
#define MB_DI_REG_START                         0U
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
* @brief  Masking function.
* @param  pucFrame - pointer to static frame buffer
*         len      - pointer to static frame length buffer
* @retval function execution result
*/
MB_EXCEPTION eMBFuncMaskWriteRegister ( uint8_t* pucFrame, uint8_t* len )
{
  uint16_t     regAddress = 0U;
  uint16_t     usData     = 0U;
  uint16_t     andMask    = 0U;
  uint16_t     orMask     = 0U;
  MB_EXCEPTION status     = MB_EX_NONE;
  /* Address validation */
  regAddress = ( uint16_t )( pucFrame[STARTING_ADDRESS_HI] << 8U ) | ( uint16_t )pucFrame[STARTING_ADDRESS_LO];
  if ( regAddress < HR_REGISTER_COUNT )
  {
    if ( *len == MB_PDU_FUNC_MASK_WRITE_SIZE )
    {
      andMask = ( ( ( uint16_t ) pucFrame[AND_MASK_HI] ) << 8U ) |
                    ( uint16_t ) pucFrame[AND_MASK_LO];
      orMask  = ( ( ( uint16_t ) pucFrame[OR_MASK_HI] ) << 8U ) |
                    ( uint16_t ) pucFrame[OR_MASK_LO];
      eMBreadData( regAddress, &usData, 1U );
      usData = ( usData & andMask )  | ( orMask & ( uint16_t )( ~andMask ) );
      usData = ( usData >> 8U ) | ( usData << 8U );
      eMBwriteData( regAddress, &usData, 1U );
    }
    else
    {
      status = MB_EX_ILLEGAL_DATA_VALUE;
    }
  }
  else
  {
    status = MB_EX_ILLEGAL_DATA_ADDRESS;
  }
  return status;
}
#endif
/*---------------------------------------------------------------------------------------------------*/
#if ( MB_FUNC_WRITE_HOLDING_ENABLED )
/**
* @brief   Write holding register function.
* @param   pucFrame - pointer to static frame buffer
*         len    - pointer to static frame length buffer
* @retval function execution result
*/
MB_EXCEPTION eMBFuncWriteHoldingRegister ( uint8_t* pucFrame, uint8_t* len )
{
  uint16_t       regAddress = 0U;
  MB_EXCEPTION   status     = MB_EX_NONE;
  if ( *len == MB_PDU_FUNC_WRITE_SIZE )
  {
    regAddress = ( ( ( uint16_t )pucFrame[STARTING_ADDRESS_HI] ) << 8U ) |
                     ( uint16_t )pucFrame[STARTING_ADDRESS_LO];
    if ( eMBwriteData( ( uint8_t )regAddress, ( uint16_t* )( &pucFrame[3U] ), 1U ) != MB_DATA_STATUS_OK )
    {
      status = MB_EX_ILLEGAL_DATA_ADDRESS;
    }
  }
  else
  {
    status = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return status;
}
#endif
/*---------------------------------------------------------------------------------------------------*/
#if ( MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED )
/**
* @brief   Write multiple holding registers
* @param   pucFrame - pointer to static frame buffer
*         len    - pointer to static frame length buffer
* @retval function execution result
*/
MB_EXCEPTION eMBFuncWriteMultipleHoldingRegister( uint8_t* pucFrame, uint8_t* len )
{
  uint16_t     regAddress   = 0U;
  uint16_t     regCount     = 0U;
  uint8_t      regByteCount = 0U;
  MB_EXCEPTION status       = MB_EX_NONE;

  if ( *len >= ( MB_PDU_FUNC_WRITE_MUL_SIZE_MIN + MB_PDU_SIZE_MIN ) )
  {
    /* Address validation */
    regAddress = ( ( ( uint16_t )pucFrame[STARTING_ADDRESS_HI] ) << 8U ) |
                     ( uint16_t )pucFrame[STARTING_ADDRESS_LO];
    regCount   = ( ( ( uint16_t )pucFrame[QUANTITY_OF_OUTPUTS_HI] ) << 8U ) |
                     ( uint16_t )pucFrame[QUANTITY_OF_OUTPUTS_LO];
    if ( ( regCount + regAddress) <= OUTPUT_DATA_REGISTER_NUMBER )
    {
      regByteCount = pucFrame[BYTE_COUNT];
      if( ( regCount     >= 1U                               ) &&
          ( regCount     <= MB_PDU_FUNC_WRITE_MUL_REGCNT_MAX ) &&
          ( regByteCount == ( uint8_t ) ( 2U * regCount )    ) )
      {
        if ( eMBwriteData( regAddress, ( uint16_t* )( &pucFrame[MB_MULTI_DATA_START] ), ( regByteCount / 2U ) ) == MB_DATA_STATUS_OK )
        {
          *len = MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF;
        }
        else
        {
          status = MB_EX_ILLEGAL_DATA_ADDRESS;
        }
      }
      else
      {
        status = MB_EX_ILLEGAL_DATA_VALUE;
      }
    }
    else
    {
      status = MB_EX_ILLEGAL_DATA_ADDRESS;
    }
  }
  else
  {
    status = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return status;
}
#endif
/*---------------------------------------------------------------------------------------------------*/
#if ( MB_FUNC_READ_HOLDING_ENABLED )
/**
* @brief   Read holding register function.
* @param   pucFrame - pointer to static frame buffer
*         len    - pointer to static frame length buffer
* @retval function execution result
*/
MB_EXCEPTION eMBFuncReadHoldingRegister( uint8_t* pucFrame, uint8_t* len )
{
  uint16_t     regAddress   = 0U;          /* The Data Address of the first register requested */
  uint16_t     regCount     = 0U;          /* The total number of registers requested */
  MB_EXCEPTION status       = MB_EX_NONE;

  if ( *len == MB_PDU_FUNC_READ_SIZE )
  {
    regAddress = ( ( ( uint16_t )pucFrame[STARTING_ADDRESS_HI] )    << 8U ) |
                     ( uint16_t )pucFrame[STARTING_ADDRESS_LO];
    regCount   = ( ( ( uint16_t )pucFrame[QUANTITY_OF_OUTPUTS_HI] ) << 8U ) |
                     ( uint16_t )pucFrame[QUANTITY_OF_OUTPUTS_LO];
    if ( ( regCount + regAddress ) <= HR_REGISTER_COUNT )                        /* Register address validation */
    {
      if ( ( regCount >= 1U ) && ( regCount <= MB_PDU_FUNC_READ_REGCNT_MAX ) )
      {
        if ( eMBreadData( ( uint8_t )regAddress, ( uint16_t* )( &pucFrame[MB_PDU_DATA_OFF] ), regCount ) == MB_DATA_STATUS_OK )
        {
          pucFrame[MB_PDU_FUNC_OFF]   = MB_FUNC_READ_HOLDING_REGISTER; /* First byte contains the function code. */
          pucFrame[MB_PDU_LENGTH_OFF] = ( uint8_t )( regCount * 2U );  /* Second byte in the response contain the number of bytes. */
          *len = 2U + ( regCount * 2U );
        }
        else
        {
          status = MB_EX_ILLEGAL_DATA_ADDRESS;
        }
      }
      else
      {
        status = MB_EX_ILLEGAL_DATA_VALUE;
      }
    }
    else
    {
      status = MB_EX_ILLEGAL_DATA_ADDRESS;
    }
  }
  else
  {
    status = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return status;
}
#endif
/*---------------------------------------------------------------------------------------------------*/
#if ( MB_FUNC_READ_INPUT_ENABLED )
/**
* @brief   Read input register
* @param   pucFrame - pointer to static frame buffer
*         len    - pointer to static frame length buffer
* @retval function execution result
*/
MB_EXCEPTION eMBFuncReadInputRegister ( uint8_t* pucFrame, uint8_t* len )
{
  uint16_t     regAddress   = 0U;
  uint16_t     regCount     = 0U;
  uint16_t     data         = 0U;
  uint8_t      i            = 0U;
  MB_EXCEPTION status       = MB_EX_NONE;
  uint8_t      *pucFrameCur = NULL;

  if ( *len == MB_PDU_FUNC_READ_SIZE )
  {
    regAddress = ( ( ( uint16_t )pucFrame[1U] ) << 8U ) | ( uint16_t )pucFrame[2U];
    regCount   = ( ( ( uint16_t )pucFrame[3U] ) << 8U ) | ( uint16_t )pucFrame[4U];
    /* Address validation */
    if ( ( regAddress >= MB_INPUT_REG_START ) &&
         ( regAddress < ( MB_INPUT_REG_START + IR_REGISTER_COUNT ) ) &&
         ( ( regCount + regAddress ) <= ( MB_INPUT_REG_START + IR_REGISTER_COUNT) ) )
    {
      if ( ( regCount >= 1U )  && ( regCount <= MB_PDU_FUNC_READ_REGCNT_MAX ) )
      {
        pucFrameCur     = &pucFrame[MB_PDU_FUNC_OFF];      /* Set the current PDU data pointer to the beginning. */
        pucFrameCur[0U] = MB_FUNC_READ_INPUT_REGISTER;     /* First byte contains the function code. */
        pucFrameCur[1U] = ( uint8_t ) ( regCount * 2U );   /* Second byte in the response contain the number of bytes. */
        for ( i=0U; i < regCount; i++ )
        {
          eMBreadData( ( i + regAddress ), &data, 1U );
          pucFrameCur[2U + ( 2U * i )] = ( uint8_t )( data >> 8U );
          pucFrameCur[3U + ( 2U * i )] = ( uint8_t )( data & 0x00FFU );
        }
        *len = 2U + ( regCount * 2U );
      }
      else
      {
        status = MB_EX_ILLEGAL_DATA_VALUE;
      }
    }
    else
    {
      status = MB_EX_ILLEGAL_DATA_ADDRESS;
    }
  }
  else
  {
    status = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return status;
}
#endif
/*---------------------------------------------------------------------------------------------------*/
#if ( MB_FUNC_READ_DISCRETE_INPUTS_ENABLED )
/**
* @brief   Read bits from register
* @param   pucFrame - pointer to static frame buffer
*         len    - pointer to static frame length buffer
* @retval function execution result
*/
MB_EXCEPTION eMBFuncReadDiscreteInputs( uint8_t* pucFrame, uint8_t* len )
{
  uint16_t     regAddress   = 0U;
  uint16_t     regCount     = 0U;
  uint8_t      *pucFrameCur = NULL;
  MB_EXCEPTION status       = MB_EX_NONE;

  if ( *len == MB_PDU_FUNC_READ_SIZE )
  {
    regAddress = ( ( ( uint16_t )pucFrame[1U] ) << 8U) | ( uint16_t )pucFrame[2U];
    regCount   = ( ( ( uint16_t )pucFrame[3U] ) << 8U) | ( uint16_t )pucFrame[4U];
    /* Address validation */
    if ( ( regAddress >= MB_DI_REG_START) &&
         ( regAddress < ( MB_DI_REG_START + DI_REG_COUNT ) ) &&
         ( ( regCount + regAddress ) <= ( MB_DI_REG_START + DI_REG_COUNT ) ) )
    {
      if ( ( regCount >= 1U ) && ( regCount <= MB_PDU_FUNC_READ_BITCNT_MAX ) )
      {
        pucFrameCur     = &pucFrame[MB_PDU_FUNC_OFF];      /* Set the current PDU data pointer to the beginning. */
        pucFrameCur[0U] = MB_FUNC_READ_DISCRETE_INPUTS;    /* First byte contains the function code. */
        if ( ( regCount & 0x07U ) > 0U )                          /* Second byte in the response contain the number of bytes. */
        {
          pucFrameCur[1U] = ( uint8_t )( ( regCount >> 3U ) + 1U );
          *len  = 2U + ( uint8_t )( ( regCount >> 3U ) + 1U );
        }
        else
        {
          pucFrameCur[1U] = ( uint8_t )( regCount >> 3U );
          *len  = 2U + ( uint8_t )( regCount >> 3U );
        }
        vMBreadBitData ( regAddress, pucFrameCur , regCount, DISCRET_INPUT );
      }
      else
      {
        status = MB_EX_ILLEGAL_DATA_VALUE;
      }
    }
    else
    {
      status = MB_EX_ILLEGAL_DATA_ADDRESS;
    }
  }
  else
  {
    status = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return status;
}
#endif
/*---------------------------------------------------------------------------------------------------*/
#if ( MB_FUNC_READ_COILS_ENABLED )
/**
* @brief   Read coil register function
* @param   pucFrame - pointer to static frame buffer
*         len    - pointer to static frame length buffer
* @retval function execution result
*/
MB_EXCEPTION eMBFuncReadCoils( uint8_t* pucFrame, uint8_t* len )
{
  uint16_t     regAddress  = 0U;
  uint16_t     regCount    = 0U;
  uint8_t*     pucFrameCur = NULL;
  MB_EXCEPTION status      = MB_EX_NONE;

  if ( *len == MB_PDU_FUNC_READ_SIZE )
  {
    regAddress = ( ( ( uint16_t )pucFrame[1U] ) << 8U ) | ( uint16_t )pucFrame[2U];
    regCount   = ( ( ( uint16_t )pucFrame[3U] ) << 8U ) | ( uint16_t )pucFrame[4U];
    /* Address validation */
    if ( ( regAddress >= MB_COILS_REG_START ) &&
         ( regAddress < ( MB_COILS_REG_START + CR_COILS_COUNT ) ) &&
         ( ( regCount + regAddress ) <= ( MB_COILS_REG_START + CR_COILS_COUNT ) ) )
    {
      if ( ( regCount >= 1U ) && ( regCount <= MB_PDU_FUNC_READ_BITCNT_MAX ) )
      {
        pucFrameCur     = &pucFrame[MB_PDU_FUNC_OFF];      /* Set the current PDU data pointer to the beginning. */
        pucFrameCur[0U] = MB_FUNC_READ_COILS;              /* First byte contains the function code. */
        if ( ( regCount & 0x07U ) > 0U )                  /* Second byte in the response contain the number of bytes. */
        {
          pucFrameCur[1U] = ( uint8_t )( ( regCount >> 3U ) + 1U );
          *len  = 2U + ( uint8_t )( ( regCount >> 3U ) + 1U );
        }
        else
        {
          pucFrameCur[1U] = ( uint8_t )( regCount >> 3U );
          *len            = 2U + pucFrameCur[1U];
        }
        vMBreadBitData ( regAddress, pucFrameCur , regCount, COIL ) ;
      }
      else
      {
        status = MB_EX_ILLEGAL_DATA_VALUE;
      }
    }
    else
    {
      status = MB_EX_ILLEGAL_DATA_ADDRESS;
    }
  }
  else
  {
    status = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return status;
}
#endif
/*---------------------------------------------------------------------------------------------------*/
#if ( MB_FUNC_WRITE_COIL_ENABLED )
/**
* @brief   Write coil register function
* @param   pucFrame - pointer to static frame buffer
*         len    - pointer to static frame length buffer
* @retval function execution result
*/
MB_EXCEPTION eMBFuncWriteCoil( uint8_t* pucFrame, uint8_t* len )
{
  uint16_t     regAddress = 0U;
  uint16_t     output     = 0U;
  MB_EXCEPTION status     = MB_EX_NONE;

  output = ( uint16_t )( pucFrame[OUTPUT_VALUE_HI] << 8U ) | ( uint16_t )( pucFrame[OUTPUT_VALUE_LO] );                          // Проверка валидности выходных данных
  if ( ( *len == MB_PDU_FUNC_WRITE_SIZE ) &&
       ( ( output == 0x0000U ) || ( output == 0xFF00U ) ) )
  {
    regAddress = ( uint16_t )( pucFrame[STARTING_ADDRESS_HI] << 8U ) | ( uint16_t )( pucFrame[STARTING_ADDRESS_LO] );              // Проверка валидности адресса
    if ( ( regAddress >= MB_COILS_REG_START ) &&
         ( regAddress < ( MB_COILS_REG_START + CR_COILS_COUNT ) ) )
    {
      taskENTER_CRITICAL();
      vMBwriteCoil( regAddress, output );
      taskEXIT_CRITICAL();
    }
    else
    {
      status = MB_EX_ILLEGAL_DATA_ADDRESS;
    }
  }
  else
  {
    status = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return status;
}
#endif
/*---------------------------------------------------------------------------------------------------*/
#if ( MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED )
/**
* @brief   Write multiple coil registers function
* @param   pucFrame - pointer to static frame buffer
*         len    - pointer to static frame length buffer
* @retval function execution result
*/
MB_EXCEPTION eMBFuncWriteMultipleCoils( uint8_t* pucFrame, uint8_t* len )
{
  uint16_t     regAddress     = 0U;
  uint16_t     regCount       = 0U;
  uint16_t     ucRegByteCount = 0U;
  MB_EXCEPTION status         = MB_EX_NONE;

  regCount = ( ( ( uint16_t )pucFrame[QUANTITY_OF_OUTPUTS_HI]) << 8U ) | ( uint16_t )( pucFrame[QUANTITY_OF_OUTPUTS_LO] );
  /* Address validation */
  ucRegByteCount = regCount >> 3U;        // Согласно требования протокола, вычисляем необходимое кол-во байт для передчи
  if ( ( regCount & 0x0007U ) > 0U )
  {
    ucRegByteCount++;                       //Указанного в PDU количества регистров
  }
  if ( ( *len >= ( MB_PDU_FUNC_WRITE_MUL_SIZE_MIN + MB_PDU_SIZE_MIN ) ) &&
     ( ( regCount >= 1U ) &&
       ( regCount <= MB_PDU_FUNC_READ_BITCNT_MAX ) &&
       ( pucFrame[BYTE_COUNT] == ucRegByteCount) ) )
  {
    /* Address validation */
    regAddress = ( uint16_t )( pucFrame[STARTING_ADDRESS_HI] << 8U ) | ( uint16_t )( pucFrame[STARTING_ADDRESS_LO] );
    if ( ( regAddress >= MB_COILS_REG_START ) &&
         ( regAddress < ( MB_COILS_REG_START + CR_COILS_COUNT ) ) &&
         ( ( regCount + regAddress ) <= ( MB_COILS_REG_START + CR_COILS_COUNT ) ) )
    {
      eMBwriteData( regAddress, ( uint16_t* )&pucFrame[6U], regCount );
      *len = MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF;
    }
    else
    {
      status = MB_EX_ILLEGAL_DATA_ADDRESS;
    }
  }
  else
  {
    status = MB_EX_ILLEGAL_DATA_VALUE;
  }
  return status;
}
#endif
/*---------------------------------------------------------------------------------------------------*/
#if ( MB_FUNC_DIAGNOSTICS_ENEBLED )
/**
* @brief   Diagnostics  function
* @param   pucFrame - pointer to static frame buffer
*         len    - pointer to static frame length buffer
* @retval function execution result
*/
MB_EXCEPTION eMBFuncDiagnostics( uint8_t* pucFrame, uint8_t* len )
{
  uint16_t        SubFunctionCode = 0U;
  MB_EXCEPTION  status         = MB_EX_NONE;

  SubFunctionCode = ( uint16_t )( pucFrame[SUB_FUNCTION_HI] << 8U ) | ( uint16_t )( pucFrame[SUB_FUNCTION_LO] );
  switch ( SubFunctionCode )
  {
    case  RETURN_QUERY_DATA:
      break;
    case  RESTART_COMUNICATION_OPTION:
      status = MB_EX_REINIT;
      break;
    case  RETURN_DIAGNOSTIC_REGISTER:
      break;
    case  FORCE_LISTEN_ONLY_MODE:
      vMBListenOnlyModeEnablae();
      break;
    default:
      status = MB_EX_ILLEGAL_FUNCTION;
      break;
  }
  return status;
}
#endif
/*---------------------------------------------------------------------------------------------------*/
#if ( MB_FUNC_SET_RS_PARAMETERS_ENEBLED )
/**
* @brief   Change UART parameters function
*
* @param   pucFrame - pointer to static frame buffer
*         len    - pointer to static frame length buffer
* @retval function execution result
*/
MB_EXCEPTION eMBFuncSetRSParameters( uint8_t* pucFrame, uint8_t* len )
{
  MB_EXCEPTION status          = MB_EX_REINIT;
  uint16_t     subFunctionCode = 0U;
  uint8_t      data            = 0U;

  subFunctionCode = ( uint16_t )( pucFrame[SUB_FUNCTION_HI] << 8U ) | ( uint16_t )( pucFrame[SUB_FUNCTION_LO] );
  data = pucFrame[SUB_FUNCTION_PARAM];
  switch ( subFunctionCode )
  {
    case SET_BOUNDRATE:
      // Проверяем, что заничение скорости предачи валидно, если да, то переписыаем новое значение в регистр инициализации и устанавливае флаг
      // по которому параметры канала будут измнены в оснвоном цикле
      if ( IS_MB_BAUD_RATE( subFunctionCode ) > 0U )
      {
        vMBwriteShortToUserRegister( USER_REGISTER_BOUND_RATE_ADR, data, ROM_WR );
        status = MB_EX_REINIT;
      }
      else
      {
        status = MB_EX_ILLEGAL_DATA_VALUE;
      }
      break;
    case  SET_PARITY:
      if ( IS_MB_PARITY( subFunctionCode ) > 0U )
      {
        vMBwriteShortToUserRegister( USER_REGISTER_PARITY_ADR, data, ROM_WR );
        status = MB_EX_REINIT;
      }
      else
      {
        status = MB_EX_ILLEGAL_DATA_VALUE;
      }
      break;
    case SET_NET_ADR:
      if ( IS_MB_ADDRESS( subFunctionCode ) > 0U )
      {
        vMBSetAdrReg( data );
        status = MB_EX_NONE;
      }
      else
      {
        status = MB_EX_ILLEGAL_DATA_VALUE;
      }
      break;
    default:
      status = MB_EX_ILLEGAL_FUNCTION;
      break;
  }
  return status;
}
#endif

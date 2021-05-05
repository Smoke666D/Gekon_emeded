/*
 * mbregister.c
 *
 *  Created on: 14 нояб. 2019 г.
 *      Author: mikhail.mikhailov
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "mbregister.h"
#include "mbcrc.h"
#include "mbuart.h"
/*----------------------- Structures ----------------------------------------------------------------*/
static  EventGroupHandle_t       xEventGroups[OS_MB_EVENT_ARRAY_SIZE];
/*----------------------- Variables -----------------------------------------------------------------*/
static uint16_t   REGISTER [REGISTER_COUNT + 2U];
/*----------------------- Constant ------------------------------------------------------------------*/
#if HR_REGISTER_COUNT  > 0
  const uint16_t HoldingRegisterDefault[HR_REGISTER_COUNT] = REGISTER_DEFAULT_VAL;
#endif
#if DI_REGISTER_COUNT  > 0
  const uint16_t DIRegisterDefault[DI_REGISTER_COUNT] = { 0x0000U, 0x7733U, 0xF0F0U };
#endif
#if CR_REGISTER_COUNT  > 0
  const uint16_t CRRegisterDefault[CR_REGISTER_COUNT] = { 0x0000U, 0x7733U, 0xF0F0U, 0x7733U, 0x0F0FU, 0x0000U, 0x0000U} ;
#endif
#if USER_REGISTER_COUNT  > 0
  const uint16_t USERRegisterDefault[USER_REGISTER_COUNT] = { DEFINE_BAUD_RATE, DEFINE_PARITY, DEFINE_ADDRESS };
#endif
/*----------------------- Functions -----------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Initialization of event group array
* @param   none
* @retval status of initialization
*/
MB_INIT_STATE eMBeventGroupInit ( void )
{
  uint32_t       i   = 0U;
  MB_INIT_STATE   res = EB_INIT_OK;

  for( i=0; i<OS_MB_EVENT_ARRAY_SIZE; i++)
  {
    xEventGroups[i] = xEventGroupCreate();
    if ( xEventGroups[i] == NULL )
    {
      res = EB_INIT_ERROR;
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Set bit in event group array
* @param   Address of bit
* @retval The value of the event group before
*         the specified bits were cleared
*/
EventBits_t eMBeventSet( uint8_t adr)
{
  uint8_t     group  = 0U;
  uint8_t     shift  = 0U;
  EventBits_t uxBits = 0U;

  group = ( uint8_t )( adr / OS_MB_EVENT_GROUP_SIZE );
  shift = adr - ( group * OS_MB_EVENT_GROUP_SIZE );
  if ( xEventGroups[group] != NULL)
  {
    uxBits = xEventGroupSetBits( xEventGroups[group], ( 1UL << shift ) );
  }
  return uxBits;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Reset bit in event group array
* @param   Address of bit
* @retval The value of the event group before
*         the specified bits were cleared
*/
EventBits_t eMBeventReset ( uint8_t adr)
{
  uint8_t     group  = 0U;
  uint8_t     shift  = 0U;
  EventBits_t uxBits = 0U;

  group = ( uint8_t )( adr / OS_MB_EVENT_GROUP_SIZE );
  shift = adr - ( group * OS_MB_EVENT_GROUP_SIZE );
  if ( xEventGroups[group] != NULL)
  {
    uxBits = xEventGroupClearBits( xEventGroups[group], ( 1UL << shift ) );
  }
  return uxBits;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Reset bit in event group array
* @param   Address of bit
* @retval The value of the event group before
*         the specified bits were cleared
*/
void vMBeventFullReset ( void )
{
  uint8_t  i    = 0U;
  uint16_t mask = 0U;

  for( i=0U; i<OS_MB_EVENT_GROUP_SIZE; i++ )
  {
    mask |= 1U << i;
  }
  for( i=0U; i<OS_MB_EVENT_ARRAY_SIZE; i++ )
  {
    if ( xEventGroups[i] != NULL)
    {
      xEventGroupClearBits( xEventGroups[i], mask );
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Get event group array
* @param   Pointer for reading data
* @retval none
*/
void vMBeventGet ( EventBits_t* uxBits )
{
  uint8_t i = 0U;
  for( i=0; i < OS_MB_EVENT_ARRAY_SIZE; i++ )
  {
    if ( xEventGroups[i] != NULL)
    {
      uxBits[i] = xEventGroupGetBits( xEventGroups[i] );
    }
  }

  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Write data from buffer REGISTER to ROM
* @param   none
* @retval none
*/
void vMBwriteROMRegister ( void )
{
  uint16_t  BufferData[ ROM_REGISTER_COUNT + 2U ] = { 0U };
  uint32_t  i = 0U;

  BufferData[1U] = ROM_REGISTER_COUNT;
  for ( i=0U; i<REGISTER_COUNT; i++ )
  {
    #if ( HR_REGISTER_COUNT > 0U )
      if ( ( i >= ( HR_REGISTER_ADR_START + HR_RAM_COUNT ) ) &&  ( i < DI_REGISTER_ADR_START ) )
      {
        BufferData[2U + i - HR_ROM_OFF] = REGISTER[i];
      }
    #endif
    #if ( CR_REGISTER_COUNT > 0U )
      if ( ( i >= ( CR_REGISTER_ADR_START + CR_RAM_COUNT ) ) &&  ( i < USER_REGISTER_ADR_START ) )
      {
        BufferData[2U + i - CR_ROM_OFF] = REGISTER[i];
      }
    #endif
    #if ( USER_REGISTER_COUNT > 0U )
      if ( i >= ( USER_REGISTER_ADR_START + USER_RAM_COUNT ) )
      {
        BufferData[2U + i - USER_ROM_OFF] = REGISTER[i];
      }
    #endif
  }
  BufferData[0U] = usMBCRC16( ( uint8_t* )( &BufferData[1U] ), ( ROM_REGISTER_COUNT + 1U ) );
  //eMBPageWrite( 0U, ( uint8_t* )BufferData, ( ROM_REGISTER_COUNT + 2U ) );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
MB_ROM_INIT_STATE eMBregisterCheck ( eMBUartBaudRate *baudRate, eMBUartConfig *parity, uint8_t *adr )
{
  uint16_t         rAdr = USER_REGISTER_ADR_START + USER_RAM_COUNT - USER_ROM_OFF + 2U;
  MB_ROM_INIT_STATE   res  = MB_ROM_INIT_FAIL;
  #if ( USER_REGISTER_COUNT  > 0U )
    *baudRate = uMBreadROMShort( rAdr + USER_REGISTER_BOUND_RATE_ADR );
    *parity   = uMBreadROMShort( rAdr + USER_REGISTER_PARITY_ADR );
    *adr      = uMBreadROMShort( rAdr + USER_REGISTER_ADDRESS_ADR );
  #endif
  if ( !IS_MB_BAUD_RATE( *baudRate ) || !IS_MB_PARITY( *parity ) || !IS_MB_ADDRESS( *adr ) )
  {
    res = MB_ROM_INIT_FAIL;
  }
  else
  {
    res = MB_ROM_INIT_OK;
  }
  return res;
}






/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Initialization and checking ROM.
*         If it invalid set as default
* @param   none
* @retval 0 - ROM invalid, set define; 1 - read data from ROM
*/
MB_ROM_INIT_STATE eMBregisterInit ( void )
{
  uint32_t  i   = 0U;
  uint16_t  res = 0U;
  vMBeventFullReset();
  if ( eMBcheckROM() != ROM_OK )   /* If ROM is invalid -> set it as define */
  {
    res = MB_ROM_INIT_FAIL;
    for ( i=0U; i<REGISTER_COUNT; i++)
    {
      #if ( HR_REGISTER_COUNT  > 0U )
        if ( (i < ( HR_REGISTER_ADR_START + HR_REGISTER_COUNT ) ) && (i >= HR_REGISTER_ADR_START ) )
        {
          REGISTER[i] = HoldingRegisterDefault[i - HR_REGISTER_ADR_START];
        }
      #endif
      #if ( CR_REGISTER_COUNT > 0U )
        if ( (i < ( CR_REGISTER_ADR_START + CR_REGISTER_COUNT ) ) &&  (i >= CR_REGISTER_ADR_START ) )
        {
          REGISTER[i] = CRRegisterDefault[i - CR_REGISTER_ADR_START];
        }
      #endif
      #if ( USER_REGISTER_COUNT > 0U )
        if ( (i < ( USER_REGISTER_ADR_START + USER_REGISTER_COUNT) ) && (i >= USER_REGISTER_ADR_START ) )
        {
          REGISTER[i] = USERRegisterDefault[i - USER_REGISTER_ADR_START];
        }
      #endif
    }
    vMBwriteROMRegister();
  }
  else
  {
    res = MB_ROM_INIT_OK;
  }
  for (i=0U; i<REGISTER_COUNT; i++)
  {
    #if ( HR_REGISTER_COUNT > 0U )
      if ( (i >= ( HR_REGISTER_ADR_START + HR_RAM_COUNT ) ) && ( i < DI_REGISTER_ADR_START ) )
      {
        REGISTER[i] = uMBreadROMShort( i - HR_ROM_OFF + 2U );
      }
      else
    #endif
    #if ( CR_REGISTER_COUNT  > 0U )
      if ( ( i >= ( CR_REGISTER_ADR_START + CR_RAM_COUNT ) ) && ( i < USER_REGISTER_ADR_START ) )
      {
        REGISTER[i] = uMBreadROMShort( i - CR_ROM_OFF + 2U );
      }
      else
    #endif
    #if ( USER_REGISTER_COUNT  > 0U )
      if ( i >= ( USER_REGISTER_ADR_START + USER_RAM_COUNT ) )
      {
        REGISTER[i] = uMBreadROMShort( i - USER_ROM_OFF + 2U );
      }
      else
    #endif
    REGISTER[i] = 0U;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

MB_DATA_STATUS eMBreadData ( uint16_t adr, uint16_t* data, uint16_t length )
{
  MB_DATA_STATUS res = MB_DATA_STATUS_OK;
  return res;
}

MB_DATA_STATUS eMBwriteData ( uint16_t adr, const uint16_t* data, uint16_t length )
{
  MB_DATA_STATUS res = MB_DATA_STATUS_OK;
  return res;
}

/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Read data from user register
* @param   adr   - address of the user register
* @retval data from user register
*/
uint16_t uMBreadUserRegister ( uint16_t adr )
{
  return REGISTER[USER_REGISTER_ADR_START + adr];
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Write data to user registers
* @param   adr   - address of the user register
*         data  - A pointer to the writing data
*                 (char array, high bit is first)
*         count - length of data
* @retval none
*/
void vMBwriteUserRegister ( uint16_t adr, uint8_t* data, uint16_t length )
{
  uint16_t i = 0U;
  for ( i=0U; i<length; i++ )
  {
    REGISTER[USER_REGISTER_ADR_START + adr + i ] = ( ( ( uint16_t )data[i * 2U] ) << 8U ) |
                                                     ( ( uint32_t )data[( i * 2U ) + 1U] );
  }
  if ( (adr + length) > USER_RAM_COUNT )
  {
    vMBwriteROMRegister();
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Single write data to user registers
* @param   adr   - address of the user register
*         data  - the writing data (char
*                 array, high bit is first)
* @retval none
*/
void vMBwriteShortToUserRegister ( uint16_t adr, uint16_t data, eMBRAMwrType wr )
{
  REGISTER[USER_REGISTER_ADR_START + adr] = data;
  if ( ( adr >= USER_RAM_COUNT ) && ( wr == ROM_WR ) )
  {
    vMBwriteROMRegister();
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Write data to coil registers
* @param   adr   - address of the coil register
*         data  - A pointer to the writing data
*         count - length of data
* @retval none
*/
void vMBwriteCoils ( uint16_t adr, uint8_t* data, uint16_t count )
{
  uint8_t   register_offset   = 0U;
  uint8_t   current_offset    = 0U;
  uint8_t   data_mask         = 0U;
  uint16_t  current_adr       = 0U;
  uint16_t  i                 = 0U;
  uint8_t*  current_pointer   = NULL;

  current_adr         = adr;
  current_pointer     = ( uint8_t* ) & REGISTER[CR_REGISTER_ADR_START];
  current_pointer[0U] = ( adr >> 3U );
  for ( i=count; i>=0U; i=i-8U )
  {
    register_offset = current_adr & 0x0007U;
    if ( i >= 8U )
    {
      current_pointer[current_offset] = ( current_pointer[current_offset] & ( 0xFFU >> ( 8U - register_offset ) ) ) |
                                        ( data[current_offset] << register_offset );
      current_pointer[current_offset + 1U] = ( current_pointer[current_offset + 1U] & ( 0xFFU << ( register_offset + 1U ) ) ) |
                                             ( data[current_offset] >> ( 8U - register_offset ) );
      current_offset++;
      current_adr += 8U;
    }
    else
    {
      if ( i > ( 8U - register_offset ) )
      {
        current_pointer[current_offset] = ( current_pointer[current_offset] & ( 0xFFU >> ( 8U - register_offset ) ) ) |
                                          ( data[current_offset] << register_offset );
        current_pointer[current_offset + 1U] = ( current_pointer[current_offset + 1U] & ( 0xFFU << ( i - ( 8U - register_offset ) ) ) ) |
                                               ( data[current_offset] >> ( 8U - ( i - register_offset ) ) );
      }
      else
      {
        data_mask = ( 0xFFU << ( register_offset + i ) ) | ( 0xFFU >> ( 8U - register_offset ) );
        current_pointer[current_offset] = ( current_pointer[current_offset] & data_mask ) |
                                          ( ( data[current_offset] << register_offset ) & ( ( uint8_t )( ~data_mask ) ) ) ;
      }
    }
  }
  if ( ( ( adr + count ) >> 4U ) >= CR_RAM_COUNT )
  {
    vMBwriteROMRegister();
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Write single data to coil registers
* @param   adr   - address of the coil register
*         data  - the writing data
* @retval none
*/
void vMBwriteCoil ( uint16_t adr, uint16_t OutputValue )
{
  uint8_t   register_offset = 0U;
  uint8_t   data_mask       = 0U;
  uint8_t*  current_pointer = NULL;

  current_pointer     = ( uint8_t* ) & REGISTER[CR_REGISTER_ADR_START];
  current_pointer[0U] = ( adr >> 3U );
  register_offset     = adr & 0x0007U;
  data_mask           = ( 0xFFU << ( register_offset + 1U ) ) |
                        ( 0xFFU >> ( 8U - register_offset ) );
  if ( OutputValue > 0U )
  {
    current_pointer[0U] = ( current_pointer[0U] | ( ( uint8_t )( ~data_mask ) ) );
  }
  else
  {
    current_pointer[0U] = ( current_pointer[0U] & data_mask );
  }
  if ( ( adr >> 4U ) >= CR_RAM_COUNT )
  {
    vMBwriteROMRegister();
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Read bit data from registers
* @param   adr      - address of the register
*         data     - pointer to reading data buffer
*         count    - length of reading data
*         dataType - bit data type (COIL or DISCRET_INPUT)
* @retval none
*/
void vMBreadBitData ( uint16_t adr, uint8_t *data, uint16_t count, BIT_DATA_TYPE dataType )
{
  uint8_t   register_offset = 0U;
  uint8_t   current_offset  = 0U;
  uint16_t  current_adr     = 0U;
  uint16_t  i               = 0U;
  uint8_t*  current_pointer = NULL;

  current_adr = adr;
  switch ( dataType )
  {
    case DISCRET_INPUT:
      current_pointer = ( uint8_t* ) & REGISTER[DI_REGISTER_ADR_START];
      break;
    case COIL:
      current_pointer = ( uint8_t* ) & REGISTER[CR_REGISTER_ADR_START];
      break;
    default:
      break;
  }
  current_pointer[0U] = ( adr >> 3U ); /* Address of register with first bit */
  for ( i=count; i > 0U; i=i - 8U )
  {
    register_offset = current_adr & 0x0007U;
    data[current_offset] = ( current_pointer[current_offset] >> register_offset ) |
                           ( current_pointer[current_offset + 1U] << ( 8U - register_offset ) );
    current_offset++;
    current_adr += 8U;
    if ( i < 8U )                    /* Reset invalid bits, if number of bits multiple 8 */
    {
      data[current_offset] = data[current_offset] & ( 0xFFU >> i );
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/

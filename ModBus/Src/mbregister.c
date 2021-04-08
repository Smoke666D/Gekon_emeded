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
#include "mbmemory.h"
/*----------------------- Structures ----------------------------------------------------------------*/
static	EventGroupHandle_t 			xEventGroups[OS_MB_EVENT_ARRAY_SIZE];
/*----------------------- Variables -----------------------------------------------------------------*/
static USHORT   REGISTER [REGISTER_COUNT + 2U];
/*----------------------- Constant ------------------------------------------------------------------*/
#if HR_REGISTER_COUNT  > 0
  const USHORT HoldingRegisterDefault[HR_REGISTER_COUNT] = REGISTER_DEFAULT_VAL;
#endif
#if DI_REGISTER_COUNT  > 0
  const USHORT DIRegisterDefault[DI_REGISTER_COUNT] = { 0x0000U, 0x7733U, 0xF0F0U };
#endif
#if CR_REGISTER_COUNT  > 0
  const USHORT CRRegisterDefault[CR_REGISTER_COUNT] = { 0x0000U, 0x7733U, 0xF0F0U, 0x7733U, 0x0F0FU, 0x0000U, 0x0000U} ;
#endif
#if USER_REGISTER_COUNT  > 0
  const USHORT USERRegisterDefault[USER_REGISTER_COUNT] = { DEFINE_BAUD_RATE, DEFINE_PARITY, DEFINE_ADDRESS };
#endif
/*----------------------- Functions -----------------------------------------------------------------*/
USHORT 						usMBReadROMShort( USHORT addr );
CHECK_ROM_ERROR 	eMBCheckROM( void );
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Initialization of event group array
* @param 	none
* @retval status of initialization
*/
eMBInitState eMBEventGroupInit( void )
{
	ULONG 				i   = 0U;
	eMBInitState 	res = EBInit_OK;

	for( i=0; i < OS_MB_EVENT_ARRAY_SIZE; i++)
	{
		xEventGroups[i] = xEventGroupCreate();
		if ( xEventGroups[i] == NULL )
		{
			res = EBInit_ERROR;
		}
	}
	return res;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Set bit in event group array
* @param 	Address of bit
* @retval The value of the event group before
* 				the specified bits were cleared
*/
EventBits_t eMBEventSet( UCHAR adr)
{
	UCHAR 			group  = 0U;
	UCHAR 			shift  = 0U;
	EventBits_t uxBits = 0U;

	group = ( UCHAR )( adr / OS_MB_EVENT_GROUP_SIZE );
	shift = adr - ( group * OS_MB_EVENT_GROUP_SIZE );
	if ( xEventGroups[group] != NULL)
	{
		uxBits = xEventGroupSetBits( xEventGroups[group], ( 1UL << shift ) );
	}
	return uxBits;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Reset bit in event group array
* @param 	Address of bit
* @retval The value of the event group before
* 				the specified bits were cleared
*/
EventBits_t eMBEventReset( UCHAR adr)
{
	UCHAR 			group  = 0U;
	UCHAR 			shift  = 0U;
	EventBits_t uxBits = 0U;

	group = ( UCHAR )( adr / OS_MB_EVENT_GROUP_SIZE );
	shift = adr - ( group * OS_MB_EVENT_GROUP_SIZE );
	if ( xEventGroups[group] != NULL)
	{
		uxBits = xEventGroupClearBits( xEventGroups[group], ( 1UL << shift ) );
	}
	return uxBits;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Reset bit in event group array
* @param 	Address of bit
* @retval The value of the event group before
* 				the specified bits were cleared
*/
void vMBEventFullReset( void )
{
	UCHAR 	i = 0U;
	USHORT	mask = 0U;

	for( i=0U; i < OS_MB_EVENT_GROUP_SIZE; i++ )
	{
		mask |= 1U << i;
	}
	for( i=0U; i < OS_MB_EVENT_ARRAY_SIZE; i++ )
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
* @brief 	Get event group array
* @param 	Pointer for reading data
* @retval none
*/
void vMBEventGet( EventBits_t* uxBits )
{
	UCHAR i = 0U;

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
* @brief 	Read data from ROM
* @param 	address of data in memory
* @retval data unsigned short (uint16_t)
*/
USHORT usMBReadROMShort( USHORT addr )
{
  return usMBReadUint16( 0, ( addr * 2U ) );
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Check ROM for size and copy it to buffer REGISTER
* @param 	none
* @retval state of ROM
*/
CHECK_ROM_ERROR eMBCheckROM( void )
{
  CHECK_ROM_ERROR ErrorCode = ROM_OK;
  ULONG 					i 				= 0U;
  USHORT					calcCRC   = 0U;
  USHORT					readCRC   = 0U;
  USHORT					len				= 0U;

  len = usMBReadROMShort( ROM_REGISTER_COUNT_ADR );
  if ( len != ROM_REGISTER_COUNT )
  {
    ErrorCode = ROM_COUNT_ERORR;
  }
  else
  {
    for ( i=0; i < (ROM_REGISTER_COUNT + 1); i++ )
    {
      REGISTER[i] = usMBReadROMShort( i + ROM_REGISTER_DATA_OFS );
    }
    calcCRC = usMBCRC16( ( UCHAR* )( REGISTER ), ( ROM_REGISTER_COUNT + 1U ) );
    readCRC = usMBReadROMShort( ROM_REGISTER_CRC_ADR );
    if ( calcCRC != readCRC )
    {
      ErrorCode = ROM_CRC_ERROR;
    }
  }
  return ErrorCode;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Write data from buffer REGISTER to ROM
* @param 	none
* @retval none
*/
void vMBWriteROMRegister( void )
{
	USHORT	BufferData[ ROM_REGISTER_COUNT + 2U ];
	ULONG 	i = 0U;

	BufferData[1U] = ROM_REGISTER_COUNT;
  for ( i=0U; i < REGISTER_COUNT; i++ )
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
    vMBWatchDogReset();
  }
  BufferData[0U] = usMBCRC16( ( UCHAR* )( &BufferData[1U] ), ( ROM_REGISTER_COUNT + 1U ) );
  eMBPageWrite( 0U, ( UCHAR* )BufferData, ( ROM_REGISTER_COUNT + 2U ) );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
eMBROMInitType eMBRegisterCheck( eMBUartBaudRate *baudRate, eMBUartConfig *parity, UCHAR *adr )
{
	USHORT 					rAdr = USER_REGISTER_ADR_START + USER_RAM_COUNT - USER_ROM_OFF + 2U;
	eMBROMInitType 	res  = ROM_INIT_FAIL;

	vMBWatchDogReset();

	#if ( USER_REGISTER_COUNT  > 0U )
  	*baudRate = usMBReadROMShort( rAdr + USER_REGISTER_BOUND_RATE_ADR );
    *parity   = usMBReadROMShort( rAdr + USER_REGISTER_PARITY_ADR );
    *adr   		= usMBReadROMShort( rAdr + USER_REGISTER_ADDRESS_ADR );
  #endif
  if ( !IS_MB_BAUD_RATE( *baudRate ) || !IS_MB_PARITY( *parity ) || !IS_MB_ADDRESS( *adr ) )
  {
  	res = ROM_INIT_FAIL;
  }
  else
  {
  	res = ROM_INIT_OK;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Initialization and checking ROM.
* 				If it invalid set as default
* @param 	none
* @retval 0 - ROM invalid, set define; 1 - read data from ROM
*/
eMBROMInitType vMBRegisterInit( void )
{
	ULONG 	i 	= 0U;
	USHORT	res = 0U;

	vMBWatchDogReset();
	vMBEventFullReset();
  if ( eMBCheckROM() != ROM_OK ) 	/* If ROM is invalid -> set it as define */
  {
  	res = ROM_INIT_FAIL;
    for ( i=0U; i < REGISTER_COUNT; i++)
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
      vMBWatchDogReset();
    }
    vMBWriteROMRegister();
  }
  else
  {
  	res = ROM_INIT_OK;
  }
  vMBWatchDogReset();
  for (i=0U; i < REGISTER_COUNT; i++)
  {
    #if ( HR_REGISTER_COUNT > 0U )
      if ( (i >= ( HR_REGISTER_ADR_START + HR_RAM_COUNT ) ) && ( i < DI_REGISTER_ADR_START ) )
      {
        REGISTER[i] = usMBReadROMShort( i - HR_ROM_OFF + 2U );
      }
      else
    #endif
    #if ( CR_REGISTER_COUNT  > 0U )
      if ( ( i >= ( CR_REGISTER_ADR_START + CR_RAM_COUNT ) ) && ( i < USER_REGISTER_ADR_START ) )
      {
        REGISTER[i] = usMBReadROMShort( i - CR_ROM_OFF + 2U );
      }
      else
    #endif
    #if ( USER_REGISTER_COUNT  > 0U )
      if ( i >= ( USER_REGISTER_ADR_START + USER_RAM_COUNT ) )
      {
        REGISTER[i] = usMBReadROMShort( i - USER_ROM_OFF + 2U );
      }
      else
    #endif
    REGISTER[i] = 0U;
  }
  vMBWatchDogReset();
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Write data to the holding register.
* 			 	Generate flag of holding modification.
* @param 	adr   - address of the holding register
* 				data  - pointer to the writing data
* 				count - length of data
* @retval none
*/
void vMBWriteHolding( USHORT adr, UCHAR *data, USHORT count )
{
	USHORT i = 0U;

  for ( i=0U; i < count; i++ )
  {
    REGISTER[HR_REGISTER_ADR_START + adr + i ] = ( ( ( USHORT )data[i * 2U] ) << 8U ) | ( USHORT )data[( i * 2U ) + 1U];
  }
  vMBWatchDogReset();
  if ( ( adr + count ) > HR_RAM_COUNT )
  {
  	taskENTER_CRITICAL();
  	vMBWriteROMRegister();
  	taskEXIT_CRITICAL();
  }
  eMBEventSet( adr );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Write single unsigned short (uint16_t) data
* 				to the holding register. Generate flag of
* 				holding modification
* @param 	adr   - address of the holding register
* 				data  - data for writing
* @retval none
*/
void vMBWriteShortToHolding( USHORT adr, USHORT data )
{
  REGISTER[HR_REGISTER_ADR_START + adr] = data;
  if ( adr > HR_RAM_COUNT )
  {
  	vMBWriteROMRegister();
  }
  eMBEventSet( adr );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Read data from input register
* @param 	adr   - address of the input register
* @retval data from input register
*/
USHORT usMBReadInput( USHORT adr )
{
  return REGISTER[IR_REGISTER_ADR_START + adr];
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Read data from holding register
* @param 	adr   - address of the holding register
* @retval data from holding register
*/
USHORT usMBReadHolding( USHORT adr )
{
  return REGISTER[HR_REGISTER_ADR_START + adr];
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Read data from user register
* @param 	adr   - address of the user register
* @retval data from user register
*/
USHORT usMBReadUserRegister( USHORT adr )
{
  return REGISTER[USER_REGISTER_ADR_START + adr];
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Write data to user registers
* @param 	adr   - address of the user register
* 				data  - A pointer to the writing data
* 								(char array, high bit is first)
* 				count - length of data
* @retval none
*/
void vMBWriteUserRegister( USHORT adr, UCHAR *data, USHORT count )
{
	USHORT i = 0U;

  for ( i=0U; i < count; i++ )
  {
    REGISTER[USER_REGISTER_ADR_START + adr + i ] = ( ( ( USHORT )data[i * 2U] ) << 8U ) |
    		                                             ( ( ULONG )data[( i * 2U ) + 1U] );
    vMBWatchDogReset();
  }
  if ( (adr + count) > USER_RAM_COUNT )
  {
  	vMBWriteROMRegister();
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Single write data to user registers
* @param 	adr   - address of the user register
* 				data  - the writing data (char
* 								array, high bit is first)
* @retval none
*/
void vMBWriteShortToUserRegister( USHORT adr, USHORT data, eMBRAMwrType wr )
{
  REGISTER[USER_REGISTER_ADR_START + adr] = data;
  if ( ( adr >= USER_RAM_COUNT ) && ( wr == ROM_WR ) )
  {
  	vMBWriteROMRegister();
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Write data to coil registers
* @param 	adr   - address of the coil register
* 				data  - A pointer to the writing data
* 				count - length of data
* @retval none
*/
void vMBWriteCoils( USHORT adr, UCHAR *data, USHORT count )
{
	UCHAR 	register_offset 	= 0U;
	UCHAR 	current_offset 		= 0U;
	UCHAR 	data_mask 				= 0U;
	USHORT 	current_adr 			= 0U;
	USHORT	i 								= 0U;
	UCHAR 	*current_pointer;

  current_adr      		= adr;
  current_pointer  		= ( UCHAR* ) & REGISTER[CR_REGISTER_ADR_START];
  current_pointer[0U] = ( adr >> 3U );
  for ( i=count; i >= 0U; i=i-8U )
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
        																	( ( data[current_offset] << register_offset ) & ( ( UCHAR )( ~data_mask ) ) ) ;
      }
    }
  }
  if ( ( ( adr + count ) >> 4U ) >= CR_RAM_COUNT )
  {
  	vMBWriteROMRegister();
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Write single data to coil registers
* @param 	adr   - address of the coil register
* 				data  - the writing data
* @retval none
*/
void vMBWriteCoil( USHORT adr, USHORT OutputValue )
{
	UCHAR 	register_offset 	= 0U;
	UCHAR 	data_mask 				= 0U;
	UCHAR 	*current_pointer;

  current_pointer			= ( UCHAR* ) & REGISTER[CR_REGISTER_ADR_START];
  current_pointer[0U] = ( adr >> 3U );
  register_offset  		= adr & 0x0007U;
  data_mask 					= ( 0xFFU << ( register_offset + 1U ) ) |
  											( 0xFFU >> ( 8U - register_offset ) );
  if ( OutputValue > 0U )
  {
    current_pointer[0U] = ( current_pointer[0U] | ( ( UCHAR )( ~data_mask ) ) );
  }
  else
  {
    current_pointer[0U] = ( current_pointer[0U] & data_mask );
  }
  vMBWatchDogReset();
  if ( ( adr >> 4U ) >= CR_RAM_COUNT )
  {
  	vMBWriteROMRegister();
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Read bit data from registers
* @param 	adr   	 - address of the register
* 				data  	 - pointer to reading data buffer
* 				count 	 - length of reading data
* 				dataType - bit data type (COIL or DISCRET_INPUT)
* @retval none
*/
void vMBReadBitData( USHORT adr, UCHAR *data, USHORT count, BIT_DATA_TYPE dataType )
{
  UCHAR 	register_offset 	= 0U;
  UCHAR  	current_offset 		= 0U;
  USHORT 	current_adr 			= 0U;
  USHORT 	i 								= 0U;
  UCHAR 	*current_pointer;

  current_adr = adr;
  switch ( dataType )
  {
    case DISCRET_INPUT:
      current_pointer = ( UCHAR* ) & REGISTER[DI_REGISTER_ADR_START];
      break;
    case COIL:
      current_pointer = ( UCHAR* ) & REGISTER[CR_REGISTER_ADR_START];
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
    if ( i < 8U )										/* Reset invalid bits, if number of bits multiple 8 */
    {
      data[current_offset] = data[current_offset] & ( 0xFFU >> i );
    }
  }
  vMBWatchDogReset();
  return;
}
/*---------------------------------------------------------------------------------------------------*/

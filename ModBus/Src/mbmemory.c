/*
 * mbmemory.c
 *
 *  Created on: 14 нояб. 2019 г.
 *      Author: mikhail.mikhailov
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "mbport.h"
/*----------------------- Variables -----------------------------------------------------------------*/
static uint8_t  mbPage = 63U;
#if defined(FLASH_BANK2_END)
static uint32_t  mbBank = FLASH_BANK_2;
#else
static uint32_t  mbBank = FLASH_BANK_1;
#endif
/*----------------------- Functions -----------------------------------------------------------------*/
static uint16_t usMBReadFromFlashUint16(uint32_t adr);
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Setup the page of flash for storege of ModBus
* @param 	page - ModBus flash page
* 				bank - ModBus flash bank
* @retval none
*/
eMBInitState ucMBSetMemory( uint8_t page, uint32_t bank )
{
	eMBInitState res = EBInit_ERROR;

	if ( IS_FLASH_NB_PAGES( 0U, page ) > 0U )
	{
		mbPage = page;
		mbBank = bank;
		res = EBInit_OK;
	}
	return res;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Hardware level reading function of 16 bit data
* @param 	adr  - address of uint16 data in memory
* @retval data from memory
*/
uint16_t usMBReadFromFlashUint16(uint32_t adr)
{
	assert_param( IS_FLASH_PROGRAM_ADDRESS( adr ) );			/* Check address */
	return ( uint16_t )( ( *( __IO uint32_t* ) adr ) );
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Read data directly from memory
* @param 	page - page number of virtual section
* 				adr  - address of uint16 data in page
* @retval data from memory
*/
USHORT usMBReadUint16( USHORT page, USHORT adr )
{
	uint8_t	absPage = ( mbPage + page );

	assert_param( IS_FLASH_NB_PAGES( adr, absPage ) );		/* Check address */
	return usMBReadFromFlashUint16( ( absPage * FLASH_PAGE_SIZE ) + adr );
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief 	Write data to page of memory
* @param 	page  - page number of virtual section
* 				data  - data of full page
* 				count - length of data
* @retval result of writing
*/
eMBROMprocType eMBPageWrite( UCHAR page, UCHAR* data, USHORT count )
{
	uint32_t 								pageError = 0U;
	uint32_t								pageAdr		= 0U;
	uint16_t								output		= 0U;
	uint16_t								i         = 0U;
	uint8_t									absPage   = ( mbPage + page );
	eMBROMprocType					res 			= ROM_FAIL;
	FLASH_EraseInitTypeDef 	eraseInit;


	/* Page address validation */
	pageAdr		= absPage * FLASH_PAGE_SIZE;
	assert_param( IS_FLASH_NB_PAGES( 0U, absPage ) );
	/* Setup erase structure */
	eraseInit.Banks       = mbBank;
	eraseInit.TypeErase   = FLASH_TYPEERASE_PAGES; 	/* */
	eraseInit.NbPages     = 1U;
	eraseInit.PageAddress = pageAdr;
	/* Writing */
	if ( HAL_FLASH_Unlock() == HAL_OK )
	{
		if ( HAL_FLASHEx_Erase( &eraseInit, &pageError ) == HAL_OK )
		{
			for ( i=0U; i < ( 2U * count ); i+=2U )
			{
				output = ( uint16_t )( data[i + 1U] << 8U ) | data[i];
				HAL_FLASH_Program( FLASH_TYPEPROGRAM_HALFWORD, ( pageAdr + i ), output );
			}
			if ( HAL_FLASH_Lock() == HAL_OK )
			{
				res = ROM_DONE;
			}
		}
		else
		{
			HAL_FLASH_Lock();
		}
	}
	return res;
}
/*---------------------------------------------------------------------------------------------------*/

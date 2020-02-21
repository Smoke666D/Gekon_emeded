/*
 * rest.c
 *
 *  Created on: 19 февр. 2020 г.
 *      Author: mikhail.mikhailov
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "rest.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
/*----------------------- Structures ----------------------------------------------------------------*/

/*----------------------- Constant ------------------------------------------------------------------*/

/*----------------------- Variables -----------------------------------------------------------------*/

/*----------------------- Functions -----------------------------------------------------------------*/
uint8_t 	uRESTmakeStartRecord( char* output, const char* header );
uint8_t 	uRESTmakeDigRecord( char* output, const char* header, uint32_t data );
uint8_t 	uRESTmakeStrRecord( char* output, const char* header, char* data, uint8_t dataLen );

uint32_t 	uRESTmakeConfig( char* output, eConfigReg* reg );
uint32_t 	uRESTmakeBitMap( char* output, eConfigBitMap* bitMap, uint8_t last );
uint32_t  uRESTmakeBitMapArray( char* output, uint8_t len, eConfigBitMap* bitMap );
/*---------------------------------------------------------------------------------------------------*/
uint32_t uRESTmakeConfig( char* output, eConfigReg* reg )
{
	uint32_t 	position = 1U;

	output[0U] = '{';
	position += uRESTmakeDigRecord( &output[position], CONFIG_REG_PAGE_STR,  				reg->page );
	position += uRESTmakeDigRecord( &output[position], CONFIG_REG_ADR_STR, 	 				reg->adr );
	position += uRESTmakeDigRecord( &output[position], CONFIG_REG_SCALE_STR, 				reg->scale );
	position += uRESTmakeDigRecord( &output[position], CONFIG_REG_VALUE_STR, 				reg->value );
	position += uRESTmakeDigRecord( &output[position], CONFIG_REG_MIN_STR, 	 				reg->min );
	position += uRESTmakeDigRecord( &output[position], CONFIG_REG_MAX_STR, 	 				reg->max );
	position += uRESTmakeStrRecord( &output[position], CONFIG_REG_UNITS_STR, 				reg->units, 1U );
	position += uRESTmakeStrRecord( &output[position], CONFIG_REG_TYPE_STR,	 				&reg->type, MAX_UNITS_LENGTH );
	position += uRESTmakeDigRecord( &output[position], CONFIG_REG_LEN_STR, 	 				reg->len );
	position += uRESTmakeDigRecord( &output[position], CONFIG_REG_BIT_MAP_SIZE_STR, reg->bitMapSize );
	position += uRESTmakeBitMapArray( &output[position], reg->bitMapSize, reg->bitMap );
	position++;
	output[position] = '}';
	return position;
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uRESTmakeBitMap( char* output, eConfigBitMap* bitMap, uint8_t last )
{
	uint32_t 	position 	= 1U;
	uint32_t	result   	= 0U;
	char			buffer[2] = { '}', 0x00U };

	if ( strcpy( output, "{" ) != NULL )
	{
		position += uRESTmakeDigRecord( &output[position], CONFIG_REG_PAGE_STR, bitMap->mask );
		position += uRESTmakeDigRecord( &output[position], CONFIG_REG_PAGE_STR, bitMap->shift );
		position += uRESTmakeDigRecord( &output[position], CONFIG_REG_PAGE_STR, bitMap->min );
		position += uRESTmakeDigRecord( &output[position], CONFIG_REG_PAGE_STR, bitMap->max );
		if ( last == 0 )
		{
			buffer[1] = ',';
		}
		if ( strcat( output, buffer ) != NULL )
		{
			result = position;
		}
	}
	return result;
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uRESTmakeBitMapArray( char* output, uint8_t len, eConfigBitMap* bitMap )
{
	uint8_t 	i 				= 0U;
	uint32_t 	position 	= 1U;

	output[0U] = '[';
	for ( i=0U; i< ( len - 1U ); i++ )
	{
		position += uRESTmakeBitMap( &output[position], &bitMap[i], 0U );
	}
	position += uRESTmakeBitMap( &output[position], &bitMap[i], 1U );
	position++;
	output[position] = ']';

	return position;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uRESTmakeStartRecord( char* output, const char* header )
{
	uint8_t shift  = strlen( header );
	uint8_t i      = 0U;

	output[0U] = QUOTES_ANCII;
	for ( i=0U; i<shift; i++ )
	{
		output[i + 1U] = header[i];
	}
	output[shift + 1U] = QUOTES_ANCII;
	output[shift + 2U] = ':';
	output[shift + 3U] = ' ';
	return ( shift  + 4U );
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uRESTmakeDigRecord( char* output, const char* header, uint32_t data )
{
	uint8_t	shift  = uRESTmakeStartRecord( output, header );
	uint8_t	result = 0U;
	itoa( data, &output[shift], 10U );
	if ( strcat( output, "," ) != NULL )
	{
		result = shift + strlen( &output[shift] );
	}
	return result;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uRESTmakeStrRecord( char* output, const char* header, char* data, uint8_t dataLen )
{
	uint8_t	shift   = uRESTmakeStartRecord( output, header ) + 1U;
	uint8_t i       = 0U;

	output[shift] = QUOTES_ANCII;
	shift++;
	for ( i=0U; i<dataLen; i++ )
	{
		output[shift + i] = data[i];
	}
	shift += dataLen + 1U;
	output[shift] = QUOTES_ANCII;
	shift++;
	output[shift] = ',';
	shift++;
	return shift;
}
/*---------------------------------------------------------------------------------------------------*/

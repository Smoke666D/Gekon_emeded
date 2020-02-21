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
const char *restRequeststr[REST_REQUEST_NUMBER] = { REST_REQUEST_CONFIGS };
/*----------------------- Variables -----------------------------------------------------------------*/

/*----------------------- Functions -----------------------------------------------------------------*/
uint8_t 	uRESTmakeStartRecord( char* output, const char* header );
uint8_t 	uRESTmakeDigRecord( char* output, const char* header, uint32_t data, RESTrecordPos last );
uint8_t 	uRESTmakeStrRecord( char* output, const char* header, char* data, uint8_t dataLen, RESTrecordPos last );

uint32_t 	uRESTmakeConfig( char* output, eConfigReg* reg );
uint32_t 	uRESTmakeBitMap( char* output, eConfigBitMap* bitMap, RESTrecordPos last );
uint32_t  uRESTmakeBitMapArray( char* output, uint8_t len, eConfigBitMap* bitMap );
/*---------------------------------------------------------------------------------------------------*/
uint32_t uRESTmakeConfig( char* output, eConfigReg* reg )
{
	uint32_t 	position = 1U;

	output[0U] = '{';
	position += uRESTmakeDigRecord( &output[position], CONFIG_REG_PAGE_STR,  				reg->page, REST_CONT_RECORD );
	position += uRESTmakeDigRecord( &output[position], CONFIG_REG_ADR_STR, 	 				reg->adr, REST_CONT_RECORD );
	position += uRESTmakeDigRecord( &output[position], CONFIG_REG_SCALE_STR, 				reg->scale, REST_CONT_RECORD );
	position += uRESTmakeDigRecord( &output[position], CONFIG_REG_VALUE_STR, 				reg->value, REST_CONT_RECORD );
	position += uRESTmakeDigRecord( &output[position], CONFIG_REG_MIN_STR, 	 				reg->min, REST_CONT_RECORD );
	position += uRESTmakeDigRecord( &output[position], CONFIG_REG_MAX_STR, 	 				reg->max, REST_CONT_RECORD );
	position += uRESTmakeStrRecord( &output[position], CONFIG_REG_UNITS_STR, 				reg->units, MAX_UNITS_LENGTH, REST_CONT_RECORD );
	position += uRESTmakeStrRecord( &output[position], CONFIG_REG_TYPE_STR,	 				&reg->type, 1U, REST_CONT_RECORD );
	position += uRESTmakeDigRecord( &output[position], CONFIG_REG_LEN_STR, 	 				reg->len, REST_CONT_RECORD );
	position += uRESTmakeDigRecord( &output[position], CONFIG_REG_BIT_MAP_SIZE_STR, reg->bitMapSize, REST_CONT_RECORD );
	position += uRESTmakeBitMapArray( &output[position], reg->bitMapSize, reg->bitMap );
	position++;
	output[position] = '}';
	position++;
	return position;
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uRESTmakeBitMap( char* output, eConfigBitMap* bitMap, RESTrecordPos last )
{
	uint32_t 	position 	= 1U;

	output[0U] = '{';
	position += uRESTmakeDigRecord( &output[position], BIT_MAP_MASK_STR, 	bitMap->mask, REST_CONT_RECORD );
	position += uRESTmakeDigRecord( &output[position], BIT_MAP_SHIFT_STR, bitMap->shift, REST_CONT_RECORD );
	position += uRESTmakeDigRecord( &output[position], BIT_MAP_MIN_STR, 	bitMap->min, REST_CONT_RECORD );
	position += uRESTmakeDigRecord( &output[position], BIT_MAP_MAX_STR, 	bitMap->max, REST_LAST_RECORD );
	output[position] = '}';
	position++;
	if ( last == REST_CONT_RECORD )
	{
		output[position] = ',';
		position++;
	}
	return position;
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uRESTmakeBitMapArray( char* output, uint8_t len, eConfigBitMap* bitMap )
{
	uint8_t 	i 				= 0U;
	uint32_t 	position 	= 0U;

	position += uRESTmakeStartRecord( output, CONFIG_REG_BIT_MAP_STR );
	output[position] = '[';
	position++;
	for ( i=0U; i<( len - 1U ); i++ )
	{
		position += uRESTmakeBitMap( &output[position], &bitMap[i], REST_CONT_RECORD );
	}
	position += uRESTmakeBitMap( &output[position], &bitMap[i], REST_LAST_RECORD );
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
uint8_t uRESTmakeDigRecord( char* output, const char* header, uint32_t data, RESTrecordPos last )
{
	uint8_t	shift  = uRESTmakeStartRecord( output, header );

	itoa( data, &output[shift], 10U );
	shift += strlen( &output[shift] );
	if ( last == REST_CONT_RECORD )
	{
		output[shift] = ',';
		shift++;
	}
	return shift;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uRESTmakeStrRecord( char* output, const char* header, char* data, uint8_t dataLen, RESTrecordPos last )
{
	uint8_t	shift   = uRESTmakeStartRecord( output, header );
	uint8_t i       = 0U;

	output[shift] = QUOTES_ANCII;
	shift++;
	for ( i=0U; i<dataLen; i++ )
	{
		output[shift + i] = data[i];
	}
	shift += dataLen;
	output[shift] = QUOTES_ANCII;
	shift++;
	if ( last == REST_CONT_RECORD )
	{
		output[shift] = ',';
		shift++;
	}
	return shift;
}
/*---------------------------------------------------------------------------------------------------*/

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
uint8_t 	uRESTmakeDigRecord( char* output, const char* header, uint16_t data, RESTrecordPos last );
uint8_t 	uRESTmakeSignedRecord( char* output, const char* header, signed char data, RESTrecordPos last );
uint8_t 	uRESTmakeStrRecord( char* output, const char* header, char* data, uint8_t dataLen, RESTrecordPos last );

uint32_t 	uRESTmakeConfig( char* output, eConfigReg* reg );
uint32_t 	uRESTmakeBitMap( char* output, eConfigBitMap* bitMap, RESTrecordPos last );
uint32_t  uRESTmakeBitMapArray( char* output, uint8_t len, eConfigBitMap* bitMap );

REST_ERROR eRESTpareingRecord(  const char* input, const char* header, char* data );
REST_ERROR eRESTparsingDigRecord( const char* input, const char* header, uint16_t data );
REST_ERROR eRESTparsingSignedRecord( const char* input, const char* header, signed char data );
REST_ERROR eRESTparsingStrRecord( const char* input, const char* header, char* data, uint8_t length );
REST_ERROR eRESTparsingBitMapArray( const char* input, const char* header, eConfigBitMap* bitMap, uint8_t size );
/*---------------------------------------------------------------------------------------------------*/
REST_ADDRESS eRESTgetRequest( char* path, REST_REQUEST* request, uint16_t* adr )
{
	char*   			pchStr 		 = NULL;
	uint8_t 			i      		 = 0U;
	REST_ADDRESS 	res 	 		 = REST_NO_ADR;
	uint32_t			length     = 0U;
	uint8_t				p1       	 = 0U;
	uint8_t				j        	 = 0U;
	char					buffer[5U] = { 0x00U, 0x00U, 0x00U, 0x00U, 0x00U };

	while( pchStr == NULL )
	{
		pchStr = strstr( path, restRequeststr[i] );
		i++;
		if ( i >= REST_REQUEST_NUMBER )
		{
			break;
		}
	}
	if ( pchStr != NULL )
	{
		*request = i - 1U;
		length  = strlen( pchStr );
		for( i=0U; i<length; i++ )
		{
			if ( p1 > 0U )
			{
				buffer[j] = pchStr[i];
				j++;
			}
			if ( pchStr[i] == '/' )
			{
				if ( p1 == 0U )
				{
					p1 = i;
				}
			}
		}
		if ( buffer[0U] > 0U )
		{
			*adr = atoi( buffer );
			res = REST_YES_ADR;
		}
	}
	else
	{
		*request = REST_REQUEST_ERROR;
	}

	return res;
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uRESTmakeConfig( char* output, eConfigReg* reg )
{
	uint32_t 	position = 1U;

	output[0U] = '{';
	position += uRESTmakeDigRecord( &output[position], 		CONFIG_REG_PAGE_STR,  				reg->page, 	REST_CONT_RECORD );
	position += uRESTmakeDigRecord( &output[position], 		CONFIG_REG_ADR_STR, 	 				reg->adr, 	REST_CONT_RECORD );
	position += uRESTmakeSignedRecord( &output[position], CONFIG_REG_SCALE_STR, 				reg->scale, REST_CONT_RECORD );
	position += uRESTmakeDigRecord( &output[position], 		CONFIG_REG_VALUE_STR, 				reg->value, REST_CONT_RECORD );
	position += uRESTmakeDigRecord( &output[position], 		CONFIG_REG_MIN_STR, 	 				reg->min, 	REST_CONT_RECORD );
	position += uRESTmakeDigRecord( &output[position], 		CONFIG_REG_MAX_STR, 	 				reg->max, 	REST_CONT_RECORD );
	position += uRESTmakeStrRecord( &output[position], 		CONFIG_REG_UNITS_STR, 				reg->units, MAX_UNITS_LENGTH, REST_CONT_RECORD );
	position += uRESTmakeStrRecord( &output[position], 		CONFIG_REG_TYPE_STR,	 				&reg->type, 1U, REST_CONT_RECORD );
	position += uRESTmakeDigRecord( &output[position], 		CONFIG_REG_LEN_STR, 	 				reg->len, 	REST_CONT_RECORD );
	position += uRESTmakeDigRecord( &output[position], 		CONFIG_REG_BIT_MAP_SIZE_STR, 	reg->bitMapSize, REST_CONT_RECORD );
	position += uRESTmakeBitMapArray( &output[position], reg->bitMapSize, reg->bitMap );
	position++;
	output[position] = '}';
	position++;
	return position;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTparsingConfig( char* input, eConfigReg* reg )
{
	REST_ERROR 	res = REST_OK;
	char*				pchSt;

	pchSt = strchr( input, '{' );
	if (pchSt != NULL)
	{
		res = eRESTparsingDigRecord( input, CONFIG_REG_PAGE_STR, reg->page );
		if ( res == REST_OK )
		{
			res = eRESTparsingDigRecord( input, CONFIG_REG_ADR_STR, reg->adr );
			if ( res == REST_OK )
			{
				res = eRESTparsingSignedRecord( input, CONFIG_REG_SCALE_STR, reg->scale );
				if (res == REST_OK )
				{
					res = eRESTparsingDigRecord( input, CONFIG_REG_VALUE_STR, reg->value );
					if (res == REST_OK )
					{
						res = eRESTparsingDigRecord( input, CONFIG_REG_MIN_STR, reg->min );
						if (res == REST_OK )
						{
							res = eRESTparsingDigRecord( input, CONFIG_REG_MAX_STR, reg->max );
							if (res == REST_OK )
							{
								res = eRESTparsingStrRecord( input, CONFIG_REG_UNITS_STR, reg->units, MAX_UNITS_LENGTH );
								if ( res == REST_OK )
								{
									res = eRESTparsingStrRecord( input, CONFIG_REG_TYPE_STR, &reg->type, 1U );
									if ( res == REST_OK )
									{
										res = eRESTparsingDigRecord( input, CONFIG_REG_LEN_STR, reg->len );
										if ( ( res == REST_OK ) && ( reg->len > 0 ) )
										{
											res = eRESTparsingBitMapArray( input, CONFIG_REG_BIT_MAP_SIZE_STR, reg->bitMap, reg->bitMapSize );
										}
									}
								}
							}
						}
					}
				}
			}
		}
	} else res = REST_MESSAGE_FORMAT_ERROR;

	return res;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTpareingRecord(  const char* input, const char* header, char* data )
{
	REST_ERROR 	res = REST_OK;
	char*				pchSt;
	char*				pchEn;
	char*				pchQ;
	uint8_t			calcLength = 0U;

	pchSt = strstr( input, header );
	if ( pchSt != NULL )
	{
		pchEn = strchr( pchSt, ',' );
		if ( pchEn == NULL )
		{
			pchEn = strchr( pchSt, '}' );
		}
		if ( pchEn != NULL )
		{
			pchSt += strlen( header ) + 2U;
			if ( pchSt[0] == '"' )
			{
				pchSt += 1U;
				pchEn -= 1U;
			}
			calcLength = pchEn - pchSt;
			if ( calcLength > 0 )
			{
				if ( strncpy( data, pchSt, calcLength ) == NULL )
				{
					res = REST_RECORD_NO_DATA_ERROR;
				}
			} else res = REST_RECORD_NO_DATA_ERROR;
		} else res = REST_RECORD_FORMAT_ERROR;
	} else res = REST_RECORD_LOST_ERROR;

	return res;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTparsingBitMapArray( const char* input, const char* header, eConfigBitMap* bitMap, uint8_t size )
{
	REST_ERROR 			res = REST_OK;
	uint8_t					i   = 0U;
	eConfigBitMap*	bitPointer;
	char*						pchSt;
	char* 					pchAr;

	pchSt = strstr( input, header );
	if ( pchSt != NULL )
	{
		pchSt = strchr( pchSt, '[' );
		if ( pchSt != NULL )
		{
			pchAr = pchSt;
			if ( strchr( pchSt, ']' ) != NULL )
			{
				for ( i=0U; i<size; i++ )
				{
					pchAr = strchr( pchAr, '{' );
					if ( pchAr != NULL )
					{
						bitPointer = &bitMap[i];
						pchAr += 1U;
						res = eRESTparsingDigRecord( pchAr, BIT_MAP_MASK_STR, bitPointer->mask );
						if ( res == REST_OK )
						{
							res = eRESTparsingDigRecord( pchAr, BIT_MAP_SHIFT_STR, bitPointer->shift );
							if ( res == REST_OK )
							{
								res = eRESTparsingDigRecord( pchAr, BIT_MAP_MIN_STR, bitPointer->min );
								if ( res == REST_OK )
								{
									res = eRESTparsingDigRecord( pchAr, BIT_MAP_MAX_STR, bitPointer->max );
									if ( res != REST_OK )
									{
										break;
									}
								} else break;
							} else break;
						} else break;
					} else break;
				}
			} else res = REST_RECORD_ARRAY_FORMAT_ERROR;
		} else res = REST_RECORD_ARRAY_FORMAT_ERROR;
	} else res = REST_RECORD_LOST_ERROR;
	return res;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTparsingStrRecord( const char* input, const char* header, char* data, uint8_t length )
{
	REST_ERROR 	res        = REST_OK;
	uint8_t			calcLength = 0U;
	char				buffer[6U] = { 0U, 0U, 0U, 0U, 0U, 0U };

	res = eRESTpareingRecord( input, header, buffer );
	if ( res == REST_OK )
	{
		calcLength = strlen( buffer );
		if ( calcLength <= length )
		{
			if( strncpy( data, buffer, calcLength ) == NULL )
			{
				res = REST_RECORD_COPY_ERROR;
			}
		} else res = REST_RECORD_LENGTH_ERROR;
	}

	return res;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTparsingDigRecord( const char* input, const char* header, uint16_t data )
{
	REST_ERROR 	res    = REST_OK;
	char*				pchStr = NULL;
	uint8_t			round  = 0U;
	char				buffer[REST_DIGIT_BUFFER_SIZE];

	res = eRESTpareingRecord( input, header, buffer );
	if ( res == REST_OK )
	{
		pchStr = strchr( buffer, '.' );
		if ( pchStr != NULL )
		{
			if ( pchStr[1] > 5U )
			{
				round = 1U;
			}
			pchStr[0] = 0U;
		}
		data = atoi( buffer ) + round;
	}

	return res;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTparsingSignedRecord( const char* input, const char* header, signed char data )
{
	REST_ERROR 	res = REST_OK;
	char				buffer[6U];

	res = eRESTpareingRecord( input, header, buffer );
	if ( res == REST_OK )
	{
		data = atoi( buffer );
	}

	return res;
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
	if ( len > 0)
	{
		for ( i=0U; i<( len - 1U ); i++ )
		{
			position += uRESTmakeBitMap( &output[position], &bitMap[i], REST_CONT_RECORD );
		}
		position += uRESTmakeBitMap( &output[position], &bitMap[i], REST_LAST_RECORD );
	}
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
	//output[shift + 3U] = ' ';
	return ( shift  + 3U );
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uRESTmakeSignedRecord( char* output, const char* header, signed char data, RESTrecordPos last )
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
uint8_t uRESTmakeDigRecord( char* output, const char* header, uint16_t data, RESTrecordPos last )
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

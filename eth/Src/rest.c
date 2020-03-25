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
#include "fix16.h"
/*----------------------- Structures ----------------------------------------------------------------*/

/*----------------------- Constant ------------------------------------------------------------------*/
const char *restRequeststr[REST_REQUEST_NUMBER] = { REST_REQUEST_CONFIGS, REST_REQUEST_CHARTS };
/*----------------------- Variables -----------------------------------------------------------------*/

/*----------------------- Functions -----------------------------------------------------------------*/
uint8_t    uRESTmakeStartRecord( char* output, const char* header );
uint8_t    uRESTmakeDigRecord( char* output, const char* header, uint16_t data, RESTrecordPos last );
uint8_t    uRESTmake16FixDigRecord( char* output, const char* header, fix16_t data, RESTrecordPos last );
uint8_t    uRESTmakeSignedRecord( char* output, const char* header, signed char data, RESTrecordPos last );
uint8_t    uRESTmakeStrRecord( char* output, const char* header, char* data, uint8_t dataLen, RESTrecordPos last );

uint32_t   uRESTmakeConfig( char* output, eConfigReg* reg );
uint32_t   uRESTmakeBitMap( char* output, eConfigBitMap* bitMap, RESTrecordPos last );
uint32_t   uRESTmakeBitMapArray( char* output, uint8_t len, eConfigBitMap* bitMap );


uint32_t   uRESTmakeDotArray( char* output, uint16_t len, eChartDot* dot );
uint32_t   uRESTmakeDot( char* output, eChartDot* dot, RESTrecordPos last );

REST_ERROR eRESTpareingRecord(  const char* input, const char* header, char* data );
REST_ERROR eRESTparsingDig16Record( const char* input, const char* header, uint16_t* data );
REST_ERROR eRESTparsingDig8Record( const char* input, const char* header, uint8_t* data );
REST_ERROR eRESTparsingSignedRecord( const char* input, const char* header, signed char* data );
REST_ERROR eRESTparsingStrRecord( const char* input, const char* header, char* data, uint8_t length );
REST_ERROR eRESTparsingBitMapArray( const char* input, const char* header, eConfigBitMap* bitMap, uint8_t size );
/*---------------------------------------------------------------------------------------------------*/
REST_ADDRESS eRESTgetRequest( char* path, REST_REQUEST* request, uint16_t* adr )
{
  char*         pchStr     = NULL;
  uint8_t       i          = 0U;
  REST_ADDRESS  res        = REST_NO_ADR;
  uint32_t      length     = 0U;
  uint8_t       p1         = 0U;
  uint8_t       j          = 0U;
  char          buffer[5U] = { 0x00U, 0x00U, 0x00U, 0x00U, 0x00U };

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
uint32_t uRESTmakeChart( char* output, eChartData* chart )
{
  uint32_t position = 1U;

  output[0U] = '{';
  position += uRESTmake16FixDigRecord( &output[position], CHART_DATA_XMIN_STR,  chart->xmin,  REST_CONT_RECORD );
  position += uRESTmake16FixDigRecord( &output[position], CHART_DATA_XMAX_STR,  chart->xmax,  REST_CONT_RECORD );
  position += uRESTmake16FixDigRecord( &output[position], CHART_DATA_YMIN_STR,  chart->ymin,  REST_CONT_RECORD );
  position += uRESTmake16FixDigRecord( &output[position], CHART_DATA_YMAX_STR,  chart->ymax,  REST_CONT_RECORD );
  position += uRESTmakeStrRecord(      &output[position], CHART_DATA_XUNIT_STR, chart->xunit, CHART_UNIT_LENGTH, REST_CONT_RECORD );
  position += uRESTmakeStrRecord(      &output[position], CHART_DATA_YUNIT_STR, chart->yunit, CHART_UNIT_LENGTH, REST_CONT_RECORD );
  position += uRESTmakeDigRecord(      &output[position], CHART_DATA_SIZE_STR,  chart->size,  REST_CONT_RECORD );
  position += uRESTmakeDotArray(       &output[position], chart->size, chart->dots );
  position++;
  output[position] = '}';
  position++;
  return position;
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uRESTmakeDotArray( char* output, uint16_t len, eChartDot* dot )
{
  uint32_t position = 0;
  uint8_t  i        = 0;

  position += uRESTmakeStartRecord( output, CHART_DATA_DOTS_STR );
  output[position] = '[';
  position++;
  if ( len > 0)
  {
    for ( i=0U; i<( len - 1U ); i++ )
    {
      position += uRESTmakeDot( &output[position], &dot[i], REST_CONT_RECORD );
    }
    position += uRESTmakeDot( &output[position], &dot[i], REST_LAST_RECORD );
  }
  output[position] = ']';

  return position;
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uRESTmakeDot( char* output, eChartDot* dot, RESTrecordPos last )
{
  uint32_t position = 1U;

  output[0U] = '{';
  position += uRESTmake16FixDigRecord( &output[position], CHART_DOT_X_STR, dot->x, REST_CONT_RECORD );
  position += uRESTmake16FixDigRecord( &output[position], CHART_DOT_Y_STR, dot->y, REST_LAST_RECORD );
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
/*
 * Make response content for GET request
 * input:		output - output string buffer for content
 * 					reg    - pointer to the configuration register
 * output:	length of the content
 */
uint32_t uRESTmakeConfig( char* output, eConfigReg* reg )
{
  uint32_t position = 1U;

  output[0U] = '{';
  position += uRESTmakeDigRecord( &output[position],    CONFIG_REG_PAGE_STR,          reg->page,  REST_CONT_RECORD );
  position += uRESTmakeDigRecord( &output[position],    CONFIG_REG_ADR_STR,           reg->adr,   REST_CONT_RECORD );
  position += uRESTmakeSignedRecord( &output[position], CONFIG_REG_SCALE_STR,         reg->scale, REST_CONT_RECORD );
  position += uRESTmakeDigRecord( &output[position],    CONFIG_REG_VALUE_STR,         reg->value, REST_CONT_RECORD );
  position += uRESTmakeDigRecord( &output[position],    CONFIG_REG_MIN_STR,           reg->min,   REST_CONT_RECORD );
  position += uRESTmakeDigRecord( &output[position],    CONFIG_REG_MAX_STR,           reg->max,   REST_CONT_RECORD );
  position += uRESTmakeStrRecord( &output[position],    CONFIG_REG_UNITS_STR,         reg->units, MAX_UNITS_LENGTH, REST_CONT_RECORD );
  position += uRESTmakeStrRecord( &output[position],    CONFIG_REG_TYPE_STR,          &reg->type, 1U, REST_CONT_RECORD );
  position += uRESTmakeDigRecord( &output[position],    CONFIG_REG_LEN_STR,           reg->len,   REST_CONT_RECORD );
  position += uRESTmakeDigRecord( &output[position],    CONFIG_REG_BIT_MAP_SIZE_STR,  reg->bitMapSize, REST_CONT_RECORD );
  position += uRESTmakeBitMapArray( &output[position], reg->bitMapSize, reg->bitMap );
  position++;
  output[position] = '}';
  position++;
  return position;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTparsingConfig( char* input, eConfigReg* reg )
{
  REST_ERROR res = REST_OK;
  char*      pchSt;

  pchSt = strchr( input, '{' );
  if (pchSt != NULL)
  {
    res = eRESTparsingDig16Record( input, CONFIG_REG_PAGE_STR, &reg->page );
    if ( res == REST_OK )
    {
      res = eRESTparsingDig16Record( input, CONFIG_REG_ADR_STR, &reg->adr );
      if ( res == REST_OK )
      {
        res = eRESTparsingSignedRecord( input, CONFIG_REG_SCALE_STR, &reg->scale );
        if (res == REST_OK )
        {
          res = eRESTparsingDig16Record( input, CONFIG_REG_VALUE_STR, &reg->value );
          if (res == REST_OK )
          {
            res = eRESTparsingDig16Record( input, CONFIG_REG_MIN_STR, &reg->min );
            if (res == REST_OK )
            {
              res = eRESTparsingDig16Record( input, CONFIG_REG_MAX_STR, &reg->max );
              if (res == REST_OK )
              {
                res = eRESTparsingStrRecord( input, CONFIG_REG_UNITS_STR, reg->units, MAX_UNITS_LENGTH );
                if ( res == REST_OK )
                {
                  res = eRESTparsingStrRecord( input, CONFIG_REG_TYPE_STR, &reg->type, 1U );
                  if ( res == REST_OK )
                  {
                    res = eRESTparsingDig8Record( input, CONFIG_REG_LEN_STR, &reg->len );
                    if ( ( res == REST_OK ) && ( reg->len > 0 ) )
                    {
                      res = eRESTparsingBitMapArray( input, CONFIG_REG_BIT_MAP_SIZE_STR, &reg->bitMap[0], reg->bitMapSize );
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
  REST_ERROR  res = REST_OK;
  char*       pchSt;
  char*       pchEn;
  uint8_t     calcLength = 0U;
  uint8_t     i = 0U;

  for( i=0; i<REST_DIGIT_BUFFER_SIZE; i++ )
  {
    data[i] = 0;
  }
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
        if ( strncpy( data, pchSt, calcLength ) != NULL )
        {
          data[calcLength] = 0U;
        } else res = REST_RECORD_NO_DATA_ERROR;
      } else res = REST_RECORD_NO_DATA_ERROR;
    } else res = REST_RECORD_FORMAT_ERROR;
  } else res = REST_RECORD_LOST_ERROR;

  return res;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTparsingBitMapArray( const char* input, const char* header, eConfigBitMap* bitMap, uint8_t size )
{
  REST_ERROR     res = REST_OK;
  uint8_t        i   = 0U;
  eConfigBitMap* bitPointer;
  char*          pchSt;
  char*          pchAr;

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
            res = eRESTparsingDig16Record( pchAr, BIT_MAP_MASK_STR, &bitPointer->mask );
            if ( res == REST_OK )
            {
              res = eRESTparsingDig8Record( pchAr, BIT_MAP_SHIFT_STR, &bitPointer->shift );
              if ( res == REST_OK )
              {
                res = eRESTparsingDig8Record( pchAr, BIT_MAP_MIN_STR, &bitPointer->min );
                if ( res == REST_OK )
                {
                  res = eRESTparsingDig8Record( pchAr, BIT_MAP_MAX_STR, &bitPointer->max );
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
  REST_ERROR  res        = REST_OK;
  uint8_t     calcLength = 0U;
  char        buffer[6U] = { 0U, 0U, 0U, 0U, 0U, 0U };

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
REST_ERROR eRESTparsingDig16Record( const char* input, const char* header, uint16_t* data )
{
  REST_ERROR  res    = REST_OK;
  char*       pchStr = NULL;
  uint8_t     round  = 0U;
  char        buffer[REST_DIGIT_BUFFER_SIZE];

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
    *data = ( uint16_t )( atoi( buffer ) + round );
  }

  return res;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTparsingDig8Record( const char* input, const char* header, uint8_t* data )
{
  REST_ERROR  res    = REST_OK;
  char*       pchStr = NULL;
  uint8_t     round  = 0U;
  char        buffer[REST_DIGIT_BUFFER_SIZE];

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
    *data = ( uint8_t )( atoi( buffer ) + round );
  }

  return res;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTparsingSignedRecord( const char* input, const char* header, signed char* data )
{
  REST_ERROR  res = REST_OK;
  char        buffer[6U];

  res = eRESTpareingRecord( input, header, buffer );
  if ( res == REST_OK )
  {
    *data = ( signed char )( atoi( buffer ) );
  }

  return res;
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uRESTmakeBitMap( char* output, eConfigBitMap* bitMap, RESTrecordPos last )
{
  uint32_t position = 1U;

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
  uint8_t  i        = 0U;
  uint32_t position = 0U;

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
  return ( shift  + 3U );
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uRESTmakeSignedRecord( char* output, const char* header, signed char data, RESTrecordPos last )
{
  uint8_t shift  = uRESTmakeStartRecord( output, header );

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
  uint8_t shift = uRESTmakeStartRecord( output, header );

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
uint8_t uRESTmake16FixDigRecord( char* output, const char* header, fix16_t data, RESTrecordPos last )
{
  uint8_t shift = uRESTmakeStartRecord( output, header );

  itoa( (uint32_t)data, &output[shift], 10U );
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
	char    buf[2]  = {0x00, 0x00};

	output[shift] = QUOTES_ANCII;
	shift++;
	output[shift + 0U] = '%';
	output[shift + 1U] = 'D';
	output[shift + 2U] = '0';
	output[shift + 3U] = '%';
	shift += 4U;
	for ( i=0U; i<dataLen; i++ )
	{
	  itoa((uint8_t)data[i], buf, 16U);
	  output[shift + 2*i + 0U] = buf[0];
	  output[shift + 2*i + 1U] = buf[1];
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

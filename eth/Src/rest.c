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
#include "common.h"
/*----------------------- Structures ----------------------------------------------------------------*/

/*----------------------- Constant ------------------------------------------------------------------*/
const char *restRequeststr[REST_REQUEST_NUMBER] = { REST_REQUEST_CONFIGS, REST_REQUEST_CHARTS, REST_REQUEST_SAVE_CONFIGS };
/*----------------------- Variables -----------------------------------------------------------------*/

/*----------------------- Functions -----------------------------------------------------------------*/
uint8_t    uRESTmakeStartRecord ( const char* header, char* output );
uint8_t    uRESTmakeDigRecord ( const char* header, uint16_t data, RESTrecordPos last, char* output );
uint8_t    uRESTmakeValueRecord ( const char* header, uint16_t* data, uint16_t len, uint16_t type, RESTrecordPos last, char* output );
uint8_t    uRESTmake16FixDigRecord ( const char* header, fix16_t data, RESTrecordPos last, char* output );
uint8_t    uRESTmakeSignedRecord ( const char* header, signed char data, RESTrecordPos last, char* output );
uint8_t    uRESTmakeStrRecord ( const char* header, uint16_t* data, uint8_t dataLen, RESTrecordPos last, char* output );
uint32_t   uRESTmakeBitMap ( eConfigBitMap* bitMap, RESTrecordPos last, char* output );
uint32_t   uRESTmakeBitMapArray ( uint8_t len, eConfigBitMap* bitMap, char* output );
uint32_t   uRESTmakeDotArray ( const eChartDot* dot, uint16_t len,char* output );
uint32_t   uRESTmakeDot ( const eChartDot* dot, RESTrecordPos last, char* output );
REST_ERROR eRESTpareingRecord (  const char* input, const char* header, char* data );
REST_ERROR eRESTparsingValueRecord ( const char* input, const char* header, uint16_t type, uint16_t* data, uint16_t len );
REST_ERROR eRESTparsingDig16Record ( const char* input, const char* header, uint16_t* data );
REST_ERROR uRESTparsing16FixDigRecord ( const char* input, const char* header, fix16_t* data );
REST_ERROR eRESTparsingDig8Record ( const char* input, const char* header, uint8_t* data );
REST_ERROR eRESTparsingSignedRecord ( const char* input, const char* header, signed char* data );
REST_ERROR eRESTparsingStrRecord ( const char* input, const char* header, uint16_t* data, uint8_t length );
REST_ERROR eRESTparsingBitMapArray ( const char* input, const char* header, eConfigBitMap* bitMap, uint8_t size );
REST_ERROR eRESTparsingDotArray ( const char* input, const char* header, eChartDot* dot, uint8_t size );
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
      *adr = strtol( buffer, NULL, 10U );
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
  position += uRESTmake16FixDigRecord( CHART_DATA_XMIN_STR,  chart->xmin,  REST_CONT_RECORD, &output[position] );
  position += uRESTmake16FixDigRecord( CHART_DATA_XMAX_STR,  chart->xmax,  REST_CONT_RECORD, &output[position] );
  position += uRESTmake16FixDigRecord( CHART_DATA_YMIN_STR,  chart->ymin,  REST_CONT_RECORD, &output[position] );
  position += uRESTmake16FixDigRecord( CHART_DATA_YMAX_STR,  chart->ymax,  REST_CONT_RECORD, &output[position] );
  position += uRESTmakeStrRecord(      CHART_DATA_XUNIT_STR, chart->xunit, CHART_UNIT_LENGTH, REST_CONT_RECORD, &output[position] );
  position += uRESTmakeStrRecord(      CHART_DATA_YUNIT_STR, chart->yunit, CHART_UNIT_LENGTH, REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord(      CHART_DATA_SIZE_STR,  chart->size,  REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDotArray(                             chart->dots,  chart->size,      &output[position] );
  position++;
  output[position] = '}';
  position++;
  return position;
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uRESTmakeDotArray( const eChartDot* dot, uint16_t len, char* output )
{
  uint32_t position = 0;
  uint8_t  i        = 0;

  position += uRESTmakeStartRecord( CHART_DATA_DOTS_STR, output );
  output[position] = '[';
  position++;
  if ( len > 0U)
  {
    for ( i=0U; i<( len - 1U ); i++ )
    {
      position += uRESTmakeDot( &dot[i], REST_CONT_RECORD, &output[position] );
    }
    position += uRESTmakeDot( &dot[i], REST_LAST_RECORD, &output[position] );
  }
  output[position] = ']';
  return position;
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uRESTmakeDot( const eChartDot* dot, RESTrecordPos last, char* output )
{
  uint32_t position = 1U;

  output[0U] = '{';
  position += uRESTmake16FixDigRecord( CHART_DOT_X_STR, dot->x, REST_CONT_RECORD, &output[position] );
  position += uRESTmake16FixDigRecord( CHART_DOT_Y_STR, dot->y, REST_LAST_RECORD, &output[position] );
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
 * input:  output - output string buffer for content
 *         reg    - pointer to the configuration register
 * output: length of the content
 */
uint32_t uRESTmakeConfig( eConfigReg* reg, char* output )
{
  uint32_t position = 1U;

  output[0U] = '{';
  position += uRESTmakeDigRecord( CONFIG_REG_ADR_STR,           reg->atrib->adr,   REST_CONT_RECORD, &output[position] );
  position += uRESTmakeSignedRecord( CONFIG_REG_SCALE_STR,      reg->scale,        REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord( CONFIG_REG_LEN_STR,           reg->atrib->len,   REST_CONT_RECORD, &output[position] );
  position += uRESTmakeValueRecord( CONFIG_REG_VALUE_STR,       reg->value,        reg->atrib->len, reg->atrib->type , REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord( CONFIG_REG_MIN_STR,           reg->atrib->min,   REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord( CONFIG_REG_MAX_STR,           reg->atrib->max,   REST_CONT_RECORD, &output[position] );
  position += uRESTmakeStrRecord( CONFIG_REG_UNITS_STR,         reg->units,        MAX_UNITS_LENGTH, REST_CONT_RECORD, &output[position] );
  position += uRESTmakeStrRecord( CONFIG_REG_TYPE_STR,          &reg->atrib->type, 1U, REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord( CONFIG_REG_BIT_MAP_SIZE_STR,  reg->atrib->bitMapSize, REST_CONT_RECORD, &output[position] );
  position += uRESTmakeBitMapArray( reg->atrib->bitMapSize, reg->bitMap, &output[position] );
  position++;
  output[position] = '}';
  position++;
  return position;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTparsingChart( char* input, eChartData* chart )
{
  REST_ERROR res = REST_OK;
  char*      pchSt;

  pchSt = strchr( input, '{' );
  if (pchSt != NULL)
  {
  if ( strstr( pchSt, "data" ) == NULL)
  {
      res = uRESTparsing16FixDigRecord( input, CHART_DATA_XMIN_STR, &chart->xmin );
      if ( res == REST_OK)
      {
        res = uRESTparsing16FixDigRecord( input, CHART_DATA_XMAX_STR, &chart->xmax );
        if ( res == REST_OK)
        {
          res = uRESTparsing16FixDigRecord( input, CHART_DATA_YMIN_STR, &chart->ymin );
          if ( res == REST_OK)
        {
            res = uRESTparsing16FixDigRecord( input, CHART_DATA_YMAX_STR, &chart->ymax );
            if ( res == REST_OK)
            {
              res = eRESTparsingStrRecord( input, CHART_DATA_XUNIT_STR, chart->xunit, CHART_UNIT_LENGTH );
              if ( res == REST_OK)
              {
                res = eRESTparsingStrRecord( input, CHART_DATA_YUNIT_STR, chart->yunit, CHART_UNIT_LENGTH );
                if ( res == REST_OK)
                {
                  res = eRESTparsingDig16Record( input, CHART_DATA_SIZE_STR, &chart->size );
                  if ( ( res == REST_OK) && ( chart->size > 0U ) )
                  {
                    res = eRESTparsingDotArray( input, CHART_DATA_DOTS_STR, chart->dots, chart->size );
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  else
  {
  res = REST_MESSAGE_FORMAT_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTparsingConfig ( char* input, eConfigReg* reg )
{
  REST_ERROR res   = REST_OK;
  uint8_t    i     = 0U;
  char*      pchSt = NULL;

  pchSt = strchr( input, '{' );
  if (pchSt != NULL)
  {
    if ( res == REST_OK )
    {
      res = eRESTparsingDig16Record( input, CONFIG_REG_ADR_STR, &reg->atrib->adr );
      for ( i=0U; i<SETTING_REGISTER_NUMBER; i++ )
      {
        if ( configReg[i]->atrib->adr == reg->atrib->adr )
        {
          break;
        }
      }
      if ( res == REST_OK )
      {
        res = eRESTparsingSignedRecord( input, CONFIG_REG_SCALE_STR, &reg->scale );
        if (res == REST_OK )
        {
          res = eRESTparsingDig16Record( input, CONFIG_REG_MIN_STR, &reg->atrib->min );
          if (res == REST_OK )
          {
            res = eRESTparsingDig16Record( input, CONFIG_REG_MAX_STR, &reg->atrib->max );
            if (res == REST_OK )
            {
              res = eRESTparsingStrRecord( input, CONFIG_REG_UNITS_STR, reg->units, MAX_UNITS_LENGTH );
              if ( res == REST_OK )
              {
                res = eRESTparsingStrRecord( input, CONFIG_REG_TYPE_STR, &reg->atrib->type, 1U );
                if ( res == REST_OK )
                {
                  res = eRESTparsingDig8Record( input, CONFIG_REG_LEN_STR, &reg->atrib->len );
                  if ( res == REST_OK )
                  {
                    res = eRESTparsingValueRecord( input, CONFIG_REG_VALUE_STR, reg->atrib->type, &reg->value[0U], reg->atrib->len );
                    if (res == REST_OK )
                    {
                      res = eRESTparsingDig8Record( input, CONFIG_REG_BIT_MAP_SIZE_STR, &reg->atrib->bitMapSize );
                      if ( ( res == REST_OK ) && ( reg->atrib->bitMapSize > 0U ) )
                      {
                        res = eRESTparsingBitMapArray( input, CONFIG_REG_BIT_MAP_STR, &reg->bitMap[0], reg->atrib->bitMapSize );
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  else
  {
  res = REST_MESSAGE_FORMAT_ERROR;
  }
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
    data[i] = 0U;
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
      pchSt = &pchSt[strlen( header ) + 2U];
      if ( pchSt[0] == '"' )
      {
        pchSt++;
        pchEn--;
      }
      calcLength = strlen( pchSt ) - strlen( pchEn );
      if ( calcLength > 0U )
      {
        if ( strncpy( data, pchSt, calcLength ) != NULL )
        {
          data[calcLength] = 0U;
        }
        else
        {
          res = REST_RECORD_NO_DATA_ERROR;
        }
      }
      else
      {
      res = REST_RECORD_NO_DATA_ERROR;
      }
    }
    else
    {
      res = REST_RECORD_FORMAT_ERROR;
    }
  }
  else
  {
  res = REST_RECORD_LOST_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTparsingDotArray( const char* input, const char* header, eChartDot* dot, uint8_t size )
{
  REST_ERROR res = REST_OK;
  uint16_t   i   = 0U;
  eChartDot* dotPointer;
  char*      pchSt;
  char*      pchAr;

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
          dotPointer = &dot[i];
            pchAr++;
            res = uRESTparsing16FixDigRecord( pchAr, CHART_DOT_X_STR, &dotPointer->x );
            if ( res == REST_OK )
            {
              res = uRESTparsing16FixDigRecord( pchAr, CHART_DOT_Y_STR, &dotPointer->y );
              if ( res != REST_OK )
              {
                break;
              }
            }
            else
            {
              break;
            }
          }
          else
          {
          break;
          }
        }
      }
      else
      {
      res = REST_RECORD_ARRAY_FORMAT_ERROR;
      }
    }
    else
    {
      res = REST_RECORD_ARRAY_FORMAT_ERROR;
    }
  }
  else
  {
  res = REST_RECORD_LOST_ERROR;
  }
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
            pchAr++;
            res = eRESTparsingDig16Record( pchAr, BIT_MAP_MASK_STR, &bitPointer->mask );
            if ( res == REST_OK )
            {
              res = eRESTparsingDig8Record( pchAr, BIT_MAP_SHIFT_STR, &bitPointer->shift );
              if ( res != REST_OK )
              {
                break;
              }
            }
            else
            {
              break;
            }
          }
          else
          {
          break;
          }
        }
      }
      else
      {
      res = REST_RECORD_ARRAY_FORMAT_ERROR;
      }
    }
    else
    {
      res = REST_RECORD_ARRAY_FORMAT_ERROR;
    }
  }
  else
  {
  res = REST_RECORD_LOST_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTparsingStrRecord( const char* input, const char* header, uint16_t* data, uint8_t length )
{
  REST_ERROR  res         = REST_OK;
  uint8_t     i           = 0U;
  uint8_t     j           = 0U;
  char        buffer[24U] = {0U,0U,0U,0U,0U,0U,0U,0U,0U,0U,0U,0U,0U,0U,0U,0U,0U,0U,0U,0U,0U,0U,0U,0U};
  char        cBuf[2U]    = {0U,0U};
  uint8_t     hexBuf[8U]  = {0U,0U,0U,0U,0U,0U,0U,0U};

  res = eRESTpareingRecord( input, header, buffer );
  if ( res == REST_OK )
  {
  for( i=0U; i<8U; i++ )
  {
      if ( buffer[j] == '%' )
      {
      cBuf[0U] = buffer[j + 1U];
      cBuf[1U] = buffer[j + 2U];
        hexBuf[i] = strtol( cBuf, NULL, 16U );
        j += 3U;
      }
      else
      {
        hexBuf[i] = buffer[j];
        j++;
      }
  }
  j = 0U;
  for( i=0U; i<length; i++ )
  {
      if ( ( hexBuf[j] & 0x80U ) > 0U )
      {
        data[i] = ( ( ( uint16_t ) hexBuf[j] ) << 8U ) | ( uint16_t ) hexBuf[j + 1U];
        j += 2U;
      }
      else
      {
        data[i] = ( uint16_t ) hexBuf[j];
        j++;
      }
  }

  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTparsingValueRecord( const char* input, const char* header, uint16_t type, uint16_t* data, uint16_t len )
{
  REST_ERROR  res    = REST_OK;
  uint16_t    i      = 0U;
  uint8_t     j      = 0U;
  char*       pchStr = NULL;
  char*       pchEnd = NULL;
  char*       pchDot = NULL;
  uint8_t     round  = 0U;
  char        buffer[7U];
  char        strBuf[7U];

  if ( len == 1U )
  {
  res = eRESTparsingDig16Record( input, header, &data[0U] );
  }
  else
  {
  if ( type == ( uint16_t )'S' )
  {
    for ( i=0U; i<7U; i++ )
    {
    strBuf[i] = 0U;
    }
    pchStr = strstr( input, header );
    if ( pchStr != NULL )
    {
    pchStr = strchr( pchStr, '[' );
    if ( pchStr != NULL )
    {
      pchEnd = pchStr;
      for ( i=0U; i<len; i++ )
      {
            pchStr = strchr( pchEnd, QUOTES_ANCII );
            pchStr++;
            pchEnd = strchr( &pchStr[1U], QUOTES_ANCII );
            for ( j=0U; j<( strlen( pchStr ) - strlen( pchEnd ) ); j++ )
            {
              strBuf[j] = pchStr[j];
            }
            vDecodeURI( strBuf, &data[i], 1U );
            pchEnd++;
          }

    }

    }
  }
  else
  {
    res = eRESTpareingRecord( input, header, pchStr );
    if ( res == REST_OK )
    {
        pchStr = strchr( pchStr, '[' );
        pchStr = &pchStr[1U];
        if ( pchStr != NULL )
        {
          for ( i=0U; i<len; i++ )
          {
        pchEnd = strchr( pchStr, ',' );
        pchDot = strchr( buffer, '.' );
      if ( pchDot != NULL )
        {
          if ( pchDot[1U] > 5U )
          {
            round = 1U;
          }
          pchDot[0U] = 0U;
        }
        if ( strncpy( buffer, pchStr, strlen( pchStr ) - strlen( pchEnd ) ) != NULL )
        {
          data[i] = ( uint16_t )( ( uint16_t )strtol( buffer, NULL, 10U ) + round );
          pchStr = &pchEnd[1U];
        }
      }
        }
      }
  }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTparsingDig16Record( const char* input, const char* header, uint16_t* data )
{
  REST_ERROR  res    = REST_OK;
  char*       pchStr = NULL;
  uint8_t     round  = 0U;
  char        buffer[7U];

  res = eRESTpareingRecord( input, header, buffer );
  if ( res == REST_OK )
  {
    pchStr = strchr( buffer, '.' );
    if ( pchStr != NULL )
    {
      if ( pchStr[1U] > 5U )
      {
        round = 1U;
      }
      pchStr[0U] = 0U;
    }
    *data = ( uint16_t )( ( uint16_t )strtol( buffer, NULL, 10U ) + round );
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR uRESTparsing16FixDigRecord( const char* input, const char* header, fix16_t* data )
{
  REST_ERROR  res    = REST_OK;
  char*       pchStr = NULL;
  uint8_t     round  = 0U;
  char        buffer[7U];

  res = eRESTpareingRecord( input, header, buffer );
  if ( res == REST_OK )
  {
    pchStr = strchr( buffer, '.' );
    if ( pchStr != NULL )
    {
      if ( pchStr[1U] > 5U )
      {
        round = 1U;
      }
      pchStr[0U] = 0U;
    }
    *data = ( uint32_t )( ( uint32_t )strtol( buffer, NULL, 10U ) + round );
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTparsingDig8Record( const char* input, const char* header, uint8_t* data )
{
  REST_ERROR  res    = REST_OK;
  char*       pchStr = NULL;
  uint8_t     round  = 0U;
  char        buffer[7U];

  res = eRESTpareingRecord( input, header, buffer );
  if ( res == REST_OK )
  {
    pchStr = strchr( buffer, '.' );
    if ( pchStr != NULL )
    {
      if ( pchStr[1U] > 5U )
      {
        round = 1U;
      }
      pchStr[0U] = 0U;
    }
    *data = ( uint8_t )( ( uint8_t )strtol( buffer, NULL, 10U ) + round );
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
    *data = ( signed char )( strtol( buffer, NULL, 10U ) );
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uRESTmakeBitMap( eConfigBitMap* bitMap, RESTrecordPos last, char* output )
{
  uint32_t position = 1U;

  output[0U] = '{';
  position += uRESTmakeDigRecord( BIT_MAP_MASK_STR,  bitMap->mask,  REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord( BIT_MAP_SHIFT_STR, bitMap->shift, REST_CONT_RECORD, &output[position] );
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
uint32_t uRESTmakeBitMapArray( uint8_t len, eConfigBitMap* bitMap, char* output )
{
  uint8_t  i        = 0U;
  uint32_t position = 0U;

  position += uRESTmakeStartRecord( CONFIG_REG_BIT_MAP_STR, output );
  output[position] = '[';
  position++;
  if ( len > 0U)
  {
    for ( i=0U; i<( len - 1U ); i++ )
    {
      position += uRESTmakeBitMap( &bitMap[i], REST_CONT_RECORD, &output[position] );
    }
    position += uRESTmakeBitMap( &bitMap[i], REST_LAST_RECORD, &output[position] );
  }
  output[position] = ']';
  return position;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uRESTmakeStartRecord( const char* header, char* output )
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
uint8_t uRESTmakeSignedRecord( const char* header, signed char data, RESTrecordPos last, char* output )
{
  uint8_t shift = uRESTmakeStartRecord( header, output );
  char*   pStr  = NULL;

  pStr = itoa( data, &output[shift], 10U );
  shift += strlen( &output[shift] );
  if ( last == REST_CONT_RECORD )
  {
    output[shift] = ',';
    shift++;
  }
  return shift;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uRESTmakeValueRecord( const char* header, uint16_t* data, uint16_t len, uint16_t type, RESTrecordPos last, char* output )
{
  uint16_t i     = 0U;
  uint8_t  shift = uRESTmakeStartRecord( header, output );
  char*    pStr  = NULL;

  if ( len == 1U )
  {
    pStr   = itoa( data[0U], &output[shift], 10U );
    shift += strlen( &output[shift] );
  }
  else
  {
  output[shift] = '[';
    shift++;
    for( i=0U; i<len; i++ )
    {
      if ( type == ( uint16_t )( 'S' ) )
      {
        output[shift] = QUOTES_ANCII;
        shift++;
        shift += uEncodeURI( &data[i], 1U, &output[shift]);
        output[shift] = QUOTES_ANCII;
        shift++;
      }
      else
      {
        pStr   = itoa( data[i], &output[shift], 10U );
        shift += strlen( &output[shift] );
      }
      output[shift] = ',';
      shift++;
    }
    output[shift - 1U] = ']';
  }
  if ( last == REST_CONT_RECORD )
  {
    output[shift] = ',';
    shift++;
  }
  return shift;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uRESTmakeDigRecord( const char* header, uint16_t data, RESTrecordPos last, char* output )
{
  uint8_t shift = uRESTmakeStartRecord( header, output );
  char*   pStr  = NULL;

  pStr = itoa( data, &output[shift], 10U );
  shift += strlen( &output[shift] );
  if ( last == REST_CONT_RECORD )
  {
    output[shift] = ',';
    shift++;
  }
  return shift;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uRESTmake16FixDigRecord( const char* header, fix16_t data, RESTrecordPos last, char* output )
{
  uint8_t shift = uRESTmakeStartRecord( header, output );
  char*   pStr  = NULL;

  pStr = itoa( (uint32_t)data, &output[shift], 10U );
  shift += strlen( &output[shift] );
  if ( last == REST_CONT_RECORD )
  {
    output[shift] = ',';
    shift++;
  }
  return shift;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uRESTmakeStrRecord( const char* header, uint16_t* data, uint8_t dataLen, RESTrecordPos last, char* output )
{
  uint8_t shift = uRESTmakeStartRecord( header, output );

  output[shift] = QUOTES_ANCII;
  shift++;
  shift += uEncodeURI( data, dataLen, &output[shift]);
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

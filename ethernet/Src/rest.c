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
#include "dataAPI.h"
#include "system.h"
#include "measurement.h"
/*----------------------- Structures ----------------------------------------------------------------*/

/*----------------------- Constant ------------------------------------------------------------------*/
const char *restRequestStr[REST_REQUEST_NUMBER] =
{
  REST_REQUEST_CONFIGS,            /* 0 */
  REST_REQUEST_CHARTS,             /* 1 */
  REST_REQUEST_SAVE_CONFIGS,       /* 2 */
  REST_REQUEST_SAVE_CHARTS,        /* 3 */
  REST_REQUEST_TIME,               /* 4 */
  REST_REQUEST_DATA,               /* 5 */
  REST_REQUEST_LOG,                /* 6 */
  REST_REQUEST_LOG_ERASE,          /* 7 */
  REST_REQUEST_PASSWORD,           /* 8 */
  REST_REQUEST_AUTH,               /* 9 */
  REST_REQUEST_ERASE_PASSWORD,     /* 10 */
  REST_REQUEST_MEMORY,             /* 11 */
  REST_REQUEST_MEASUREMENT,        /* 12 */
  REST_REQUEST_MEASUREMENT_ERASE,  /* 13 */
  REST_REQUEST_MEASUREMENT_LENGTH, /* 14 */
  REST_REQUEST_OUTPUT,             /* 15 */
  REST_REQUEST_SYSTEM              /* 16 */
};
/*----------------------- Variables -----------------------------------------------------------------*/
/*----------------------- Functions -----------------------------------------------------------------*/
uint8_t    uRESTmakeStartRecord ( const char* header, char* output );
uint8_t    uRESTmakeDigRecord ( const char* header, uint32_t data, RESTrecordPos last, char* output );
uint8_t    uRESTmakeValueRecord ( const char* header, uint16_t* data, uint16_t len, uint16_t type, RESTrecordPos last, char* output );
uint8_t    uRESTmake16FixDigRecord ( const char* header, fix16_t data, RESTrecordPos last, char* output );
uint8_t    uRESTmakeSignedRecord ( const char* header, signed char data, RESTrecordPos last, char* output );
uint16_t   uRESTmakeStrRecord ( const char* header, const uint16_t* data, uint8_t dataLen, RESTrecordPos last, char* output );
uint32_t   uRESTmakeSystemRecord ( const SYSTEM_DATA* data, char* output );
uint32_t   uRESTmakeBitMap ( const eConfigBitMap* bitMap, RESTrecordPos last, char* output );
uint32_t   uRESTmakeBitMapArray ( uint8_t len, const eConfigBitMap* bitMap, char* output );
uint32_t   uRESTmakeDotArray ( const eChartDot* dot, uint16_t len,char* output );
uint32_t   uRESTmakeDot ( const eChartDot* dot, RESTrecordPos last, char* output );
REST_ERROR eRESTparsingRecord (  const char* input, const char* header, char* data );
REST_ERROR eRESTparsingValueRecord ( const char* input, const char* header, uint16_t type, uint16_t* data, uint16_t len );
REST_ERROR eRESTparsingDig16Record ( const char* input, const char* header, uint16_t* data );
REST_ERROR uRESTparsing16FixDigRecord ( const char* input, const char* header, fix16_t* data );
REST_ERROR eRESTparsingDig8Record ( const char* input, const char* header, uint8_t* data );
REST_ERROR eRESTparsingSignedRecord ( const char* input, const char* header, signed char* data );
REST_ERROR eRESTparsingStrRecord ( const char* input, const char* header, uint16_t* data, uint8_t length );
REST_ERROR eRESTparsingBitMapArray ( const char* input, const char* header, eConfigBitMap* bitMap, uint8_t size );
REST_ERROR eRESTparsingDotArray ( const char* input, const char* header, eChartDot* dot, uint8_t size );
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
REST_ADDRESS eRESTgetRequest ( char* path, REST_REQUEST* request, uint16_t* adr )
{
  char*         pchStr     = NULL;
  uint8_t       i          = 0U;
  REST_ADDRESS  res        = REST_NO_ADR;
  uint32_t      length     = 0U;
  uint8_t       p1         = 0U;
  uint8_t       j          = 0U;
  char          buffer[5U] = { 0U };

  while ( pchStr == NULL )
  {
    pchStr = strstr( path, restRequestStr[i] );
    i++;
    if ( i >= REST_REQUEST_NUMBER )
    {
      break;
    }
  }
  if ( pchStr != NULL )
  {
    i--;
    *request = i;
    pchStr  += strlen( restRequestStr[i] ) - 1U;
    length   = strlen( pchStr );

    for ( i=0U; i<length; i++ )
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
      res  = REST_YES_ADR;
    }
  }
  else
  {
    *request = REST_REQUEST_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
uint32_t uRESTmakeChart ( const eChartData* chart, char* output )
{
  uint32_t position = 1U;
  output[0U] = '{';
  position += uRESTmakeDigRecord( CHART_X_TYPE_STR,    chart->xType, REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord( CHART_Y_TYPE_STR,    chart->yType, REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord( CHART_DATA_SIZE_STR, chart->size,  REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDotArray(                       chart->dots,  chart->size,      &output[position] );
  position++;
  output[position] = '}';
  position++;
  return position;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
uint32_t uRESTmakeDotArray ( const eChartDot* dot, uint16_t len, char* output )
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
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
uint32_t uRESTmakeDot ( const eChartDot* dot, RESTrecordPos last, char* output )
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
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
uint32_t uRESTmakeConfig ( const eConfigReg* reg, char* output )
{
  uint32_t position = 1U;
  output[0U] = '{';
  position += uRESTmakeDigRecord( CONFIG_REG_ADR_STR,           reg->atrib->adr,   REST_CONT_RECORD, &output[position] );
  position += uRESTmakeSignedRecord( CONFIG_REG_SCALE_STR,      reg->atrib->scale, REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord( CONFIG_REG_LEN_STR,           reg->atrib->len,   REST_CONT_RECORD, &output[position] );
  position += uRESTmakeValueRecord( CONFIG_REG_VALUE_STR,       reg->value,        reg->atrib->len, reg->atrib->type , REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord( CONFIG_REG_MIN_STR,           reg->atrib->min,   REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord( CONFIG_REG_MAX_STR,           reg->atrib->max,   REST_CONT_RECORD, &output[position] );
  position += uRESTmakeStrRecord( CONFIG_REG_UNITS_STR,         ( uint16_t* )( &( reg->atrib->units ) ), MAX_UNITS_LENGTH, REST_CONT_RECORD, &output[position] );
  position += uRESTmakeStrRecord( CONFIG_REG_TYPE_STR,          ( uint16_t* )( &( reg->atrib->type ) ),  1U, REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord( CONFIG_REG_BIT_MAP_SIZE_STR,  reg->atrib->bitMapSize, REST_CONT_RECORD, &output[position] );
  position += uRESTmakeBitMapArray( reg->atrib->bitMapSize, reg->atrib->bitMap, &output[position] );
  output[position] = '}';
  position++;
  return position;
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uRESTmakeShortCongig ( const eConfigReg* reg, char* output )
{
  uint32_t position = 1U;
  output[0U] = '{';
  position += uRESTmakeDigRecord(   CONFIG_REG_ADR_STR,   reg->atrib->adr, REST_CONT_RECORD, &output[position] );
  position += uRESTmakeValueRecord( CONFIG_REG_VALUE_STR, reg->value,      reg->atrib->len, reg->atrib->type , REST_LAST_RECORD, &output[position] );
  output[position] = '}';
  position++;
  return position;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
uint32_t uRESTmakeLog ( const LOG_RECORD_TYPE* record, char* output )
{
  uint32_t position = 1U;
  output[0U] = '{';
  position += uRESTmakeDigRecord( LOG_TIME_STR,   record->time,         REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord( LOG_TYPE_STR,   record->event.type,   REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord( LOG_ACTION_STR, record->event.action, REST_LAST_RECORD, &output[position] );
  output[position] = '}';
  position++;
  return position;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
uint32_t uRESTmakeTime ( const RTC_TIME* time, char* output )
{
  uint32_t position = 1U;
  output[0U] = '{';
  position += uRESTmakeDigRecord( TIME_HOUR_STR,  ( uint16_t )time->hour,  REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord( TIME_MIN_STR,   ( uint16_t )time->min,   REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord( TIME_SEC_STR,   ( uint16_t )time->sec,   REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord( TIME_YEAR_STR,  ( uint16_t )time->year,  REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord( TIME_MONTH_STR, ( uint16_t )time->month, REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord( TIME_DAY_STR,   ( uint16_t )time->day,   REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord( TIME_WDAY_STR,  ( uint16_t )time->wday,  REST_LAST_RECORD, &output[position] );
  output[position] = '}';
  position++;
  return position;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
uint32_t uRESTmakeMemorySize ( char* output )
{
  uint32_t position = 1U;
  output[0U] = '{';
  position += uRESTmakeDigRecord( MEMORY_SIZE_STR, uMEASUREMENTgetStorageSize(), REST_LAST_RECORD, &output[position] );
  output[position] = '}';
  position++;
  return position;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
uint32_t uRESTmakeMeasurementLength ( char* output )
{
  uint32_t position = 1U;
  output[0U] = '{';
  position += uRESTmakeDigRecord( MESUREMENT_LENGTH_STR, uMEASUREMENTgetSize(), REST_LAST_RECORD, &output[position] );
  output[position] = '}';
  position++;
  return position;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
uint32_t uRESTmakeSystemRecord ( const SYSTEM_DATA* data, char* output )
{
  uint32_t position = 0U;
  position += uRESTmakeStartRecord( data->name, &output[position] );
  output[position] = '{';
  position++;
  position += uRESTmakeDigRecord( SYSTEM_TOTAL_STR, data->total, REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord( SYSTEM_USAGE_STR, data->usage, REST_LAST_RECORD, &output[position] );
  output[position] = '}';
  position++;
  return position;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
uint32_t uRESTmakeSystem ( char* output )
{
  uint32_t    position = 1U;
  uint8_t     i        = 0U;
  SYSTEM_DATA data     = { 0U };
  output[0U] = '{';
  vSYSgetHeapData( &data );
  position += uRESTmakeSystemRecord( &data, &output[position] );
  if ( uSYSgetTaskumber() > 0U )
  {
    output[position] = ',';
    position++;
    for ( i=0U; i<uSYSgetTaskumber(); i++ )
    {
      vSYSgetTaskData( i, &data );
      position += uRESTmakeSystemRecord( &data, &output[position] );
      if ( i < ( uSYSgetTaskumber() - 1U ) )
      {
        output[position] = ',';
        position++;
      }
    }
  }
  output[position] = '}';
  position++;
  return position;
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uRESTmakeMeasurement ( uint32_t n, char* output )
{
  return 0;
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uRESTmakeOutput ( const eConfigReg* data, const char* name, char* output )
{
  uint32_t position = 1U;
  output[0U] = '{';
  position += uRESTmakeDigRecord(    CONFIG_REG_ADR_STR,   data->atrib->adr,                                                  REST_CONT_RECORD, &output[position] );
  //position += uRESTmakeStrRecord(    OUTPUT_NAME_STR,      ( uint16_t* )( name ),                    strlen( name ),          REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord(    CONFIG_REG_VALUE_STR, data->value[0U],                                                   REST_CONT_RECORD, &output[position] );
  position += uRESTmakeSignedRecord( CONFIG_REG_SCALE_STR, data->atrib->scale,                                                REST_CONT_RECORD, &output[position] );
  position += uRESTmakeStrRecord(    CONFIG_REG_UNITS_STR, ( uint16_t* )( &( data->atrib->units ) ), MAX_UNITS_OUTPUT_LENGTH, REST_CONT_RECORD, &output[position] );
  position += uRESTmakeStrRecord(    CONFIG_REG_TYPE_STR,  ( uint16_t* )( &( data->atrib->type ) ),  1U,                      REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord(    CONFIG_REG_MIN_STR,   data->atrib->min,                                                  REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord(    CONFIG_REG_MAX_STR,   data->atrib->max,                                                  REST_CONT_RECORD, &output[position] );
  position += uRESTmakeBitMapArray( data->atrib->bitMapSize, data->atrib->bitMap, &output[position] );
  output[position] = '}';
  position++;
  return position;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
uint32_t uRESTmakeData ( uint16_t data, char* output )
{
  uint32_t position = 1U;
  output[0U] = '{';
  position += uRESTmakeDigRecord( FREE_DATA_STR,  data,  REST_LAST_RECORD, &output[position] );
  output[position] = '}';
  position++;
  return position;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTparsingTime ( char* input, RTC_TIME* time )
{
  REST_ERROR res = REST_OK;
  if ( strchr( input, '{' ) != NULL )
  {
    res = eRESTparsingDig8Record( input, TIME_HOUR_STR,  &time->hour );
    if ( res == REST_OK )
    {
      res = eRESTparsingDig8Record( input, TIME_MIN_STR,   &time->min );
      if ( res == REST_OK )
      {
        res = eRESTparsingDig8Record( input, TIME_SEC_STR,   &time->sec );
        if ( res == REST_OK )
        {
          res = eRESTparsingDig8Record( input, TIME_YEAR_STR,  &time->year );
          if ( res == REST_OK )
          {
            res = eRESTparsingDig8Record( input, TIME_MONTH_STR, &time->month );
            if ( res == REST_OK )
            {
              res = eRESTparsingDig8Record( input, TIME_DAY_STR,   &time->day );
              if ( res == REST_OK )
              {
                res = eRESTparsingDig8Record( input, TIME_WDAY_STR,  &time->wday );
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
REST_ERROR eRESTparsingPassword ( char* input, PASSWORD_TYPE* password )
{
  REST_ERROR res   = REST_OK;
  char*      pchSt = NULL;
  uint16_t   buf   = 0U;
  pchSt = strchr( input, '{' );
  if ( pchSt != NULL )
  {
    res = eRESTparsingDig16Record( input, PASSWORD_STATUS_STR, &buf );
    password->status = ( uint8_t )buf;
    if ( res == REST_OK )
    {
      res = eRESTparsingDig16Record( input, PASSWORD_DATA_STR, &password->data );
    }
  }
  else
  {
    res = REST_MESSAGE_FORMAT_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTparsingAuth ( char* input, PASSWORD_TYPE* password )
{
  REST_ERROR res = REST_OK;
  if ( strchr( input, '{' ) != NULL )
  {
      res = eRESTparsingDig16Record( input, PASSWORD_DATA_STR, &password->data );
  }
  else
  {
    res = REST_MESSAGE_FORMAT_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTparsingFreeData ( char* input, uint16_t* data )
{
  REST_ERROR res = REST_OK;
  if ( strchr( input, '{' ) != NULL )
  {
    res = eRESTparsingDig16Record( input, FREE_DATA_STR, data );
  }
  else
  {
    res = REST_MESSAGE_FORMAT_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTparsingChart ( char* input, uint16_t adr )
{
  REST_ERROR res = REST_OK;
  uint16_t   buf = 0U;
  while ( xSemaphoreTake( xCHARTgetSemophore(), SEMAPHORE_TAKE_DELAY ) != pdTRUE ){}
  if ( strchr( input, '{' ) != NULL )
  {
    if ( strstr( input, "data" ) == NULL )
    {
      res = eRESTparsingDig16Record( input, CHART_DATA_SIZE_STR, &buf );
      charts[adr]->size = buf;
      if ( ( res == REST_OK ) && ( charts[adr]->size > 0U ) )
      {
        res = eRESTparsingDotArray( input, CHART_DATA_DOTS_STR, charts[adr]->dots, charts[adr]->size );
      }
    }
  }
  else
  {
    res = REST_MESSAGE_FORMAT_ERROR;
  }
  xSemaphoreGive( xCHARTgetSemophore() );
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTparsingShortConfig ( const char* input )
{
  REST_ERROR res                        = REST_OK;
  uint16_t   valueBuf[MAX_VALUE_LENGTH] = { 0U };
  uint8_t    adr                        = 0U;

  res = eRESTparsingDig8Record( input, CONFIG_REG_ADR_STR, &adr );
  if ( res == REST_OK )
  {
    res = eRESTparsingValueRecord( input, CONFIG_REG_VALUE_STR, configReg[adr]->atrib->type, valueBuf, configReg[adr]->atrib->len );
    if ( res == REST_OK )
    {
      if ( eDATAAPIconfig( DATA_API_CMD_WRITE, adr, valueBuf, NULL, NULL ) != DATA_API_STAT_OK )
      {
        res = REST_RECORD_COPY_ERROR;
      }
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTparsingConfig ( char* input, uint16_t adr )
{
  REST_ERROR res                        = REST_OK;
  uint16_t   valueBuf[MAX_VALUE_LENGTH] = { 0U };
  if ( strchr( input, '{' ) != NULL )
  {
    res = eRESTparsingValueRecord( input, CONFIG_REG_VALUE_STR, configReg[adr]->atrib->type, valueBuf, configReg[adr]->atrib->len );
    if ( res == REST_OK )
    {
      if ( eDATAAPIconfig( DATA_API_CMD_WRITE, adr, valueBuf, NULL, NULL ) != DATA_API_STAT_OK )
      {
        res = REST_RECORD_COPY_ERROR;
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
REST_ERROR eRESTparsingRecord ( const char* input, const char* header, char* data )
{
  REST_ERROR res        = REST_OK;
  char*      pchSt      = NULL;
  char*      pchEn      = NULL;
  uint8_t    calcLength = 0U;
  uint8_t    i          = 0U;
  for ( i=0U; i<REST_DIGIT_BUFFER_SIZE; i++ )
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
REST_ERROR eRESTparsingDotArray ( const char* input, const char* header, eChartDot* dot, uint8_t size )
{
  REST_ERROR res    = REST_OK;
  uint16_t   i      = 0U;
  char*      pchSt  = strstr( input, header );;
  char*      pchAr  = NULL;
  fix16_t    buffer = 0U;
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
            pchAr++;
            res = uRESTparsing16FixDigRecord( pchAr, CHART_DOT_X_STR, &buffer );
            dot[i].x = buffer;
            if ( res == REST_OK )
            {
              res = uRESTparsing16FixDigRecord( pchAr, CHART_DOT_Y_STR, &buffer );
              dot[i].y = buffer;
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
REST_ERROR eRESTparsingBitMapArray ( const char* input, const char* header, eConfigBitMap* bitMap, uint8_t size )
{
  REST_ERROR     res        = REST_OK;
  uint8_t        i          = 0U;
  eConfigBitMap* bitPointer = NULL;
  char*          pchSt      = NULL;
  char*          pchAr      = NULL;
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
REST_ERROR eRESTparsingStrRecord ( const char* input, const char* header, uint16_t* data, uint8_t length )
{
  REST_ERROR  res         = REST_OK;
  uint8_t     i           = 0U;
  uint8_t     j           = 0U;
  char        buffer[24U] = { 0U };
  char        cBuf[2U]    = { 0U };
  uint8_t     hexBuf[8U]  = { 0U };
  res = eRESTparsingRecord( input, header, buffer );
  if ( res == REST_OK )
  {
    for ( i=0U; i<8U; i++ )
    {
      if ( buffer[j] == '%' )
      {
        cBuf[0U]  = buffer[j + 1U];
        cBuf[1U]  = buffer[j + 2U];
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
    for ( i=0U; i<length; i++ )
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
REST_ERROR eRESTparsingValueRecord ( const char* input, const char* header, uint16_t type, uint16_t* data, uint16_t len )
{
  REST_ERROR  res        = REST_OK;
  uint16_t    i          = 0U;
  uint8_t     j          = 0U;
  char*       pchStr     = NULL;
  char*       pchEnd     = NULL;
  char*       pchDot     = NULL;
  uint8_t     round      = 0U;
  uint8_t     calcLength = 0U;
  char        buffer[7U] = { 0U };
  char        strBuf[7U] = { 0U };
  if ( len == 1U )
  {
    res = eRESTparsingDig16Record( input, header, &data[0U] );
  }
  else
  {
    if ( type == CONFIG_TYPE_CHAR )
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
      pchStr = strstr( input, header );
      if ( pchStr != NULL )
      {
        pchStr = strchr( pchStr, '[' );
        if ( pchStr != NULL )
        {
          pchStr++;
          for ( i=0U; i<len; i++ )
          {
            if ( i != 0U )
            {
              pchStr = strchr( pchStr, ',' );
            }
            if ( pchStr != NULL )
            {
              if ( i != 0U )
              {
                pchStr++;
              }
              if ( i == (len - 1U ) )
              {
                pchEnd = strchr( pchStr, ']' );
              }
              else
              {
                pchEnd = strchr( pchStr, ',' );
              }
              if ( pchEnd != NULL )
              {
                calcLength = strlen( pchStr ) - strlen( pchEnd );
                if ( calcLength > 0U )
                {
                  if ( strncpy( buffer, pchStr, calcLength ) != NULL )
                  {
                    buffer[calcLength] = 0U;
                    pchDot = strchr( buffer, '.' );
                    if ( pchDot != NULL )
                    {
                      if ( pchDot[1U] > 5U )
                      {
                        round = 1U;
                      }
                      pchDot[0U] = 0U;
                    }
                    data[i] = ( uint16_t )( ( uint16_t )strtol( buffer, NULL, 10U ) + round );
                  }
                  else
                  {
                    res = REST_RECORD_NO_DATA_ERROR;
                    break;
                  }
                }
                else
                {
                  res = REST_RECORD_NO_DATA_ERROR;
                  break;
                }
              }
              else
              {
                res = REST_RECORD_NO_DATA_ERROR;
                break;
              }
            }
            else
            {
              res = REST_RECORD_NO_DATA_ERROR;
              break;
            }
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
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
REST_ERROR eRESTparsingDig16Record ( const char* input, const char* header, uint16_t* data )
{
  REST_ERROR  res        = REST_OK;
  char*       pchStr     = NULL;
  uint8_t     round      = 0U;
  char        buffer[7U] = { 0U };
  res = eRESTparsingRecord( input, header, buffer );
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
REST_ERROR uRESTparsing16FixDigRecord ( const char* input, const char* header, fix16_t* data )
{
  REST_ERROR  res        = REST_OK;
  char*       pchStr     = NULL;
  uint8_t     round      = 0U;
  char        buffer[7U] = { 0U };
  res = eRESTparsingRecord( input, header, buffer );
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
  REST_ERROR  res        = REST_OK;
  char*       pchStr     = NULL;
  uint8_t     round      = 0U;
  char        buffer[7U] = { 0U };

  res = eRESTparsingRecord( input, header, buffer );
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
REST_ERROR eRESTparsingSignedRecord ( const char* input, const char* header, signed char* data )
{
  REST_ERROR  res        = REST_OK;
  char        buffer[6U] = { 0U };
  res = eRESTparsingRecord( input, header, buffer );
  if ( res == REST_OK )
  {
    *data = ( signed char )( strtol( buffer, NULL, 10U ) );
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
uint32_t uRESTmakeBitMap ( const eConfigBitMap* bitMap, RESTrecordPos last, char* output )
{
  uint32_t position = 1U;
  output[0U] = '{';
  position += uRESTmakeDigRecord( BIT_MAP_MASK_STR,  bitMap->mask,  REST_CONT_RECORD, &output[position] );
  position += uRESTmakeDigRecord( BIT_MAP_SHIFT_STR, bitMap->shift, REST_LAST_RECORD, &output[position] );
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
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
uint32_t uRESTmakeBitMapArray ( uint8_t len, const eConfigBitMap* bitMap, char* output )
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
  position++;
  return position;
}
/*---------------------------------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
uint8_t uRESTmakeStartRecord ( const char* header, char* output )
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
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
uint8_t uRESTmakeSignedRecord ( const char* header, signed char data, RESTrecordPos last, char* output )
{
  uint8_t shift = uRESTmakeStartRecord( header, output );
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
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
uint8_t uRESTmakeValueRecord ( const char* header, uint16_t* data, uint16_t len, uint16_t type, RESTrecordPos last, char* output )
{
  uint16_t i     = 0U;
  uint8_t  shift = uRESTmakeStartRecord( header, output );
  if ( len == 1U )
  {
    itoa( data[0U], &output[shift], 10U );
    shift += strlen( &output[shift] );
  }
  else
  {
    output[shift] = '[';
    shift++;
    for ( i=0U; i<len; i++ )
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
        itoa( data[i], &output[shift], 10U );
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
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
uint8_t uRESTmakeDigRecord ( const char* header, uint32_t data, RESTrecordPos last, char* output )
{
  uint8_t shift = uRESTmakeStartRecord( header, output );
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
uint8_t uRESTmake16FixDigRecord ( const char* header, fix16_t data, RESTrecordPos last, char* output )
{
  uint8_t shift = uRESTmakeStartRecord( header, output );
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
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
uint16_t uRESTmakeStrRecord ( const char* header, const uint16_t* data, uint8_t dataLen, RESTrecordPos last, char* output )
{
  uint16_t shift = uRESTmakeStartRecord( header, output );
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

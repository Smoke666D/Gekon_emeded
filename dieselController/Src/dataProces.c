/*
 * dataProces.c
 *
 *  Created on: 15 июн. 2020 г.
 *      Author: photo_Mickey
 */
/*--------------------------------- Includes ---------------------------------*/
#include "dataProces.h"
/*--------------------------------- Constant ---------------------------------*/
static const fix16_t fix16_fact[SERIES_SIZE] = {
    F16( 1U ),
    F16( 1U ),
    F16( 2U ),
    F16( 6U ),
    F16( 24U ),
    //F16( 120U ),
    //F16( 720U ),
    //F16( 5040U ),
    //F16( 40320U ),
    //F16( 362880U ),
    //F16( 3628800U )
};
static const fix16_t fix16_2U = F16( 2U );
/*----------------------------------------------------------------------------*/
/*
 *
 */
fix16_t fix16_naturalPow ( fix16_t x, uint8_t n )
{
  uint8_t i   = 0U;
  fix16_t res = fix16_one;
  if ( x == 0U )
  {
    res = 0U;
  }
  else
  {
    for ( i=0U; i<n; i++ )
    {
      res = fix16_mul( res, x );
    }
  }
  return res;
}
/*----------------------------------------------------------------------------*/
/*
 * https://en.wikipedia.org/wiki/Natural_logarithm#Series
 *
 */
fix16_t fix16_ln ( fix16_t x )
{
  fix16_t res = 0U;
  fix16_t buf = 0U;
  uint8_t i   = 0U;
  if ( !( x & 0x80000000 ) )
  {
    buf = fix16_div( fix16_sub( x, fix16_one ), fix16_add( x, fix16_one ) );
    for ( i=1U; i<SERIES_LN_SIZE; i+=2U )
    {
      res = fix16_add( res, fix16_mul( fix16_div( fix16_one, fix16_from_int( i ) ), fix16_naturalPow( buf, i ) ) );
    }
    res = fix16_mul( res, fix16_2U );
    /*
    if ( x < fix16_one )
    {
      for ( i=1U; i<(SERIES_SIZE + 1U); i++ )
      {
        buf = fix16_div( fix16_naturalPow( fix16_sub( x, fix16_one ), i ), fix16_from_int( i ) );
        if ( ( i % 2U ) == 0U )
        {
          res = fix16_sub( res, buf );
        }
        else
        {
          res = fix16_add( res, buf );
        }
      }
    }
    else if ( x > fix16_one )
    {

    }
    else
    {

    }
    */
  }
  return res;
}
/*----------------------------------------------------------------------------*/
/*
 * x^y = e^(y*ln(x))
 * https://en.wikipedia.org/wiki/Exponential_function#Formal_definition
 */
fix16_t fix16_pow ( fix16_t x, fix16_t y )
{
  uint8_t i      = 0U;
  fix16_t expPow = 0U;
  fix16_t res    = 0U;

  if ( x == 0U )
  {
    res = 0U;
  }
  else if ( y == 0U )
  {
    res = fix16_one;
  }
  else
  {
    expPow = fix16_mul( fix16_ln( x ), y );
    for ( i=0U; i<SERIES_SIZE; i++ )
    {
      res = fix16_add( res, fix16_div( fix16_naturalPow( expPow, i ), fix16_fact[i] ) );
    }
  }
  return res;
}
/*----------------------------------------------------------------------------*/
uint8_t getBitMap( const eConfigReg* reg, uint8_t bit )
{
  return ( uint8_t )( ( reg->value[0U] & reg->atrib->bitMap[bit].mask ) >> reg->atrib->bitMap[bit].shift );
}
/*----------------------------------------------------------------------------*/
uint16_t getUintValue ( const eConfigReg* reg )
{
  return reg->value[0U];
}
/*----------------------------------------------------------------------------*/
fix16_t getScale ( const eConfigReg* reg )
{
  return scaleMulArray[(uint8_t)(reg->atrib->scale - MIN_CONFIG_SCALE)];
}
/*----------------------------------------------------------------------------*/
uint16_t setValue ( const eConfigReg* reg, fix16_t data )
{
  return ( uint16_t )( fix16_to_int( fix16_div( data, getScale( reg ) ) ) );
}
/*----------------------------------------------------------------------------*/
fix16_t getValue ( const eConfigReg* reg )
{
  return fix16_mul( getScale( reg ), fix16_from_int( reg->value[0U] ) );
}
/*----------------------------------------------------------------------------*/
fix16_t getMaxValue ( const eConfigReg* reg)
{
  return fix16_mul( scaleMulArray[(uint8_t)(reg->atrib->scale - MIN_CONFIG_SCALE)], fix16_from_int( reg->atrib->max ) );
}

/*
 * dataProces.c
 *
 *  Created on: 15 июн. 2020 г.
 *      Author: photo_Mickey
 */
/*--------------------------------- Includes ---------------------------------*/
#include "dataProces.h"
/*----------------------------------------------------------------------------*/
/* Power function for fix16_t: exp(y*log(a))
 * input:  x - base
 *         y - power
 * output: result of power
 */
fix16_t fix16_pow ( fix16_t x, fix16_t y )
{
  if ( y == 0U        ) return fix16_one;
  if ( y == fix16_one ) return x;
  if ( y >= 681391U   ) return fix16_maximum;
  if ( y <= -772243   ) return 0;
  //return fix16_log( x );
  //return fix16_exp( fix16_mul( y, fix16_log( x ) ) );
}

uint8_t getBitMap( const eConfigReg* reg, uint8_t bit )
{
  return ( uint8_t )( reg->value[0U] & reg->bitMap[bit].mask >> reg->bitMap[bit].shift );
}

fix16_t getValue( const eConfigReg* reg )
{
  fix16_t a = fix16_from_int( reg->value[0U] );

  fix16_t m = fix16_from_int( 10U );
  fix16_t d = fix16_from_int( 0 );

  fix16_t b = fix16_pow( m, d);
  //float   c  = fix16_to_float(b);

  //fix16_t b = fix16_mod( fix16_from_int( 10U ), fix16_from_int( ( int )reg->scale ) );
  //return fix16_mul( a, b );
  return fix16_one;
}
/*----------------------------------------------------------------------------*/

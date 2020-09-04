/*
 * dataProces.c
 *
 *  Created on: 15 июн. 2020 г.
 *      Author: photo_Mickey
 */
/*--------------------------------- Includes ---------------------------------*/
#include "dataProces.h"
/*----------------------------------------------------------------------------*/
uint8_t getBitMap( const eConfigReg* reg, uint8_t bit )
{
  return ( uint8_t )( reg->value[0U] & reg->bitMap[bit].mask >> reg->bitMap[bit].shift );
}

fix16_t getValue( const eConfigReg* reg )
{
  fix16_t a = fix16_from_int( reg->value[0U] );
  fix16_t b = fix16_mod( fix16_from_int( 10U ), fix16_from_int( ( int )reg->scale ) );
  return fix16_mul( a, b );
}
/*----------------------------------------------------------------------------*/

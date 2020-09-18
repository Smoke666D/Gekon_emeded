/*
 * dataProces.c
 *
 *  Created on: 15 июн. 2020 г.
 *      Author: photo_Mickey
 */
/*--------------------------------- Includes ---------------------------------*/
#include "dataProces.h"
/*--------------------------------- Constant ---------------------------------*/

/*----------------------------------------------------------------------------*/
uint8_t getBitMap( const eConfigReg* reg, uint8_t bit )
{
  uint16_t value  = reg->value[0U];
  uint16_t mask   = reg->bitMap[bit].mask;
  uint16_t shift  = reg->bitMap[bit].shift;
  uint8_t  result = ( uint8_t )( ( value & mask ) >> shift );
  return result;
}
/*----------------------------------------------------------------------------*/
uint16_t getUintValue ( const eConfigReg* reg )
{
  return reg->value[0U];
}
/*----------------------------------------------------------------------------*/
fix16_t getValue( const eConfigReg* reg )
{
  fix16_t scale  = scaleMulArray[(uint8_t)(reg->scale - MIN_CONFIG_SCALE)];
  fix16_t value  = fix16_from_int( reg->value[0U] );
  fix16_t result = fix16_mul ( scale, value );
  return result;
}
/*----------------------------------------------------------------------------*/

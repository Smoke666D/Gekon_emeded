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
  return ( uint8_t )( reg->value[0U] & reg->bitMap[bit].mask >> reg->bitMap[bit].shift );
}

fix16_t getValue( const eConfigReg* reg )
{
  return fix16_mul ( scaleMulArray[(uint8_t)(reg->scale - MIN_CONFIG_SCALE)], fix16_from_int( reg->value[0U] ) );
}
/*----------------------------------------------------------------------------*/

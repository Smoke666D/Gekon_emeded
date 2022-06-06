/*
 * mac.c
 *
 *  Created on: 4 февр. 2022 г.
 *      Author: 79110
 */
/*--------------------------------- Includes ---------------------------------*/
#include "mac.h"
/*-------------------------------- Structures --------------------------------*/
/*--------------------------------- Constant ---------------------------------*/
/*-------------------------------- Variables ---------------------------------*/
static uint8_t mac48[MAC_EUI48_LENGTH] = 
{ 
  MAC_DEFAULT_0,
  MAC_DEFAULT_1,
  MAC_DEFAULT_2,
  MAC_DEFAULT_3,
  MAC_DEFAULT_4,
  MAC_DEFAULT_5
};
static uint8_t mac64[MAC_EUI64_LENGTH] = 
{ MAC_DEFAULT_0,
  MAC_DEFAULT_1,
  MAC_DEFAULT_2,
  MAC_EUI64_ADD3,
  MAC_EUI64_ADD4,
  MAC_DEFAULT_3,
  MAC_DEFAULT_4,
  MAC_DEFAULT_5
};
/*--------------------------------- Macros -----------------------------------*/
/*-------------------------------- Functions ---------------------------------*/



/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vMACrecalc64 ( void )
{
  mac64[0U] = mac48[0U];
  mac64[1U] = mac48[1U];
  mac64[2U] = mac48[2U];
  mac64[3U] = MAC_EUI64_ADD3;
  mac64[4U] = MAC_EUI64_ADD4;
  mac64[5U] = mac48[3U];
  mac64[6U] = mac48[4U];
  mac64[7U] = mac48[5U];
  return;
}
EEPROM_STATUS eMACinit ( const EEPROM_TYPE* eeprom )
{
  EEPROM_STATUS res = eEEPROMInit( eeprom );
  if ( res == EEPROM_OK )
  {
    res = eEEPROMreadMemory( eeprom, MAC_ADR, mac48, MAC_EUI48_LENGTH );
    if ( res == EEPROM_OK )
    {
      vMACrecalc64();
    }
  }
  return res;
}
void vMACsetDefault ( void )
{
  mac48[0U] = MAC_DEFAULT_0;
  mac48[1U] = MAC_DEFAULT_1;
  mac48[2U] = MAC_DEFAULT_2;
  mac48[3U] = MAC_DEFAULT_3;
  mac48[4U] = MAC_DEFAULT_4;
  mac48[5U] = MAC_DEFAULT_5;
  vMACrecalc64();
  return;
}
uint8_t* uMACget48 ( void )
{
  return mac48;
}
uint8_t* uMACget64 ( void )
{
  return mac64;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

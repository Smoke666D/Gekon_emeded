/*
 * utils.c
 *
 *  Created on: 18 февр. 2021 г.
 *      Author: igor.dymov
 */


#include "utils.h"



void vStrCopy(char * dest, char * source)
{
  uint8_t i;
  for (i=0;i<100;i++)
  {
    dest[i]=source[i];
    if (source[i]==0) break;
  }
  if (i==100) dest[i]=0;
  return;

}

void vStrAdd(char * dest, char * source)
{
   uint8_t i,k=0,F=0;
   for (i=0;i<100;i++)
   {
     if ( (dest[i] == 0) && (F==0))
     {
       k=i;
       F=1;
     }
     if ( F!=0 )
     {
       dest[i]=source[i-k];
       if (source[i-k]==0) break;
     }
   }
   if (i==100) dest[i]=0;
   return;
}


float fxParToFloat(uint16_t data, int8_t scale)
{
  uint16_t div=1;
  if (scale<0)
  {
    for (uint8_t i=0;i<(scale*-1);i++)
      div= div*10;
    return (float)data/(div);
  }
   return (float)data;
}

/*
 * Функция преобразования безнакового в строку
 */
/*void vUToStr( uint8_t* str, uint16_t data, signed char scale )
{
  uint8_t     fb     = 0U;
  uint8_t     i      = 0U;
  uint16_t    DD     = 10000U;
  signed char offset = 0U;
  uint8_t     point  = 0U;
  offset = scale;
  if ( offset & 0x80 )
  {
    offset = 1U;
    point  = 1U;
  }
  for ( uint8_t k=0U; k<(5U + offset); k++ )
  {
    if ( ( point == 0U ) && ( k >= 5U ) )  //Если scael был больше 0, то нужно домножить число, фактический добавить в вывод 0
    {
      str[i++] = '0';
    }
    else
    {
      if ( ( point == 1U ) && ( k == ( 6U + scale ) ) )
      {
        str[i++] = '.';
      }
      else
      {
        if ( ( point ==1U ) && ( k == ( 5 + scale ) ) && ( fb == 0U ) )
        {
          fb = 1U;
        }
        if ( ( fb == 1U ) || ( k == ( 5U + scale - 1U ) ) )
        {
          str[i++] = data / ( DD ) + '0';
        }
        else if ( ( data / DD ) > 0U )
        {
          str[i++] = data / ( DD ) + '0';
          fb       = 1U;
        }
        data = data % ( DD );
        DD   = DD / 10U;
      }
    }
  }
  str[i] = 0U;
  return;
}
*/
void vUToStr( char* str, uint16_t data, signed char scale )
{

  uint8_t     i      = 0U;
  uint16_t    DD     = 10000U;
  signed char offset = 0U;
  FLAG        fPoint = FLAG_RESET;
  FLAG        fB     = FLAG_RESET;
  if ( scale < 0U )
  {
    offset = 1U;
    fPoint  = FLAG_SET;
  }
  for ( uint8_t k = 0U; k < (5U + offset); k++ )
  {
      if ( ( fPoint == FLAG_SET ) && ( k == ( 5U + scale ) ) )
      {
        str[i++] = '.';
      }
      else
      {
        if ( ( fPoint == FLAG_SET ) && ( k == ( 5U + scale - 1 ) ))
        {
          fB = FLAG_SET;
        }

        if  ( fB == FLAG_SET )
        {
          str[i++] = data / ( DD ) + '0';
        }
        else
        {
          if ( ( data / DD ) > 0U )
          {
            str[i++] = data / ( DD ) + '0';
            fB       = FLAG_SET;
          }
        }
        data = data % ( DD );
        DD   = DD / 10U;
      }

  }
  str[i] = 0U;
  return;



}


void vUCTOSTRING ( uint8_t * str, uint8_t data )
{
  uint8_t fb = 0U;
  uint8_t i  = 0U;
  uint8_t DD = 100U;
  for ( uint8_t k=0U; k<3U; k++ )
  {
    if ( fb > 0U )
    {
      str[i++] = data / ( DD ) + '0';
    }
    else
    {
      if ( ( data / DD ) > 0U )
      {
        str[i++] = data / ( DD ) + '0';
        fb       = 1U;
      }
      data = data % ( DD );
      DD   = DD / 10U;
    }
  }
  if ( i == 0U )
  {
    str[i++] = '0';
  }
  str[i] = 0U;
  return;
}

void vITOSTRING ( uint8_t * str, uint16_t data )
{
  uint8_t  fb = 0U;
  uint8_t  i  = 0U;
  uint16_t DD = 10000U;
  for ( uint8_t k=0U; k<5U; k++ )
  {
    if ( fb > 0U )
    {
      str[i++] = data / ( DD ) + '0';
    }
    else
    {
      if ( ( data / DD ) > 0U )
      {
        str[i++] = data / ( DD ) + '0';
        fb       = 1U;
      }
      data = data % ( DD );
      DD   = DD / 10U;
    }
  }
  str[i] = 0U;
  return;
}

void vBitmapToString(uint8_t * dest, uint16_t bitmap)
{



}


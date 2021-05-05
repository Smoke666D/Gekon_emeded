/*
 * mbregister.c
 *
 *  Created on: 14 нояб. 2019 г.
 *      Author: mikhail.mikhailov
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "mbregister.h"
#include "mbuart.h"
/*----------------------- Structures ----------------------------------------------------------------*/
static EventGroupHandle_t xEventGroups[OS_MB_EVENT_ARRAY_SIZE];
/*----------------------- Variables -----------------------------------------------------------------*/

/*----------------------- Constant ------------------------------------------------------------------*/

/*----------------------- Functions -----------------------------------------------------------------*/
/**
* @brief   Initialization of event group array
* @param   none
* @retval status of initialization
*/
MB_INIT_STATE eMBeventGroupInit ( void )
{
  uint32_t       i   = 0U;
  MB_INIT_STATE   res = EB_INIT_OK;

  for( i=0; i<OS_MB_EVENT_ARRAY_SIZE; i++)
  {
    xEventGroups[i] = xEventGroupCreate();
    if ( xEventGroups[i] == NULL )
    {
      res = EB_INIT_ERROR;
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Set bit in event group array
* @param   Address of bit
* @retval The value of the event group before
*         the specified bits were cleared
*/
EventBits_t eMBeventSet( uint8_t adr)
{
  uint8_t     group  = 0U;
  uint8_t     shift  = 0U;
  EventBits_t uxBits = 0U;

  group = ( uint8_t )( adr / OS_MB_EVENT_GROUP_SIZE );
  shift = adr - ( group * OS_MB_EVENT_GROUP_SIZE );
  if ( xEventGroups[group] != NULL)
  {
    uxBits = xEventGroupSetBits( xEventGroups[group], ( 1UL << shift ) );
  }
  return uxBits;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Reset bit in event group array
* @param   Address of bit
* @retval The value of the event group before
*         the specified bits were cleared
*/
EventBits_t eMBeventReset ( uint8_t adr)
{
  uint8_t     group  = 0U;
  uint8_t     shift  = 0U;
  EventBits_t uxBits = 0U;

  group = ( uint8_t )( adr / OS_MB_EVENT_GROUP_SIZE );
  shift = adr - ( group * OS_MB_EVENT_GROUP_SIZE );
  if ( xEventGroups[group] != NULL)
  {
    uxBits = xEventGroupClearBits( xEventGroups[group], ( 1UL << shift ) );
  }
  return uxBits;
}
/*---------------------------------------------------------------------------------------------------*/
/**
* @brief   Reset bit in event group array
* @param   Address of bit
* @retval The value of the event group before
*         the specified bits were cleared
*/
void vMBeventFullReset ( void )
{
  uint8_t  i    = 0U;
  uint16_t mask = 0U;

  for( i=0U; i<OS_MB_EVENT_GROUP_SIZE; i++ )
  {
    mask |= 1U << i;
  }
  for( i=0U; i<OS_MB_EVENT_ARRAY_SIZE; i++ )
  {
    if ( xEventGroups[i] != NULL)
    {
      xEventGroupClearBits( xEventGroups[i], mask );
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
MB_DATA_STATUS eMBreadData ( uint8_t adr, uint16_t* data, uint16_t length )
{
  MB_DATA_STATUS res = MB_DATA_STATUS_OK;
  uint16_t       i   = 0U;
  if ( ( ( uint16_t )adr + length ) <= OUTPUT_DATA_REGISTER_NUMBER )
  {
    for ( i=0U; i<length; i++ )
    {
      vOUTPUTupdate( adr );
      data[i] = uOUTPUTread( adr + i );
    }
  }
  else
  {
    res = MB_DATA_STATUS_ADDRESS_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
MB_DATA_STATUS eMBwriteData ( uint8_t adr, const uint16_t* data, uint16_t length )
{
  OUTPUT_STATUS  stat = OUTPUT_STATUS_BUSY;
  MB_DATA_STATUS res  = MB_DATA_STATUS_OK;
  uint16_t       i    = 0U;
  if ( ( ( uint16_t )adr + length ) <= OUTPUT_DATA_REGISTER_NUMBER )
  {
    for ( i=0U; i<length; i++ )
    {
      stat = OUTPUT_STATUS_BUSY;
      while ( stat == OUTPUT_STATUS_BUSY )
      {
        stat = vOUTPUTwrite( ( adr + i ), data[i] );
        vOUTPUTupdate( adr );
      }
      if ( stat != OUTPUT_STATUS_OK )
      {
        break;
      }
    }
    if ( stat != OUTPUT_STATUS_OK )
    {
      res = MB_DATA_STATUS_ACCESS_DENIED;
    }
  }
  else
  {
    res = MB_DATA_STATUS_ADDRESS_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/

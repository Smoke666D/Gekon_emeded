/*
 * data_manager.c
 *
 *  Created on: 4 мар. 2020 г.
 *      Author: igor.dymov
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "data_manager.h"
#include "main.h"
#include "rtc.h"
#include "stdio.h"
#include "fix16.h"
/*----------------------- Variables -----------------------------------------------------------------*/
static fix16_t  uiDataID_Temp = 0U;
static uint16_t test_data     = 100U;
/*---------------------------------------------------------------------------------------------------*/
/*
 * Функция преобразования данных типа DataRecord в строку
 */
void vdmGetData( uint8_t command, uint16_t DataID, uint8_t* pchTextString )
{
  uint8_t temp[9U];
  uint8_t i = 0U;

  switch ( command )
  {
    case READ_COMMNAD:
      if ( DataID == ID_TEST_DATA )
      {
    	;
  	  }
      else
      {
  	    switch ( DataID )
        {
          case ID_RTC_DATA:
            //vRTCGetTime(temp);
            for ( i=0U; i<9U; i++ )
            {
              pchTextString[i] = temp[i];
            }
            break;
          default:
            break;
        }
      }
      break;
    case INC_COMMAND:
      if ( uiDataID_Temp != DataID )
      {
        if ( DataID == ID_TEST_DATA )
        {
          test_data++;
        }
      }
      break;
  	case DEC_COMMNAD:
  	  uiDataID_Temp = DataID;
      if ( DataID == ID_TEST_DATA )
      {
        test_data--;
      }
  	  break;
  	case ENTER_COMMAND:
      uiDataID_Temp = 0U;
      break;
    case ESC_COMMNAD:
      uiDataID_Temp = 0U;
      break;
    default:
      break;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/


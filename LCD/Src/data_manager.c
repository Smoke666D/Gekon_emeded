/*
 * data_manager.c
 *
 *  Created on: 4 мар. 2020 г.
 *      Author: igor.dymov
 */
#include "main.h"
#include "data_manager.h"
#include "rtc.h"
static uint16_t uiDataID_Temp =0;

void vdmGetData(uint8_t command, uint16_t DataID,uint8_t * pchTextString)
{
  uint8_t temp[9];
  switch (command)
  {
  	  case READ_COMMNAD:
  		  if (DataID == ID_TEST_DATA)
  		  {
  			pchTextString[0]='1';
  			pchTextString[1]='1';
  			pchTextString[2]=':';
  			pchTextString[3]='2';
  			pchTextString[4]='0';
  			pchTextString[5]=':';
  			pchTextString[6]='5';
  			pchTextString[7]='4';
  			pchTextString[8]=0;

  		  }
  		  else
  			  switch (DataID)
  			  {
  			    case ID_RTC_DATA:
  			    	vRTCGetTime(temp);
  			    	for (uint8_t i=0;i<9;i++)
  			    		pchTextString[i]=temp[i];
  			    	break;
  			    default:
  			    	break;
  			  }
  		  break;
  	  case INC_COMMAND:
  		uiDataID_Temp = DataID;
  		break;
  	  case DEC_COMMNAD:
  		uiDataID_Temp = DataID;
  		break;
  	  case ENTER_COMMAND:
  		uiDataID_Temp = 0;
  		 break;
  	  case ESC_COMMNAD:
  		uiDataID_Temp = 0;
  		break;
  	 default:
  	   break;
  }

}

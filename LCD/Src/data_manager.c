/*
 * data_manager.c
 *
 *  Created on: 4 мар. 2020 г.
 *      Author: igor.dymov
 */
#include "main.h"
#include "data_manager.h"
#include "rtc.h"

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
  			    	 //eRTCgetTime
  			    	//pchTextString

  			    	break;
  			    default:
  			    	break;


  			  }
  		  break;
  	  default:
  		  break;


  }

}

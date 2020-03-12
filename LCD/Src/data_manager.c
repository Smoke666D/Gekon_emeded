/*
 * data_manager.c
 *
 *  Created on: 4 мар. 2020 г.
 *      Author: igor.dymov
 */
#include "main.h"
#include "data_manager.h"
#include "rtc.h"
#include "stdio.h"
static uint16_t uiDataID_Temp =0;
static uint16_t test_data = 100;



DataDescriptor  OutputData[]=
{
		{ ID_TEST_DATA,UINTEGER,&test_data},
		{ ID_RTC_DATA,TIME,NULL},
		{0,NO_DATA,NULL},
};


//Функция преобразования данных типа DataRecord в строку





void vdmGetData(uint8_t command, uint16_t DataID,uint8_t * pchTextString)
{
  uint8_t temp[9];
  switch (command)
  {
  	  case READ_COMMNAD:
  		  if (DataID == ID_TEST_DATA)
  		  {

  			pchTextString[0]=test_data/100+'0';
  			pchTextString[1]=(test_data&100)/10+'0';
  			pchTextString[2]=(test_data&100)%10+'0';
  			pchTextString[3]=0;
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
  		if (uiDataID_Temp != DataID);
  		if (DataID == ID_TEST_DATA)
  			test_data++;
  		break;
  	  case DEC_COMMNAD:
  		uiDataID_Temp = DataID;
  		if (DataID == ID_TEST_DATA)
  		  			test_data--;

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



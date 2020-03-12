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
#include "fix16.h"
static fix16_t uiDataID_Temp =0;
static uint16_t test_data = 100;


#define DATA_DISCRIPTOR_COUNT 2

DataDescriptor  OutputData[DATA_DISCRIPTOR_COUNT]=
{
		{FIX_POINT,&test_data}
		{TIME,NULL},
};



//Функция преобразования данных типа DataRecord в строку


static fix16_t DataBuffer;


void vdmGetData(uint8_t command, uint16_t DataID,uint8_t * pchTextString)
{
  uint8_t temp[9];
  switch (command)
  {
  	  case READ_COMMNAD:

  		  if (DataID == ID_TEST_DATA)
  		  {


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



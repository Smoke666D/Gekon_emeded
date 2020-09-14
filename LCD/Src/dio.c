
 /* dio.c
 *
 *  Created on: 13 сент. 2020 г.
 *      Author: 45
 */

#include "main.h"
#include "dio.h"

static EventGroupHandle_t xDIOEvent;
static StaticEventGroup_t xDIOCreatedEventGroup;
static uint8_t DINState;
static uint8_t xDINResult =0;


void vDIOInit( void)
{
  xDIOEvent = xEventGroupCreateStatic(&xDIOCreatedEventGroup);

  xEventGroupSetBits(xDIOEvent,DOUTFlag);
}






void vGetDIODC( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
   Data[0]='0';
   Data[1]=0;
   if (cmd == mREAD)
   {
     if  (DINState & ID)  Data[0] ='1';

   }
   return;
}


DIN_ERROR_CODE vDIOGetDIN(uint8_t din,uint8_t * state)
{
  DIN_ERROR_CODE result = ERROR_ARG;
  if (xEventGroupWaitBits( xDIOEvent,DOUTReady,pdFALSE,pdTRUE,100) == DOUTReady)
  {
  switch (din)
  {
    case FPI_B:
      *state = DINState & 0xFE;
      result = OK;
      break;
    case FPI_C:
      *state = DINState>>1 & 0xFE;
      result = OK;
      break;
    case FPI_D:
      *state = DINState>>2 & 0xFE;
      result = OK;
      break;
  }
  }
  else
    result = ERROR_BUSY;
  return result;
}

void vDIOStateChange()
{
  static portBASE_TYPE xHigherPriorityTaskWoken;
  /* Process locked */
  xHigherPriorityTaskWoken = pdFALSE;
  xEventGroupSetBitsFromISR(xDIOEvent,DOUTFlag,&xHigherPriorityTaskWoken);
  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
  return;
}

void vStartDIOTask(void *argument)
{
 vDIOInit();
 for(;;)
 {
   xEventGroupWaitBits( xDIOEvent,DOUTFlag,pdTRUE,pdTRUE,portMAX_DELAY);
  // xEventGroupClearBits(xDIOEvent,DOUTReady);
   DINState = 0;
   if (HAL_GPIO_ReadPin( FPI_B_GPIO_Port,  FPI_B_Pin) == GPIO_PIN_SET )
     DINState|= FPI_B;
   if (HAL_GPIO_ReadPin( FPI_C_GPIO_Port,  FPI_C_Pin) == GPIO_PIN_SET )
        DINState|= FPI_C;
   if (HAL_GPIO_ReadPin( FPI_D_GPIO_Port,  FPI_D_Pin) == GPIO_PIN_SET )
        DINState|= FPI_D;
 //  xEventGroupSetBits(xDIOEvent,DOUTReady);
 }
}

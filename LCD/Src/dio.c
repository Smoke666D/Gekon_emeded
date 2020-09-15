
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


void vDIOInit( void)
{
  xDIOEvent = xEventGroupCreateStatic(&xDIOCreatedEventGroup);
  HAL_GPIO_WritePin( DIN_OFFSET_GPIO_Port,DIN_OFFSET_Pin, GPIO_PIN_SET );
  DINState = 0;
  if (HAL_GPIO_ReadPin( FPI_B_GPIO_Port,  FPI_B_Pin) == GPIO_PIN_SET )
      DINState|= FPI_B;
  if (HAL_GPIO_ReadPin( FPI_C_GPIO_Port,  FPI_C_Pin) == GPIO_PIN_SET )
         DINState|= FPI_C;
  if (HAL_GPIO_ReadPin( FPI_D_GPIO_Port,  FPI_D_Pin) == GPIO_PIN_SET )
         DINState|= FPI_D;
  return;
}

EventGroupHandle_t vDIOGetStateEvent(void)
{
  return xDIOEvent;

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


void vDIO_B_StateChange()
{
  static portBASE_TYPE xHigherPriorityTaskWoken;
  xHigherPriorityTaskWoken = pdFALSE;
  if (HAL_GPIO_ReadPin( FPI_B_GPIO_Port,  FPI_B_Pin) == GPIO_PIN_SET )
  {
      xEventGroupSetBitsFromISR(xDIOEvent,DIN_B_SET,&xHigherPriorityTaskWoken);
      DINState|= FPI_B;
  }
  else
  {
    xEventGroupSetBitsFromISR(xDIOEvent,DIN_B_RESET,&xHigherPriorityTaskWoken);
    DINState&=~ FPI_B;
  }
  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
  return;

}

void vDIO_C_D_StateChange()
{
  static portBASE_TYPE xHigherPriorityTaskWoken;
  /* Process locked */
  xHigherPriorityTaskWoken = pdFALSE;
  if (HAL_GPIO_ReadPin( FPI_C_GPIO_Port,  FPI_C_Pin) == GPIO_PIN_SET )
    {
        xEventGroupSetBitsFromISR(xDIOEvent,DIN_C_SET,&xHigherPriorityTaskWoken);
        DINState|= FPI_C;
    }
    else
    {
      xEventGroupSetBitsFromISR(xDIOEvent,DIN_C_RESET,&xHigherPriorityTaskWoken);
       DINState&=~ FPI_C;
    }
  if (HAL_GPIO_ReadPin( FPI_D_GPIO_Port,  FPI_D_Pin) == GPIO_PIN_SET )
    {
       xEventGroupSetBitsFromISR(xDIOEvent,DIN_D_SET,&xHigherPriorityTaskWoken);
       DINState|= FPI_D;
    }
    else
    {
      xEventGroupSetBitsFromISR(xDIOEvent,DIN_D_RESET,&xHigherPriorityTaskWoken);
      DINState &=~ FPI_D;
    }
  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
  return;

}






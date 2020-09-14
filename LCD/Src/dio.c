
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
  vDIOSetDOUTCurrLim(FPO_A, ON);
  vDIOSetDOUT(FPO_A,OFF);
  vDIOSetDOUT(FPO_B,OFF);
  vDIOSetDOUTCurrLim(FPO_B, ON);
  vDIOSetDOUT(FPO_C,OFF);
  vDIOSetDOUT(FPO_D,OFF);
  vDIOSetDOUTCurrLim(FPO_E, ON);
  vDIOSetDOUT(FPO_E,OFF);
  vDIOSetDOUT(FPO_F,OFF);
  xEventGroupSetBits(xDIOEvent,DOUTFlag);
}


uint8_t uDIOGetDOUT(uint8_t DOUT)
{
  uint8_t result = OFF;
  switch (DOUT)
    {
      case FPO_A:
        if (HAL_GPIO_ReadPin( POUT_A_GPIO_Port, POUT_A_Pin) == GPIO_PIN_SET )
          result = ON;
        break;
      case FPO_B:
        if (HAL_GPIO_ReadPin( POUT_B_GPIO_Port, POUT_B_Pin) == GPIO_PIN_SET )
          result = ON;
        break;
      case FPO_C:
        if (HAL_GPIO_ReadPin( POUT_C_GPIO_Port, POUT_C_Pin) == GPIO_PIN_SET )
         result = ON;
        break;
      case FPO_D:
        if (HAL_GPIO_ReadPin( POUT_D_GPIO_Port, POUT_D_Pin) == GPIO_PIN_SET )
          result = ON;
        break;
      case FPO_E:
        if (HAL_GPIO_ReadPin( POUT_E_GPIO_Port, POUT_E_Pin) == GPIO_PIN_SET )
           result = ON;
        break;
      case FPO_F:
        if (HAL_GPIO_ReadPin( POUT_F_GPIO_Port, POUT_F_Pin) == GPIO_PIN_SET )
           result = ON;
        break;
      default:
        break;
    }
    return result;
}

void vDIOSetDOUT(uint8_t DOUT, uint8_t Data)
{
  switch (DOUT)
  {
    case FPO_A:
      if (Data == OFF )
          HAL_GPIO_WritePin( POUT_A_GPIO_Port, POUT_A_Pin, GPIO_PIN_RESET );
      else
          HAL_GPIO_WritePin( POUT_A_GPIO_Port, POUT_A_Pin, GPIO_PIN_SET );
      break;
    case FPO_B:
      if (Data == OFF )
          HAL_GPIO_WritePin( POUT_B_GPIO_Port, POUT_B_Pin, GPIO_PIN_RESET );
      else
          HAL_GPIO_WritePin( POUT_B_GPIO_Port, POUT_B_Pin, GPIO_PIN_SET );
      break;
    case FPO_C:
      if (Data == OFF )
         HAL_GPIO_WritePin( POUT_C_GPIO_Port, POUT_B_Pin, GPIO_PIN_RESET );
      else
         HAL_GPIO_WritePin( POUT_C_GPIO_Port, POUT_B_Pin, GPIO_PIN_SET );
      break;
    case FPO_D:
      if (Data == OFF )
         HAL_GPIO_WritePin( POUT_D_GPIO_Port, POUT_D_Pin, GPIO_PIN_RESET );
      else
         HAL_GPIO_WritePin( POUT_D_GPIO_Port, POUT_D_Pin, GPIO_PIN_SET );
      break;
    case FPO_E:
      if (Data == OFF )
         HAL_GPIO_WritePin( POUT_E_GPIO_Port, POUT_E_Pin, GPIO_PIN_RESET );
      else
         HAL_GPIO_WritePin( POUT_E_GPIO_Port, POUT_E_Pin, GPIO_PIN_SET );
      break;
    case FPO_F:
      if (Data == OFF )
         HAL_GPIO_WritePin( POUT_F_GPIO_Port, POUT_E_Pin, GPIO_PIN_RESET );
      else
         HAL_GPIO_WritePin( POUT_F_GPIO_Port, POUT_E_Pin, GPIO_PIN_SET );
      break;
    default:
      break;
  }
  return;
}

void vDIOSetDOUTCurrLim(uint8_t DOUT, uint8_t Data)
{
  switch (DOUT)
  {
    case FPO_A:
    case FPO_B:
      if (Data == OFF )
          HAL_GPIO_WritePin( POUT_AB_CS_GPIO_Port, POUT_AB_CS_Pin, GPIO_PIN_SET );
      else
          HAL_GPIO_WritePin(POUT_AB_CS_GPIO_Port, POUT_AB_CS_Pin, GPIO_PIN_RESET );
      break;
    case FPO_C:
    case FPO_D:
      if (Data == OFF )
         HAL_GPIO_WritePin(POUT_CD_CS_GPIO_Port,POUT_CD_CS_Pin, GPIO_PIN_SET );
      else
         HAL_GPIO_WritePin(POUT_CD_CS_GPIO_Port,POUT_CD_CS_Pin, GPIO_PIN_RESET );
      break;
    case FPO_E:
    case FPO_F:
      if (Data == OFF )
         HAL_GPIO_WritePin( POUT_EF_CS_GPIO_Port, POUT_EF_CS_Pin, GPIO_PIN_SET );
      else
         HAL_GPIO_WritePin(POUT_EF_CS_GPIO_Port, POUT_EF_CS_Pin, GPIO_PIN_RESET );
      break;
    default:
      break;
  }
  return;
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
   if (HAL_GPIO_ReadPin( FPI_C_GPIO_Port,  FPI_B_Pin) == GPIO_PIN_SET )
        DINState|= FPI_C;
   if (HAL_GPIO_ReadPin( FPI_D_GPIO_Port,  FPI_B_Pin) == GPIO_PIN_SET )
        DINState|= FPI_D;
 //  xEventGroupSetBits(xDIOEvent,DOUTReady);
 }
}

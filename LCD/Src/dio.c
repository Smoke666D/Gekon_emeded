
 /* dio.c
 *
 *  Created on: 13 сент. 2020 г.
 *      Author: 45
 */

#include "main.h"
#include "dio.h"

static EventGroupHandle_t xDIOEvent;
static StaticEventGroup_t xDIOCreatedEventGroup;

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




void vStartDIOTask(void *argument)
{
 vDIOInit();
 for(;;)
 {

   xEventGroupWaitBits( xDIOEvent,DOUTFlag,pdTRUE,pdTRUE,portMAX_DELAY);


}
}

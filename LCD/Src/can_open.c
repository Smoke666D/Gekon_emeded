/*
 * can_open.c
 *
 *  Created on: 23 мар. 2021 г.
 *      Author: 45
 */

#include "can_open.h"

static QueueHandle_t pxQ_CAN_MSG;
static xQueueHandle  xQ_CAN_MSG = NULL;



void vCanOpenInit()
{
  //Созадем очередь входящих данных
  pxQ_CAN_MSG = xQueueCreateStatic( CAN_RECIVE_QUEUE_SIZE, sizeof( CO_IF_FRM ), pxQ_CAN_MSG, &xQ_CAN_MSG );


}


/*
 *  Функция для вызова из обработчика прервания приема по CAN. Перемещает сообщение в очередь CAN
 */
void vCanOpenRecieve( CAN_HandleTypeDef *hcan )
{
  CO_IF_FRM CAN1_Rx_m;
  CAN_RxHeaderTypeDef tt;
  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

  if ( HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &tt, &CAN1_Rx_m.Data) == HAL_OK )
  {
    CAN1_Rx_m.DLC = (uint8_t)tt.DLC;
    CAN1_Rx_m.Identifier = tt.IDE;
    if(NULL != xQ_CAN_MSG)
    {
      xQueueSendFromISR(xQ_CAN_MSG, &CAN1_Rx_m, &xHigherPriorityTaskWoken);
    }
  }
  if( xHigherPriorityTaskWoken != pdFALSE )
  {
      portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
  }
}


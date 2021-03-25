/*
 * can_open.c
 *
 *  Created on: 23 мар. 2021 г.
 *      Author: 45
 */

#include "can_open.h"


void CONodeFatalError(void)
{


}

int16_t COLssLoad(uint32_t *baudrate, uint8_t *nodeId)
{


}

void COPdoTransmit(CO_IF_FRM *frm)
{


}
void COTmrLock(void)
{

}

void COTmrUnlock(void)
{

}
void CONmtModeChange(CO_NMT *nmt, CO_MODE mode)
{


}
/******************************************************************************
* PRIVATE FUNCTIONS
******************************************************************************/

static void    STM32F2CanInit   (void);
static void    STM32F2CanEnable (uint32_t baudrate);
static int16_t STM32F2CanSend   (CO_IF_FRM *frm);
static int16_t STM32F2CanRead   (CO_IF_FRM *frm);
static void    STM32F2CanReset  (void);
static void    STM32F2CanClose  (void);
CO_NODE  xENRG_NODE;
CO_NODE_SPEC spec;

CAN_HandleTypeDef * pcan;

/*static struct CO_IF_DRV_T AppDriver = {
    &STM32F2CanDriver,
    &STM32F2TimerDriver,
    &STM32F2NvmDriver
};
*/
const CO_IF_CAN_DRV STM32F2CanDriver = {
    STM32F2CanInit,
    STM32F2CanEnable,
    STM32F2CanRead,
    STM32F2CanSend,
    STM32F2CanReset,
    STM32F2CanClose
};



static StaticQueue_t  pxRXQ_CAN_MSG;
static QueueHandle_t  xRXQ_CAN_MSG = NULL;
static StaticQueue_t  pxTXQ_CAN_MSG;
static QueueHandle_t  xTXQ_CAN_MSG = NULL;
uint8_t  xRXQ_CAN_BUFFER[CAN_RECIVE_QUEUE_SIZE*sizeof(CO_IF_FRM)];
uint8_t  xTXQ_CAN_BUFFER[CAN_TRANSMIT_QUEUE_SIZE*sizeof(CO_IF_FRM)];
CO_NODE  xENRG_NODE;
CO_NODE_SPEC spec;


CAN_FilterTypeDef sCAN1FilterConfig;


void vCan_Open_Inti(CAN_HandleTypeDef *hcan)
{
  pcan = hcan;
  xRXQ_CAN_MSG = xQueueCreateStatic( CAN_RECIVE_QUEUE_SIZE, sizeof( CO_IF_FRM ), xRXQ_CAN_BUFFER, &pxRXQ_CAN_MSG );/*Созадем очередь входящих данных*/
  xTXQ_CAN_MSG = xQueueCreateStatic( CAN_TRANSMIT_QUEUE_SIZE, sizeof( CO_IF_FRM ), xTXQ_CAN_BUFFER, &pxTXQ_CAN_MSG );/*Созадем очередь исходящих данных*/
  CONodeInit(&xENRG_NODE,&spec);
}


static void    STM32F2CanInit   (void)
{
  sCAN1FilterConfig.FilterBank = 0;
  sCAN1FilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sCAN1FilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sCAN1FilterConfig.FilterIdHigh = 0x0000;
  sCAN1FilterConfig.FilterIdLow = 0x0000;
  sCAN1FilterConfig.FilterMaskIdHigh = 0x0000;
  sCAN1FilterConfig.FilterMaskIdLow = 0x0000;
  sCAN1FilterConfig.FilterFIFOAssignment = 0;
  sCAN1FilterConfig.FilterActivation = ENABLE;
  HAL_CAN_ConfigFilter(pcan, &sCAN1FilterConfig);
  return;
}
static void    STM32F2CanEnable (uint32_t baudrate)
{

}
static int16_t STM32F2CanSend   (CO_IF_FRM *frm)
{
    if ( xQueueSend(xTXQ_CAN_MSG,frm,0U) == pdTRUE)
    {
      return sizeof(CO_IF_FRM);
    }
    else
    {
      return ((int16_t)-1u);
    }
}

/* CALLBack функция возвращающая сообщение из очереди
 * Paramert:
 * frm pointer to the receive frame buffer
 * Returned Value:
>0 the size of CO_IF_FRM on success
<0 the internal CanBus error code
*/

static int16_t STM32F2CanRead   (CO_IF_FRM *frm)
{
     int16_t       result = 0u;
     CO_IF_FRM    rx;
     if (pcan->State !=  HAL_CAN_STATE_READY)
     {
         return ((int16_t)-1u);
     }
     if ( xQueueReceive( xRXQ_CAN_MSG , &rx, 0U ) == pdPASS  )
     {
         frm->Identifier = rx.Identifier;
         frm->DLC        = rx.DLC;
         for (uint32_t byte=0U;byte < 8u; byte++)
         {
            if (frm->DLC > byte)
            {
               frm->Data[byte] = rx.Data[byte] & 0xFFu;
            }
            else
            {
               frm->Data[byte] = 0u;
            }
            result = sizeof(CO_IF_FRM);
         }
     }
     return (result);
}


static void    STM32F2CanReset  (void)
{
  xQueueReset(xRXQ_CAN_MSG);
  return;
}
static void    STM32F2CanClose  (void)
{

  return;
}





/*
 *  Функция для вызова из обработчика прервания приема по CAN. Перемещает сообщение в очередь CAN
 */
void vCanOpenRecieve( CAN_HandleTypeDef *hcan )
{
  CO_IF_FRM CAN1_Rx_m;
  CAN_RxHeaderTypeDef tt;
  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

  if ( HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &tt, CAN1_Rx_m.Data) == HAL_OK )
  {
    CAN1_Rx_m.DLC = (uint8_t)tt.DLC;
    CAN1_Rx_m.Identifier = tt.IDE;
    if( xRXQ_CAN_MSG  != NULL )
    {
      xQueueSendFromISR( xRXQ_CAN_MSG, &CAN1_Rx_m, &xHigherPriorityTaskWoken);
    }
  }
  if( xHigherPriorityTaskWoken != pdFALSE )
  {
      portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
  }
}

/*
 *  Rx Task
 *
 */
void StartCANOPENRXTask(void *argument)
{
    CO_IF_FRM CAN1_Rx_m;
    for (;;)
    {
        xQueuePeek( xRXQ_CAN_MSG, &CAN1_Rx_m, portMAX_DELAY ); /*Ждем пока во входной очереди не появится какой-то пакет*/
        CONodeProcess( &xENRG_NODE);
    }
}
/*
 *  Tx Task
 */
void StartCANOPENTXTask(void *argument)
{
     CO_IF_FRM    tx;
     CAN_TxHeaderTypeDef tx_can;
     uint8_t data[8];
     tx_can.ExtId = 0U;
     tx_can.IDE = CAN_ID_STD;
     tx_can.TransmitGlobalTime = DISABLE;
     for (;;)
     {
          if ( xQueueReceive( xTXQ_CAN_MSG , &tx, portMAX_DELAY ) == pdPASS  )
          {
              tx_can.DLC = tx.DLC;
              tx_can.RTR = CAN_RTR_DATA;
              tx_can.StdId = tx.Identifier;
              for (uint32_t byte=0U;byte < 8u; byte++)
              {
                  if (tx.DLC > byte)
                  {
                      data[byte] = tx.Data[byte];
                  }
                  else
                  {
                      data[byte] = 0u;
                  }
              }
              HAL_CAN_AddTxMessage(pcan, &tx_can, data, (uint32_t*)CAN_TX_MAILBOX0);
          }
     }
}


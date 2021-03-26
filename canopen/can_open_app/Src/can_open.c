/*
 * can_open.c
 *
 *  Created on: 23 мар. 2021 г.
 *      Author: 45
 */

#include "can_open.h"
#include "node_spec.h"



CAN_HandleTypeDef * pcan;
TIM_HandleTypeDef * ptim;


CO_NODE  xENRG_NODE;


void vCan_Open_Inti(CAN_HandleTypeDef *hcan)
{

  pcan = hcan;
  vCanDrvInit();
  CONodeInit(&xENRG_NODE,&AppSpec);
}



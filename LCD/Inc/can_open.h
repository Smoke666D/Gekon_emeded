/*
 * can_open.h
 *
 *  Created on: 23 мар. 2021 г.
 *      Author: 45
 */

#ifndef INC_CAN_OPEN_H_
#define INC_CAN_OPEN_H_

#include "main.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "co_if_can.h"

void vCanOpenInit(void);
void vCanOpenRecieve(CAN_HandleTypeDef *hcan);

#define CAN_RECIVE_QUEUE_SIZE  16U

#endif /* INC_CAN_OPEN_H_ */

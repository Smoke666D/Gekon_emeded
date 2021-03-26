/*
 * can_drv.h
 *
 *  Created on: 26 мар. 2021 г.
 *      Author: 45
 */

#ifndef INC_CAN_DRV_H_
#define INC_CAN_DRV_H_

#include "can_open.h"


#define CAN_RECIVE_QUEUE_SIZE    16U
#define CAN_TRANSMIT_QUEUE_SIZE  16U

extern const CO_IF_CAN_DRV STM32F2CanDriver;

void vCanDrvInit();

#endif /* INC_CAN_DRV_H_ */

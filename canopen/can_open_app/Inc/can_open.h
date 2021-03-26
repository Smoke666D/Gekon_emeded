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
#include "co_core.h"


#define APP_NODE_ID       1u                  /* CANopen node ID             */
#define APP_BAUDRATE      250000u             /* CAN baudrate                */
#define APP_TMR_N         16u                 /* Number of software timers   */
#define APP_TICKS_PER_SEC 1000u               /* Timer clock frequency in Hz */
#define APP_OBJ_N         128u                /* Object dictionary max size  */

extern CO_NODE            xENRG_NODE;
extern CAN_HandleTypeDef  * pcan;
extern TIM_HandleTypeDef * ptim;

void vCan_Open_Inti(CAN_HandleTypeDef *hcan);
void vCanOpenRecieve(CAN_HandleTypeDef *hcan);
void InitCAN();


#endif /* INC_CAN_OPEN_H_ */

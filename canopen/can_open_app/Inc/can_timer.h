/*
 * can_timer.h
 *
 *  Created on: 26 мар. 2021 г.
 *      Author: 45
 */
#ifndef INC_CAN_TIMER_H_
#define INC_CAN_TIMER_H_


#include "can_open.h"

extern const CO_IF_TIMER_DRV STM32F2TimerDriver;

void vCanTimerCallback();

#endif

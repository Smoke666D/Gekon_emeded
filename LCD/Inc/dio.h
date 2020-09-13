/*
 * dio.h
 *
 *  Created on: 13 сент. 2020 г.
 *      Author: 45
 */

#ifndef INC_DIO_H_
#define INC_DIO_H_


#include "main.h"
#include "config.h"
#include "data_type.h"
#include "dataAPI.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "menu.h"
#include "stm32f2xx_hal.h"



#define OFF    0x00
#define ON     0x01
#define FPO_A  0x01
#define FPO_B  0x02
#define FPO_C  0x03
#define FPO_D  0x04
#define FPO_E  0x05
#define FPO_F  0x06
#define DOUTFlag  0x01

void vStartDIOTask(void *argument);
uint8_t uDIOGetDOUT(uint8_t DOUT);
void vDIOSetDOUT(uint8_t DOUT, uint8_t Data);
void vDIOSetDOUTCurrLim(uint8_t DOUT, uint8_t Data);

#endif /* INC_DIO_H_ */

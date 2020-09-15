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




#define FPI_B  0x01
#define FPI_C  0x02
#define FPI_D  0x04

#define DIN_A_SET   0x01
#define DIN_A_RESET 0x02
#define DIN_B_SET   0x04
#define DIN_B_RESET 0x08
#define DIN_C_SET   0x10
#define DIN_C_RESET 0x20
#define DIN_D_SET   0x40
#define DIN_D_RESET 0x80



void vDIO_B_StateChange(void);
void vDIO_C_D_StateChange(void);
void vStartDIOTask(void *argument);
void vDIOInit( void);
void vGetDIODC( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID );

#endif /* INC_DIO_H_ */

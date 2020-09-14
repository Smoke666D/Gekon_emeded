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


typedef enum
{
  OK,
  ERROR_BUSY,
  ERROR_ARG,
} DIN_ERROR_CODE;


typedef enum {
  ON,
  OFF,
} DIN_STATE;

#define FPI_B  0x01
#define FPI_C  0x02
#define FPI_D  0x03

#define OFF    0x00
#define ON     0x01
#define FPO_A  0x01
#define FPO_B  0x02
#define FPO_C  0x03
#define FPO_D  0x04
#define FPO_E  0x05
#define FPO_F  0x06
#define DOUTFlag  0x01
#define DOUTReady 0x02

void vStartDIOTask(void *argument);
void vDIOStateChange(void);
void vGetDIODC( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID );

#endif /* INC_DIO_H_ */

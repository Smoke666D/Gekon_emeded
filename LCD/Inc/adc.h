/*
 * adc.h
 *
 *  Created on: Aug 31, 2020
 *      Author: igor.dymov
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "main.h"
#include "config.h"
#include "data_type.h"
#include "dataAPI.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "menu.h"

#define ADC1_READY  0x01
#define ADC2_READY  0x01
#define ADC3_READY  0x01

#define ADC1_CHANNELS 3
#define ADC2_CHANNELS 4
#define ADC3_CHANNELS 4
#define ADC3_ADD_CHANNEL 5
#define ADC_FRAME_SIZE  512
#define ADC_ADD_FRAME_SIZE ADC3_ADD_CHANNEL*4



typedef enum
{
  DC,
  AC,
  IDLE,
} xADCFSMType;

void vADC1_Ready(void);
void vADC2_Ready(void);
void vADC3_Ready(void);


void vADCInit(void);
void StartADCTask(void *argument);
float  fADC3Init(uint16_t freq);
void vGetADCDC( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID );
#endif /* INC_ADC_H_ */

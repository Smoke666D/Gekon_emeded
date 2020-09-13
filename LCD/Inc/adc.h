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
#include "stm32f2xx_hal.h"

#define ADC1_READY  0x01
#define ADC2_READY  0x02
#define ADC3_READY  0x04

#define ADC1_CHANNEL  0x01
#define ADC2_CHANNEL  0x02
#define ADC3_CHANNEL  0x04

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


void vADC_Ready(uint8_t adc_number);
void vDecNetural(uint8_t chanel);
void vADC3R(DMA_HandleTypeDef *_hdma);
void vADCInit(void);
void StartADCTask(void *argument);
float  fADC3Init(uint16_t freq);
void vGetADCDC( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID );
#endif /* INC_ADC_H_ */

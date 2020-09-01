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


void vADCInit(void);
void StartADCTask(void *argument);
float  fADC3Init(uint16_t freq);
#endif /* INC_ADC_H_ */

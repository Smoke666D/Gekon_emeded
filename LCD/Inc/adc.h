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

#define ADC1_READY         0x01U
#define ADC2_READY         0x02U
#define ADC3_READY         0x04U
#define ADC_READY          0x08U
#define ADC1_CHANNEL       0x01U
#define ADC2_CHANNEL       0x02U
#define ADC3_CHANNEL       0x04U
#define ADC1_CHANNELS      3U
#define ADC2_CHANNELS      4U
#define ADC3_CHANNELS      4U
#define ADC3_ADD_CHANNEL   5U
#define ADC_FRAME_SIZE     1024//700U//512U
#define ADC_ADD_FRAME_SIZE ( ADC3_ADD_CHANNEL * 4U )

/*
 * Номиналы резисторов
 */


#define R118_R122 104.7
#define R122 4.7
#define VDD_CF ((R118_R122/R122)*3.3/4095)
#define AC_COOF (401*3.3/4095)
#define R3   1416
#define R12      22.2
#define R12_R11  122.2
#define VT4  0
#define MAX_RESISTANCE 1000
#define DELTA  0.04*4095/3.3
#define VDD_CH  0U
#define COFDIN  1U
#define COFAIN  5U
#define CANC    6U
#define CFUEL   2U
#define CPRES   3U
#define CTEMP      7U
#define NET_F1_VDD 8U
#define NET_F2_VDD 9U
#define NET_F3_VDD 10U
#define NET_FREQ   11U
#define ADC_FREQ   12U
#define ADC_TEMP   13U

#define DC_SIZE  50

#define MIN_AMP_VALUE 100

#define NO_COMMON 0x01
#define NO_SOP    0x02
#define SC_SOP    0x04
#define NO_SCT    0x08
#define SC_SCT    0x10
#define NO_SFL    0x20
#define SC_SFL    0x04

#define ADC_OK    0x00
#define LOW_FREQ  0x01
#define HIGH_FREQ 0x02
#define ADC_ERROR 0x03


typedef enum
{
  DC,
  AC,
  IDLE,
} xADCFSMType;


void    vADC_Ready(uint8_t adc_number);
void    vDecNetural(int16_t * data);
void    vADC3R(DMA_HandleTypeDef *_hdma);
void    vADCInit(void);
void    StartADCTask(void *argument);
uint16_t  fADC3Init(uint16_t freq);
void    vGetADCDC( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID );
uint8_t vADCFindFreq(int16_t * data, uint16_t * count);
void    SetSQR(int16_t * data);
void    vADCConvertToVDD(uint8_t AnalogSwitch);
fix16_t xADCGetSOP();
fix16_t xADCGetSCT();
fix16_t xADCGetSFL();
uint8_t vADCGetADC3Data();

uint8_t vADCGetADC12Data();
fix16_t  xADCRMS(int16_t * source, uint8_t off, uint16_t size );
int16_t  xADCMax(int16_t * source, uint8_t off, uint16_t size );
#endif /* INC_ADC_H_ */

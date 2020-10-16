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
#define NET_READY          0x08U
#define GEN_READY          0x10U
#define DC_READY          0x20U

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


#define R118_R122      104.7
#define R122           4.7
#define VDD_CF         ( ( R118_R122 / R122 ) * 3.3 / 4095U )
#define AC_COOF        ( 401U * 3.3 / 4095U )
#define R3             1416U
#define R12            22.2
#define R12_R11        122.2
#define VT4            0U
#define MAX_RESISTANCE 5000U
#define DELTA          ( 0.04 * 4095U / 3.3 )
#define VDD_CH         0U
#define COFDIN         1U
#define COFAIN         5U
#define CANC           6U
#define CFUEL          2U
#define CPRES          3U
#define CTEMP          7U
#define NET_F1_VDD     8U
#define NET_F2_VDD     9U
#define NET_F3_VDD     10U
#define NET_FREQ       11U
#define ADC_FREQ       12U
#define ADC_TEMP       13U
#define GEN_F1_VDD     14U
#define GEN_F2_VDD     15U
#define GEN_F3_VDD     16U
#define GEN_FREQ       17U
#define NET_ROTATION   18U
#define GEN_ROTATION   19U
#define NET_F1_F_VDD   20U
#define NET_F2_F_VDD   21U
#define NET_F3_F_VDD   22U
#define GEN_F1_F_VDD   23U
#define GEN_F2_F_VDD   24U
#define GEN_F3_F_VDD   25U
#define GEN_F1_CUR     26U
#define GEN_F2_CUR     27U
#define GEN_F3_CUR     28U

#define STAR           0x01U
#define TRIANGLE       0x02U
#define ONE_FASE       0x00U


#define DC_SIZE        50U
#define MIN_AMP_VALUE  50U
#define NO_COMMON      0x01U
#define NO_SOP         0x02U
#define SC_SOP         0x04U
#define NO_SCT         0x08U
#define SC_SCT         0x10U
#define NO_SFL         0x20U
#define SC_SFL         0x04U
#define ADC_OK         0x00U
#define LOW_FREQ       0x01U
#define HIGH_FREQ      0x02U
#define ADC_ERROR      0x03U
#define LOW_AMP        0x04U


#define NO_ROTATION   0x00U
#define B_C_ROTATION  0x01U
#define C_B_ROTATION  0x02U

typedef enum
{
  DC,
  AC,
  IDLE,
} xADCFSMType;

/*
 * Функции API драйвера
 */
fix16_t xADCGetVDD();
fix16_t xADCGetSOP();
fix16_t xADCGetSCT();
fix16_t xADCGetSFL();
fix16_t xADCGetNETL3();
fix16_t xADCGetNETL2();
fix16_t xADCGetNETL1();
fix16_t xADCGetGENL1();
fix16_t xADCGetGENL2();
fix16_t xADCGetGENL3();
fix16_t xADCGetGENL1FaseVDD();
fix16_t xADCGetGENL2FaseVDD();
fix16_t xADCGetGENL3FaseVDD();
uint8_t uADCGetValidDataFlag();


void    vADC_Ready(uint8_t adc_number);
void    vDecNetural(int16_t * data);
void    vADC3R(DMA_HandleTypeDef *_hdma);
void    vADCInit(void);
void    StartADCTask(void *argument);
void    vADC3FrInit(uint16_t freq);
void    vADC12FrInit(uint16_t freq);
void    vGetADCDC( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID );
uint8_t vADCFindFreq(int16_t * data, uint16_t * count,uint8_t off, int16_t AMP);
void    SetSQR(int16_t * data);
void    vADCConvertToVDD(uint8_t AnalogSwitch);
uint8_t uADCGetGenFaseRotation();
uint8_t uADCGetNetFaseRotation();
void  vADCSetFreqDetectParam(int16_t AMP,uint8_t * del,uint8_t * fd);
#endif /* INC_ADC_H_ */

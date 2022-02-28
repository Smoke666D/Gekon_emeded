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
#include "math.h"
#include "fix16.h"
#include "stdlib.h"
#include "engine.h"
#include "controllerTypes.h"
#include "utils.h"

#define ADC1_READY         0x01U
#define ADC2_READY         0x02U
#define ADC3_READY         0x04U
#define NET_READY          0x08U
#define GEN_READY          0x10U
#define CUR_READY          0x20U
#define DC_READY           0x40U
#define GEN_UPDATE         0x80U
#define NET_UPDATE         0x100U
#define COMMON_ERROR       0x01U
#define OP_CHANEL_ERROR    0x02U
#define CT_CHANEL_ERROR    0x04U
#define FL_CHANEL_ERROR    0x80U



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

#define GEN_RMS_CUR         0
#define GEN_FREQ            1
#define GEN_FASE_V          2
#define GEN_LINE_V          3
#define GEN_L1_CUR          4
#define GEN_L1_FREQ         5
#define GEN_L1_FASE_V       6
#define GEN_L1_LINE_V       7
#define GEN_L1_REAL_POWER   8
#define GEN_L1_ACTIVE_POWER 9
#define GEN_L1_REAC_POWER   10
#define GEN_L2_CUR          11
#define GEN_L2_FREQ         12
#define GEN_L2_FASE_V       13
#define GEN_L2_LINE_V       14
#define GEN_L2_REAL_POWER   15
#define GEN_L2_ACTIVE_POWER 16
#define GEN_L2_REAC_POWER   17
#define GEN_L3_CUR          18
#define GEN_L3_FREQ         19
#define GEN_L3_FASE_V       20
#define GEN_L3_LINE_V       21
#define GEN_L3_REAL_POWER   22
#define GEN_L3_ACTIVE_POWER 23
#define GEN_L3_REAC_POWER   24
#define GEN_APPER_POWER     25
#define GEN_REAL_POWER      26
#define GEN_ACTIVE_POWER    27
#define GEN_REACTIVE_POWER  28
#define NET_FREQ            29
#define NET_L1_FASE_V       30
#define NET_L1_LINE_V       31
#define NET_L2_FASE_V       32
#define NET_L2_LINE_V       33
#define NET_L3_FASE_V       34
#define NET_L3_LINE_V       35
#define GEN_AVER_V          36
#define GEN_AVER_A          37
#define GEN_AVER_P          38

/*
 * Номиналы резисторов
 */

#define RCSHUNT         0.01  //Сопротивление шунтирующих резисторов токовых входов
#define OPTRANSCOOF     76.4 //20    //Коофециент усиления операционного усилителя на  токовых входах
#define VRef            3.3



#define R118_R122      104.7
#define R122           4.7
#define VDD_CF         ( ( R118_R122 / R122 ) * 3.3 / 4095U )
#define AC_COOF        ( 401U * 3.3 / 4095U )
#define R3             1416U
#define R12            22.2
#define R12_R11        122.2
#define VT4            -0.4
#define MAX_RESISTANCE 5000U
#define DELTA          ( 0.04 * 4095U / 3.3 )
#define IN_VDD         2U
#define COFDIN         1U
#define COFAIN         5U
#define CANC           6U


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




typedef enum
{
  DC,
  AC,
  IDLE,
} xADCFSMType;

typedef enum
{
  NO_FREQ,
  FREQ_DETECTED,
} xADCGenDetectType;


typedef enum
{
  NO_ROTATION,
  B_C_ROTATION,
  C_B_ROTATION,
} xADCRotatinType;

/*
 * Функции API драйвера
 */
SENSOR_TYPE xADCGetFLChType( void );
SENSOR_TYPE xADCGetxOPChType( void );
fix16_t xADCGetGENActivePower( void );
fix16_t xADCGetGENReactivePower( void );
fix16_t xADCGetGENRealPower( void );
fix16_t xADCGetGENL1RealPower( void );
fix16_t xADCGetGENL2RealPower( void );
fix16_t xADCGetGENL3RealPower( void );
fix16_t xADCGetGENL1ActivePower( void );
fix16_t xADCGetGENL2ActivePower( void );
fix16_t xADCGetGENL3ActivePower( void );


fix16_t xADCGetCAC( void );
fix16_t xADCGetVDD( void );              // Функция возращает наряжения АКБ.
fix16_t xADCGetSOP( void );
fix16_t xADCGetSCT( void );
fix16_t xADCGetSFL( void );
fix16_t xADCGetNETL3();
fix16_t xADCGetNETL2( void );
fix16_t xADCGetNETL1( void );
fix16_t xADCGetNETL1Lin( void );         //Линейное напряжене Uab сети
fix16_t xADCGetNETL2Lin( void );        //Линейное напряжене Ubс сети
fix16_t xADCGetNETL3Lin( void );         //Линейное напряжене Uсa сети
fix16_t xADCGetGENL1( void );           //Фазное  напряжене Uan генератора
fix16_t xADCGetGENL2( void );           //Фазное  напряжене Ubn генератора
fix16_t xADCGetGENL3( void );           //Фазное  напряжене Ucn генератора
fix16_t xADCGetGENL1Lin( void );        //Линейное напряжене Uab генератора
fix16_t xADCGetGENL2Lin( void );        //Линейное напряжене Ubс генератора
fix16_t xADCGetGENL3Lin( void );        //Линейное напряжене Uсa генератора
fix16_t xADCGetGENL1Cur( void );        //Фазный ток Ian генератора
fix16_t xADCGetGENL2Cur( void );        //Фазный ток Ibn генератора
fix16_t xADCGetGENL3Cur( void );        //Фазный ток Icn генератора
uint8_t uADCGetValidDataFlag( void );
fix16_t xADCGetNETLFreq( void );
fix16_t xADCGetGENLFreq ( void );
fix16_t xADCGetCOSFi( void );
xADCRotatinType xADCGetGenFaseRotation( void );
xADCRotatinType xADCGetNetFaseRotation( void );
fix16_t xADCGetREG(uint16_t reg);
void    vADC_Ready(uint8_t adc_number);
void    StartADCTask(void *argument);
void    vGetADCDC( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID );
uint8_t uADCGetDCChError ( void );
SENSOR_TYPE xADCGetxCTChType(void);
xADCGenDetectType vADCGetGenFreqPres( void );
ELECTRO_SCHEME xADCGetScheme(void);
#endif /* INC_ADC_H_ */

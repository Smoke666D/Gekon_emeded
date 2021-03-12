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
#include "adcdata.h"

#define ADC1_READY         0x01U
#define ADC2_READY         0x02U
#define ADC3_READY         0x04U
#define NET_READY          0x08U
#define GEN_READY          0x10U
#define CUR_READY          0x20U
#define DC_READY           0x40U
#define DC_UPDATE          0x40U
#define GEN_UPDATE         0x80U
#define NET_UPDATE         0x100U

#define COMMON_ERROR       0x01U
#define OP_CHANEL_ERROR    0x02U
#define CT_CHANEL_ERROR    0x04U
#define FL_CHANEL_ERROR    0x08U




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



#define DEV_VDD             0
#define DEV_CHARGE_VDD      1
#define DEV_TEMP            2
#define DEV_SOP             3
#define DEV_SCT             4
#define DEV_SFL             5
#define DEV_CSA             6
#define DEV_CSD             7
#define DEV_CAS             8
#define NET_FASE_V_L1       9
#define NET_FASE_V_L2       10
#define NET_FASE_V_L3       11
#define GEN_FASE_V_L1       12
#define GEN_FASE_V_L2       13
#define GEN_FASE_V_L3       14
#define GEN_FASE_C_L1       15
#define GEN_FASE_C_L2       16
#define GEN_FASE_C_L3       17

#define GEN_RMS_CUR         0
#define GEN_FREQ            1
#define GEN_FASE_V          2
#define GEN_LINE_V          3
#define GEN_L1_LINE_C       4
#define GEN_L1_FREQ         5
#define GEN_L1_LINE_V       6
#define GEN_L1_REAL_POWER   7
#define GEN_L1_ACTIVE_POWER 8
#define GEN_L1_REAC_POWER   9
#define GEN_L2_LINE_C       10
#define GEN_L2_LINE_V       11
#define GEN_L2_REAL_POWER   12
#define GEN_L2_ACTIVE_POWER 13
#define GEN_L2_REAC_POWER   14
#define GEN_L3_LINE_C       15
#define GEN_L3_LINE_V       16
#define GEN_L3_REAL_POWER   17
#define GEN_L3_ACTIVE_POWER 18
#define GEN_L3_REAC_POWER   19
#define GEN_APPER_POWER     20
#define GEN_REAL_POWER      21
#define GEN_ACTIVE_POWER    22
#define GEN_REACTIVE_POWER  23
#define NET_FREQ            25
#define NET_L1_LINE_V       26
#define NET_L2_LINE_V       27
#define NET_L3_LINE_V       28
#define GEN_AVER_COS        29
#define GEN_AVER_V          30
#define GEN_MAX_C           31
#define NET_L1_FASE_V       32
#define NET_L3_FASE_V       33
#define NET_L2_FASE_V       34
#define GEN_L3_FASE_V       35
#define GEN_L2_FASE_V       36
#define GEN_L1_FASE_V       37
#define GEN_L1_CUR          38
#define GEN_L2_CUR          39
#define GEN_L3_CUR          40
/*
 * Номиналы резисторов
 */

#define RCSHUNT         0.01  //Сопротивление шунтирующих резисторов токовых входов
#define OPTRANSCOOF     20    //Коофециент усиления операционного усилителя на  токовых входах
#define VRef            3.3



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

/*
 * Параметры частоты дискретезации
 */
#define LOW_ADC_FREQ    2000U
#define HIGH_ADC_FREQ   70000U
#define MIDLE_ADC_FREQ  40000U

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
uint8_t xADCGetFLChType(void);
uint8_t xADCGetxOPChType(void);
uint8_t xADCGetxCTChType(void);
fix16_t xADCGetGENActivePower();
fix16_t xADCGetGENReactivePower();
fix16_t xADCGetGENRealPower();
fix16_t xADCGetGENL1RealPower();
fix16_t xADCGetGENL2RealPower();
fix16_t xADCGetGENL3RealPower();
fix16_t xADCGetGENL1ActivePower();
fix16_t xADCGetGENL2ActivePower();
fix16_t xADCGetGENL3ActivePower();
fix16_t xADCGetGENL1ReactivePower();
fix16_t xADCGetGENL2ReactivePower();
fix16_t xADCGetGENL3ReactivePower();

fix16_t xADCGetCAC();
fix16_t xADCGetVDD();              // Функция возращает наряжения АКБ.
fix16_t xADCGetSOP();
fix16_t xADCGetSCT();
fix16_t xADCGetSFL();
fix16_t xADCGetNETL3();
fix16_t xADCGetNETL2();
fix16_t xADCGetNETL1();
fix16_t xADCGetNETL1Lin();         //Линейное напряжене Uab сети
fix16_t xADCGetNETL2Lin();        //Линейное напряжене Ubс сети
fix16_t xADCGetNETL3Lin();         //Линейное напряжене Uсa сети
fix16_t xADCGetGENL1();           //Фазное  напряжене Uan генератора
fix16_t xADCGetGENL2();           //Фазное  напряжене Ubn генератора
fix16_t xADCGetGENL3();           //Фазное  напряжене Ucn генератора
fix16_t xADCGetGENL1Lin();        //Линейное напряжене Uab генератора
fix16_t xADCGetGENL2Lin();        //Линейное напряжене Ubс генератора
fix16_t xADCGetGENL3Lin();        //Линейное напряжене Uсa генератора
fix16_t xADCGetGENL1Cur();        //Фазный ток Ian генератора
fix16_t xADCGetGENL2Cur();        //Фазный ток Ibn генератора
fix16_t xADCGetGENL3Cur();        //Фазный ток Icn генератора
fix16_t xADCGetGENL1CurLin();
fix16_t xADCGetGENL2CurLin();
fix16_t xADCGetGENL3CurLin();
uint8_t uADCGetValidDataFlag();
fix16_t xADCGetNETLFreq();
fix16_t xADCGetGENLFreq();
fix16_t xADCGetCOSFi();
fix16_t xADCGetGENAverV();
fix16_t xADCGetGENMaxC();
xADCRotatinType xADCGetGenFaseRotation();
xADCRotatinType xADCGetNetFaseRotation();
void    vADC_Ready(uint8_t adc_number);
void    StartADCTask(void *argument);
void    vGetADCDC( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID );
uint8_t uADCGetDCChError();
fix16_t  xADCConvert (void * data);
fix16_t  xADCConvertTemp (void * data);
fix16_t  vADCSetAinState( void * data);
fix16_t  xConvertFaseV(void * data);
fix16_t  xConvertFaseC(void * data);
xADCGenDetectType vADCGetGenFreqPres( void );
ELECTRO_SCHEME xADCGetScheme(void);
#endif /* INC_ADC_H_ */

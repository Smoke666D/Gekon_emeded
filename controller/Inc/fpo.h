/*
 * fpo.h
 *
 *  Created on: 12 июн. 2020 г.
 *      Author: photo_Mickey
 */

#ifndef INC_FPO_H_
#define INC_FPO_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "controllerTypes.h"
/*------------------------ Macros --------------------------------------*/
/*------------------------ Define --------------------------------------*/
#define  FPO_NUMBER           10U
#define  FPO_DIS_NUMBER       ( FPO_NUMBER / 2U )
#define  FPO_FUNCTION_NUMBER  22U
#define  FPO_A                0U
#define  FPO_B                1U
#define  FPO_C                2U
#define  FPO_D                3U
#define  FPO_E                4U
#define  FPO_F                5U
#define  FPO_G                6U
#define  FPO_H                7U
#define  FPO_I                8U
#define  FPO_J                9U
#define  FPO_DIS_AB           0U
#define  FPO_DIS_CD           1U
#define  FPO_DIS_EF           2U
#define  FPO_DIS_GH           3U
#define  FPO_DIS_IJ           4U
/*------------------------- Check --------------------------------------*/
#if ( ( FPO_NUMBER % 2U ) != 0U )
  #error Wrong FPO number
#endif
/*------------------------- Enum ---------------------------------------*/
typedef enum
{
  FPO_FUN_NONE,                    /* 0  Не использовать */
  FPO_FUN_DPS_READY,               /* 1  ДЭС готов  */
  FPO_FUN_READY_TO_START,          /* 2  Готов к запуску */
  FPO_FUN_GEN_READY,               /* 3  Готовность генератора */
  FPO_FUN_ALARM,                   /* 4  Общий аварийный сигнал */
  FPO_FUN_MAINS_FAIL,              /* 5  Общая ошибка сети */
  FPO_FUN_WARNING,                 /* 6  Общее предупреждение */
  FPO_FUN_TURN_ON_GEN,             /* 7  Включить генератор */
  FPO_FUN_TURN_ON_GEN_IMPULSE,     /* 8  Импульс включения генератора */
  FPO_FUN_TURN_OFF_GEN_IMPULSE,    /* 9  Импульс выключения генератора */
  FPO_FUN_TURN_ON_MAINS,           /* 10 Включить сеть */
  FPO_FUN_TURN_ON_MAINS_IMPULSE,   /* 11 Импульс включения сети */
  FPO_FUN_TURN_OFF_MAINS_IMPULSE,  /* 12 Импульс включения сети */
  FPO_FUN_COOLANT_COOLER,          /* 13 Управление охладителем охлаждающей жидкости */
  FPO_FUN_COOLANT_HEATER,          /* 14 Управление подогревателем охлаждающей  жидкости */
  FPO_FUN_STOP_SOLENOID,           /* 15 Стоповый соленоид */
  FPO_FUN_FUEL_BOOST,              /* 16 Подкачка топлива */
  FPO_FUN_FUEL_RELAY,              /* 17 Топливное реле */
  FPO_FUN_STARTER_RELAY,           /* 18 Реле стартера */
  FPO_FUN_PREHEAT,                 /* 19 Выход предпрогрева */
  FPO_FUN_IDLING,                  /* 20 Работа на холостом ходу */
  FPO_FUN_BUZZER,                  /* 21 Звуковая сигнализация */
} FPO_FUNCTION;

typedef enum
{
  FPO_POL_NORMAL_OPEN,
  FPO_POL_NORMAL_CLOSE,
} FPO_POLARITY;
/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  GPIO_TypeDef*  outPortA;
  uint16_t       outPinA;
  GPIO_TypeDef*  outPortB;
  uint16_t       outPinB;
  GPIO_TypeDef*  disPortAB;
  uint16_t       disPinAB;

  GPIO_TypeDef*  outPortC;
  uint16_t       outPinC;
  GPIO_TypeDef*  outPortD;
  uint16_t       outPinD;
  GPIO_TypeDef*  disPortCD;
  uint16_t       disPinCD;

  GPIO_TypeDef*  outPortE;
  uint16_t       outPinE;
  GPIO_TypeDef*  outPortF;
  uint16_t       outPinF;
  GPIO_TypeDef*  disPortEF;
  uint16_t       disPinEF;

  GPIO_TypeDef*  outPortG;
  uint16_t       outPinG;
  GPIO_TypeDef*  outPortH;
  uint16_t       outPinH;
  GPIO_TypeDef*  disPortGH;
  uint16_t       disPinGH;

  GPIO_TypeDef*  outPortI;
  uint16_t       outPinI;
  GPIO_TypeDef*  outPortJ;
  uint16_t       outPinJ;
  GPIO_TypeDef*  disPortIJ;
  uint16_t       disPinIJ;
} FPO_INIT;

typedef struct __packed
{
  /* Phisical */
  GPIO_TypeDef*  port;   /* GPIO port*/
  uint16_t       pin;    /* Pin number */
  /* Logic */
  uint8_t        mask;
  FPO_FUNCTION   function : 5U;
  FPO_POLARITY   polarity : 1U;
  TRIGGER_STATE  state    : 1U;
} FPO;

typedef struct __packed
{
  GPIO_TypeDef*  port;   /* GPIO port*/
  uint16_t       pin;    /* Pin number */
} FPO_DIS;
/*----------------------- Functions ------------------------------------*/
void          vFPOinit ( const FPO_INIT* init );
void          vFPOtest ( void );
void          vFPOdataInit ( void );
uint8_t       uFPOisEnable ( FPO_FUNCTION fun );
void          vFPOsetPump ( RELAY_STATUS stat );
TRIGGER_STATE eFPOgetPump ( void );
void          vFPOsetBooster ( RELAY_STATUS stat );
TRIGGER_STATE eFPOgetBooster ( void );
void          vFPOsetPreheater ( RELAY_STATUS stat );
TRIGGER_STATE eFPOgetPreheater ( void );
void          vFPOsetCooler ( RELAY_STATUS stat );
TRIGGER_STATE eFPOgetCooler ( void );
void          vFPOsetHeater ( RELAY_STATUS stat );
TRIGGER_STATE eFPOgetHeater ( void );
void          vFPOsetStarter( RELAY_STATUS stat );
TRIGGER_STATE eFPOgetStarter ( void );
void          vFPOsetStopSolenoid ( RELAY_STATUS stat );
TRIGGER_STATE eFPOgeyStopSolenoid ( void );
void          vFPOsetIdle ( RELAY_STATUS stat );
TRIGGER_STATE eFPOgeyIdle ( void );
void          vFPOsetGenSw ( RELAY_STATUS stat );
TRIGGER_STATE eFPOgetGenSw ( void );
void          vFPOsetGenOnImp ( RELAY_STATUS stat );
TRIGGER_STATE eFPOgetGenOnImp ( void );
void          vFPOsetGenOffImp ( RELAY_STATUS stat );
TRIGGER_STATE eFPOgetGenOffImp ( void );
void          vFPOsetMainsSw ( RELAY_STATUS stat );
TRIGGER_STATE eFPOgetMainsSw ( void );
void          vFPOsetMainsOnImp ( RELAY_STATUS stat );
TRIGGER_STATE eFPOgetMainsOnImp ( void );
void          vFPOsetMainsOffImp ( RELAY_STATUS stat );
TRIGGER_STATE eFPOgetMainsOffImp ( void );
void          vFPOsetWarning ( RELAY_STATUS stat );
TRIGGER_STATE eFPOgetWarning ( void );
void          vFPOsetMainsFail ( RELAY_STATUS stat );
TRIGGER_STATE eFPOgetMainsFail ( void );
void          vFPOsetAlarm ( RELAY_STATUS stat );
TRIGGER_STATE eFPOgetAlarm ( void );
void          vFPOsetGenReady ( RELAY_STATUS stat );
TRIGGER_STATE eFPOgetGenReady ( void );
void          vFPOsetReadyToStart ( RELAY_STATUS stat );
TRIGGER_STATE eFPOgetReadyToStart ( void );
void          vFPOsetDpsReady ( RELAY_STATUS stat );
TRIGGER_STATE eFPOgetDpsReady ( void );
void          vFPOsetBuzzer ( RELAY_STATUS stat );
TRIGGER_STATE eFPOgetBuzzer ( void );
TRIGGER_STATE eFPOgetState ( uint8_t n );
uint16_t      uFPOgetData ( void );
/*----------------------------------------------------------------------*/
#endif /* INC_FPO_H_ */

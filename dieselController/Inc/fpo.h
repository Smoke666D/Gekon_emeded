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
#define  FPO_NUMBER           6U
#define  FPO_DIS_NUMBER       ( FPO_NUMBER / 2U )
#define  FPO_FUNCTION_NUMBER  21U
#define  FPO_A                0U
#define  FPO_B                1U
#define  FPO_C                2U
#define  FPO_D                3U
#define  FPO_E                4U
#define  FPO_F                5U
#define  FPO_DIS_AB           0U
#define  FPO_DIS_CD           1U
#define  FPO_DIS_EF           2U
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
} FPO_INIT;

typedef struct __packed
{
  /* Phisical */
  GPIO_TypeDef*  port;   /* GPIO port*/
  uint16_t       pin;    /* Pin number */
  /* Logic */
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
void          vFPOdataInit ( void );
uint8_t       uFPOisEnable ( FPO_FUNCTION fun );
void          vFPOsetPump ( RELAY_STATUS stat );
void          vFPOsetBooster ( RELAY_STATUS stat );
void          vFPOsetPreheater ( RELAY_STATUS stat );
void          vFPOsetCooler ( RELAY_STATUS stat );
void          vFPOsetHeater ( RELAY_STATUS stat );
void          vFPOsetStarter( RELAY_STATUS stat );
void          vFPOsetStopSolenoid ( RELAY_STATUS stat );
void          vFPOsetIdle ( RELAY_STATUS stat );
void          vFPOsetGenSw ( RELAY_STATUS stat );
void          vFPOsetGenOnImp ( RELAY_STATUS stat );
void          vFPOsetGenOffImp ( RELAY_STATUS stat );
void          vFPOsetMainsSw ( RELAY_STATUS stat );
void          vFPOsetMainsOnImp ( RELAY_STATUS stat );
void          vFPOsetMainsOffImp ( RELAY_STATUS stat );
void          vFPOsetWarning ( RELAY_STATUS stat );
void          vFPOsetMainsFail ( RELAY_STATUS stat );
void          vFPOsetAlarm ( RELAY_STATUS stat );
void          vFPOsetGenReady ( RELAY_STATUS stat );
void          vFPOsetReadyToStart ( RELAY_STATUS stat );
void          vFPOsetDpsReady ( RELAY_STATUS stat );
TRIGGER_STATE eFPOgetState ( uint8_t n );
/*----------------------------------------------------------------------*/
#endif /* INC_FPO_H_ */

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
#define  FPO_FUNCTION_NUMBER  22U
#define  FPO_A                0U
#define  FPO_B                1U
#define  FPO_C                2U
#define  FPO_D                3U
#define  FPO_E                4U
#define  FPO_F                5U
/*------------------------- Enum ---------------------------------------*/
typedef enum
{
  FPO_FUN_NONE,                    /* Не использовать */
  FPO_FUN_AUTO_MODE,               /* Автоматический режим  */
  FPO_FUN_COMMON_NET_FAIL,         /* Общее нарушение сети */
  FPO_FUN_READY_TO_START,          /* Готов к запуску */
  FPO_FUN_GEN_READY,               /* Готовность генератора */
  FPO_FUN_ALARM,                   /* Общий аварийный сигнал */
  FPO_FUN_DES_FAIL,                /* Неисправность ДЭС */
  FPO_FUN_WARNING,                 /* Общее предупреждение */
  FPO_FUN_TURN_ON_GEN,             /* Включить генератор */
  FPO_FUN_TURN_ON_GEN_IMPULSE,     /* Импульс включения генератора */
  FPO_FUN_TURN_OFF_GEN_IMPULSE,    /* Импульс выключения генератора */
  FPO_FUN_TURN_ON_MAINS,           /* Включить сеть */
  FPO_FUN_TURN_ON_MAINS_IMPULSE,   /* Импульс включения сети */
  FPO_FUN_TURN_OFF_MAINS_IMPULSE,  /* Импульс включения сети */
  FPO_FUN_COOLANT_COOLER,          /* Управление охладителем охлаждающей жидкости */
  FPO_FUN_COOLANT_HEATER,          /* Управление подогревателем охлаждающей  жидкости */
  FPO_FUN_STOP_SOLENOID,           /* Стоповый соленоид */
  FPO_FUN_FUEL_BOOST,              /* Подкачка топлива */
  FPO_FUN_FUEL_RELAY,              /* Топливное реле */
  FPO_FUN_STARTER_RELAY,           /* Реле стартера */
  FPO_FUN_PREHEAT,                 /* Выход предпрогрева */
  FPO_FUN_IDLING,                  /* Работа на холостом ходу */
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
  GPIO_TypeDef*  outPort;   /* GPIO port*/
  uint16_t       outPin;    /* Pin number */
  GPIO_TypeDef*  disPort;   /* GPIO port*/
  uint16_t       disPin;    /* Pin number */
  /* Logic */
  FPO_FUNCTION   function;
  FPO_POLARITY   polarity;
} FPO;
/*----------------------- Functions ------------------------------------*/
void    vFPOinit ( FPO_INIT* init );
uint8_t uFPOisEnable ( FPO_FUNCTION fun );
void    vFPOsetPump ( RELAY_STATUS stat );
void    vFPOsetBooster ( RELAY_STATUS stat );
void    vFPOsetPreheater ( RELAY_STATUS stat );
void    vFPOsetCooler ( RELAY_STATUS stat );
void    vFPOsetHeater ( RELAY_STATUS stat );
void    vFPOsetStarter( RELAY_STATUS stat );
void    vFPOsetStopSolenoid ( RELAY_STATUS stat );
void    vFPOsetIdle ( RELAY_STATUS stat );
void    vFPOsetGenSw ( RELAY_STATUS stat );
void    vFPOsetGenOnImp ( RELAY_STATUS stat );
void    vFPOsetGenOffImp ( RELAY_STATUS stat );
void    vFPOsetMainsSw ( RELAY_STATUS stat );
void    vFPOsetMainsOnImp ( RELAY_STATUS stat );
void    vFPOsetMainsOffImp ( RELAY_STATUS stat );
/*----------------------------------------------------------------------*/
#endif /* INC_FPO_H_ */

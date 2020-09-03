/*
 * fpi.h
 *
 *  Created on: 12 июн. 2020 г.
 *      Author: photo_Mickey
 */

#ifndef INC_FPI_H_
#define INC_FPI_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "logicCommon.h"
/*------------------------ Macros --------------------------------------*/
/*------------------------ Define --------------------------------------*/
#define  FPI_USER_MESSAGE_LENGTH  16U
#define  FPI_NUMBER               4U
#define  FPI_FUNCTION_NUM         11U
#define  FPI_A                    0U
#define  FPI_B                    1U
#define  FPI_C                    2U
#define  FPI_D                    3U
/*------------------------- Enum ---------------------------------------*/
typedef enum
{
  FPI_FUN_NONE,               /* Не использовать */
  FPI_FUN_USER,               /* Пользовательская */
  FPI_FUN_ALARM_RESET,        /* Сброс аварийного сигнала */
  FPI_FUN_OIL_LOW_PRESSURE,   /* Датчик давления масла */
  FPI_FUN_HIGHT_ENGINE_TEMP,  /* Высокая температура двигателя */
  FPI_FUN_LOW_FUEL,           /* Сигнал низкого уровня топлива */
  FPI_FUN_REMOTE_START,       /* Дистанционный запуск без нагрузки */
  FPI_FUN_IDLING,             /* Работа на холостом ходу */
  FPI_FUN_BAN_AUTO_START,     /* Запрет автоматического запуска */
  FPI_FUN_BAN_GEN_LOAD,       /* Запрет нагрузки генератора */
  FPI_FUN_BAN_AUTO_SHUTDOWN,  /* Запрет автоматического останова при восстановлении параметров сети */
} FPI_FUNCTION;

typedef enum
{
  FPI_POL_NORMAL_OPEN,
  FPI_POL_NORMAL_CLOSE,
} FPI_POLARITY;

typedef enum
{
  FPI_LEVEL_LOW,
  FPI_LEVEL_HIGH,
} FPI_LEVEL;

typedef enum
{
  FPI_ACT_EMERGENCY_STOP,      /* Аварийный останов */
  FPI_ACT_LOAD_SHUTDOWN,       /* Отключение нагрузки */
  FPI_ACT_WARNING,             /* Предупреждение */
  FPI_ACT_MESSAGE,             /* Индикация */
  FPI_ACT_NONE,                /* Действие в соответсвии с основной программой*/
} FPI_ACTION;

typedef enum
{
  FRI_ARM_ALWAYS,              /* Всегда */
  FPI_ARM_TIMER_STOP,          /* Оканчания таймера блокировки контроля ДГУ */
  FPI_ARM_STARTER_SCROLL_END,  /* Окончания прокрутки стартера */
  FRI_ARM_NEVER,               /* Никогда */
} FPI_ARMING;

typedef enum
{
  FPI_BLOCK,
  FPI_IDLE,
  FPI_TRIGGERED,
  FPI_ARMED,
  FPI_FINISH,
} FPI_STATE;
/*----------------------- Callbacks ------------------------------------*/
typedef uint8_t ( *armingCallBack )( void ); /* Stream call back type */
/*----------------------- Structures -----------------------------------*/
typedef struct
{
  FPI_LEVEL    level;
  FPI_FUNCTION function;
  FPI_ACTION   action;
  uint16_t*    message;
} FPI_EVENT;

typedef struct
{
  /* Phisical */
  GPIO_TypeDef*   port;                             /* GPIO port*/
  uint16_t        pin;                              /* Pin number */
  /* Logic */
  FPI_POLARITY    polarity;                         /* Polarity of triggering */
  FPI_FUNCTION    function;                         /* Meaning of the input */
  FPI_ACTION      action;                           /* What to do on trig */
  FPI_ARMING      arming;                           /* Condition of triggering */
  uint16_t        delay;                            /* Delay after arming in seconds */
  uint16_t        message[FPI_USER_MESSAGE_LENGTH]; /* User string message to the display */
  FPI_LEVEL       level;                            /* Current level */
  /* Callbacks */
  armingCallBack  getArming;                        /* Function check condition of triggering */
  /* System */
  FPI_STATE       state;                            /* Condition of the FPI */
  timerID_t       timerID;
} FPI;

typedef struct
{
  GPIO_TypeDef*  portA;
  uint16_t       pinA;
  GPIO_TypeDef*  portB;
  uint16_t       pinB;
  GPIO_TypeDef*  portC;
  uint16_t       pinC;
  GPIO_TypeDef*  portD;
  uint16_t       pinD;
} FPI_INIT;
/*----------------------- Extern ---------------------------------------*/
/*----------------------- Functions ------------------------------------*/
void          vFPIinit ( FPI_INIT* init );
QueueHandle_t pFPIgetQueue ( void );
void          vFPIsetBlock ( void );
FPI_LEVEL     eFPIcheckLevel ( FPI_FUNCTION function );
/*----------------------------------------------------------------------*/
#endif /* INC_FPI_H_ */

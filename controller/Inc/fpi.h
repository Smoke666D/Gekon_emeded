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
#include "controllerTypes.h"
/*------------------------ Macros --------------------------------------*/
/*------------------------ Define --------------------------------------*/
#define  FPI_USER_MESSAGE_LENGTH  16U
#define  FPI_NUMBER               4U
#define  FPI_FUNCTION_NUM         12U
#define  FPI_A                    0U
#define  FPI_B                    1U
#define  FPI_C                    2U
#define  FPI_D                    3U
#define  FPI_TASK_DELAY           200U
/*------------------------- Enum ---------------------------------------*/
typedef enum
{
  FPI_FUN_NONE,               /* 0  Не использовать                   */
  FPI_FUN_USER,               /* 1  Пользовательская                  */
  FPI_FUN_ALARM_RESET,        /* 2  Сброс аварийного сигнала          */
  FPI_FUN_OIL_LOW_PRESSURE,   /* 3  Датчик давления масла             */
  FPI_FUN_HIGHT_ENGINE_TEMP,  /* 4  Высокая температура двигателя     */
  FPI_FUN_LOW_FUEL,           /* 5  Сигнал низкого уровня топлива     */
  FPI_FUN_REMOTE_START,       /* 6  Дистанционный запуск без нагрузки */
  FPI_FUN_IDLING,             /* 7  Работа на холостом ходу           */
  FPI_FUN_BAN_AUTO_START,     /* 8  Запрет автоматического запуска    */
  FPI_FUN_BAN_GEN_LOAD,       /* 9  Запрет нагрузки генератора        */
  FPI_FUN_BAN_AUTO_SHUTDOWN,  /* 10 Запрет автоматического останова при восстановлении параметров сети */
  FPI_FUN_EMERGENCY_STOP      /* 11 */
} FPI_FUNCTION;

typedef enum
{
  FPI_POL_NORMAL_OPEN,
  FPI_POL_NORMAL_CLOSE,
} FPI_POLARITY;

typedef enum
{
  FPI_LEVEL_LOW  = 0x00U,
  FPI_LEVEL_HIGH,
} FPI_LEVEL;

typedef enum
{
  FPI_ACT_EMERGENCY_STOP,      /* 0 Аварийный останов */
  FPI_ACT_SHUTDOWN,       /* 1 Отключение нагрузки */
  FPI_ACT_WARNING,             /* 2 Предупреждение */
  FPI_ACT_NONE,                /* 3 Действие в соответсвии с основной программой*/
} FPI_ACTION;

typedef enum
{
  FRI_ARM_ALWAYS,              /* 0 Всегда */
  FPI_ARM_TIMER_STOP,          /* 1 Оканчания таймера блокировки контроля ДГУ */
  FPI_ARM_STARTER_SCROLL_END,  /* 2 Окончания прокрутки стартера */
  FRI_ARM_NEVER,               /* 3 Никогда */
} FPI_ARMING;

typedef enum
{
  FPI_IDLE,
  FPI_TRIGGERED,
} FPI_STATE;
/*----------------------- Callbacks ------------------------------------*/
typedef TRIGGER_STATE ( *armingCallBack )( void ); /* Stream call back type */
/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  FPI_LEVEL    level;
  FPI_FUNCTION function;
  uint8_t      number;
} FPI_EVENT;

typedef struct __packed
{
  /* Phisical */
  GPIO_TypeDef*   port;                             /* GPIO port*/
  uint16_t        pin;                              /* Pin number */
  /* Logic */
  FPI_POLARITY    polarity : 1U;                    /* Polarity of triggering */
  FPI_FUNCTION    function : 4U;                    /* Meaning of the input */
  FPI_ARMING      arming   : 2U;                    /* Condition of triggering */
  FPI_LEVEL       level    : 1U;                    /* Current level */
  FPI_STATE       state    : 2U;                    /* Condition of the FPI */
  TRIGGER_STATE   trigger   : 1U;
  uint16_t        message[FPI_USER_MESSAGE_LENGTH]; /* User string message to the display */
  ERROR_TYPE      userError;                        /* Error for user function */
  /* Callbacks */
  armingCallBack  getArming;                        /* Function check condition of triggering */
  /* System */
  SYSTEM_TIMER    timer;
} FPI;

typedef struct __packed
{
  GPIO_TypeDef*  portA;
  uint16_t       pinA;
  GPIO_TypeDef*  portB;
  uint16_t       pinB;
  GPIO_TypeDef*  portC;
  uint16_t       pinC;
  GPIO_TypeDef*  portD;
  uint16_t       pinD;
  GPIO_TypeDef*  portCS;
  uint16_t       pinCS;
} FPI_INIT;
/*----------------------- Extern ---------------------------------------*/
extern osThreadId_t fpiHandle;
/*----------------------- Functions ------------------------------------*/
void              vFPIinit ( const FPI_INIT* init );
void              vFPIreset ( void );
void              vFPIprint ( FPI_FUNCTION function, const char* str );
QueueHandle_t     pFPIgetQueue ( void );
void              vFPIsetBlock ( void );
TRIGGER_STATE     eFPIgetState ( uint8_t n );
uint16_t*         uFPIgetMessage ( uint8_t n );
uint16_t          uFPIgetData ( void );
uint16_t          uFPIgetRawData ( void );
SYSTEM_EVENT_TYPE eFPIgetUserEventType ( uint8_t n );
FPI_LEVEL         eFPIcheckLevel ( FPI_FUNCTION function );
/*----------------------------------------------------------------------*/
#endif /* INC_FPI_H_ */

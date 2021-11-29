/*
 * system.h
 *
 *  Created on: 8 нояб. 2021 г.
 *      Author: 79110
 */

#ifndef INC_SYSTEM_H_
#define INC_SYSTEM_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "cmsis_os.h"
/*------------------------ Define --------------------------------------*/
#define  OPTIMIZ
#define  OPTIMIZ_LEVEL   "-O2"
#define  WRITE_LOG_TO_SD

#define  FPI_TASK_PRIORITY             osPriorityLow
#define  ENGINE_TASK_PRIORITY          osPriorityLow     /* Engine and Electro priority need to be equal */
#define  ELECTRO_TASK_PRIORITY         osPriorityLow     /* Engine and Electro priority need to be equal */
#define  CONTROLLER_TASK_PRIORITY      osPriorityNormal
#define  MEASUREMENT_TASK_PRIORITY     osPriorityLow
#define  MB_TASK_PRIORITY              osPriorityNormal
#define  FATSD_TASK_PRIORITY           osPriorityLow
#define  ETHERNET_LINK_TASK_PRIORITY   osPriorityLow
#define  FPI_TASK_STACK_SIZE           ( 1024U - 180U * 4U  ) /* HWM = 25*/
#define  ENGINE_TASK_STACK_SIZE        1024U
#define  ELECTRO_TASK_STACK_SIZE       1024U
#define  CONTROLLER_TASK_STACK_SIZE    1024U
#define  MEASUREMENT_TASK_STACK_SIZE   1024U
#define  MB_TASK_STACK_SIZE            512U
#define  ETHERNET_LINK_TASK_STACK_SIZE 512U
#define  FATSD_TASK_STACK_SIZE         1024U

#if ( ENGINE_TASK_PRIORITY != ELECTRO_TASK_PRIORITY )
  #error Engine and electro tasks have difrent priority
#endif

#define SYS_MAX_TSAK_NUMBER     20U
#define SYS_BAR_LENGTH          10U
/*------------------------ Macros --------------------------------------*/
/*------------------------ Enum ----------------------------------------*/
/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  osThreadId_t thread;
  uint32_t     size;
} TASK_ANALIZE;

typedef struct __packed
{
  char*    name;
  uint16_t usage;
  uint16_t total;
} SYSTEM_DATA;
/*----------------------- Functions ------------------------------------*/
void    vSYSaddTask ( osThreadId_t thread, uint32_t size );
void    vSYSprintData ( void );
void    vSYSgetHeapData ( SYSTEM_DATA* data );
uint8_t uSYSgetTaskumber ( void );
void    vSYSgetTaskData ( uint8_t n, SYSTEM_DATA* data );
/*----------------------------------------------------------------------*/
#endif /* INC_SYSTEM_H_ */

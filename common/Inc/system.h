/*
 * system.h
 *
 *  Created on: 8 нояб. 2021 г.
 *      Author: 79110
 */

#ifndef INC_SYSTEM_H_
#define INC_SYSTEM_H_
/*----------------------- Includes -------------------------------------*/
/*------------------------ Define --------------------------------------*/
#define  OPTIMIZ
#define  OPTIMIZ_LEVEL   "-O2"

#define  FPI_TASK_PRIORITY             osPriorityLow
#define  ENGINE_TASK_PRIORITY          osPriorityLow     /* Engine and Electro priority need to be equal */
#define  ELECTRO_TASK_PRIORITY         osPriorityLow     /* Engine and Electro priority need to be equal */
#define  CONTROLLER_TASK_PRIORITY      osPriorityNormal
#define  MEASUREMENT_TASK_PRIORITY     osPriorityLow
#define  MB_TASK_PRIORITY              osPriorityNormal
#define  ETHERNET_LINK_TASK_PRIORITY   osPriorityLow
#define  FPI_TASK_STACK_SIZE           ( 1024U - 180U * 4U  ) /* HWM = 25*/
#define  ENGINE_TASK_STACK_SIZE        1024U
#define  ELECTRO_TASK_STACK_SIZE       1024U
#define  CONTROLLER_TASK_STACK_SIZE    1024U
#define  MEASUREMENT_TASK_STACK_SIZE   1024U
#define  MB_TASK_STACK_SIZE            512U
#define  ETHERNET_LINK_TASK_STACK_SIZE 512U

#if ( ENGINE_TASK_PRIORITY != ELECTRO_TASK_PRIORITY )
  #error Engine and electro tasks have difrent priority
#endif
/*------------------------ Macros --------------------------------------*/
/*------------------------ Enum ----------------------------------------*/
/*----------------------- Structures -----------------------------------*/
/*----------------------- Functions ------------------------------------*/
/*----------------------------------------------------------------------*/
#endif /* INC_SYSTEM_H_ */

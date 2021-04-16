/*
 * alarm.h
 *
 *  Created on: 29 окт. 2020 г.
 *      Author: Mike Mihailov
 */

#ifndef INC_ALARM_H_
#define INC_ALARM_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "controllerTypes.h"
/*------------------------ Define --------------------------------------*/
#define  ACTIV_ERROR_LIST_SIZE      20U
#define  DEFINE_ERROR_LIST_ADR      ( ACTIV_ERROR_LIST_SIZE + 1U )
/*------------------------ Macros --------------------------------------*/
/*----------------------- Constant -------------------------------------*/
/*------------------------- Enum ---------------------------------------*/
typedef enum
{
  ERROR_LIST_STATUS_EMPTY,
  ERROR_LIST_STATUS_NOT_EMPTY,
  ERROR_LIST_STATUS_OVER,
} ERROR_LIST_STATUS;
/*----------------------- Callbacks ------------------------------------*/
/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  ERROR_LIST_STATUS status : 2U;
  LOG_RECORD_TYPE   array[ACTIV_ERROR_LIST_SIZE];
  uint8_t           counter;
} ACTIVE_ERROR_LIST;
/*----------------------- Extern ---------------------------------------*/
/*----------------------- Functions ------------------------------------*/
void              vALARMinit ( void );
void              vALARMreInit ( void );
void              vALARMcheck ( ALARM_TYPE* alarm, fix16_t val );
void              vALARMreset ( ALARM_TYPE* alarm );
void              vERRORreset ( ERROR_TYPE* error );
void              vERRORcheck ( ERROR_TYPE* error, uint8_t flag );
void              vERRORrelax ( ERROR_TYPE* error );
ERROR_LIST_STATUS eLOGICERactiveErrorList ( ERROR_LIST_CMD cmd, LOG_RECORD_TYPE* record, uint8_t* adr );
TRIGGER_STATE     eERRORisActive ( ERROR_TYPE* error );
fix16_t           fALARMgetHysteresis ( void );
/*----------------------------------------------------------------------*/
#endif /* INC_ALARM_H_ */

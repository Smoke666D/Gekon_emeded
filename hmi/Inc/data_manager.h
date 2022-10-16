/*
 * data_manager.h
 *
 *  Created on: 4 мар. 2020 г.
 *      Author: igor.dymov
 */

#ifndef INC_DATA_MANAGER_H_
#define INC_DATA_MANAGER_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
/*------------------------------ Define --------------------------------------*/
#define READ_COMMNAD          0x00U
#define INC_COMMAND           0x01U
#define DEC_COMMNAD           0x02U
#define ENTER_COMMAND         0x03U
#define ESC_COMMNAD	          0x04U
#define ID_TEST_DATA          1U
#define ID_RTC_DATA           2U
#define DATA_DISCRIPTOR_COUNT 2U
#define MAX_NUMBER_STRING_LEN 10U
/*------------------------------ Enum ----------------------------------------*/
/* Возмонжные типы данных */
typedef enum
{
  FIX_POINT,
  CONFIG_DATA,
  TIME_DATA,
  UINTEGER,
  TIME,
  NO_DATA,
} DATA_TYPE;
/* Дискриптор данных для вывода */
typedef struct __packed
{
  uint16_t  DataID;
	DATA_TYPE Type;
	void*     pDataPointer;
} DataDescriptor;
/* Структура для хранения данных */
typedef struct __packed
{
   uint16_t Number;
   uint16_t Factor;
   uint8_t  Sign;
} DataRecord;
/*----------------------------- Functions ------------------------------------*/
void vdmGetData(uint8_t command, uint16_t DataID,uint8_t * pchTextString);
/*----------------------------------------------------------------------------*/
#endif /* INC_DATA_MANAGER_H_ */

/*
 * data_manager.h
 *
 *  Created on: 4 мар. 2020 г.
 *      Author: igor.dymov
 */

#ifndef INC_DATA_MANAGER_H_
#define INC_DATA_MANAGER_H_


#define READ_COMMNAD  0x00
#define INC_COMMAND   0x01
#define DEC_COMMNAD   0x02
#define ENTER_COMMAND 0x03
#define ESC_COMMNAD	  0x04


#define ID_TEST_DATA   1
#define ID_RTC_DATA    2




//Возмонжные типы данных
typedef enum
{
  FIX_POINT,
  CONFIG_DATA,
  TIME_DATA,
  UINTEGER,
  TIME,
  NO_DATA,
} DATA_TYPE;

//Дискриптор данных для вывода
typedef struct
{
    uint16_t DataID;
	DATA_TYPE Type;
	void * pDataPointer;
} DataDescriptor;


#define MAX_NUMBER_STRING_LEN 10
//Структура для хранения данных
typedef struct
{
   uint16_t Number;
   uint16_t Factor;
   uint8_t Sign;
} DataRecord;




void vdmGetData(uint8_t command, uint16_t DataID,uint8_t * pchTextString);

#endif /* INC_DATA_MANAGER_H_ */

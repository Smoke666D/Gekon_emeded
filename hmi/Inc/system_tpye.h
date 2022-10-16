/*
 * system_tpye.h
 *
 *  Created on: 13 мар. 2020 г.
 *      Author: igor.dymov
 */

#ifndef INC_SYSTEM_TPYE_H_
#define INC_SYSTEM_TPYE_H_
/*----------------------------- Includes -------------------------------------*/
#include "fix16.h"
/*------------------------------ Enum ----------------------------------------*/
typedef enum
{
  READ,
  WRITE,
  DEC,
  INC,
  ESC,
} CALL_BACK_FUNCTION_COMMNAD;
typedef enum
{
  FIX_POINT,
  UBYTE,
  BYTE,
  WORD,
  UWORD
} DATA_TYPE;  /* System data type */
/*---------------------------- Structures --------------------------------------*/
typedef struct __packed
{
  fix16_t Data;
  fix16_t Step;
  fix16_t Max;
  fix16_t Min;
} FixPointDataUinit;
typedef struct __packed
{
  uint8_t Data;
  uint8_t Step;
  uint8_t Max;
  uint8_t Min;
} ByteDataUinit;
typedef struct __packed
{
  uint16_t Data;
  uint16_t Step;
  uint16_t Max;
  uint16_t Min;
} WordDataUinit;
/*----------------------------------------------------------------------------*/
#endif /* INC_SYSTEM_TPYE_H_ */

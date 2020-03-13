/*
 * system_tpye.h
 *
 *  Created on: 13 мар. 2020 г.
 *      Author: igor.dymov
 */

#ifndef INC_SYSTEM_TPYE_H_
#define INC_SYSTEM_TPYE_H_

#include "fix16.h"


typedef enum
{
  READ,
  WRITE,
  DEC,
  INC,
  ESC,
} CALL_BACK_FUNCTION_COMMNAD;

//System data type
typedef enum
{
  FIX_POINT,
  UBYTE,
  BYTE,
  WORD,
  UWORD
} DATA_TYPE;


typedef struct
{
	fix16_t Data;
	fix16_t Step;
	fix16_t Max;
	fix16_t Min;
} FixPointDataUinit;

typedef struct
{
	uint8_t Data;
	uint8_t Step;
	uint8_t Max;
	uint8_t Min;
} ByteDataUinit;

typedef struct
{
	uint16_t Data;
	uint16_t Step;
	uint16_t Max;
	uint16_t Min;
} WordDataUinit;



#endif /* INC_SYSTEM_TPYE_H_ */

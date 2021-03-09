/*
 * adcdata.h
 *
 *  Created on: 9 мар. 2021 г.
 *      Author: igor.dymov
 */

#ifndef INC_ADCDATA_H_
#define INC_ADCDATA_H_

#include "main.h"
#include "fix16.h"

/*typedef struct
{
  fix16_t  xDataConvert;
  uint16_t uDataSrc;
  fix16_t  xCoof;
  fix16_t  xOffset;
} xADCData;
*/

#define   SEN_TYPE_NONE         0x00
#define   SEN_TYPE_NORMAL_OPEN  0x01
#define   SEN_TYPE_NORMAL_CLOSE 0x02
#define   SEN_TYPE_RESISTIVE    0x03
#define   SEN_TYPE_CURRENT      0x04

#define   SEN_TYPE_MASK         0x0F

#define   COMMON_SENS_ERROR     0x10
#define   CHANNEL_SENS_ERROR    0x20

#define   SEN_ERROR_MASK        0xF0

typedef struct __packed
{
    uint8_t     StateConfig;
    fix16_t     xDataConvert;
    uint16_t    uDataSrc;
    fix16_t     (* pFunc)( void *, ...);
} xADCValue;

#endif /* INC_ADCDATA_H_ */

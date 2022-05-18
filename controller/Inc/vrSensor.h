/*
 * vrSensor.h
 *
 *  Created on: 23 июн. 2020 г.
 *      Author: photo_Mickey
 */

#ifndef INC_VRSENSOR_H_
#define INC_VRSENSOR_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "fix16.h"
/*------------------------ Macros --------------------------------------*/
/*------------------------ Define --------------------------------------*/
/*------------------------- Enum ---------------------------------------*/
/*----------------------- Callbacks ------------------------------------*/
/*----------------------- Structures -----------------------------------*/
/*----------------------- Extern ---------------------------------------*/
#if defined ( UNIT_TEST )
  extern volatile uint16_t vrCounter;
  extern volatile fix16_t  vrSpeed;
  extern uint16_t vrTeethNumber;
#endif
/*----------------------- Functions ------------------------------------*/
void    vVRinit ( TIM_HandleTypeDef* tim );
void    vVRextiCallback ( void );
void    vVRtimCallback ( void );
fix16_t fVRgetSpeed ( void );
/*----------------------------------------------------------------------*/
#endif /* INC_VRSENSOR_H_ */

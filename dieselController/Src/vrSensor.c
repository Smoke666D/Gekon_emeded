/*
 * vrSensor.c
 *
 *  Created on: 23 июн. 2020 г.
 *      Author: photo_Mickey
 */
/*--------------------------------- Includes ---------------------------------*/
#include "vrSensor.h"
#include "config.h"
#include "common.h"
/*-------------------------------- Structures --------------------------------*/
static TIM_HandleTypeDef* vrTIM = NULL;
/*--------------------------------- Constant ---------------------------------*/
const  fix16_t  vrCoef        = F16( VR_MIN_SEC );
/*-------------------------------- Variables ---------------------------------*/
static uint16_t vrCounter     = 1U;
static uint16_t vrTeethNumber = 0U;
static fix16_t  vrSpeed       = 0U;
/*-------------------------------- Functions ---------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vVRinit ( TIM_HandleTypeDef* tim )
{
  vrTIM         = tim;
  vrTeethNumber = speedToothNumber.value[0U];
  vrCounter     = 0U;
  HAL_TIM_Base_Start_IT( tim );
  return;
}

void vVRextiCallback ( void )
{
  vrCounter++;
  return;
}

void vVRtimCallback ( void )
{
  vrSpeed   = fix16_mul( fix16_div( fix16_from_int( vrCounter ), fix16_from_int( vrTeethNumber ) ), vrCoef ); /* RPM */
  vrCounter = 0U;
  return;
}

fix16_t fVRgetSpeed( void )
{
  return vrSpeed;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

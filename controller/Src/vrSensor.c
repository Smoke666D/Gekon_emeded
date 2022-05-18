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
#include "system.h"
#include "constants.h"
/*-------------------------------- Structures --------------------------------*/
static TIM_HandleTypeDef* vrTIM = NULL;
/*--------------------------------- Constant ---------------------------------*/
/*-------------------------------- Variables ---------------------------------*/
#if defined ( UNIT_TEST )
  volatile uint16_t vrCounter = 1U;
  volatile fix16_t  vrSpeed   = 0U;
  uint16_t vrTeethNumber      = 0U;
#else
  volatile static uint16_t vrCounter     = 1U;
  volatile static fix16_t  vrSpeed       = 0U;
  static uint16_t vrTeethNumber = 0U;
#endif

/*-------------------------------- Functions ---------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vVRinit ( TIM_HandleTypeDef* tim )
{
  vrTIM         = tim;
  vrTeethNumber = speedToothNumber.value[0U];
  vrCounter     = 0U;
  HAL_TIM_Base_Start_IT( tim );
  return;
}

#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vVRextiCallback ( void )
{
  vrCounter++;
  return;
}

#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vVRtimCallback ( void )
{
  vrSpeed   = fix16_mul( fix16_div( fix16_from_int( vrCounter ), fix16_from_int( vrTeethNumber ) ), fix60 ); /* RPM */
  vrCounter = 0U;
  return;
}

#ifdef OPTIMIZ
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
fix16_t fVRgetSpeed( void )
{
  return vrSpeed;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

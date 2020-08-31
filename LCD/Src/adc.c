/*
 * adc.c
 *
 *  Created on: Aug 31, 2020
 *      Author: igor.dymov
 */


#include "adc.h"


void vADCInit(void)
{
  HAL_GPIO_WritePin( ON_INPOW_GPIO_Port,ON_INPOW_Pin, GPIO_PIN_SET );
}


void StartADCTask(void *argument)
{


   vADCInit();
   for(;;)
   {


   }

}

/*
 * test.c
 *
 *  Created on: 25 апр. 2022 г.
 *      Author: 79110
 */

#include "tests.h"
#include "unity.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"

static osThreadId_t testHandle = NULL;

void vTESTsendReport ( void )
{
  vUNITwriteOutput();
  while ( eSERIALgetSerialState() == 0U )
  {
    osDelay( 10U );
  }
  return;
}

void vTESTtask ( void *argument )
{
  uint8_t done = 0U;
  for (;;)
  {
    if ( done == 0U )
    {
      UnityBegin( "" );
      runTest_cli();
      runTest_rtc();
      runTest_usbhid();
      runTest_eeprom();
      runTest_vr();
      runTets_storage();
      runTest_chart();
      runTest_outputProcessing();
      runTest_rest();
      runTest_http();
      runTets_dataAPI();
      runTest_dataSD();
      UnityEnd();
      done = 1U;
    }
    osDelay( 1000U );
  }
  return;
}

void vTESTInit ( void )
{
  const osThreadAttr_t tetsTask_attributes = {
    .name       = "UnitTestTask",
    .priority   = ( osPriority_t ) osPriorityNormal,
    .stack_size = 5120U
  };
  testHandle = osThreadNew( vTESTtask, NULL, &tetsTask_attributes );
  return;
}

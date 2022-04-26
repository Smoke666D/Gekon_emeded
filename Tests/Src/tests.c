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
      vTESTsendReport();
      runTest_rtc();
      vTESTsendReport();
      runTest_usbhid();
      vTESTsendReport();
      runTest_eeprom();
      vTESTsendReport();
      runTest_vr();
      vTESTsendReport();
      runTets_storage();
      vTESTsendReport();
      runTets_dataAPI();
      vTESTsendReport();
      runTest_chart();
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
    .stack_size = 4096U
  };
  testHandle = osThreadNew( vTESTtask, NULL, &tetsTask_attributes );
  return;
}
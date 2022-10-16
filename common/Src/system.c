/*
 * system.c
 *
 *  Created on: 8 нояб. 2021 г.
 *      Author: 79110
 */
/*--------------------------------- Includes ---------------------------------*/
#include "system.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"
#include "controllerTypes.h"
#include <string.h>
/*-------------------------------- Structures --------------------------------*/
/*--------------------------------- Constant ---------------------------------*/
/*-------------------------------- Variables ---------------------------------*/
#ifdef DEBUG
  static TASK_ANALIZE tasks[SYS_MAX_TSAK_NUMBER] = { 0U };
  static uint8_t      taskNumber                 = 0U;
#endif
/*-------------------------------- Functions ---------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vSYSprintLoadBar ( uint8_t procents )
{
  uint8_t i       = 0U;
  uint8_t wm      = ( uint8_t )( procents / SYS_BAR_LENGTH );
  char    buf[4U] = { " " };
  if ( procents > 100U )
  {
    wm = 100U / SYS_BAR_LENGTH;
  }
  for ( i=0U; i<wm; i++ )
  {
    vSYSserial( "█", 1U );
  }
  for ( i=wm; i<SYS_BAR_LENGTH; i++ )
  {
    vSYSserial( "▒", 1U );
  }
  ( void )sprintf( buf, " %u", procents );
  vSYSserial( buf, strlen( buf ) );
  vSYSserial( "% ", 2U );
  return;
}
/*----------------------------------------------------------------------------*/
void vSYSprintUsedData ( uint16_t usage, uint16_t total )
{
  char buf[10U] = { " " };
  ( void )sprintf( buf, " %u", usage );
  vSYSserial( buf, strlen( buf ) );
  vSYSserial( "/", 1U );
  ( void )sprintf( buf, " %u", total );
  vSYSserial( " bytes\r\n", 8U );
  return;
}
/*----------------------------------------------------------------------------*/
void vSYSprintHeapData ( void )
{
  uint16_t usage = xPortGetMinimumEverFreeHeapSize();
  uint8_t  used  = ( uint8_t )( usage * 100U / configTOTAL_HEAP_SIZE );
  vSYSserial( "Heap             : ", 19U );
  vSYSprintLoadBar( used );
  vSYSprintUsedData( usage, ( uint16_t )( configTOTAL_HEAP_SIZE ) );
  return;
}
/*----------------------------------------------------------------------------*/
void vSYSprintTaskData ( TASK_ANALIZE task )
{
  uint16_t usage = ( uint16_t )( task.size - uxTaskGetStackHighWaterMark( task.thread ) * 4U );
  uint8_t  used  = ( uint8_t )( task.size / usage );
  char*    name  = pcTaskGetName( task.thread );
  vSYSserial( name, strlen( name ) );
  vSYSserial( " : ", 3U );
  vSYSprintLoadBar( used );
  vSYSprintUsedData( usage, ( uint16_t )( task.size ) );
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vSYSaddTask ( osThreadId_t thread, uint32_t size )
{
  #ifdef DEBUG
    if ( taskNumber < SYS_MAX_TSAK_NUMBER )
    {
      tasks[taskNumber].thread = thread;
      tasks[taskNumber].size   = size;
      taskNumber++;
    }
  #endif
  return;
}
/*----------------------------------------------------------------------------*/
void vSYSprintData ( void )
{
  #ifdef DEBUG
    vSYSserial( "------------------ freeRTOS ------------------\n", 47U );
    vSYSprintHeapData();
    for ( uint8_t i=0U; i<taskNumber; i++ )
    {
      vSYSprintTaskData( tasks[i] );
      vSYSserial( "\n", 1U );
    }
    vSYSserial( "----------------------------------------------\n", 47U );
  #endif
}
/*----------------------------------------------------------------------------*/
void vSYSgetHeapData ( SYSTEM_DATA* data )
{
  data->name  = "heap";
  data->usage = xPortGetMinimumEverFreeHeapSize();
  data->total = configTOTAL_HEAP_SIZE;
  return;
}
/*----------------------------------------------------------------------------*/
uint8_t uSYSgetTaskumber ( void )
{
  return taskNumber;
}
/*----------------------------------------------------------------------------*/
void vSYSgetTaskData ( uint8_t n, SYSTEM_DATA* data )
{
  if ( n < SYS_MAX_TSAK_NUMBER )
  {
    data->name  = pcTaskGetName( tasks[n].thread );
    data->total = tasks[n].size;
    data->usage = ( uint16_t )( tasks[n].size - uxTaskGetStackHighWaterMark( tasks[n].thread ) * 4U );
  }
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

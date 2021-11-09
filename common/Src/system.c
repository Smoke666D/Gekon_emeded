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
/*-------------------------------- Structures --------------------------------*/
/*--------------------------------- Constant ---------------------------------*/
/*-------------------------------- Variables ---------------------------------*/
#ifdef DEBUG
  static TASK_ANALIZE tasks[SYS_MAX_TSAK_NUMBER] = { 0U };
  static uint32_t     taskNumber                 = 0U;
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
    vLOGICprintDebug( "█" );
  }
  for ( i=wm; i<SYS_BAR_LENGTH; i++ )
  {
    vLOGICprintDebug( "▒" );
  }
  sprintf( buf, " %u", procents );
  vLOGICprintDebug( buf );
  vLOGICprintDebug( "% " );
  return;
}

void vSYSprintUsedData ( uint16_t usage, uint16_t total )
{
  char buf[10U] = { " " };
  sprintf( buf, " %u", usage );
  vLOGICprintDebug( buf );
  vLOGICprintDebug( "/" );
  sprintf( buf, " %u", configTOTAL_HEAP_SIZE );
  vLOGICprintDebug( " bytes\r\n" );
  return;
}

void vSYSprintHeapData ( void )
{
  uint16_t usage = xPortGetMinimumEverFreeHeapSize();
  uint8_t  used  = ( uint8_t )( usage * 100U / configTOTAL_HEAP_SIZE );
  vLOGICprintDebug( "Heap             : " );
  vSYSprintLoadBar( used );
  vSYSprintUsedData( usage, ( uint16_t )( configTOTAL_HEAP_SIZE ) );
  return;
}
void vSYSprintTaskData ( TASK_ANALIZE task )
{
  uint16_t usage = ( uint16_t )( task.size - uxTaskGetStackHighWaterMark( task.thread ) * 4U );
  uint8_t  used  = ( uint8_t )( task.size / usage );
  vLOGICprintDebug( pcTaskGetName( task.thread ) );
  vLOGICprintDebug( " : " );
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

void vSYSgetData ( void )
{
  #ifdef DEBUG
    uint32_t i = 0U;
    vLOGICprintDebug( "------------------ freeRTOS ------------------" );

    vSYSprintHeapData();
    for ( i=0U; i<taskNumber; i++ )
    {
      vSYSprintTaskData( tasks[i] );
    }
    vLOGICprintDebug( "----------------------------------------------" );
  #endif
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

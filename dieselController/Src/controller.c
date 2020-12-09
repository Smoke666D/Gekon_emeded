  /*
 *
 * controller.c
 *
 *  Created on: 13 июл. 2020 г.
 *      Author: 79110
 */
/*
 * fpo.c
 *
 *  Created on: 12 июн. 2020 г.
 *      Author: photo_Mickey
 */
/*--------------------------------- Includes ---------------------------------*/
#include "controller.h"
#include "common.h"
#include "electro.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "engine.h"
#include "config.h"
#include "dataProces.h"
#include "log.h"
#include "fpi.h"
#include "menu.h"
#include "journal.h"
#include "alarm.H"
/*-------------------------------- Structures --------------------------------*/
CONTROLLER_INIT controllerGPIO = { 0U };
CONTROLLER_TYPE controller     = { 0U };
/*---------------------------------- MACROS ----------------------------------*/
#define  LOG_WARNINGS_ENABLE    ( getBitMap( &logSetup, LOG_SAVE_WARNING_EVENTS_ENB_ADR ) )
#define  LOG_POSITIVE_ENABLE    ( getBitMap( &logSetup, LOG_POSITIVE_EVENTS_ENB_ADR ) )
#define  POWER_OFF_IMMEDIATELY  ( getBitMap( &mainsSetup, MAINS_POWER_OFF_IMMEDIATELY_ENB_ADR ) )
/*--------------------------------- Constant ---------------------------------*/
/*-------------------------------- Variables ---------------------------------*/
static CONTROLLER_TURNING stopState  = CONTROLLER_TURNING_IDLE;
static CONTROLLER_TURNING startState = CONTROLLER_TURNING_IDLE;
/*-------------------------------- External ----------------------------------*/
osThreadId_t controllerHandle = NULL;
/*-------------------------------- Functions ---------------------------------*/
void vCONTROLLERtask ( void* argument );
/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vCONTROLLERsetLED ( HMI_COMMAND led, uint8_t state )
{
  GPIO_PinState GPIOstate = GPIO_PIN_RESET;
  if ( state == 0U )
  {
    GPIOstate = GPIO_PIN_SET;
  }
  switch( led )
  {
    case HMI_CMD_START:
      if ( controllerGPIO.startGPIO != NULL )
      {
        HAL_GPIO_WritePin( controllerGPIO.startGPIO, controllerGPIO.startPIN, GPIOstate );
      }
      break;
    case HMI_CMD_STOP:
      if ( controllerGPIO.stopGPIO != NULL )
      {
        HAL_GPIO_WritePin( controllerGPIO.stopGPIO, controllerGPIO.stopPIN, GPIOstate );
      }
      break;
    case HMI_CMD_AUTO:
      if ( controllerGPIO.autoGPIO != NULL )
      {
        HAL_GPIO_WritePin( controllerGPIO.autoGPIO, controllerGPIO.autoPIN, GPIOstate );
      }
      break;
    case HMI_CMD_LOAD:
      if ( controllerGPIO.loadGPIO != NULL )
      {
        HAL_GPIO_WritePin( controllerGPIO.loadGPIO, controllerGPIO.loadPIN, GPIOstate );
      }
      break;
    case HMI_CMD_MANUAL:
      if ( controllerGPIO.manualGPIO != NULL )
      {
        HAL_GPIO_WritePin( controllerGPIO.manualGPIO, controllerGPIO.manualPIN, GPIOstate );
      }
      break;
    default:
      break;
  }
  return;
}
/*----------------------------------------------------------------------------*/
void vCONTROLLERstartAutoStart ( PERMISSION delay )
{
  if ( eENGINEisStartBan() == PERMISSION_DISABLE )
  {
    if ( ( controller.mode         == CONTROLLER_MODE_AUTO   ) &&
         ( controller.banAutoStart == 0U                     ) &&
         ( controller.state        == CONTROLLER_STATUS_IDLE ) )
    {
      if ( delay == PERMISSION_ENABLE )
      {
        controller.state = CONTROLLER_STATUS_START_WITH_DELAY;
      }
      else
      {
        controller.state = CONTROLLER_STATUS_START;
      }
      stopState        = CONTROLLER_TURNING_IDLE;
      startState       = CONTROLLER_TURNING_IDLE;
    }
    else
    {
      vLOGICprintDebug( ">>Autostart       : Engine start ban\r\n" );
    }
  }
  return;
}
/*----------------------------------------------------------------------------*/
void vCONTROLLERstartAutoStop ( PERMISSION delay )
{
  if ( ( controller.mode            == CONTROLLER_MODE_AUTO   ) &&
       ( controller.banAutoShutdown == 0U                     ) &&
       ( controller.state           == CONTROLLER_STATUS_WORK ) )
  {
    if ( delay == PERMISSION_ENABLE )
    {
      controller.state = CONTROLLER_STATUS_PLAN_STOP_WITH_DELAY;
    }
    else
    {
      controller.state = CONTROLLER_STATUS_PLAN_STOP;
    }
    stopState        = CONTROLLER_TURNING_IDLE;
    startState       = CONTROLLER_TURNING_IDLE;
  }
  return;
}
/*----------------------------------------------------------------------------*/
void vCONTROLLEReventProcess ( LOG_RECORD_TYPE record )
{
  LOG_RECORD_TYPE buffer = record;
  vLOGICprintEvent( record.event );
  switch ( record.event.action )
  {
    case ACTION_NONE:
      if ( LOG_POSITIVE_ENABLE > 0U )
      {
        eLOGaddRecord( &record );
      }
      break;

    case ACTION_WARNING:
      vFPOsetWarning( RELAY_ON );
      if ( LOG_WARNINGS_ENABLE > 0U )
      {
        eLOGaddRecord( &record );
      }
      vFPOsetWarning( RELAY_ON );
      break;

    case ACTION_EMERGENCY_STOP:
      vCONTROLLERsetLED( HMI_CMD_START, RELAY_OFF );
      vCONTROLLERsetLED( HMI_CMD_STOP,  RELAY_ON );
      controller.state = CONTROLLER_STATUS_ALARM;
      vENGINEsendCmd( ENGINE_CMD_EMEGENCY_STOP );
      vFPOsetGenReady( RELAY_OFF );
      vFPOsetAlarm( RELAY_ON );
      vFPOsetReadyToStart( RELAY_OFF );
      vFPOsetDpsReady( RELAY_OFF );
      eLOGaddRecord( &record );
      break;

    case ACTION_PLAN_STOP:
      vENGINEsendCmd( ENGINE_CMD_PLAN_STOP );
      break;

    case ACTION_BAN_START:
      vENGINEsendCmd( ENGINE_CMD_BAN_START );
      vFPOsetReadyToStart( RELAY_OFF );
      eLOGaddRecord( &record );
      break;

    case ACTION_AUTO_START:
      vCONTROLLERstartAutoStart( PERMISSION_DISABLE );
      break;

    case ACTION_AUTO_STOP:
      if ( LOG_POSITIVE_ENABLE > 0U )
      {
        if ( controller.mode == CONTROLLER_MODE_MANUAL )
        {
          buffer.event.action = ACTION_NONE;
        }
        eLOGaddRecord( &buffer );
      }
      vCONTROLLERstartAutoStop( PERMISSION_DISABLE );
      break;

    default:
      break;
  }
  return;
}
/*----------------------------------------------------------------------------*/
void vCONTROLLERplanStop ( ENGINE_STATUS engineState, ELECTRO_STATUS generatorState, ELECTRO_STATUS mainsState, PERMISSION delayOnStart )
{
  ELECTRO_COMMAND electroCmd = ELECTRO_CMD_NONE;

  switch ( stopState )
  {
    case CONTROLLER_TURNING_IDLE:
      vLOGICprintDebug( ">>Plan stop       : Start\r\n" );
      if ( delayOnStart == PERMISSION_ENABLE )
      {
        controller.timer.delay = controller.stopDelay;
        vLOGICstartTimer( &controller.timer );
        stopState = CONTROLLER_TURNING_START_DELAY;
        vLOGICprintDebug( ">>Plan stop       : Delay trigger\r\n" );
      }
      else
      {
        stopState = CONTROLLER_TURNING_RELOAD;
        vLOGICprintDebug( ">>Plan stop       : Turn to mains\r\n" );
      }
      break;

    case CONTROLLER_TURNING_START_DELAY:
      if ( uLOGICisTimer( controller.timer ) > 0U )
      {
        stopState = CONTROLLER_TURNING_RELOAD;
        vLOGICprintDebug( ">>Plan stop       : Turn to mains\r\n" );
      }
      if ( eFPIcheckLevel( FPI_FUN_REMOTE_START ) == FPI_LEVEL_HIGH )
      {
        controller.state = CONTROLLER_STATUS_WORK;
        stopState        = CONTROLLER_TURNING_IDLE;
        vLOGICresetTimer( controller.timer );
        vLOGICprintDebug( ">>Plan stop       : Cancel\r\n" );
      }
      break;

    case CONTROLLER_TURNING_RELOAD:
      if ( generatorState == ELECTRO_STATUS_LOAD )
      {
        electroCmd = ELECTRO_CMD_LOAD_MAINS;
        xQueueSend( pELECTROgetCommandQueue(), &electroCmd, portMAX_DELAY );
      }
      stopState = CONTROLLER_TURNING_ENGINE;
      vLOGICprintDebug( ">>Plan stop       : Wait for mains connections\r\n" );
      break;

    case CONTROLLER_TURNING_ENGINE:
      if ( mainsState == ELECTRO_STATUS_LOAD )
      {
        vLOGICprintDebug( ">>Plan stop       : Turn off engine\r\n" );
        vENGINEsendCmd( ENGINE_CMD_PLAN_STOP );
        stopState = CONTROLLER_TURNING_FINISH;
      }
      break;

    case CONTROLLER_TURNING_FINISH:
      if ( engineState == ENGINE_STATUS_IDLE )
      {
        vCONTROLLERsetLED( HMI_CMD_START, RELAY_OFF );
        vCONTROLLERsetLED( HMI_CMD_STOP, RELAY_ON   );
        controller.state = CONTROLLER_STATUS_IDLE;
        stopState        = CONTROLLER_TURNING_IDLE;
        vLOGICprintDebug( ">>Plan stop       : Finish\r\n" );
      }
      break;

    default:
      stopState = ENGINE_STATUS_IDLE;
      vLOGICprintDebug( ">>Plan stop       : Error" );
      break;
  }
  return;
}
/*----------------------------------------------------------------------------*/
void vCONTROLLERstart ( ENGINE_STATUS engineState, ELECTRO_STATUS generatorState, ELECTRO_STATUS mainsState, PERMISSION delayOnStart )
{
  ELECTRO_COMMAND electroCmd = ELECTRO_CMD_NONE;
  switch ( startState )
  {
    case CONTROLLER_TURNING_IDLE:
      if ( eENGINEisStartBan() == PERMISSION_DISABLE )
      {
        if ( delayOnStart == PERMISSION_ENABLE )
        {
          controller.timer.delay = controller.startDelay;
          vLOGICstartTimer( &controller.timer );
          startState = CONTROLLER_TURNING_START_DELAY;
          vLOGICprintDebug( ">>Autostart       : Delay trigger\r\n" );
        }
        else
        {
          startState = CONTROLLER_TURNING_READY;
        }
      }
      else
      {
        vLOGICprintDebug( ">>Autostart       : Engine start ban\r\n" );
      }
      break;

    case CONTROLLER_TURNING_START_DELAY:
      if ( uLOGICisTimer( controller.timer ) > 0U )
      {
        startState = CONTROLLER_TURNING_READY;
      }
      if ( eFPIcheckLevel( FPI_FUN_REMOTE_START ) == FPI_LEVEL_LOW )
      {
        controller.state = CONTROLLER_STATUS_IDLE;
        startState       = CONTROLLER_TURNING_IDLE;
        vLOGICresetTimer( controller.timer );
        vLOGICprintDebug( ">>Autostart       : Cancel\r\n" );
      }
      break;

    case CONTROLLER_TURNING_READY:
      vLOGICprintDebug( ">>Autostart       : Start\r\n" );
      vCONTROLLERsetLED( HMI_CMD_START, RELAY_ON  );
      vCONTROLLERsetLED( HMI_CMD_STOP,  RELAY_OFF );
      vFPIsetBlock();
      if ( ( POWER_OFF_IMMEDIATELY > 0U  ) &&
           ( controller.banGenLoad == 0U ) )
      {
        electroCmd = ELECTRO_CMD_LOAD_GENERATOR;
        xQueueSend( pELECTROgetCommandQueue(), &electroCmd, portMAX_DELAY );
        vLOGICprintDebug( ">>Autostart       : Immediately turn to generator\r\n" );
      }
      startState = CONTROLLER_TURNING_ENGINE;
      break;
    case CONTROLLER_TURNING_ENGINE:
      vENGINEsendCmd( ENGINE_CMD_START );
      startState = CONTROLLER_TURNING_RELOAD;
      vLOGICprintDebug( ">>Autostart       : Turn on engine\r\n" );
      break;
    case CONTROLLER_TURNING_RELOAD:
      if ( ( engineState           == ENGINE_STATUS_WORK ) &&
           ( controller.banGenLoad == 0U ) )
      {
        electroCmd = ELECTRO_CMD_LOAD_GENERATOR;
        xQueueSend( pELECTROgetCommandQueue(), &electroCmd, portMAX_DELAY );
        startState = CONTROLLER_TURNING_DELAY;
        vLOGICprintDebug( ">>Autostart       : Turn to generator\r\n" );
      }
      break;
    case CONTROLLER_TURNING_DELAY:
      if ( generatorState == ELECTRO_STATUS_LOAD )
      {
        controller.state = CONTROLLER_STATUS_WORK;
        startState       = CONTROLLER_TURNING_IDLE;
        vLOGICprintDebug( ">>Autostart       : Finish\r\n" );
      }
      break;
    default:
      startState = CONTROLLER_TURNING_IDLE;
      vLOGICprintDebug( ">>Autostart       : Error\r\n" );
      break;
  }
  return;
}
/*----------------------------------------------------------------------------*/
void vCONTROLLERautoProcess ( ENGINE_STATUS engineState, ELECTRO_STATUS generatorState, ELECTRO_STATUS mainsState )
{
  switch ( controller.state )
  {
    case CONTROLLER_STATUS_IDLE:
      break;
    case CONTROLLER_STATUS_ALARM:
      break;
    /*--------------------------------------------------------------*/
    /*------------------------ REMOTE START ------------------------*/
    /*--------------------------------------------------------------*/
    case CONTROLLER_STATUS_START:
      if ( controller.banAutoStart == 0U )
      {
        vCONTROLLERstart( engineState, generatorState, mainsState, PERMISSION_DISABLE );
      }
      break;
    case CONTROLLER_STATUS_START_WITH_DELAY:
      vCONTROLLERstart( engineState, generatorState, mainsState, PERMISSION_ENABLE );
      break;
    /*--------------------------------------------------------------*/
    /*------------------------- REMOTE STOP ------------------------*/
    /*--------------------------------------------------------------*/
    case CONTROLLER_STATUS_PLAN_STOP:
      if ( controller.banAutoShutdown == 0U )
      {
        vCONTROLLERplanStop( engineState, generatorState, mainsState, PERMISSION_DISABLE );
      }
      break;
    case CONTROLLER_STATUS_PLAN_STOP_WITH_DELAY:
      vCONTROLLERplanStop( engineState, generatorState, mainsState, PERMISSION_ENABLE );
      break;
    /*--------------------------------------------------------------*/
    /*--------------------------------------------------------------*/
    /*--------------------------------------------------------------*/
    case CONTROLLER_STATUS_WORK:
      break;
    default:
      break;
  }
  /*--------------------------------------------------------------*/
  return;
}
/*----------------------------------------------------------------------------*/
void vCONTROLLERmanualProcess ( ENGINE_STATUS engineState, ELECTRO_STATUS generatorState, ELECTRO_STATUS mainsState )
{
  if ( ( engineState      == ENGINE_STATUS_WORK      ) &&
       ( controller.state == CONTROLLER_STATUS_START ) )
  {
    controller.state = CONTROLLER_STATUS_WORK;
  }

  if ( controller.state == CONTROLLER_STATUS_PLAN_STOP )
  {
    vCONTROLLERplanStop( engineState, generatorState, mainsState, PERMISSION_DISABLE );
  }
  return;
}
/*----------------------------------------------------------------------------*/
void vCONTROLLERdataInit ( void )
{
  controller.stopDelay  = getValue( &timerReturnDelay );
  controller.startDelay = getValue( &timerStartDelay  );
  return;
}
/*----------------------------------------------------------------------------*/
void vCONTROLLERprintStatus ( void )
{
  vSYSSerial( ">>Controller mode : " );
  if ( controller.mode == CONTROLLER_MODE_MANUAL )
  {
    vSYSSerial( "Manual\r\n" );
  }
  else
  {
    vSYSSerial( "Auto\r\n" );
  }

  return;
/*----------------------------------------------------------------------------*/
}
void vCONTROLLERcheckAutoOn ( ENGINE_STATUS engineState )
{
  if ( controller.state != CONTROLLER_STATUS_ALARM )
  {
    vFPOsetDpsReady( RELAY_ON );
    if ( eELECTROgetMainsErrorFlag() == TRIGGER_SET )
    {
      vCONTROLLERstartAutoStart( PERMISSION_DISABLE );
    }
    else if ( eFPIcheckLevel( FPI_FUN_REMOTE_START ) == FPI_LEVEL_HIGH )
    {
      vCONTROLLERstartAutoStart( PERMISSION_ENABLE );
    }
    else
    {

    }
    if ( engineState != ENGINE_STATUS_IDLE )
    {
      if ( eELECTROgetMainsErrorFlag() == TRIGGER_IDLE )
      {
        vCONTROLLERstartAutoStop( PERMISSION_DISABLE );
      }
      else if ( eFPIcheckLevel( FPI_FUN_REMOTE_START ) != FPI_LEVEL_HIGH )
      {
        vCONTROLLERstartAutoStop( PERMISSION_ENABLE );
      }
      else
      {

      }
    }
  }
  return;
}
/*----------------------------------------------------------------------------*/
void vCONTROLLERresetAlarm ( void )
{
  if ( ( controller.state  == CONTROLLER_STATUS_ALARM ) ||
       ( controller.state  == CONTROLLER_STATUS_IDLE  ) )
  {
    controller.state = CONTROLLER_STATUS_IDLE;
    vENGINEsendCmd( ENGINE_CMD_RESET_TO_IDLE );
    vFPIreset();
    vFPOsetAlarm( RELAY_OFF );
    vFPOsetMainsFail( RELAY_OFF );
    eLOGICERactiveErrorList( ERROR_LIST_CMD_ERASE, NULL, NULL );
    controller.mode = CONTROLLER_MODE_MANUAL;
    vCONTROLLERsetLED( HMI_CMD_AUTO,   RELAY_OFF );
    vCONTROLLERsetLED( HMI_CMD_MANUAL, RELAY_ON  );
    vCONTROLLERprintStatus();
  }
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vCONTROLLERinit ( const CONTROLLER_INIT* init )
{
  controllerGPIO = *init;
  vCONTROLLERsetLED( HMI_CMD_START,  0U );
  vCONTROLLERsetLED( HMI_CMD_STOP,   0U );
  vCONTROLLERsetLED( HMI_CMD_AUTO,   0U );
  vCONTROLLERsetLED( HMI_CMD_LOAD,   0U );
  vCONTROLLERsetLED( HMI_CMD_MANUAL, 1U );
  vCONTROLLERdataInit();
  const osThreadAttr_t controllerTask_attributes = {
    .name       = "fpiTask",
    .priority   = ( osPriority_t ) CONTROLLER_TASK_PRIORITY,
    .stack_size = CONTROLLER_TASK_STACK_SIZE
  };
  controllerHandle = osThreadNew( vCONTROLLERtask, NULL, &controllerTask_attributes );
  vMenuMessageInit( controllerHandle );
  vCONTROLLERsetLED( HMI_CMD_STOP, RELAY_ON );
  return;
}
/*----------------------------------------------------------------------------*/
CONTROLLER_STATE eCONTROLLERgetStatus ( void )
{
  return controller.state;
}
/*----------------------------------------------------------------------------*/
CONTROLLER_MODE eCONTROLLERgetMode ( void )
{
  return controller.mode;
}
/*----------------------------------------------------------------------------*/
void vCONTROLLERtask ( void* argument )
{
  ENGINE_STATUS   engineState          = ENGINE_STATUS_IDLE;
  ELECTRO_STATUS  generatorState       = ELECTRO_STATUS_IDLE;
  ELECTRO_STATUS  mainsState           = ELECTRO_STATUS_IDLE;
  SYSTEM_EVENT    interiorEvent        = { 0U };
  LOG_RECORD_TYPE inputEvent           = { 0U };
  FPI_EVENT       inputFpiEvent        = { 0U };
  uint32_t        inputNotifi          = 0U;
  uint8_t         inputKeyboardCommand = HMI_CMD_NONE;
  uint32_t        systemEvent          = 0U;
  for (;;)
  {
    /*--------------------------------------------------------------------------------------------*/
    /*------------------------------------ GET BLOCKS STATUS -------------------------------------*/
    /*--------------------------------------------------------------------------------------------*/
    engineState    = eENGINEgetEngineStatus();
    generatorState = eELECTROgetGeneratorStatus();
    mainsState     = eELECTROgetMainsStatus();
    /*--------------------------------------------------------------------------------------------*/
    /*--------------------------------- KEYBOARD & SYSTEM INPUT ----------------------------------*/
    /*--------------------------------------------------------------------------------------------*/

    /*--------------------------------------- SYSTEM INPUT ---------------------------------------*/
    systemEvent = xEventGroupGetBits( xDATAAPIgetEventGroup() );
    if ( ( systemEvent & DATA_API_FLAG_CONTROLLER_TASK_CONFIG_REINIT ) > 0U )
    {
      vCONTROLLERdataInit();
      vFPOdataInit();
      vALARMreInit();
      vCONTROLLERresetAlarm();
      xEventGroupClearBits( xDATAAPIgetEventGroup(), DATA_API_FLAG_CONTROLLER_TASK_CONFIG_REINIT );
    }
    /*-------------------------------------- KEYBOARD INPUT ---------------------------------------*/
    if ( xTaskNotifyWait( 0U, 0xFFFFFFFFU, &inputNotifi, TASK_NOTIFY_WAIT_DELAY ) == pdPASS )
    {
      inputKeyboardCommand = ( uint8_t )( inputNotifi & HMI_CMD_MASK );
      switch ( inputKeyboardCommand )
      {
        case HMI_CMD_START:
          if ( ( controller.state != CONTROLLER_STATUS_ALARM ) && ( controller.mode  == CONTROLLER_MODE_MANUAL ) )
          {
            /* START */
            if ( engineState == ENGINE_STATUS_IDLE )
            {
              if ( eENGINEisStartBan() == PERMISSION_DISABLE )
              {
                vCONTROLLERsetLED( HMI_CMD_START, RELAY_ON  );
                vCONTROLLERsetLED( HMI_CMD_STOP,  RELAY_OFF );
                vFPIsetBlock();
                controller.state = CONTROLLER_STATUS_START;
                vENGINEsendCmd( ENGINE_CMD_START );
              }
              else
              {
                vLOGICprintDebug( ">>Start           : Engine start ban\r\n" );
                vCONTROLLERsetLED( HMI_CMD_START, RELAY_ON  );
                osDelay(10U);
                vCONTROLLERsetLED( HMI_CMD_START, RELAY_OFF );
              }
            }
            /* LOAD */
            else
            {
              if ( ( CONTROLLER_LOAD_BTN_EXIST == 0U                     ) &&
                   ( controller.state          != CONTROLLER_STATUS_IDLE ) &&
                   ( controller.banGenLoad     == 0U                     ) &&
                   ( engineState               == ENGINE_STATUS_WORK     ) )
              {
                vCONTROLLERsetLED( HMI_CMD_LOAD, RELAY_ON );
                vELECTROsendCmd( ELECTRO_CMD_LOAD_GENERATOR );
              }
            }
          }
          break;
        case HMI_CMD_LOAD:
          if ( ( controller.mode       == CONTROLLER_MODE_MANUAL )  &&
               ( engineState           == ENGINE_STATUS_WORK     ) )
          {
            if ( ( generatorState        == ELECTRO_STATUS_IDLE ) &&
                 ( controller.banGenLoad == 0U                  ) )
            {
              vCONTROLLERsetLED( HMI_CMD_LOAD, RELAY_ON );
              vELECTROsendCmd( ELECTRO_CMD_LOAD_GENERATOR );
            }
            else if ( generatorState == ELECTRO_STATUS_LOAD )
            {
              vCONTROLLERsetLED( HMI_CMD_LOAD, RELAY_OFF );
              vELECTROsendCmd( ELECTRO_CMD_LOAD_MAINS );
            }
            else
            {

            }
          }
          break;
        case HMI_CMD_STOP :
          if ( controller.mode  == CONTROLLER_MODE_MANUAL  )
          {
            if ( generatorState == ELECTRO_STATUS_LOAD )
            {
              vCONTROLLERsetLED( HMI_CMD_LOAD, RELAY_OFF );
              vELECTROsendCmd( ELECTRO_CMD_LOAD_MAINS );
            }
            else
            {
              switch ( controller.state )
              {
                case CONTROLLER_STATUS_IDLE:
                  vCONTROLLERresetAlarm();
                  break;
                case CONTROLLER_STATUS_ALARM:
                  vCONTROLLERresetAlarm();
                  break;
                case CONTROLLER_STATUS_WORK:
                  if ( eELECTROgetStatus() == ELECTRO_PROC_STATUS_IDLE )
                  {
                    controller.state = CONTROLLER_STATUS_PLAN_STOP;
                    stopState        = CONTROLLER_TURNING_IDLE;
                  }
                  break;
                case CONTROLLER_STATUS_START:
                  interiorEvent.type   = EVENT_INTERRUPTED_START;
                  interiorEvent.action = ACTION_EMERGENCY_STOP;
                  vSYSeventSend( interiorEvent, NULL );
                  controller.state = CONTROLLER_STATUS_ALARM;
                  stopState        = CONTROLLER_TURNING_IDLE;
                  break;
                case CONTROLLER_STATUS_PLAN_STOP:
                  vCONTROLLERsetLED( HMI_CMD_STOP, RELAY_ON );
                  interiorEvent.type   = EVENT_INTERRUPTED_STOP;
                  interiorEvent.action = ACTION_EMERGENCY_STOP;
                  vSYSeventSend( interiorEvent, NULL );
                  controller.state = CONTROLLER_STATUS_ALARM;
                  stopState        = CONTROLLER_TURNING_IDLE;
                  break;
                case CONTROLLER_STATUS_PLAN_STOP_WITH_DELAY:
                  vCONTROLLERsetLED( HMI_CMD_STOP, RELAY_ON );
                  interiorEvent.type   = EVENT_INTERRUPTED_STOP;
                  interiorEvent.action = ACTION_EMERGENCY_STOP;
                  vSYSeventSend( interiorEvent, NULL );
                  controller.state = CONTROLLER_STATUS_ALARM;
                  stopState        = CONTROLLER_TURNING_IDLE;
                  break;
                default:
                  break;
              }
            }
          }
          else
          {
            vCONTROLLERresetAlarm();
          }
          break;
        case HMI_CMD_AUTO:
          /* MANUAL */
          if ( ( CONTROLLER_MANUAL_BTN_EXIST == 0U ) && ( controller.mode == CONTROLLER_MODE_AUTO ) )
          {
            vCONTROLLERsetLED( HMI_CMD_AUTO,   RELAY_OFF );
            vCONTROLLERsetLED( HMI_CMD_MANUAL, RELAY_ON  );
            vFPOsetDpsReady( RELAY_OFF );
            controller.mode = CONTROLLER_MODE_MANUAL;
          }
          /* AUTO */
          else if ( controller.mode  == CONTROLLER_MODE_MANUAL )
          {
            controller.mode = CONTROLLER_MODE_AUTO;
            vCONTROLLERsetLED( HMI_CMD_AUTO,   RELAY_ON  );
            vCONTROLLERsetLED( HMI_CMD_MANUAL, RELAY_OFF );
            vCONTROLLERcheckAutoOn ( engineState );
          }
          else
          {

          }
          vCONTROLLERprintStatus();
          break;
        case HMI_CMD_MANUAL:
          if ( controller.mode == CONTROLLER_MODE_AUTO )
          {
            vCONTROLLERsetLED( HMI_CMD_AUTO,   RELAY_OFF );
            vCONTROLLERsetLED( HMI_CMD_MANUAL, RELAY_ON  );
            vFPOsetDpsReady( RELAY_OFF );
            controller.mode = CONTROLLER_MODE_MANUAL;
          }
          break;
        case HMI_CMD_ACK:
          vCONTROLLERresetAlarm();
          break;
        default:
          break;
      }
    }
    /*--------------------------------------------------------------------------------------------*/
    /*-------------------------------------- DIGITAL INPUTS --------------------------------------*/
    /*--------------------------------------------------------------------------------------------*/
    if ( xQueueReceive( pFPIgetQueue(), &inputFpiEvent, 0U ) == pdPASS )
    {
      switch ( inputFpiEvent.function )
      {
        /*----------------------------- Пользовательская -----------------------------*/
        case FPI_FUN_USER:
          break;
        /*------------------------- Сброс аварийного сигнала -------------------------*/
        case FPI_FUN_ALARM_RESET:
          if ( inputFpiEvent.level == FPI_LEVEL_HIGH )
          {
            vCONTROLLERresetAlarm();
          }
          break;
        /*---- Запрет автоматического останова при восстановлении параметров сети ----*/
        case FPI_FUN_BAN_AUTO_SHUTDOWN:
          if ( inputFpiEvent.level == FPI_LEVEL_HIGH )
          {
            controller.banAutoShutdown = 1U;
            vFPIprint( FPI_FUN_BAN_AUTO_SHUTDOWN, "SET" );
          }
          else
          {
            controller.banAutoShutdown = 0U;
            vFPIprint( FPI_FUN_BAN_AUTO_SHUTDOWN, "RESET" );
          }
          break;
        /*---------------------- Запрет автоматического запуска ----------------------*/
        case FPI_FUN_BAN_AUTO_START:
          if ( inputFpiEvent.level == FPI_LEVEL_HIGH )
          {
            controller.banAutoStart = 1U;
            vFPIprint( FPI_FUN_BAN_AUTO_START, "SET" );
          }
          else
          {
            controller.banAutoStart = 0U;
            vFPIprint( FPI_FUN_BAN_AUTO_START, "RESET" );
          }
          break;
        /*------------------------ Запрет нагрузки генератора ------------------------*/
        case FPI_FUN_BAN_GEN_LOAD:
          if ( inputFpiEvent.level == FPI_LEVEL_HIGH )
          {
            controller.banGenLoad = 1U;
            vFPIprint( FPI_FUN_BAN_GEN_LOAD, "SET" );
          }
          else
          {
            controller.banGenLoad = 0U;
            vFPIprint( FPI_FUN_BAN_GEN_LOAD, "RESET" );
          }
          break;
        /*----------------------- Дистанционный запуск, останов ----------------------*/
        case FPI_FUN_REMOTE_START:
          if ( inputFpiEvent.level == FPI_LEVEL_HIGH )
          {
            vCONTROLLERstartAutoStart( PERMISSION_ENABLE );
          }
          else
          {
            vCONTROLLERstartAutoStop( PERMISSION_ENABLE );
          }
          break;
        /*---------------------- Высокая температура двигателя -----------------------*/
        case FPI_FUN_HIGHT_ENGINE_TEMP:
          if ( inputFpiEvent.level == FPI_LEVEL_HIGH )
          {
            interiorEvent.type   = EVENT_ENGINE_HIGHT_TEMP;
            interiorEvent.action = ACTION_EMERGENCY_STOP;
            vSYSeventSend( interiorEvent, NULL );
          }
          break;
        /*----------------------- Сигнал низкого уровня топлива ----------------------*/
        case FPI_FUN_LOW_FUEL:
          if ( inputFpiEvent.level == FPI_LEVEL_HIGH )
          {
            interiorEvent.type   = EVENT_FUEL_LOW_LEVEL;
            interiorEvent.action = ACTION_EMERGENCY_STOP;
            vSYSeventSend( interiorEvent, NULL );
          }
          break;
        /*-------------------------- Датчик давления масла ---------------------------*/
        case FPI_FUN_OIL_LOW_PRESSURE:
          if ( inputFpiEvent.level == FPI_LEVEL_HIGH )
          {
            interiorEvent.type   = EVENT_OIL_LOW_PRESSURE;
            interiorEvent.action = ACTION_EMERGENCY_STOP;
            vSYSeventSend( interiorEvent, NULL );
          }
          break;
        /*-------------------------- Работа на холостом ходу -------------------------*/
        case FPI_FUN_IDLING:
          if ( ( engineState         == ENGINE_STATUS_WORK ) &&
               ( inputFpiEvent.level == FPI_LEVEL_HIGH     ) )
          {
            vENGINEsendCmd( ENGINE_CMD_GOTO_IDLE );
          }
          if ( ( engineState         == ENGINE_STATUS_WORK_ON_IDLE ) &&
               ( inputFpiEvent.level == FPI_LEVEL_LOW              ) )
          {
            vENGINEsendCmd( ENGINE_CMD_GOTO_NORMAL );
          }
          break;
        default:
          break;
      }
    }
    /*--------------------------------------------------------------------------------------------*/
    /*---------------------------------- CONTROLLER PROCESSING -----------------------------------*/
    /*--------------------------------------------------------------------------------------------*/
    if ( controller.state != CONTROLLER_STATUS_ALARM )
    {
      switch( controller.mode )
      {
        case CONTROLLER_MODE_MANUAL:
          vCONTROLLERmanualProcess( engineState, generatorState, mainsState );
          break;
        case CONTROLLER_MODE_AUTO:
          vCONTROLLERautoProcess( engineState, generatorState, mainsState );
          break;
        default:
          break;
      }
    }
    /*--------------------------------------- EVENT OUTPUT --------------------------------------*/
    if ( xQueueReceive( pLOGICgetEventQueue(), &inputEvent, 0U ) == pdPASS )
    {
      vCONTROLLEReventProcess( inputEvent );
    }
  }
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/




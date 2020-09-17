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
#include "electro.h"
#include "controller.h"
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "engine.h"
#include "config.h"
#include "dataProces.h"
#include "log.h"
#include "fpi.h"
/*-------------------------------- Structures --------------------------------*/
CONTROLLER_TYPE controller =
{
  .mode            = CONTROLLER_MODE_MANUAL,
  .banAutoStart    = 0U,
  .banAutoShutdown = 0U,
  .banGenLoad      = 0U,
  .stopDelay       = 0U,
  .timerID         = 0U,
};
/*---------------------------------- MACROS ----------------------------------*/
#define  LOG_WARNINGS_ENABLE    ( getBitMap( &logSetup, 0U ) )
#define  POWER_OFF_IMMEDIATELY  ( getBitMap( &mainsSetup, 1U ) )
/*--------------------------------- Constant ---------------------------------*/
/*-------------------------------- Variables ---------------------------------*/
static CONTROLLER_TURNING stopState  = CONTROLLER_TURNING_IDLE;
static CONTROLLER_TURNING startState = CONTROLLER_TURNING_IDLE;
/*-------------------------------- Functions ---------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vCONTROLLEReventProcess ( SYSTEM_EVENT event )
{
  switch ( event.action )
  {
    case ACTION_WARNING:
      if ( LOG_WARNINGS_ENABLE > 0U )
      {
        vLOGwriteRecord( event );
      }
      // >>Send warning to LCD
      break;

    case ACTION_EMERGENCY_STOP:
      controller.state = CONTROLLER_STATUS_ALARM;
      xQueueSend( pENGINEgetCommandQueue(), ENGINE_CMD_EMEGENCY_STOP, portMAX_DELAY );
      vLOGwriteRecord( event );
      break;

    case ACTION_LOAD_GENERATOR:
      if ( LOG_WARNINGS_ENABLE > 0U )
      {
        vLOGwriteRecord( event );
      }
      // >>Send warning to LCD
      controller.state = CONTROLLER_STATUS_START;
      break;

    case ACTION_LOAD_MAINS:
      if ( controller.mode == CONTROLLER_MODE_AUTO )
      {
        controller.state = CONTROLLER_STATUS_PLAN_STOP;
      }
      break;

    case ACTION_LOAD_SHUTDOWN:
      vLOGwriteRecord( event );
      controller.state = CONTROLLER_STATUS_SHUTDOWN;
      break;

    default:
      break;
  }
  return;
}
/*----------------------------------------------------------------------------*/
QueueHandle_t pKEYBOARDgetCommandQueue()
{
  QueueHandle_t out = NULL;
  return out;
}
/*----------------------------------------------------------------------------*/
void vCONTROLLERplanStop ( ENGINE_STATUS engineState, ELECTRO_STATUS generatorState, ELECTRO_STATUS mainsState, uint8_t delayStop )
{
  ENGINE_COMMAND  engineCmd  = ENGINE_CMD_NONE;
  ELECTRO_COMMAND electroCmd = ELECTRO_CMD_NONE;

  switch ( stopState )
  {
    case CONTROLLER_TURNING_IDLE:
      if ( delayStop >0U )
      {
        vLOGICstartTimer( controller.stopDelay, &controller.timerID );
        stopState = CONTROLLER_TURNING_DELAY;
      }
      else
      {
        vLOGICstartTimer( 0U, &controller.timerID );
        stopState = CONTROLLER_TURNING_RELOAD;
      }
      break;

    case CONTROLLER_TURNING_DELAY:
      if ( uLOGICisTimer( controller.timerID ) )
      {
        stopState = CONTROLLER_TURNING_ENGINE;
      }
      if ( eFPIcheckLevel( FPI_FUN_REMOTE_START ) != FPI_LEVEL_HIGH )
      {
        controller.state = CONTROLLER_STATUS_IDLE;
        stopState     = CONTROLLER_TURNING_IDLE;
      }
      break;

    case CONTROLLER_TURNING_RELOAD:
      electroCmd = ELECTRO_CMD_LOAD_MAINS;
      xQueueSend( pELECTROgetCommandQueue(), &electroCmd, portMAX_DELAY );
      stopState = CONTROLLER_TURNING_ENGINE;
      break;

    case CONTROLLER_TURNING_ENGINE:
      if ( mainsState == ELECTRO_STATUS_LOAD )
      {
        engineCmd = ENGINE_CMD_PLAN_STOP;
        xQueueSend( pENGINEgetCommandQueue(), &engineCmd, portMAX_DELAY );
      }
      break;

    case CONTROLLER_TURNING_FINISH:
      if ( engineState == ENGINE_STATUS_IDLE )
      {
        controller.state = CONTROLLER_STATUS_IDLE;
        stopState        = CONTROLLER_TURNING_IDLE;
      }
      break;

    default:
      stopState = ENGINE_STATUS_IDLE;
      break;
  }
  return;
}
/*----------------------------------------------------------------------------*/
void vCONTROLLERautoProcess ( ENGINE_STATUS engineState, ELECTRO_STATUS generatorState, ELECTRO_STATUS mainsState )
{
  ENGINE_COMMAND  engineCmd  = ENGINE_CMD_NONE;
  ELECTRO_COMMAND electroCmd = ELECTRO_CMD_NONE;

  /*------------------------ REMOTE START ------------------------*/
  if ( ( controller.state        == CONTROLLER_STATUS_START ) &&
       ( controller.banAutoStart == 0U ) )
  {
    switch ( startState )
    {
      case CONTROLLER_TURNING_IDLE:
        vFPIsetBlock();
        if ( ( POWER_OFF_IMMEDIATELY > 0U  ) &&
             ( controller.banGenLoad == 0U ) )
        {
          electroCmd = ELECTRO_CMD_LOAD_GENERATOR;
          xQueueSend( pELECTROgetCommandQueue(), &electroCmd, portMAX_DELAY );
        }
        startState = CONTROLLER_TURNING_ENGINE;
        break;

      case CONTROLLER_TURNING_ENGINE:
        engineCmd = ENGINE_CMD_START;
        xQueueSend( pENGINEgetCommandQueue(), &engineCmd, portMAX_DELAY );
        startState = CONTROLLER_TURNING_RELOAD;
        break;

      case CONTROLLER_TURNING_RELOAD:
        if ( ( engineState           == ENGINE_STATUS_WORK ) &&
             ( controller.banGenLoad == 0U ) )
        {
          electroCmd = ELECTRO_CMD_LOAD_GENERATOR;
          xQueueSend( pELECTROgetCommandQueue(), &electroCmd, portMAX_DELAY );
          startState = CONTROLLER_TURNING_DELAY;
        }
        break;

      case CONTROLLER_TURNING_DELAY:
        if ( generatorState == ELECTRO_STATUS_LOAD )
        {
          controller.state = CONTROLLER_STATUS_WORK;
          startState       = CONTROLLER_TURNING_IDLE;
        }
        break;

      default:
        startState = CONTROLLER_TURNING_IDLE;
        break;
    }
  }
  /*------------------------- REMOTE STOP ------------------------*/
  if ( ( ( controller.state           == CONTROLLER_STATUS_PLAN_STOP ) &&
         ( controller.banAutoShutdown == 0U                          ) ) ||
         ( controller.state           == CONTROLLER_STATUS_SHUTDOWN  ) )
  {
    vCONTROLLERplanStop( engineState, generatorState, mainsState, 1U );
  }
  /*--------------------------------------------------------------*/
  return;
}
/*----------------------------------------------------------------------------*/
void vCONTROLLERmanualProcess ( ENGINE_STATUS engineState, ELECTRO_STATUS generatorState, ELECTRO_STATUS mainsState )
{
  if ( ( engineState      == ENGINE_STATUS_WORK      ) &&
       ( generatorState   == ELECTRO_STATUS_LOAD     ) &&
       ( controller.state == CONTROLLER_STATUS_START ) )
  {
    controller.state = CONTROLLER_STATUS_WORK;
  }

  if ( ( controller.state == CONTROLLER_STATUS_PLAN_STOP ) ||
       ( controller.state == CONTROLLER_STATUS_SHUTDOWN  ) )
  {
    vCONTROLLERplanStop( engineState, generatorState, mainsState, 0U );
  }
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vCONTROLLERinit ( void )
{
  controller.stopDelay = getValue( &timerReturnDelay );
  return;
}
/*----------------------------------------------------------------------------*/
void vCONTROLLERtask ( void const* argument )
{
  ENGINE_STATUS   engineState          = eENGINEgetEngineStatus();
  ELECTRO_STATUS  generatorState       = eELECTROgetGeneratorStatus();
  ELECTRO_STATUS  mainsState           = eELECTROgetMainsStatus();
  ENGINE_COMMAND  engineCmd            = ENGINE_CMD_NONE;
  HMI_COMMAND     inputKeyboardCommand = HMI_CMD_NONE;
  ELECTRO_COMMAND electroCmd           = ELECTRO_CMD_NONE;
  SYSTEM_EVENT    interiorEvent;
  SYSTEM_EVENT    inputEvent;
  FPI_EVENT       inputFpiEvent;

  for (;;)
  {
    if ( controller.state != CONTROLLER_STATUS_ALARM )
    {
      /*---------------------------------- AUTOMATIC PROCESSING ----------------------------------*/
      if ( controller.mode  == CONTROLLER_MODE_AUTO )
      {
        vCONTROLLERautoProcess( engineState, generatorState, mainsState );
      }
      /*------------------------------------ MANUAL PROCESSING -----------------------------------*/
      if ( controller.mode  == CONTROLLER_MODE_MANUAL )
      {
        vCONTROLLERmanualProcess( engineState, generatorState, mainsState );
      }
    }
    /*-------------------------------------- KEYBOARD INPUT --------------------------------------*/
    if ( xQueueReceive( pKEYBOARDgetCommandQueue(), &inputKeyboardCommand, 0U ) == pdPASS )
    {
      switch ( inputKeyboardCommand )
      {
        case HMI_CMD_START:
          vFPIsetBlock();
          if ( ( controller.mode  == CONTROLLER_MODE_MANUAL ) &&
               ( controller.state != CONTROLLER_STATUS_ALARM ) )
          {
            controller.state = CONTROLLER_STATUS_START;
            engineCmd        = ENGINE_CMD_START;
            xQueueSend( pENGINEgetCommandQueue(), &engineCmd, portMAX_DELAY );
          }
          break;
        case HMI_CMD_LOAD:
          if ( ( controller.mode       == CONTROLLER_MODE_MANUAL  )  &&
               ( controller.state      == CONTROLLER_STATUS_START ) &&
               ( controller.banGenLoad == 0U                      ) &&
               ( engineState           == ENGINE_STATUS_WORK      ) )
          {
            electroCmd = ELECTRO_CMD_LOAD_GENERATOR;
            xQueueSend( pELECTROgetCommandQueue(), &electroCmd, portMAX_DELAY );
          }
          break;
        case HMI_CMD_STOP :
          if ( ( controller.mode  == CONTROLLER_MODE_MANUAL  ) &&
               ( controller.state != CONTROLLER_STATUS_ALARM ) )
          {
            controller.state = CONTROLLER_STATUS_PLAN_STOP;
          }
          break;
        case HMI_CMD_AUTO:
          if ( controller.mode == CONTROLLER_MODE_MANUAL )
          {
            controller.mode = CONTROLLER_MODE_AUTO;
            if ( ( ( controller.state == CONTROLLER_STATUS_START ) ||
                   ( controller.state == CONTROLLER_STATUS_WORK  ) ) &&
                 ( eFPIcheckLevel( FPI_FUN_REMOTE_START ) != FPI_LEVEL_HIGH ) )
            {
              controller.state = CONTROLLER_STATUS_PLAN_STOP;
            }
          }
          break;
        case HMI_CMD_MANUAL:
          if ( controller.mode == CONTROLLER_MODE_AUTO )
          {
            controller.mode = CONTROLLER_MODE_MANUAL;
          }
          break;
        default:
          break;
      }
    }
    /*-------------------------------------- DIGITAL INPUTS -------------------------------------*/
    if ( xQueueReceive( pFPIgetQueue(), &inputFpiEvent, 0U ) )
    {
      switch ( inputFpiEvent.function )
      {
        case FPI_FUN_USER:                         /* Пользовательская */
          break;
        case FPI_FUN_ALARM_RESET:                  /* Сброс аварийного сигнала */
          if ( ( controller.state    == CONTROLLER_STATUS_ALARM ) &&
               ( inputFpiEvent.level == FPI_LEVEL_HIGH ) )
          {
            controller.state = CONTROLLER_STATUS_IDLE;
            engineCmd = ENGINE_CMD_RESET_TO_IDLE;
            xQueueSend( pENGINEgetCommandQueue(), &engineCmd, portMAX_DELAY );
          }
          break;
        case FPI_FUN_BAN_AUTO_SHUTDOWN:            /* Запрет автоматического останова при восстановлении параметров сети */
          if ( inputFpiEvent.level == FPI_LEVEL_HIGH )
          {
            controller.banAutoShutdown = 1U;
          }
          else
          {
            controller.banAutoShutdown = 0U;
          }
          break;
        case FPI_FUN_BAN_AUTO_START:               /* Запрет автоматического запуска */
          if ( inputFpiEvent.level == FPI_LEVEL_HIGH )
          {
            controller.banAutoStart = 1U;
          }
          else
          {
            controller.banAutoStart = 0U;
          }
          break;
        case FPI_FUN_BAN_GEN_LOAD:                 /* Запрет нагрузки генератора */
          if ( inputFpiEvent.level == FPI_LEVEL_HIGH )
          {
            controller.banGenLoad = 1U;
          }
          else
          {
            controller.banGenLoad = 0U;
          }
          break;
        case FPI_FUN_REMOTE_START:                 /* Дистанционный запуск, останов */
          if ( ( controller.mode  == CONTROLLER_MODE_AUTO ) &&
               ( controller.state != CONTROLLER_STATUS_ALARM ) )
          {
            if ( ( controller.banAutoStart == 0U ) &&
                 ( inputFpiEvent.level     == FPI_LEVEL_HIGH ) )
            {
              controller.state = CONTROLLER_STATUS_START;
            }
            if ( ( controller.banAutoShutdown == 0U ) &&
                 ( inputFpiEvent.level        == FPI_LEVEL_LOW ) )
            {
              controller.state = CONTROLLER_STATUS_PLAN_STOP;
            }
          }
          break;
        case FPI_FUN_HIGHT_ENGINE_TEMP:            /* Высокая температура двигателя */
          if ( inputFpiEvent.level     == FPI_LEVEL_HIGH )
          {
            interiorEvent.type   = EVENT_ENGINE_HIGHT_TEMP;
            interiorEvent.action = ACTION_EMERGENCY_STOP;
            xQueueSend( pLOGICgetEventQueue(), &interiorEvent, portMAX_DELAY );
          }
          break;
        case FPI_FUN_LOW_FUEL:                     /* Сигнал низкого уровня топлива */
          if ( inputFpiEvent.level     == FPI_LEVEL_HIGH )
          {
            interiorEvent.type   = EVENT_FUEL_LOW_LEVEL;
            interiorEvent.action = ACTION_EMERGENCY_STOP;
            xQueueSend( pLOGICgetEventQueue(), &interiorEvent, portMAX_DELAY );
          }
          break;
        case FPI_FUN_OIL_LOW_PRESSURE:             /* Датчик давления масла */
          if ( inputFpiEvent.level     == FPI_LEVEL_HIGH )
          {
            interiorEvent.type   = EVENT_OIL_LOW_PRESSURE;
            interiorEvent.action = ACTION_EMERGENCY_STOP;
            xQueueSend( pLOGICgetEventQueue(), &interiorEvent, portMAX_DELAY );
          }
          break;
        case FPI_FUN_IDLING:                       /* Работа на холостом ходу */
          if ( ( engineState         == ENGINE_STATUS_WORK ) &&
               ( inputFpiEvent.level == FPI_LEVEL_HIGH ) )
          {
            engineCmd = ENGINE_CMD_GOTO_IDLE;
            xQueueSend( pENGINEgetCommandQueue(), &engineCmd, portMAX_DELAY );
          }
          if ( ( engineState         == ENGINE_STATUS_WORK_ON_IDLE ) &&
               ( inputFpiEvent.level == FPI_LEVEL_LOW ) )
          {
            engineCmd = ENGINE_CMD_GOTO_NORMAL;
            xQueueSend( pENGINEgetCommandQueue(), &engineCmd, portMAX_DELAY );
          }
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



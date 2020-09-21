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
static osThreadId_t  controllerHandle = NULL;
/*---------------------------------- MACROS ----------------------------------*/
#define  LOG_WARNINGS_ENABLE    ( getBitMap( &logSetup, 0U ) )
#define  POWER_OFF_IMMEDIATELY  ( getBitMap( &mainsSetup, 1U ) )
/*--------------------------------- Constant ---------------------------------*/
/*-------------------------------- Variables ---------------------------------*/
static CONTROLLER_TURNING stopState  = CONTROLLER_TURNING_IDLE;
static CONTROLLER_TURNING startState = CONTROLLER_TURNING_IDLE;
/*-------------------------------- Functions ---------------------------------*/
void vCONTROLLERtask ( void const* argument );
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
        vLOGaddRecord( event );
      }
      // >>Send warning to LCD
      break;

    case ACTION_EMERGENCY_STOP:
      controller.state = CONTROLLER_STATUS_ALARM;
      xQueueSend( pENGINEgetCommandQueue(), ENGINE_CMD_EMEGENCY_STOP, portMAX_DELAY );
      vLOGaddRecord( event );
      break;

    case ACTION_LOAD_GENERATOR:
      if ( LOG_WARNINGS_ENABLE > 0U )
      {
        vLOGaddRecord( event );
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
      vLOGaddRecord( event );
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
      if ( uLOGICisTimer( controller.timerID ) > 0U )
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
void vCONTROLLERfpiProcess ()
{

}
/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vCONTROLLERinit ( void )
{
  controller.stopDelay = getValue( &timerReturnDelay );
  const osThreadAttr_t controllerTask_attributes = {
    .name       = "fpiTask",
    .priority   = ( osPriority_t ) osPriorityLow,
    .stack_size = 1024U
  };
  controllerHandle = osThreadNew( vCONTROLLERtask, NULL, &controllerTask_attributes );
  vMenuMessageInit( controllerHandle );
  return;
}
/*----------------------------------------------------------------------------*/
void vCONTROLLERtask ( void const* argument )
{
  ENGINE_STATUS   engineState          = eENGINEgetEngineStatus();
  ELECTRO_STATUS  generatorState       = eELECTROgetGeneratorStatus();
  ELECTRO_STATUS  mainsState           = eELECTROgetMainsStatus();
  ENGINE_COMMAND  engineCmd            = ENGINE_CMD_NONE;
  ELECTRO_COMMAND electroCmd           = ELECTRO_CMD_NONE;
  SYSTEM_EVENT    interiorEvent        = { .type = EVENT_NONE, .action = ACTION_NONE };
  SYSTEM_EVENT    inputEvent           = { .type = EVENT_NONE, .action = ACTION_NONE };
  FPI_EVENT       inputFpiEvent        = { .level = FPI_LEVEL_LOW, .function = FPI_FUN_NONE, .action = FPI_ACT_NONE, .message = NULL };
  uint32_t        inputKeyboardCommand = HMI_CMD_NONE;

  uint8_t  imulator = 0U;

  for (;;)
  {
    if ( imulator == 0U )
    {
      inputKeyboardCommand = HMI_CMD_START;
      imulator = 1U;
    }
    /*------------------------------------ GET BLOCKS STATUS -------------------------------------*/
    engineState    = eENGINEgetEngineStatus();
    generatorState = eELECTROgetGeneratorStatus();
    mainsState     = eELECTROgetMainsStatus();
    /*-------------------------------------- KEYBOARD INPUT --------------------------------------*/
    //if ( xTaskNotifyWait( pdFALSE, pdTRUE, &inputKeyboardCommand, KEY_NOTIFY_WAIT_DELAY ) == pdPASS )
    if ( 1 > 0 )
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
      inputKeyboardCommand = HMI_CMD_NONE;
    }
    /*-------------------------------------- DIGITAL INPUTS -------------------------------------*/
    if ( xQueueReceive( pFPIgetQueue(), &inputFpiEvent, 0U ) == pdPASS )
    {
      switch ( inputFpiEvent.function )
      {
        /*----------------------------- Пользовательская -----------------------------*/
        case FPI_FUN_USER:
          break;
        /*------------------------- Сброс аварийного сигнала -------------------------*/
        case FPI_FUN_ALARM_RESET:
          if ( ( controller.state    == CONTROLLER_STATUS_ALARM ) &&
               ( inputFpiEvent.level == FPI_LEVEL_HIGH ) )
          {
            controller.state = CONTROLLER_STATUS_IDLE;
            engineCmd = ENGINE_CMD_RESET_TO_IDLE;
            xQueueSend( pENGINEgetCommandQueue(), &engineCmd, portMAX_DELAY );
          }
          break;
        /*---- Запрет автоматического останова при восстановлении параметров сети ----*/
        case FPI_FUN_BAN_AUTO_SHUTDOWN:
          if ( inputFpiEvent.level == FPI_LEVEL_HIGH )
          {
            controller.banAutoShutdown = 1U;
          }
          else
          {
            controller.banAutoShutdown = 0U;
          }
          break;
        /*---------------------- Запрет автоматического запуска ----------------------*/
        case FPI_FUN_BAN_AUTO_START:
          if ( inputFpiEvent.level == FPI_LEVEL_HIGH )
          {
            controller.banAutoStart = 1U;
          }
          else
          {
            controller.banAutoStart = 0U;
          }
          break;
        /*------------------------ Запрет нагрузки генератора ------------------------*/
        case FPI_FUN_BAN_GEN_LOAD:
          if ( inputFpiEvent.level == FPI_LEVEL_HIGH )
          {
            controller.banGenLoad = 1U;
          }
          else
          {
            controller.banGenLoad = 0U;
          }
          break;
        /*----------------------- Дистанционный запуск, останов ----------------------*/
        case FPI_FUN_REMOTE_START:
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
        /*---------------------- Высокая температура двигателя -----------------------*/
        case FPI_FUN_HIGHT_ENGINE_TEMP:
          if ( inputFpiEvent.level     == FPI_LEVEL_HIGH )
          {
            interiorEvent.type   = EVENT_ENGINE_HIGHT_TEMP;
            interiorEvent.action = ACTION_EMERGENCY_STOP;
            xQueueSend( pLOGICgetEventQueue(), &interiorEvent, portMAX_DELAY );
          }
          break;
        /*----------------------- Сигнал низкого уровня топлива ----------------------*/
        case FPI_FUN_LOW_FUEL:
          if ( inputFpiEvent.level     == FPI_LEVEL_HIGH )
          {
            interiorEvent.type   = EVENT_FUEL_LOW_LEVEL;
            interiorEvent.action = ACTION_EMERGENCY_STOP;
            xQueueSend( pLOGICgetEventQueue(), &interiorEvent, portMAX_DELAY );
          }
          break;
        /*-------------------------- Датчик давления масла ---------------------------*/
        case FPI_FUN_OIL_LOW_PRESSURE:
          if ( inputFpiEvent.level     == FPI_LEVEL_HIGH )
          {
            interiorEvent.type   = EVENT_OIL_LOW_PRESSURE;
            interiorEvent.action = ACTION_EMERGENCY_STOP;
            xQueueSend( pLOGICgetEventQueue(), &interiorEvent, portMAX_DELAY );
          }
          break;
        /*-------------------------- Работа на холостом ходу -------------------------*/
        case FPI_FUN_IDLING:
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
    /*---------------------------------- CONTROLLER PROCESSING ----------------------------------*/
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




/*
 * status.c
 *
 *  Created on: 22 февр. 2021 г.
 *      Author: 79110
 */
/*--------------------------------- Includes ---------------------------------*/
#include "status.h"
#include "engine.h"
#include "controller.h"
/*-------------------------------- Structures --------------------------------*/
static DEVICE_INFO deviceInfo = { 0U };
/*--------------------------------- Constant ---------------------------------*/
static const char* deviceStatusDic[DEVICE_STATUS_NUMBER] =
{
  "Загрузка...",        /* 00 */
  "Готов к запуску",    /* 01 */
  "Дистанционный пуск", /* 02 */
  "Предпрогрев",        /* 03 */
  "Работа стартера",    /* 04 */
  "Пауза стартера",     /* 05 */
  "Возбуждение",        /* 06 */
  "Прогрев Х.Х.",       /* 07 */
  "Прогрев",            /* 08 */
  "В работе",           /* 09 */
  "Охлаждение",         /* 10 */
  "Охлаждение Х.Х.",    /* 11 */
  "Останов",            /* 12 */
  "Аварийный останов",  /* 13 */
  "Запрет пуска"        /* 14 */
};
/*-------------------------------- Variables ---------------------------------*/
static DEVICE_STATUS deviceStatus = DEVICE_STATUS_IDLE;
/*-------------------------------- External ----------------------------------*/
/*-------------------------------- Functions ---------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/
PERMISSION eSTATUSisTimer ( DEVICE_STATUS status )
{
  PERMISSION res = PERMISSION_ENABLE;
  if ( ( status == DEVICE_STATUS_IDLE           ) ||
       ( status == DEVICE_STATUS_READY_TO_START ) ||
       ( status == DEVICE_STATUS_WORKING        ) ||
       ( status == DEVICE_STATUS_EMERGENCY_STOP ) ||
       ( status == DEVICE_STATUS_BAN_START      ) )
  {
    res = PERMISSION_DISABLE;
  }
  return res;
}

void vTT ( void )
{
  STARTER_STATUS   starter  = STARTER_IDLE;
  PLAN_STOP_STATUS planStop = STOP_IDLE;
  ENGINE_STATUS    engine   = eENGINEgetEngineStatus();

}
/*----------------------------------------------------------------------------*/
/*----------------------- PABLIC ---------------------------------------------*/
/*----------------------------------------------------------------------------*/
const char* uSTATUSgetString ( DEVICE_STATUS status )
{
  return &deviceStatusDic[status][0U];
}
/*----------------------------------------------------------------------------*/
DEVICE_INFO vSTATUSget ( void )
{
  return deviceInfo;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

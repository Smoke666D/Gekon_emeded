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
/*--------------------------------- Constant ---------------------------------*/
static const uint16_t deviceStatusDic[DEVICE_STATUS_NUMBER][DEVICE_STATUS_LENGTH] =
{
 /*________________*/
  {' '}, /* 00 */
  {' '}, /* 01 */
  {' '}, /* 02 */
  {' '}, /* 03 */
  {' '}, /* 04 */
  {' '}, /* 05 */
  {' '}, /* 06 */
  {' '}, /* 07 */
  {' '}, /* 08 */
  {' '}, /* 09 */
  {' '}, /* 10 */
  {' '}, /* 11 */
  {' '}, /* 12 */
  {' '}, /* 13 */
  {' '} /* 14 */
};
/*-------------------------------- Variables ---------------------------------*/
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
/*----------------------------------------------------------------------------*/
/*----------------------- PABLIC ---------------------------------------------*/
/*----------------------------------------------------------------------------*/
uint16_t* uSTATUSgetString ( DEVICE_STATUS status )
{
  return &deviceStatusDic[status][0U];
}

void vSTATUSget ( DEVICE_STATUS* status, PERMISSION* timer, fix16_t time )
{
  STARTER_STATUS   starter  = STARTER_IDLE;
  PLAN_STOP_STATUS planStop = STOP_IDLE;
  ENGINE_STATUS    engine   = eENGINEgetEngineStatus();


  *timer = eSTATUSisTimer( *status );
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

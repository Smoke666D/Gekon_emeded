/*
 * fpo.c
 *
 *  Created on: 12 июн. 2020 г.
 *      Author: photo_Mickey
 */
/*--------------------------------- Includes ---------------------------------*/
#include "fpo.h"
#include "stm32f2xx_hal_gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "config.h"
#include "dataProces.h"
#include "common.h"
/*-------------------------------- Structures --------------------------------*/
static FPO     fpos[FPO_NUMBER]         = { 0U };
static FPO_DIS fpos_dis[FPO_DIS_NUMBER] = { 0U };
/*--------------------------------- Constant ---------------------------------*/
const uint8_t eFPOfuctionList[FPO_FUNCTION_NUMBER] =
{
  FPO_FUN_NONE,                   /* 0 */
  FPO_FUN_DPS_READY,              /* 1 */
  FPO_FUN_READY_TO_START,         /* 2 */
  FPO_FUN_GEN_READY,              /* 3 */
  FPO_FUN_ALARM,                  /* 4 */
  FPO_FUN_MAINS_FAIL,             /* 5 */
  FPO_FUN_WARNING,                /* 6 */
  FPO_FUN_TURN_ON_GEN,            /* 7 */
  FPO_FUN_TURN_ON_GEN_IMPULSE,    /* 8 */
  FPO_FUN_TURN_OFF_GEN_IMPULSE,   /* 9 */
  FPO_FUN_TURN_ON_MAINS,          /* 10 */
  FPO_FUN_TURN_ON_MAINS_IMPULSE,  /* 11 */
  FPO_FUN_TURN_OFF_MAINS_IMPULSE, /* 12 */
  FPO_FUN_COOLANT_COOLER,         /* 13 */
  FPO_FUN_COOLANT_HEATER,         /* 14 */
  FPO_FUN_STOP_SOLENOID,          /* 15 */
  FPO_FUN_FUEL_BOOST,             /* 16 */
  FPO_FUN_FUEL_RELAY,             /* 17 */
  FPO_FUN_STARTER_RELAY,          /* 18 */
  FPO_FUN_PREHEAT,                /* 19 */
  FPO_FUN_IDLING                  /* 20 */
};
const char* cFPOfunctionNames[FPO_FUNCTION_NUMBER] =
{
  "NONE",
  "DPS_READY",
  "READY_TO_START",
  "GEN_READY",
  "ALARM",
  "MAINS_FAIL",
  "WARNING",
  "TURN_ON_GEN",
  "TURN_ON_GEN_IMPULSE",
  "TURN_OFF_GEN_IMPULSE",
  "TURN_ON_MAINS",
  "TURN_ON_MAINS_IMPULSE",
  "TURN_OFF_MAINS_IMPULSE",
  "COOLANT_COOLER",
  "COOLANT_HEATER",
  "STOP_SOLENOID",
  "FUEL_BOOST",
  "FUEL_RELAY",
  "STARTER_RELAY",
  "PREHEAT",
  "IDLING"
};
const char* cFPOnames[FPO_NUMBER] =
{
  "A",
  "B",
  "C",
  "D",
  "E",
  "F"
};
/*-------------------------------- Variables ---------------------------------*/
static FPO* dpsReadyFPO     = NULL;  /* 1 */
static FPO* readyToStartFPO = NULL;  /* 2 */
static FPO* genReadyFPO     = NULL;  /* 3 */
static FPO* alarmFPO        = NULL;  /* 4 */
static FPO* mainsFailFPO    = NULL;  /* 5 */
static FPO* warningFPO      = NULL;  /* 6 */
static FPO* genSwFPO        = NULL;  /* 7 */
static FPO* genImpOnFPO     = NULL;  /* 8 */
static FPO* genImpOffFPO    = NULL;  /* 9  */
static FPO* mainsSwFPO      = NULL;  /* 10 */
static FPO* mainsImpOnFPO   = NULL;  /* 11 */
static FPO* mainsImpOffFPO  = NULL;  /* 12 */
static FPO* coolerFPO       = NULL;  /* 13 */
static FPO* heaterFPO       = NULL;  /* 14 */
static FPO* stopSolenoidFPO = NULL;  /* 15 */
static FPO* boosterFPO      = NULL;  /* 16 */
static FPO* pumpFPO         = NULL;  /* 17 */
static FPO* starterFPO      = NULL;  /* 18 */
static FPO* preheaterFPO    = NULL;  /* 19 */
static FPO* idleFPO         = NULL;  /* 20 */

static uint16_t fpoDataReg      = 0U;
/*-------------------------------- Functions ---------------------------------*/
void vFPOanaliz ( FPO** fpo, FPO_FUNCTION fun );
void vFPOsetRelay ( FPO* fpo, RELAY_STATUS stat );
void vFPOprintSetup ( void );
void vFPOdisSetup ( void );
void vFPOsetupStartup ( void );
/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vFPOanaliz ( FPO** fpo, FPO_FUNCTION fun )
{
  uint8_t i = 0U;

  *fpo = NULL;
  for ( i=0U; i<FPO_NUMBER; i++ )
  {
    if ( fpos[i].function == fun )
    {
      *fpo = &fpos[i];
      break;
    }
  }
  return;
}
/*----------------------------------------------------------------------------*/
void vFPOsetRelay ( FPO* fpo, RELAY_STATUS stat )
{
  GPIO_PinState cmd = GPIO_PIN_RESET;
  if ( fpo != NULL )
  {
    if ( ( ( stat == RELAY_ON  ) && ( fpo->polarity == FPO_POL_NORMAL_OPEN  ) ) ||
         ( ( stat == RELAY_OFF ) && ( fpo->polarity == FPO_POL_NORMAL_CLOSE ) ) )
    {
      fpoDataReg |= fpo->mask;
      cmd         = GPIO_PIN_SET;
      fpo->state  = TRIGGER_SET;
    }
    else
    {
      fpoDataReg &= ~fpo->mask;
      fpo->state  = TRIGGER_IDLE;
    }
    HAL_GPIO_WritePin( fpo->port, fpo->pin, cmd );
  }
  return;
}
/*----------------------------------------------------------------------------*/
void vFPOprintSetup ( void )
{
  uint8_t i = 0U;
  for ( i=0U; i<FPO_NUMBER; i++ )
  {
    vSYSSerial( ">>FPO " );
    vSYSSerial( cFPOnames[i] );
    vSYSSerial( "        : " );
    vSYSSerial( cFPOfunctionNames[ ( uint8_t )( fpos[i].function ) ] );
    vSYSSerial( "\n\r" );
  }
  vSYSSerial( "\n\r" );
  return;
}
/*----------------------------------------------------------------------------*/
void vFPOdisSetup ( void )
{
  uint8_t i = 0U;
  for ( i=0U; i<FPO_DIS_NUMBER; i++ )
  {
    if ( ( fpos[2U * i].function      == FPO_FUN_NONE ) &&
         ( fpos[2U * i + 1U].function == FPO_FUN_NONE ) )
    {
      HAL_GPIO_WritePin( fpos_dis[i].port, fpos_dis[i].pin, GPIO_PIN_RESET );
    }
    else
    {
      HAL_GPIO_WritePin( fpos_dis[i].port, fpos_dis[i].pin, GPIO_PIN_SET );
    }
  }
  return;
}
/*----------------------------------------------------------------------------*/
void vFPOsetupStartup ( void )
{
  uint8_t i = 0U;
  for ( i=0U; i<FPO_NUMBER; i++ )
  {
    if ( fpos[i].polarity == FPO_POL_NORMAL_OPEN )
    {
      HAL_GPIO_WritePin( fpos[i].port, fpos[i].pin, GPIO_PIN_RESET );
    }
    else
    {
      if ( fpos[i].function == FPO_FUN_TURN_ON_MAINS )
      {
        HAL_GPIO_WritePin( fpos[i].port, fpos[i].pin, GPIO_PIN_RESET );
      }
      else
      {
        HAL_GPIO_WritePin( fpos[i].port, fpos[i].pin, GPIO_PIN_SET );
      }
    }
  }
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
uint16_t uFPOgetData ( void )
{
  return fpoDataReg;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eFPOgetState ( uint8_t n )
{
  TRIGGER_STATE res = TRIGGER_IDLE;
  if ( n < FPO_NUMBER )
  {
    res = fpos[n].state;
  }
  return res;
}
/*----------------------------------------------------------------------------*/
uint8_t uFPOisEnable ( FPO_FUNCTION fun )
{
  uint8_t res = 0U;
  uint8_t i   = 0U;
  for ( i=0U; i<FPO_NUMBER; i++ )
  {
    if ( fpos[i].function == fun )
    {
      res = 1U;
      break;
    }
  }
  return res;
}
/*----------------------------------------------------------------------------*/
void vFPOsetDpsReady ( RELAY_STATUS stat )
{
  vFPOsetRelay( dpsReadyFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eFPOgetDpsReady ( void )
{
  return dpsReadyFPO->state;
}
/*----------------------------------------------------------------------------*/
void vFPOsetReadyToStart ( RELAY_STATUS stat )
{
  vFPOsetRelay( readyToStartFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eFPOgetReadyToStart ( void )
{
  return readyToStartFPO->state;
}
/*----------------------------------------------------------------------------*/
void vFPOsetGenReady ( RELAY_STATUS stat )
{
  vFPOsetRelay( genReadyFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eFPOgetGenReady ( void )
{
  return genReadyFPO->state;
}
/*----------------------------------------------------------------------------*/
void vFPOsetAlarm ( RELAY_STATUS stat )
{
  vFPOsetRelay( alarmFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eFPOgetAlarm ( void )
{
  return alarmFPO->state;
}
/*----------------------------------------------------------------------------*/
void vFPOsetMainsFail ( RELAY_STATUS stat )
{
  vFPOsetRelay( mainsFailFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eFPOgetMainsFail ( void )
{
  return mainsFailFPO->state;
}
/*----------------------------------------------------------------------------*/
void vFPOsetWarning ( RELAY_STATUS stat )
{
  vFPOsetRelay( warningFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eFPOgetWarning ( void )
{
  return warningFPO->state;
}
/*----------------------------------------------------------------------------*/
void vFPOsetStarter ( RELAY_STATUS stat )
{
  vFPOsetRelay( starterFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eFPOgetStarter ( void )
{
  return starterFPO->state;
}
/*----------------------------------------------------------------------------*/
void vFPOsetHeater ( RELAY_STATUS stat )
{
  vFPOsetRelay( heaterFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eFPOgetHeater ( void )
{
  return heaterFPO->state;
}
/*----------------------------------------------------------------------------*/
void vFPOsetCooler ( RELAY_STATUS stat )
{
  vFPOsetRelay( coolerFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eFPOgetCooler ( void )
{
  return coolerFPO->state;
}
/*----------------------------------------------------------------------------*/
void vFPOsetPreheater ( RELAY_STATUS stat )
{
  vFPOsetRelay( preheaterFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eFPOgetPreheater ( void )
{
  return preheaterFPO->state;
}
/*----------------------------------------------------------------------------*/
void vFPOsetBooster ( RELAY_STATUS stat )
{
  vFPOsetRelay( boosterFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eFPOgetBooster ( void )
{
  return boosterFPO->state;
}
/*----------------------------------------------------------------------------*/
void vFPOsetPump ( RELAY_STATUS stat )
{
  vFPOsetRelay( pumpFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eFPOgetPump ( void )
{
  return pumpFPO->state;
}
/*----------------------------------------------------------------------------*/
void vFPOsetStopSolenoid ( RELAY_STATUS stat )
{
  vFPOsetRelay( stopSolenoidFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eFPOgeyStopSolenoid ( void )
{
  return stopSolenoidFPO->state;
}
/*----------------------------------------------------------------------------*/
void vFPOsetIdle ( RELAY_STATUS stat )
{
  vFPOsetRelay( idleFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eFPOgeyIdle ( void )
{
  return idleFPO->state;
}
/*----------------------------------------------------------------------------*/
void vFPOsetGenSw ( RELAY_STATUS stat )
{
  vFPOsetRelay( genSwFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eFPOgetGenSw ( void )
{
  return genSwFPO->state;
}
/*----------------------------------------------------------------------------*/
void vFPOsetGenOnImp ( RELAY_STATUS stat )
{
  vFPOsetRelay( genImpOnFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eFPOgetGenOnImp ( void )
{
  return genImpOnFPO->state;
}
/*----------------------------------------------------------------------------*/
void vFPOsetGenOffImp ( RELAY_STATUS stat )
{
  vFPOsetRelay( genImpOffFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eFPOgetGenOffImp ( void )
{
  return genImpOffFPO->state;
}
/*----------------------------------------------------------------------------*/
void vFPOsetMainsSw ( RELAY_STATUS stat )
{
  vFPOsetRelay( mainsSwFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eFPOgetMainsSw ( void )
{
  return mainsSwFPO->state;
}
/*----------------------------------------------------------------------------*/
void vFPOsetMainsOnImp ( RELAY_STATUS stat )
{
  vFPOsetRelay( mainsImpOnFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eFPOgetMainsOnImp ( void )
{
  return mainsImpOnFPO->state;
}
/*----------------------------------------------------------------------------*/
void vFPOsetMainsOffImp ( RELAY_STATUS stat )
{
  vFPOsetRelay( mainsImpOffFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
TRIGGER_STATE eFPOgetMainsOffImp ( void )
{
  return mainsImpOffFPO->state;
}
/*----------------------------------------------------------------------------*/
void vFPOdataInit ( void )
{
  uint8_t i = 0U;
  /* Read parameters form memory */
  fpos[FPO_A].polarity = getBitMap( &doSetup,  DOA_N_O_C_ADR );
  fpos[FPO_B].polarity = getBitMap( &doSetup,  DOB_N_O_C_ADR );
  fpos[FPO_C].polarity = getBitMap( &doSetup,  DOC_N_O_C_ADR );
  fpos[FPO_D].polarity = getBitMap( &doSetup,  DOD_N_O_C_ADR );
  fpos[FPO_E].polarity = getBitMap( &doSetup,  DOE_N_O_C_ADR );
  fpos[FPO_F].polarity = getBitMap( &doSetup,  DOF_N_O_C_ADR );
  fpos[FPO_A].function = getBitMap( &doabType, DOA_TYPE_ADR );
  fpos[FPO_B].function = getBitMap( &doabType, DOB_TYPE_ADR );
  fpos[FPO_C].function = getBitMap( &docdType, DOC_TYPE_ADR );
  fpos[FPO_D].function = getBitMap( &docdType, DOD_TYPE_ADR );
  fpos[FPO_E].function = getBitMap( &doefType, DOE_TYPE_ADR );
  fpos[FPO_F].function = getBitMap( &doefType, DOF_TYPE_ADR );
  /* Setup masks */
  for ( i=0U; i<FPO_NUMBER; i++ )
  {
    fpos[i].mask = ( uint8_t )( 1U << i );
  }
  /* GPIO start conditions */
  vFPOdisSetup();
  vFPOsetupStartup();
  /* System part */
  vFPOanaliz( &dpsReadyFPO,     FPO_FUN_DPS_READY              );
  vFPOanaliz( &readyToStartFPO, FPO_FUN_READY_TO_START         );
  vFPOanaliz( &genReadyFPO,     FPO_FUN_GEN_READY              );
  vFPOanaliz( &alarmFPO,        FPO_FUN_ALARM                  );
  vFPOanaliz( &mainsFailFPO,    FPO_FUN_MAINS_FAIL             );
  vFPOanaliz( &warningFPO,      FPO_FUN_WARNING                );
  vFPOanaliz( &starterFPO,      FPO_FUN_STARTER_RELAY          );
  vFPOanaliz( &heaterFPO,       FPO_FUN_COOLANT_HEATER         );
  vFPOanaliz( &coolerFPO,       FPO_FUN_COOLANT_COOLER         );
  vFPOanaliz( &preheaterFPO,    FPO_FUN_PREHEAT                );
  vFPOanaliz( &boosterFPO,      FPO_FUN_FUEL_BOOST             );
  vFPOanaliz( &pumpFPO,         FPO_FUN_FUEL_RELAY             );
  vFPOanaliz( &stopSolenoidFPO, FPO_FUN_STOP_SOLENOID          );
  vFPOanaliz( &idleFPO,         FPO_FUN_IDLING                 );
  vFPOanaliz( &genSwFPO,        FPO_FUN_TURN_ON_GEN            );
  vFPOanaliz( &genImpOnFPO,     FPO_FUN_TURN_ON_GEN_IMPULSE    );
  vFPOanaliz( &genImpOffFPO,    FPO_FUN_TURN_OFF_GEN_IMPULSE   );
  vFPOanaliz( &mainsSwFPO,      FPO_FUN_TURN_ON_MAINS          );
  vFPOanaliz( &mainsImpOnFPO,   FPO_FUN_TURN_ON_MAINS_IMPULSE  );
  vFPOanaliz( &mainsImpOffFPO,  FPO_FUN_TURN_OFF_MAINS_IMPULSE );
  /* Default */
  vFPOsetMainsSw( RELAY_ON );
  vFPOsetGenSw( RELAY_OFF );
  /* Debug data */
  vFPOprintSetup();
  return;
}
/*----------------------------------------------------------------------------*/
void vFPOinit( const FPO_INIT* init )
{
  /* Phisical part */
  fpos[FPO_A].port          = init->outPortA;
  fpos[FPO_A].pin           = init->outPinA;
  fpos[FPO_B].port          = init->outPortB;
  fpos[FPO_B].pin           = init->outPinB;
  fpos_dis[FPO_DIS_AB].port = init->disPortAB;
  fpos_dis[FPO_DIS_AB].pin  = init->disPinAB;
  fpos[FPO_C].port          = init->outPortC;
  fpos[FPO_C].pin           = init->outPinC;
  fpos[FPO_D].port          = init->outPortD;
  fpos[FPO_D].pin           = init->outPinD;
  fpos_dis[FPO_DIS_CD].port = init->disPortCD;
  fpos_dis[FPO_DIS_CD].pin  = init->disPinCD;
  fpos[FPO_E].port          = init->outPortE;
  fpos[FPO_E].pin           = init->outPinE;
  fpos[FPO_F].port          = init->outPortF;
  fpos[FPO_F].pin           = init->outPinF;
  fpos_dis[FPO_DIS_EF].port = init->disPortEF;
  fpos_dis[FPO_DIS_EF].pin  = init->disPinEF;
  /* Data init */
  vFPOdataInit();
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


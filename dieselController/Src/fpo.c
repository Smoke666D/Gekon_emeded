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
/*-------------------------------- Structures --------------------------------*/
static FPO     fpos[FPO_NUMBER]         = { 0U };
static FPO_DIS fpos_dis[FPO_DIS_NUMBER] = { 0U };
/*--------------------------------- Constant ---------------------------------*/
const uint8_t eFPOfuctionList[FPO_FUNCTION_NUMBER] =
{
  FPO_FUN_NONE,
  FPO_FUN_AUTO_MODE,
  FPO_FUN_COMMON_NET_FAIL,
  FPO_FUN_READY_TO_START,
  FPO_FUN_GEN_READY,
  FPO_FUN_ALARM,
  FPO_FUN_DES_FAIL,
  FPO_FUN_WARNING,
  FPO_FUN_TURN_ON_GEN,
  FPO_FUN_TURN_ON_GEN_IMPULSE,
  FPO_FUN_TURN_OFF_GEN_IMPULSE,
  FPO_FUN_TURN_ON_MAINS,
  FPO_FUN_TURN_ON_MAINS_IMPULSE,
  FPO_FUN_TURN_OFF_MAINS_IMPULSE,
  FPO_FUN_COOLANT_COOLER,
  FPO_FUN_COOLANT_HEATER,
  FPO_FUN_STOP_SOLENOID,
  FPO_FUN_FUEL_BOOST,
  FPO_FUN_FUEL_RELAY,
  FPO_FUN_STARTER_RELAY,
  FPO_FUN_PREHEAT,
  FPO_FUN_IDLING
};
/*-------------------------------- Variables ---------------------------------*/
static FPO* starterFPO      = NULL;
static FPO* heaterFPO       = NULL;
static FPO* coolerFPO       = NULL;
static FPO* preheaterFPO    = NULL;
static FPO* boosterFPO      = NULL;
static FPO* pumpFPO         = NULL;
static FPO* stopSolenoidFPO = NULL;
static FPO* idleFPO         = NULL;
static FPO* genSwFPO        = NULL;
static FPO* genImpOnFPO     = NULL;
static FPO* genImpOffFPO    = NULL;
static FPO* mainsSwFPO      = NULL;
static FPO* mainsImpOnFPO   = NULL;
static FPO* mainsImpOffFPO  = NULL;
/*-------------------------------- Functions ---------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vFPOanaliz ( FPO* fpo, FPO_FUNCTION fun )
{
  uint8_t i = 0U;

  for ( i=0U; i<FPO_NUMBER; i++ )
  {
    if ( fpos[i].function == fun )
    {
      fpo = &fpos[i];
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
      cmd = GPIO_PIN_SET;
    }
    HAL_GPIO_WritePin( fpo->port, fpo->pin, cmd );
  }
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
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
void vFPOsetStarter ( RELAY_STATUS stat )
{
  vFPOsetRelay( starterFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
void vFPOsetHeater ( RELAY_STATUS stat )
{
  vFPOsetRelay( heaterFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
void vFPOsetCooler ( RELAY_STATUS stat )
{
  vFPOsetRelay( coolerFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
void vFPOsetPreheater ( RELAY_STATUS stat )
{
  vFPOsetRelay( preheaterFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
void vFPOsetBooster ( RELAY_STATUS stat )
{
  vFPOsetRelay( boosterFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
void vFPOsetPump ( RELAY_STATUS stat )
{
  vFPOsetRelay( pumpFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
void vFPOsetStopSolenoid ( RELAY_STATUS stat )
{
  vFPOsetRelay( stopSolenoidFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
void vFPOsetIdle ( RELAY_STATUS stat )
{
  vFPOsetRelay( idleFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
void vFPOsetGenSw ( RELAY_STATUS stat )
{
  vFPOsetRelay( genSwFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
void vFPOsetGenOnImp ( RELAY_STATUS stat )
{
  vFPOsetRelay( genImpOnFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
void vFPOsetGenOffImp ( RELAY_STATUS stat )
{
  vFPOsetRelay( genImpOffFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
void vFPOsetMainsSw ( RELAY_STATUS stat )
{
  vFPOsetRelay( mainsSwFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
void vFPOsetMainsOnImp ( RELAY_STATUS stat )
{
  vFPOsetRelay( mainsImpOnFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
void vFPOsetMainsOffImp ( RELAY_STATUS stat )
{
  vFPOsetRelay( mainsImpOffFPO, stat );
  return;
}
/*----------------------------------------------------------------------------*/
void vFPOinit( const FPO_INIT* init )
{
  uint8_t i = 0U;
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

  HAL_GPIO_WritePin( init->disPortAB, init->disPinAB, GPIO_PIN_RESET );
  HAL_GPIO_WritePin( init->disPortCD, init->disPinCD, GPIO_PIN_RESET );
  HAL_GPIO_WritePin( init->disPortEF, init->disPinEF, GPIO_PIN_RESET );
  /* Read parameters form memory */
  fpos[FPO_A].polarity = getBitMap( &doSetup,  0U );
  fpos[FPO_B].polarity = getBitMap( &doSetup,  1U );
  fpos[FPO_C].polarity = getBitMap( &doSetup,  2U );
  fpos[FPO_D].polarity = getBitMap( &doSetup,  3U );
  fpos[FPO_E].polarity = getBitMap( &doSetup,  4U );
  fpos[FPO_F].polarity = getBitMap( &doSetup,  5U );

  fpos[FPO_A].function = eFPOfuctionList[ getBitMap( &doabType, 0U ) ];
  fpos[FPO_B].function = eFPOfuctionList[ getBitMap( &doabType, 1U ) ];
  fpos[FPO_C].function = eFPOfuctionList[ getBitMap( &docdType, 0U ) ];
  fpos[FPO_D].function = eFPOfuctionList[ getBitMap( &docdType, 1U ) ];
  fpos[FPO_E].function = eFPOfuctionList[ getBitMap( &doefType, 0U ) ];
  fpos[FPO_F].function = eFPOfuctionList[ getBitMap( &doefType, 1U ) ];

  for ( i=0U; i<FPO_NUMBER; i++ )
  {
    if ( fpos[i].polarity == FPO_POL_NORMAL_OPEN )
    {
      HAL_GPIO_WritePin( fpos[i].port, fpos[i].pin, GPIO_PIN_RESET );
    }
    else
    {
      HAL_GPIO_WritePin( fpos[i].port, fpos[i].pin, GPIO_PIN_SET );
    }
  }
  for ( i=0U; i<FPO_DIS_NUMBER; i++ )
  {
    HAL_GPIO_WritePin( fpos_dis[i].port, fpos[i].pin, GPIO_PIN_SET );
  }
  /* System part */
  vFPOanaliz( starterFPO,      FPO_FUN_STARTER_RELAY          );
  vFPOanaliz( heaterFPO,       FPO_FUN_COOLANT_HEATER         );
  vFPOanaliz( coolerFPO,       FPO_FUN_COOLANT_COOLER         );
  vFPOanaliz( preheaterFPO,    FPO_FUN_PREHEAT                );
  vFPOanaliz( boosterFPO,      FPO_FUN_FUEL_BOOST             );
  vFPOanaliz( pumpFPO,         FPO_FUN_FUEL_RELAY             );
  vFPOanaliz( stopSolenoidFPO, FPO_FUN_STOP_SOLENOID          );
  vFPOanaliz( idleFPO,         FPO_FUN_IDLING                 );
  vFPOanaliz( genSwFPO,        FPO_FUN_TURN_ON_GEN            );
  vFPOanaliz( genImpOnFPO,     FPO_FUN_TURN_ON_GEN_IMPULSE    );
  vFPOanaliz( genImpOffFPO,    FPO_FUN_TURN_OFF_GEN_IMPULSE   );
  vFPOanaliz( mainsSwFPO,      FPO_FUN_TURN_ON_MAINS          );
  vFPOanaliz( mainsImpOnFPO,   FPO_FUN_TURN_ON_MAINS_IMPULSE  );
  vFPOanaliz( mainsImpOffFPO,  FPO_FUN_TURN_OFF_MAINS_IMPULSE );
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


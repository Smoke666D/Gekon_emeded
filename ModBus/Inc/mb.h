/*
 * mb.h
 *
 *  Created on: 13.11.2019
 *      Author: Mikhail.Mikhailov
 */

#ifndef INC_MB_H_
#define INC_MB_H_
/*----------------------- Includes ------------------------------------------------------------------*/
#include "mbuart.h"
#include "mbport.h"
/*----------------------- Define --------------------------------------------------------------------*/
#define MB_DEBUG                0U
#define MAX_FUN_NUM             44U
#define CUSTOM_FUN_START        66U
#define STARTING_ADDRESS_HI     1U
#define STARTING_ADDRESS_LO     2U
#define QUANTITY_OF_OUTPUTS_HI  3U
#define QUANTITY_OF_OUTPUTS_LO  4U
#define OUTPUT_VALUE_HI         3U
#define OUTPUT_VALUE_LO         4U
#define BYTE_COUNT              5U
#define AND_MASK_HI             3U
#define AND_MASK_LO             4U
#define OR_MASK_HI              5U
#define OR_MASK_LO              6U
#define SUB_FUNCTION_HI         1U
#define SUB_FUNCTION_LO         2U
#define SUB_FUNCTION_PARAM      3U
/*----------------------- Enums ---------------------------------------------------------------------*/
typedef struct __packed
{
  uint8_t ListenOnlyMode : 1U;
  uint8_t MasterMode     : 1U;
} MB_STATES_TYPE;
/*----------------------- Functions -----------------------------------------------------------------*/
void         vMBTaskInit( void );
eMBInitState eMBQueueInit(void);
void         vMBStartUpMB( void );
uint8_t      uMBGetRequestAdress( void );
uint8_t      ucMBIfMasterMode( void );
void         vMBSetMasterMode( void );
void         vMBListenOnlyModeEnablae( void );
void         vMBIncTaskTime( void );
uint32_t     uMBGetTaskTime( void );
void         vMBResetTaskTime( void );
/*---------------------------------------------------------------------------------------------------*/
#endif /* INC_MB_H_ */

/*
 * mbregister.h
 *
 *  Created on: 14.11.2019
 *      Author: Mikhail.Mikhailov
 */

#ifndef INC_MBREGISTER_H_
#define INC_MBREGISTER_H_
/*----------------------- Includes ------------------------------------------------------------------*/
#include "mbport.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "outputProcessing.h"
/*----------------------- Enums ---------------------------------------------------------------------*/
typedef enum
{
  MB_DATA_DISCRETE_INPUT,   /* 1 bit,  read only      */
  MB_DATA_COIL,             /* 1 bit,  read and write */
  MB_DATA_INPUT_REGISTER,   /* 16 bit, read only      */
  MB_DATA_HOLDING_REGISTER, /* 16 bit, read and write */
} MB_DATA_TYPE;

typedef enum
{
  MB_DATA_STATUS_OK,
  MB_DATA_STATUS_ACCESS_DENIED,
  MB_DATA_STATUS_ADDRESS_ERROR,
} MB_DATA_STATUS;

typedef enum
{
  ROM_OK,
  ROM_COUNT_ERORR,
  ROM_CRC_ERROR,
} CHECK_ROM_ERROR;

typedef enum
{
  COIL,
  DISCRET_INPUT,
} BIT_DATA_TYPE;

/*----------------------- USER MEMORY SIZE ----------------------------------------------------------*/
#define HOLDING_REGISTER_RAM_COUNT      5U
#define HOLDING_REGISTER_ROM_COUNT      5U
#define REGISTER_DEFAULT_VAL            { 0, 0 }
/*----------------------- MEMORY MAP ----------------------------------------------------------------*/
#define HR_REGISTER_ADR_START           0U
#define HR_RAM_COUNT                    ( HOLDING_REGISTER_RAM_COUNT )
#define HR_ROM_COUNT                    ( HOLDING_REGISTER_ROM_COUNT )
#define HR_REGISTER_COUNT               ( OUTPUT_DATA_REGISTER_NUMBER )
#define HR_ROM_OFF                      ( HR_RAM_COUNT )
#define DI_REGISTER_ADR_START           ( HR_REGISTER_ADR_START + HR_REGISTER_COUNT )
#define DI_RAM_INPUT_COUNT              48U
#define DI_RAM_COUNT                    3U
#define DI_INPUT_COUNT                  ( DI_RAM_COUNT )
#define DI_REG_COUNT                    ( DI_RAM_INPUT_COUNT )
#define DI_REGISTER_COUNT               ( DI_RAM_COUNT )
#define IR_REGISTER_ADR_START           ( DI_REGISTER_ADR_START + DI_REGISTER_COUNT )
#define IR_RAM_COUNT                    3U
#define IR_REGISTER_COUNT               ( IR_RAM_COUNT )
#define CR_REGISTER_ADR_START           ( IR_REGISTER_ADR_START + IR_REGISTER_COUNT )
#define CR_RAM_COILS_COUNT              16U
#define CR_ROM_COILS_COUNT              96U
#define CR_RAM_COUNT                    1U
#define CR_ROM_COUNT                    6U
#define CR_REGISTER_COUNT               ( CR_RAM_COUNT + CR_ROM_COUNT )
#define CR_COILS_COUNT                  ( CR_RAM_COILS_COUNT + CR_ROM_COILS_COUNT )
#define CR_ROM_OFF                      ( HR_RAM_COUNT + DI_RAM_COUNT+IR_RAM_COUNT+CR_RAM_COUNT )
/*----------------------- USER REGISTERS -------------------------------------------------------------*/
#define USER_REGISTER_ADR_START         ( CR_REGISTER_ADR_START + CR_REGISTER_COUNT )
#define USER_RAM_COUNT                  0U
#define USER_ROM_COUNT                  3U
#define USER_REGISTER_COUNT             ( USER_RAM_COUNT + USER_ROM_COUNT )
#define USER_ROM_OFF                    ( HR_RAM_COUNT + DI_RAM_COUNT + IR_RAM_COUNT + CR_RAM_COUNT + USER_RAM_COUNT )
#define USER_REGISTER_BOUND_RATE_ADR    0U
#define USER_REGISTER_PARITY_ADR        1U
#define USER_REGISTER_ADDRESS_ADR       2U
#define HR_REGISTER_OFF                 0U
#define DI_REGISTER_OFF                 ( HR_RAM_COUNT + HR_ROM_COUNT )
#define IR_REGISTER_OFF                 ( DI_REGISTER_OFF + IR_RAM_COUNT + IR_ROM_COUNT )
#define CR_REGISTER_OFF                 ( IR_REGISTER_OFF + CR_RAM_COUNT + CR_ROM_COUNT )
#define USER_REGISTER_OFF               ( CR_REGISTER_OFF + USER_RAM_COUNT + USER_ROM_COUNT+DI_REGISTER_COUNT )
#define REGISTER_COUNT                  ( USER_REGISTER_COUNT + CR_REGISTER_COUNT + IR_REGISTER_COUNT + HR_REGISTER_COUNT + DI_REGISTER_COUNT )
#define ROM_REGISTER_COUNT              ( HR_ROM_COUNT + CR_ROM_COUNT + USER_ROM_COUNT )                                                          /* The number of register to be stored in ROM */
/*-----------------------INTERNAL DRIVER VARIABLES --------------------------------------------------*/
#define ROM_REGISTER_COUNT_ADR         1U              /* The virtual address of two bytes determining the number of virtual ROM registers */
#define ROM_REGISTER_CRC_ADR           0U
#define ROM_REGISTER_DATA_OFS          1U
/*----------------------- Functions -----------------------------------------------------------------*/
MB_DATA_STATUS eMBreadData ( uint8_t adr, uint8_t* data, uint16_t length );
MB_DATA_STATUS eMBwriteData ( uint8_t adr, const uint16_t* data, uint16_t length );
/*-------------- OS ---------------*/
MB_INIT_STATE  eMBeventGroupInit ( void );
EventBits_t    eMBeventSet ( uint8_t adr);
EventBits_t    eMBeventReset ( uint8_t adr);
void           vMBeventFullReset ( void );
void           vMBeventGet ( EventBits_t* uxBits );
/*---------------------------------------------------------------------------------------------------*/
#endif /* INC_MBREGISTER_H_ */

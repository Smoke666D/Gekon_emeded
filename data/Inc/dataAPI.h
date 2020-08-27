/*
 * dataAPI.h
 *
 *  Created on: 27 авг. 2020 г.
 *      Author: 79110
 */

#ifndef INC_DATAAPI_H_
#define INC_DATAAPI_H_
/*----------------------- Includes -------------------------------------*/
#include <freeData.h>
#include "stm32f2xx_hal.h"
#include "EEPROM.h"
#include "config.h"
/*------------------------ Define --------------------------------------*/
#define  SEMAPHORE_TAKE_DELAY  ( ( TickType_t ) 10U )
/*------------------------- Macros -------------------------------------*/

/*-------------------------- ENUM --------------------------------------*/
typedef enum
{
  DATA_API_CMD_WRITE,    /* Save data from external data buffer to the data register */
  DATA_API_CMD_INC,      /* Increment data in the register, the external data buffer is ignored */
  DATA_API_CMD_DEC,      /* Decrement data in the register, the external data buffer is ignored */
  DATA_API_CMD_ESC,      /* Reset changes of data: write storage data from the register to the external data buffer */
  DATA_API_CMD_SAVE,     /* Save all data registers to the EEPROM. The input data is ignore */
  DATA_API_CMD_LOAD,     /* Load all data registers from the EEPROM. The input data is ignore */
} DATA_API_COMMAND;

typedef enum
{
  DATA_API_STAT_OK,            /* All done correct */
  DATA_API_STAT_BUSY,          /* The API is busy with another operation */
  DATA_API_STAT_MIN_ERROR,     /* Data is too small */
  DATA_API_STAT_MAX_ERROR,     /* Data is too big */
  DATA_API_STAT_EEPROM_ERROR,  /* Error during EEPROM write operation */
  DATA_API_STAT_ERROR,         /* Other error */
} DATA_API_STATUS;
/*----------------------- Structures -----------------------------------*/

/*------------------------ Functions -----------------------------------*/
void            vDATAAPIinit ( void );
DATA_API_STATUS eDATAAPIconfigValue ( DATA_API_COMMAND cmd, uint16_t adr, uint16_t* data );
/*----------------------------------------------------------------------*/
#endif /* INC_DATAAPI_H_ */

/*
 * dataAPI.h
 *
 *  Created on: 27 авг. 2020 г.
 *      Author: 79110
 */

#ifndef INC_DATAAPI_H_
#define INC_DATAAPI_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "EEPROM.h"
#include "freeData.h"
#include "config.h"
#include "chart.h"
#include "controllerTypes.h"
/*------------------------ Define --------------------------------------*/
#define  SEMAPHORE_TAKE_DELAY   ( ( TickType_t ) 10U )
#define  NOTIFY_TARGETS_NUMBER  1U
#define  EWA_ERASE_SIZE         8U
/*------------------------- Macros -------------------------------------*/

/*-------------------------- ENUM --------------------------------------*/
typedef enum
{
  DATA_API_MESSAGE_REINIT = 0x55,
} DATA_API_MESSAGE;

typedef enum
{
  DATA_API_CMD_READ,     /* Read data from local register */
  DATA_API_CMD_WRITE,    /* Save data from external data buffer to the data register */
  DATA_API_CMD_INC,      /* Increment data in the register, the external data buffer is ignored */
  DATA_API_CMD_DEC,      /* Decrement data in the register, the external data buffer is ignored */
  DATA_API_CMD_SAVE,     /* Save all data registers to the EEPROM. The input data is ignore */
  DATA_API_CMD_LOAD,     /* Load all data registers from the EEPROM. The input data is ignore */
  DATA_API_CMD_ERASE,    /* Erase memory sector in EEPORM */
  DATA_API_CMD_ADD,      /* Add record to variable size data */
} DATA_API_COMMAND;

typedef enum
{
  DATA_API_STAT_OK,            /* All done correct */
  DATA_API_STAT_BUSY,          /* The API is busy with another operation */
  DATA_API_STAT_INIT_ERROR,    /* Semaphore didn't initiated  */
  DATA_API_STAT_CMD_ERROR,     /* Wrong command error */
  DATA_API_STAT_MIN_ERROR,     /* Data is too small */
  DATA_API_STAT_MAX_ERROR,     /* Data is too big */
  DATA_API_STAT_ADR_ERROR,     /* Wrong address */
  DATA_API_STAT_EEPROM_ERROR,  /* Error during EEPROM write operation */
  DATA_API_STAT_ERROR,         /* Other error */
} DATA_API_STATUS;
/*----------------------- Structures -----------------------------------*/

/*------------------------ Functions -----------------------------------*/
void            vDATAAPIdataInit ( void );
void            vDATAAPIinit ( TaskHandle_t* targets );
DATA_API_STATUS eDATAAPIisInit ( void );
DATA_API_STATUS eDATAAPIconfigValue ( DATA_API_COMMAND cmd, uint16_t adr, uint16_t* data );
DATA_API_STATUS eDATAAPIconfigAtrib ( DATA_API_COMMAND cmd, uint16_t adr, eConfigAttributes* atrib );
DATA_API_STATUS eDATAAPIconfig ( DATA_API_COMMAND cmd, uint16_t adr, uint16_t* value, int8_t* scale, uint16_t* units, eConfigBitMap* bitMap );
DATA_API_STATUS eDATAAPIchart ( DATA_API_COMMAND cmd, uint16_t adr, eChartData* chart );
DATA_API_STATUS eDATAAPIfreeData ( DATA_API_COMMAND cmd, uint16_t adr, uint16_t* data );
DATA_API_STATUS eDATAAPIpassword ( DATA_API_COMMAND cmd, PASSWORD_TYPE* password );
DATA_API_STATUS eDATAAPIewa ( DATA_API_COMMAND cmd, uint32_t adr, uint8_t* data, uint16_t length );
DATA_API_STATUS eDATAAPIlog ( DATA_API_COMMAND cmd, uint16_t adr, LOG_RECORD_TYPE* record );
/*----------------------------------------------------------------------*/
#endif /* INC_DATAAPI_H_ */

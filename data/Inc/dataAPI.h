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
#include "event_groups.h"
#include "EEPROM.h"
#include "freeData.h"
#include "config.h"
#include "chart.h"
#include "controllerTypes.h"
/*------------------------ Define --------------------------------------*/
#define  SEMAPHORE_TAKE_DELAY   ( ( TickType_t ) 1000U )
#define  NOTIFY_TARGETS_NUMBER  5U
#define  EWA_ERASE_SIZE         8U
#define  REWRITE_ALL_EEPROM     0U                     /* Rewrite all data in EEPROM to define at each start. Set it to 0 for normal work */
/*------------------------- Macros -------------------------------------*/

/*-------------------------- ENUM --------------------------------------*/
typedef enum
{
  DATA_API_CMD_READ,      /* 00 Read data from local register                                                  */
  DATA_API_CMD_READ_CASH, /* 01 Read data from the EEPROM first time and from cash next time with same address */
  DATA_API_CMD_WRITE,     /* 02 Save data from external data buffer to the data register                       */
  DATA_API_CMD_INC,       /* 03 Increment data in the register, the external data buffer is ignored            */
  DATA_API_CMD_DEC,       /* 04 Decrement data in the register, the external data buffer is ignored            */
  DATA_API_CMD_SAVE,      /* 05 Save all data registers to the EEPROM. The input data is ignore                */
  DATA_API_CMD_LOAD,      /* 06 Load all data registers from the EEPROM. The input data is ignore              */
  DATA_API_CMD_ERASE,     /* 07 Erase memory sector in EEPORM                                                  */
  DATA_API_CMD_ADD,       /* 08 Add record to variable size data                                               */
  DATA_API_CMD_COUNTER,   /* 09 Read size of data                                                              */
  DATA_API_CMD_NEW        /* 10 Create new record table or file ( in file system )                             */
} DATA_API_COMMAND;

typedef enum
{
  DATA_API_STAT_OK,            /* 0 All done correct                       */
  DATA_API_STAT_BUSY,          /* 1 The API is busy with another operation */
  DATA_API_STAT_INIT_ERROR,    /* 2 Semaphore didn't initiated             */
  DATA_API_STAT_CMD_ERROR,     /* 3 Wrong command error                    */
  DATA_API_STAT_MIN_ERROR,     /* 4 Data is too small                      */
  DATA_API_STAT_MAX_ERROR,     /* 5 Data is too big                        */
  DATA_API_STAT_ADR_ERROR,     /* 6 Wrong address                          */
  DATA_API_STAT_EEPROM_ERROR,  /* 7 Error during EEPROM write operation    */
  DATA_API_STAT_ERROR,         /* 8 Other error                            */
} DATA_API_STATUS;
/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  uint16_t        adr;
  uint16_t        size;
  uint16_t        pointer;
  LOG_RECORD_TYPE record;
} LOG_CASH_TYPE;
/*------------------------- Extern -------------------------------------*/
#if defined ( UNIT_TEST )
  extern volatile LOG_CASH_TYPE logCash;
#endif
/*------------------------ Functions -----------------------------------*/
#if defined ( UNIT_TEST )
  DATA_API_STATUS eDATAAPIlogLoad ( uint16_t adr, LOG_RECORD_TYPE* record );
#endif
EEPROM_STATUS      eDATAAPIdataInit ( void );
void               vDATAAPIinit ( const EEPROM_TYPE* storage );
EEPROM_STATUS      eDATAAPIlogInit ( void );
DATA_API_STATUS    eDATAAPIisInit ( void );
DATA_API_STATUS    eDATAAPIconfigValue ( DATA_API_COMMAND cmd, uint16_t adr, uint16_t* data );
DATA_API_STATUS    eDATAAPIconfigAtrib ( DATA_API_COMMAND cmd, uint16_t adr, eConfigAttributes* atrib );
DATA_API_STATUS    eDATAAPIconfig ( DATA_API_COMMAND cmd, uint16_t adr, uint16_t* value, int8_t* scale, uint16_t* units );
DATA_API_STATUS    eDATAAPIchart ( DATA_API_COMMAND cmd, uint16_t adr, eChartData* chart );
DATA_API_STATUS    eDATAAPIfreeData ( DATA_API_COMMAND cmd, FREE_DATA_ADR adr, uint16_t* data );
DATA_API_STATUS    eDATAAPIpassword ( DATA_API_COMMAND cmd, PASSWORD_TYPE* password );
DATA_API_STATUS    eDATAAPIlog ( DATA_API_COMMAND cmd, uint16_t* adr, LOG_RECORD_TYPE* record );
DATA_API_STATUS    eDATAAPIlogPointer ( DATA_API_COMMAND cmd, uint16_t* pointer );
DATA_API_STATUS    eDATAAPImeasurement ( DATA_API_COMMAND cmd, uint16_t* adr, uint8_t length, uint16_t* data );
void               vDATAAPIincLogSize ( void );
//void               vDATAAPIprintMemoryMap ( void );
//void               vDATAprintSerialNumber ( void );
EventGroupHandle_t xDATAAPIgetEventGroup ( void );
void               vDATAAPIsendEventAll ( DATA_API_REINIT message );
uint8_t            uDATAAPIisInitDone ( void );
void               vDATAAPIinitMac ( const EEPROM_TYPE* mac );
/*----------------------------------------------------------------------*/
#endif /* INC_DATAAPI_H_ */

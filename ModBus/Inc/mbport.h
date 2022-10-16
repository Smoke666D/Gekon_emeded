/*
 * mbport.h
 *
 *  Created on: 13.11.2019
 *      Author: Mikhail.Mikhailov
 */

#ifndef INC_MBPORT_H_
#define INC_MBPORT_H_
/*----------------------- Includes ------------------------------------------------------------------*/
#include "FreeRTOSConfig.h"

/* ---------------------- Platform includes --------------------------------*/
#define MB_ADDRESS_BROADCAST                    0U       /* Modbus broadcast address.        */
#define MB_ADDRESS_MIN                          1U       /* Smallest possible slave address. */
#define MB_ADDRESS_MAX                          247U     /* Biggest possible slave address.  */
#define MB_FUNC_HANDLERS_MAX                    17U
#define MB_FUNC_OTHER_REP_SLAVEID_BUF           32U
#define MB_MAIN_FUNC_NUMBER                     44U
#define MB_CUSTOM_FUNC_NUMBER                   1U
#define MB_FUN_NUM                              ( MB_MAIN_FUNC_NUMBER + MB_CUSTOM_FUNC_NUMBER )
/* ---------------------- Functions enable ---------------------------------*/
#define MB_FUNC_OTHER_REP_SLAVEID_ENABLED       0U
#define MB_FUNC_READ_INPUT_ENABLED              0U
#define MB_FUNC_READ_HOLDING_ENABLED            1U
#define MB_FUNC_WRITE_HOLDING_ENABLED           1U
#define MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED  1U
#define MB_FUNC_READ_COILS_ENABLED              0U
#define MB_FUNC_WRITE_COIL_ENABLED              0U
#define MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED    0U
#define MB_FUNC_READ_DISCRETE_INPUTS_ENABLED    0U
#define MB_FUNC_READWRITE_HOLDING_ENABLED       0U
#define MB_FUNC_MASK_WRITE_REGISTER_ENABLED     0U
#define MB_FUNC_DIAGNOSTICS_ENEBLED             0U
#define MB_FUNC_SET_RS_PARAMETERS_ENEBLED       0U
/*----------------------- RTOS ---------------------------------------------*/
#define OS_MB_RES_QUEUE_SIZE          3U
#define OS_MB_MESSAGE_SIZE            260U
#define OS_MB_MESSAGE_TIGGER_LEVEL    10U
#define OS_MB_BLOCK_TIME              ( pdMS_TO_TICKS( 20U ) )
#if( configUSE_16_BIT_TICKS == 1U )
  #define OS_MB_EVENT_GROUP_SIZE      8
#else
  #define OS_MB_EVENT_GROUP_SIZE      24U
#endif
#define  OS_MB_EVENT_ARRAY_SIZE        ( ( uint8_t )( HR_REGISTER_COUNT / OS_MB_EVENT_GROUP_SIZE ) + 1U )
/*----------------------- Structures ----------------------------------------------------------------*/
typedef enum
{
  MB_FUNC_NONE                         = 0U,
  MB_FUNC_READ_COILS                   = 1U,
  MB_FUNC_READ_DISCRETE_INPUTS         = 2U,
  MB_FUNC_READ_HOLDING_REGISTER        = 3U,
  MB_FUNC_READ_INPUT_REGISTER          = 4U,
  MB_FUNC_WRITE_SINGLE_COIL            = 5U,
  MB_FUNC_WRITE_REGISTER               = 6U,
  MB_FUNC_DIAG_READ_EXCEPTION          = 7U,
  MB_FUNC_DIAGNOSTICS                  = 8U,
  MB_FUNC_DIAG_GET_COM_EVENT_CNT       = 11U,
  MB_FUNC_DIAG_GET_COM_EVENT_LOG       = 12U,
  MB_FUNC_WRITE_MULTIPLE_COILS         = 15U,
  MB_FUNC_WRITE_MULTIPLE_REGISTERS     = 16U,
  MB_FUNC_OTHER_REPORT_SLAVEID         = 17U,
  MB_FUNC_MASK_WRITE_REGISTER          = 22U,
  MB_FUNC_READWRITE_MULTIPLE_REGISTERS = 23U,
  MB_FUNC_SET_COMMUNICATION_PARAMETRS  = 25U,
  MB_FUNC_SET_RS_PARAMETERS            = 66U,
  MB_FUNC_ERROR                        = 128U,
} MB_FUNC;

typedef enum
{
  MB_SUB_FUNC_SET_BOUNDRATE,
  MB_SUB_FUNC_SET_PARITY,
  MB_SUB_FUNC_SET_NET_ADR,
  MB_SUB_FUNC_RETURN_QUERY_DATA,
  MB_SUB_FUNC_RESTART_COMUNICATION_OPTION,
  MB_SUB_FUNC_RETURN_DIAGNOSTIC_REGISTER,
  MB_SUB_FUNC_FORCE_LISTEN_ONLY_MODE,
} MB_SUB_FUNC;

typedef enum
{
  MB_EX_NONE                 = 0x00U,
  MB_EX_ILLEGAL_FUNCTION     = 0x01U,
  MB_EX_ILLEGAL_DATA_ADDRESS = 0x02U,
  MB_EX_ILLEGAL_DATA_VALUE   = 0x03U,
  MB_EX_SLAVE_DEVICE_FAILURE = 0x04U,
  MB_EX_ACKNOWLEDGE          = 0x05U,
  MB_EX_SLAVE_BUSY           = 0x06U,
  MB_EX_MEMORY_PARITY_ERROR  = 0x08U,
  MB_EX_GATEWAY_PATH_FAILED  = 0x0AU,
  MB_EX_GATEWAY_TGT_FAILED   = 0x0BU,
  MB_EX_REINIT               = 0xFFU,
} MB_EXCEPTION;

typedef enum
{
  EB_INIT_ERROR,
  EB_INIT_OK
} MB_INIT_STATE;

typedef enum
{
  MB_ROM_INIT_FAIL,
  MB_ROM_INIT_OK,
} MB_ROM_INIT_STATE;

typedef enum
{
  MB_ROM_FAIL,
  MB_ROM_DONE,
} MB_ROM_PROC_TYPE;

typedef enum
{
  RAM_NON_WR,
  ROM_WR,
} eMBRAMwrType;

typedef MB_EXCEPTION( *pxMBFunctionHandler ) ( uint8_t* pucFrame, uint8_t* pusLength );

typedef struct
{
  pxMBFunctionHandler pxHandler;
} MBFunctionHandlerTable;
/*----------------------- Define --------------------------------------------------------------------*/
//#define UART_RECIEVE_BUF_SIZE   2U
#define MB_PDU_SIZE_MAX         253U         /* Maximum size of a PDU. */
#define MB_PDU_SIZE_MIN         1U           /* Function Code */
#define MB_PDU_FUNC_OFF         0U           /* Offset of function code in PDU. */
#define MB_PDU_LENGTH_OFF       1U
#define MB_PDU_DATA_OFF         2U           /* Offset for response data in PDU. */
#define MB_SER_PDU_SIZE_MIN     4U           /* Minimum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_MAX     256U         /* Maximum size of a Modbus RTU frame. */
//#define MB_SER_PDU_SIZE_CRC     2U           /* Size of CRC field in PDU. */
//#define MB_SER_PDU_ADDR_OFF     0U           /* Offset of slave address in Ser-PDU. */
//#define MB_SER_PDU_PDU_OFF      1U           /* Offset of Modbus-PDU in Ser-PDU. */
//#define F_CPU                   16000000UL
/*---------------------------------------------------------------------------------------------------*/
#endif /* INC_MBPORT_H_ */

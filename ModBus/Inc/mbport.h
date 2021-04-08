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
/*----------------------- Define --------------------------------------------------------------------*/
#define         ARM_base
#define					IWDG_ON					0U

#ifdef ARM_base
  #include      "stdint.h"                      // Определение ARM типов
  #define       BOOL            uint8_t;
  #define       UCHAR           uint8_t
  #define       CHAR            int8_t
  #define       USHORT          uint16_t
  #define       SHORT           int16_t
  #define       ULONG           uint32_t
  #define       LONG            int32_t
#else                                           // Определение стандартных типов
  typedef       char            BOOL;
  typedef       unsigned char   UCHAR;
  typedef       char            CHAR;
  typedef       unsigned short  USHORT;
  typedef       short           SHORT;
  typedef       unsigned long   ULONG;
  typedef       long            LONG;
#endif
/* ---------------------- Platform includes --------------------------------*/
#define MB_ADDRESS_BROADCAST                    0U       /*! Modbus broadcast address. */
#define MB_ADDRESS_MIN                          1U       /*! Smallest possible slave address. */
#define MB_ADDRESS_MAX                          247U     /*! Biggest possible slave address. */
#define MB_FUNC_NONE                            0U
#define MB_FUNC_READ_COILS                      1U
#define MB_FUNC_READ_DISCRETE_INPUTS            2U
#define MB_FUNC_WRITE_SINGLE_COIL               5U
#define MB_FUNC_WRITE_MULTIPLE_COILS            15U
#define MB_FUNC_READ_HOLDING_REGISTER           3U			/* Read Holding Registers function */
#define MB_FUNC_READ_INPUT_REGISTER             4U
#define MB_FUNC_WRITE_REGISTER                  6U
#define MB_FUNC_WRITE_MULTIPLE_REGISTERS        16U
#define MB_FUNC_MASK_WRITE_REGISTER             22U
#define MB_FUNC_READWRITE_MULTIPLE_REGISTERS    23U
#define MB_FUNC_DIAG_READ_EXCEPTION             7U
#define MB_FUNC_DIAGNOSTICS                     8U
#define MB_FUNC_DIAG_GET_COM_EVENT_CNT          11U
#define MB_FUNC_DIAG_GET_COM_EVENT_LOG          12U
#define MB_FUNC_OTHER_REPORT_SLAVEID            17U
#define MB_FUNC_SET_COMMUNICATION_PARAMETRS     25U
#define MB_FUNC_ERROR                           128U
#define MB_FUNC_SET_RS_PARAMETRS                66U

//Codes SUB-FUNCTION
#define RETURN_QUERY_DATA                       0U
#define RESTART_COMUNICATION_OPTION             1U
#define RETURN_DIAGNOSTIC_REGISTER              2U
#define FORCE_LISTEN_ONLY_MODE                  4U
#define SET_BOUNDRATE                           0U
#define SET_PARITY	                            1U
#define	SET_NET_ADR															2U
/* ---------------------- Type definitions ---------------------------------*/
#define MB_FUNC_HANDLERS_MAX                    17U
#define MB_FUNC_OTHER_REP_SLAVEID_BUF           32U

#define MB_FUNC_OTHER_REP_SLAVEID_ENABLED       0U       	/*! \brief If the <em>Report Slave ID</em> function should be enabled. */
#define MB_FUNC_READ_INPUT_ENABLED              1U       	/*! \brief If the <em>Read Input Registers</em> function should be enabled. */
#define MB_FUNC_READ_HOLDING_ENABLED            1U       	/*! \brief If the <em>Read Holding Registers</em> function should be enabled. */
#define MB_FUNC_WRITE_HOLDING_ENABLED           1U       	/*! \brief If the <em>Write Single Register</em> function should be enabled. */
#define MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED  1U       	/*! \brief If the <em>Write Multiple registers</em> function should be enabled. */
#define MB_FUNC_READ_COILS_ENABLED              1U       	/*! \brief If the <em>Read Coils</em> function should be enabled. */
#define MB_FUNC_WRITE_COIL_ENABLED              1U       	/*! \brief If the <em>Write Coils</em> function should be enabled. */
#define MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED    1U       	/*! \brief If the <em>Write Multiple Coils</em> function should be enabled. */
#define MB_FUNC_READ_DISCRETE_INPUTS_ENABLED    1U       	/*! \brief If the <em>Read Discrete Inputs</em> function should be enabled. */
#define MB_FUNC_READWRITE_HOLDING_ENABLED       0U       	/*! \brief If the <em>Read/Write Multiple Registers</em> function should be enabled. */
#define MB_FUNC_MASK_WRITE_REGISTER_ENABLED     1U
#define MB_FUNC_DIAGNOSTICS_ENEBLED             0U
#define	MB_FUNC_SET_RS_PARAMETERS_ENEBLED				1U				/* Custom function to change RS parameters, as baud rate  */
/*----------------------- Custom function code & number ---------------------------------------------*/
#define	MB_FUNC_SET_RS_PARAMETERS								0U
#define	MB_FUNC_SET_RS_PARAMETERS_CODE					( CUSTOM_FUN_START + MB_FUNC_SET_RS_PARAMETERS )
#define	MB_FUNC_SET_RS_PARAMETERS_NUMBER				( MAX_FUN_NUM + MB_FUNC_SET_RS_PARAMETERS )
/*----------------------- RTOS ---------------------------------------------*/
#define	OS_MB_RES_QUEUE_SIZE					3U
#define	OS_MB_TASK_PRIORITY						( ( osPriority_t ) osPriorityNormal )
#define	OS_MB_TASK_STACK_SIZE					512U
#define	OS_MB_MESSAGE_SIZE 						260U
#define	OS_MB_MESSAGE_TIGGER_LEVEL		10U
#define	OS_MB_BLOCK_TIME							( pdMS_TO_TICKS( 20U ) )
#if( configUSE_16_BIT_TICKS == 1U )
	#define OS_MB_EVENT_GROUP_SIZE			8
#else
	#define OS_MB_EVENT_GROUP_SIZE			24
#endif
#define	OS_MB_EVENT_ARRAY_SIZE				( ( UCHAR )( HR_REGISTER_COUNT / OS_MB_EVENT_GROUP_SIZE ) + 1U )
/*----------------------- Structures ----------------------------------------------------------------*/
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
} eMBException;

typedef enum
{
	EBInit_ERROR,
	EBInit_OK
} eMBInitState;

typedef enum
{
  ROM_INIT_FAIL,
  ROM_INIT_OK,
} eMBROMInitType;

typedef enum
{
  ROM_FAIL,
  ROM_DONE,
} eMBROMprocType;

typedef enum
{
  RAM_NON_WR,
  ROM_WR,
} eMBRAMwrType;

typedef eMBException(*pxMBFunctionHandler) (UCHAR* pucFrame, UCHAR* pusLength);

typedef struct
{
    pxMBFunctionHandler pxHandler;
} MBFunctionHandlerTable;
/*----------------------- Define --------------------------------------------------------------------*/
#define UART_RECIEVE_BUF_SIZE   2U
#define MB_PDU_SIZE_MAX         253U     		/* Maximum size of a PDU. */
#define MB_PDU_SIZE_MIN         1U       		/* Function Code */
#define MB_PDU_FUNC_OFF         0U       		/* Offset of function code in PDU. */
#define MB_PDU_DATA_OFF         1U       		/* Offset for response data in PDU. */
#define MB_SER_PDU_SIZE_MIN     4U       		/* Minimum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_MAX     256U     		/* Maximum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_CRC     2U       		/* Size of CRC field in PDU. */
#define MB_SER_PDU_ADDR_OFF     0U       		/* Offset of slave address in Ser-PDU. */
#define MB_SER_PDU_PDU_OFF			1U					/* Offset of Modbus-PDU in Ser-PDU. */
#define F_CPU		        				16000000UL

#ifndef TRUE
  #define TRUE            			1U
#endif

#ifndef FALSE
  #define FALSE           			0U
#endif
/*---------------------------------------------------------------------------------------------------*/
#endif /* INC_MBPORT_H_ */

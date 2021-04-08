/*
 * mbrtu.h
 *
 *  Created on: 13.11.2019
 *      Author: Mikhail.Mikhailov
 */

#ifndef INC_MBRTU_H_
#define INC_MBRTU_H_
/*----------------------- Includes ------------------------------------------------------------------*/
#include "mbport.h"
#include "stddef.h"
/*----------------------- Define --------------------------------------------------------------------*/
#define MB_SER_PDU_SIZE_MIN				4U				/* Minimum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_MAX				256U			/* Maximum size of a Modbus RTU frame. */
#define MB_SER_PDU_SIZE_CRC				2U				/* Size of CRC field in PDU. */
#define MB_SER_PDU_ADDR_OFF				0U				/* Offset of slave address in Ser-PDU. */
#define MB_SER_PDU_PDU_OFF				1U				/* Offset of Modbus-PDU in Ser-PDU. */
#define RECIVE_FRAME_BUFFER_MAX		3U

#define	MB_MES_LENGTH_ADR					0U
#define	MB_MES_NET_ADR						1U
/*----------------------- Structures ----------------------------------------------------------------*/
typedef struct
{
  unsigned char BusCommunicationErrorCount;
  unsigned char BusMessageCount;
  unsigned char SlaveMessageCount;
  unsigned char SlaveNoResponseCount;
  unsigned char SlaveExceptionErrorCount;
} MODBUSCOUNTERS;

typedef enum
{
	NO_VALID_FRAME,     /*!< Receiver is in initial state. */
  VALID_FRAME,        /*!< Receiver is in idle state. */
  MB_ENOERR,
  MB_EIO,
} MBErrorCode;

typedef struct
{
		UCHAR length;
		UCHAR pdu[ MB_SER_PDU_SIZE_MAX + 2U ];
} PDUstructType;
/*----------------------- Functions -----------------------------------------------------------------*/
eMBInitState		eMBReceiveMessageInit( void );
UCHAR 					ucMBCheckMessage( void );
size_t 					vMBReceiveMessage( UCHAR* input, ULONG size );

void 						vMBCounterErrorInc( void );
UCHAR           ucMBParseATURecieverFrameBuffer( UCHAR NOK_counter );
MBErrorCode     eMBRTUReceive( UCHAR* frame, UCHAR* pucRcvAddress, UCHAR* pucFrame, UCHAR* pusLength );
void            vMBRTUSend( UCHAR* pucRcvAddress, UCHAR* pucFrame, UCHAR* pusLength );
MBErrorCode     eMBMasterRTUSend( UCHAR* pucRcvAddress, UCHAR* pucFrame, UCHAR* pusLength );
/*---------------------------------------------------------------------------------------------------*/
#endif /* INC_MBRTU_H_ */

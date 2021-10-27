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
#define MB_SER_PDU_SIZE_CRC        2U        /* Size of CRC field in PDU. */
#define MB_SER_PDU_ADDR_OFF        0U        /* Offset of slave address in Ser-PDU. */
#define MB_SER_PDU_PDU_OFF         1U        /* Offset of Modbus-PDU in Ser-PDU. */
#define RECIVE_FRAME_BUFFER_MAX    3U
#define MB_MES_LENGTH_ADR          0U
#define MB_MES_NET_ADR             1U
/*----------------------- Enums ---------------------------------------------------------------------*/
typedef enum
{
  NO_VALID_FRAME,     /*!< Receiver is in initial state. */
  VALID_FRAME,        /*!< Receiver is in idle state. */
  MB_ENOERR,
  MB_EIO,
} MBErrorCode;
/*----------------------- Structures ----------------------------------------------------------------*/
typedef struct
{
  unsigned char BusCommunicationErrorCount;
  unsigned char BusMessageCount;
  unsigned char SlaveMessageCount;
  unsigned char SlaveNoResponseCount;
  unsigned char SlaveExceptionErrorCount;
} MODBUSCOUNTERS;

typedef struct __packed
{
  uint8_t data[MB_PDU_SIZE_MAX];
  MB_FUNC func;
} MB_PDU_TYPE;

typedef struct
{
  uint8_t     length;
  uint8_t     adr;
  MB_PDU_TYPE pdu;
} MB_FRAME_TYPE;
/*----------------------- Functions -----------------------------------------------------------------*/
MB_INIT_STATE eMBreceiveMessageInit( void );
uint8_t       uMBcheckMessage ( void );
size_t        uMBreceiveMessage ( uint8_t* input, uint32_t size );
void          vMBcounterErrorInc ( void );
uint8_t       uMBparseATURecieverFrameBuffer ( uint8_t nokCounter );
MBErrorCode   eMBRTUreceive ( uint8_t* blob, MB_FRAME_TYPE* frame );
void          vMBRTUsend (  MB_FRAME_TYPE* frame  );
MBErrorCode   eMBmasterRTUSend (  MB_FRAME_TYPE* frame  );
/*---------------------------------------------------------------------------------------------------*/
#endif /* INC_MBRTU_H_ */

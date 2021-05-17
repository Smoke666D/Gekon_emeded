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
  MB_PDU_FUNC_READ_COIL_STATUS           = 0x01U,
  MB_PDU_FUNC_READ_READ_INPUT_STATUS     = 0x02U,
  MB_PDU_FUNC_READ_HOLDING_REGISTERS     = 0x03U,
  MB_PDU_FUNC_READ_INPUT_REGISTERS       = 0x04U,
  MB_PDU_FUNC_FORCE_SINGLE_COIL          = 0x05U,
  MB_PDU_FUNC_PRESET_SINGLE_REGITER      = 0x06U,
  MB_PDU_FUNC_DIAGNOSTICS                = 0x08U,
  MB_PDU_FUNC_GET_COM_EVENT_COUNTER      = 0x0BU,
  MB_PDU_FUNC_GET_COM_EVENT_LOG          = 0x0CU,
  MB_PDU_FUNC_MASK_WRITE_REGISTER        = 0x16U,
  MB_PDU_FUNC_READWRITE_HOLDING          = 0x17U,
  MB_PDU_FUNC_READ_FIFO_QUEUE            = 0x18U,
  MB_PDU_FUNC_READ_DEVICE_IDENTIFICATION = 0x2BU,
  MB_PDU_FUNC_SET_RS_PARAMETERS          = MB_FUNC_SET_RS_PARAMETERS_CODE,
  MB_PDU_FUNC_FORCE_MULTIPLE_COILS       = 0x0FU,
  MB_PDU_FUNC_PRESET_MULTIPLEREGISTERS   = 0x10U,
} MB_PDU_FUNC;

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
  uint8_t     data[MB_PDU_SIZE_MAX];
  MB_PDU_FUNC func;
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
uint8_t       uMBparseATURecieverFrameBuffer ( uint8_t NOK_counter );
MBErrorCode   eMBRTUreceive ( uint8_t* blob, MB_FRAME_TYPE* frame );
void          vMBRTUsend (  MB_FRAME_TYPE* frame  );
MBErrorCode   eMBmasterRTUSend (  MB_FRAME_TYPE* frame  );
/*---------------------------------------------------------------------------------------------------*/
#endif /* INC_MBRTU_H_ */

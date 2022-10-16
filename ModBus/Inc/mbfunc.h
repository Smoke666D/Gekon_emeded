/*
 * mbfunc.h
 *
 *  Created on: 13.11.2019
 *      Author: Mikhail.Mikhailov
 */

#ifndef INC_MBFUNC_H_
#define INC_MBFUNC_H_
/*----------------------- Includes ------------------------------------------------------------------*/
#include "mbport.h"
/*----------------------- Functions -----------------------------------------------------------------*/
MB_EXCEPTION eMBNoFunction ( uint8_t* pucFrame, uint8_t* len );

#if ( MB_FUNC_SET_RS_PARAMETERS_ENEBLED )
  MB_EXCEPTION eMBFuncSetRSParameters ( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_OTHER_REP_SLAVEID_BUF )
  MB_EXCEPTION eMBFuncReportSlaveID ( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_READ_INPUT_ENABLED )
  MB_EXCEPTION eMBFuncReadInputRegister ( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_READ_HOLDING_ENABLED )
  MB_EXCEPTION eMBFuncReadHoldingRegister( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_WRITE_HOLDING_ENABLED )
  MB_EXCEPTION eMBFuncWriteHoldingRegister( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED )
  MB_EXCEPTION eMBFuncWriteMultipleHoldingRegister( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_READ_COILS_ENABLED )
  MB_EXCEPTION eMBFuncReadCoils( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_WRITE_COIL_ENABLED )
  MB_EXCEPTION eMBFuncWriteCoil( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED )
  MB_EXCEPTION eMBFuncWriteMultipleCoils( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_READ_DISCRETE_INPUTS_ENABLED )
  MB_EXCEPTION eMBFuncReadDiscreteInputs( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_READWRITE_HOLDING_ENABLED )
  MB_EXCEPTION eMBFuncReadWriteMultipleHoldingRegister( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_MASK_WRITE_REGISTER_ENABLED )
  MB_EXCEPTION eMBFuncMaskWriteRegister( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_DIAGNOSTICS_ENEBLED )
  MB_EXCEPTION eMBFuncDiagnostics( uint8_t* pucFrame, uint8_t* len );
#endif
  /*---------------------------------------------------------------------------------------------------*/
#endif /* INC_MBFUNC_H_ */

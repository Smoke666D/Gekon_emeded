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
eMBException    eMBNoFunction ( uint8_t* pucFrame, uint8_t* len );

#if ( MB_FUNC_SET_RS_PARAMETERS_ENEBLED )
  eMBException eMBFuncSetRSParameters ( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_OTHER_REP_SLAVEID_BUF )
  eMBException eMBFuncReportSlaveID ( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_READ_INPUT_ENABLED )
  eMBException eMBFuncReadInputRegister ( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_READ_HOLDING_ENABLED )
  eMBException eMBFuncReadHoldingRegister( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_WRITE_HOLDING_ENABLED )
  eMBException eMBFuncWriteHoldingRegister( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED )
  eMBException eMBFuncWriteMultipleHoldingRegister( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_READ_COILS_ENABLED )
  eMBException eMBFuncReadCoils( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_WRITE_COIL_ENABLED )
  eMBException eMBFuncWriteCoil( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED )
  eMBException eMBFuncWriteMultipleCoils( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_READ_DISCRETE_INPUTS_ENABLED )
  eMBException eMBFuncReadDiscreteInputs( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_READWRITE_HOLDING_ENABLED )
  eMBException eMBFuncReadWriteMultipleHoldingRegister( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_MASK_WRITE_REGISTER_ENABLED )
  eMBException eMBFuncMaskWriteRegister( uint8_t* pucFrame, uint8_t* len );
#endif

#if ( MB_FUNC_DIAGNOSTICS_ENEBLED )
  eMBException eMBFuncDiagnostics( uint8_t* pucFrame, uint8_t* len );
#endif
  /*---------------------------------------------------------------------------------------------------*/
#endif /* INC_MBFUNC_H_ */

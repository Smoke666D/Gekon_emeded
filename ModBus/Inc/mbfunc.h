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
eMBException    eMBNoFunction( UCHAR* pucFrame, UCHAR* usLen );

#if ( MB_FUNC_SET_RS_PARAMETERS_ENEBLED )
	eMBException    eMBFuncSetRSParameters( UCHAR* pucFrame, UCHAR* usLen );
#endif

#if ( MB_FUNC_OTHER_REP_SLAVEID_BUF )
  eMBException eMBFuncReportSlaveID( UCHAR* pucFrame, UCHAR* usLen );
#endif

#if ( MB_FUNC_READ_INPUT_ENABLED )
  eMBException    eMBFuncReadInputRegister( UCHAR* pucFrame, UCHAR* usLen );
#endif

#if ( MB_FUNC_READ_HOLDING_ENABLED )
  eMBException    eMBFuncReadHoldingRegister( UCHAR* pucFrame, UCHAR* usLen );
#endif

#if ( MB_FUNC_WRITE_HOLDING_ENABLED )
  eMBException    eMBFuncWriteHoldingRegister( UCHAR* pucFrame, UCHAR* usLen );
#endif

#if ( MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED )
  eMBException    eMBFuncWriteMultipleHoldingRegister( UCHAR* pucFrame, UCHAR* usLen );
#endif

#if ( MB_FUNC_READ_COILS_ENABLED )
  eMBException    eMBFuncReadCoils( UCHAR* pucFrame, UCHAR* usLen );
#endif

#if ( MB_FUNC_WRITE_COIL_ENABLED )
  eMBException    eMBFuncWriteCoil( UCHAR* pucFrame, UCHAR* usLen );
#endif

#if ( MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED )
  eMBException    eMBFuncWriteMultipleCoils( UCHAR* pucFrame, UCHAR* usLen );
#endif

#if ( MB_FUNC_READ_DISCRETE_INPUTS_ENABLED )
  eMBException    eMBFuncReadDiscreteInputs( UCHAR* pucFrame, UCHAR* usLen );
#endif

#if ( MB_FUNC_READWRITE_HOLDING_ENABLED )
  eMBException    eMBFuncReadWriteMultipleHoldingRegister( UCHAR* pucFrame, UCHAR* usLen );
#endif

#if ( MB_FUNC_MASK_WRITE_REGISTER_ENABLED )
  eMBException eMBFuncMaskWriteRegister( UCHAR* pucFrame, UCHAR* usLen );
#endif

#if ( MB_FUNC_DIAGNOSTICS_ENEBLED )
  eMBException eMBFuncDiagnostics( UCHAR* pucFrame, UCHAR* usLen );
#endif
  /*---------------------------------------------------------------------------------------------------*/
#endif /* INC_MBFUNC_H_ */

/*
 * mbmemory.h
 *
 *  Created on: 14.11.2019
 *      Author: mikhail.Mikhailov
 */

#ifndef INC_MBMEMORY_H_
#define INC_MBMEMORY_H_
/*----------------------- Includes ------------------------------------------------------------------*/
#include "mbport.h"
/*----------------------- Define --------------------------------------------------------------------*/

/*----------------------- Functions -----------------------------------------------------------------*/
eMBInitState 	ucMBSetMemory( uint8_t page, uint32_t bank );
USHORT				usMBReadUint16( USHORT page, USHORT adr );
UCHAR 				eMBPageWrite( UCHAR page, UCHAR* data, USHORT count );
/*---------------------------------------------------------------------------------------------------*/
#endif /* INC_MBMEMORY_H_ */

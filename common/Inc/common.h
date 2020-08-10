/*
 * common.h
 *
 *  Created on: 19 мар. 2020 г.
 *      Author: photo_Mickey
 */

#ifndef INC_COMMON_H_
#define INC_COMMON_H_
/*----------------------- Includes -------------------------------------*/
#include    "stm32f2xx_hal.h"
/*------------------------ Define --------------------------------------*/
#define     UNIQUE_ID_ADR               0x1FFF7A10U
/*------------------------ Macros --------------------------------------*/
#define     BUILD_BUG_OR_ZERO( e )      sizeof( struct { int: -!!( e ); } )					/* Test value - logical error will be in compiling  */
#define     REVERSE_BYTE( b )           ( ( ( b << 7U ) & 0x80U ) | ( ( b << 5U ) & 0x40U ) | ( ( b << 3U ) & 0x20U ) | ( ( b << 1U ) & 0x10U ) | ( ( b >> 1U ) & 0x08U ) | ( ( b >> 3U ) & 0x04U ) | ( ( b >> 5U ) & 0x02U ) | ( ( b >> 7U ) & 0x01U ) )
#define     GET_UNIQUE_ID0              ( *( uint32_t* )( UNIQUE_ID_ADR + 0x00U ) )
#define     GET_UNIQUE_ID1              ( *( uint32_t* )( UNIQUE_ID_ADR + 0x04U ) )
#define     GET_UNIQUE_ID2              ( *( uint32_t* )( UNIQUE_ID_ADR + 0x08U ) )
/*----------------------- Structures -----------------------------------*/

/*----------------------- Functions ------------------------------------*/
void    vSYSInitSerial( UART_HandleTypeDef* uart );
void    vSYSSerial( char* msg );
void    vSYSgetUniqueID32( uint32_t* id );
void    vSYSgetUniqueID16( uint16_t* id );
uint8_t uEncodeURI( const uint16_t* input, uint8_t length, char* output );
void    vDecodeURI( const char* input, uint16_t* output, uint8_t length );
/*----------------------------------------------------------------------*/
#endif /* INC_COMMON_H_ */

/*
 * version.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: photo_Mickey
 */

#ifndef INC_VERSION_H_
#define INC_VERSION_H_
/*------------------------ Define --------------------------------------*/
#define SOFTWARE_VERSION	01750U
#define HARDWARE_VERSION	00004U
#define DEVICE_ID         00001U

#define BOOTLOADER_VERSION_ADR  0x8007FFCU

#define HARDWARE_VERSION_MAJOR  2
#define HARDWARE_VERSION_MINOR  1
#define HARDWARE_VERSION_PATCH  0

#if ( HARDWARE_VERSION_MAJOR > 255U )
#error( "Major version too big" )
#endif
#if ( HARDWARE_VERSION_MINOR > 255U )
#error( "Minor version too big" )
#endif
#if ( HARDWARE_VERSION_ASSEMB > 255U )
#error( "Fix version too big" )
#endif

#define FIRMWARE_VERSION_MAJOR  1
#define FIRMWARE_VERSION_MINOR  1
#define FIRMWARE_VERSION_PATCH  0

#if ( FIRMWARE_VERSION_MAJOR > 255U )
#error( "Major version too big" )
#endif
#if ( FIRMWARE_VERSION_MINOR > 255U )
#error( "Minor version too big" )
#endif
#if ( FIRMWARE_VERSION_FIX > 255U )
#error( "Fix version too big" )
#endif

#define DEVICE_ID_GECCO_1       "Геко-1"
/*----------------------------------------------------------------------*/
#endif /* INC_VERSION_H_ */

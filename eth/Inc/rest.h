/*
 * rest.h
 *
 *  Created on: 19 февр. 2020 г.
 *      Author: mikhail.mikhailov
 */

#ifndef INC_REST_H_
#define INC_REST_H_
/*----------------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "config.h"
#include "chart.h"
/*------------------------------ Enum ----------------------------------------*/
typedef enum
{
	REST_OK,
	REST_MESSAGE_FORMAT_ERROR,
	REST_RECORD_LOST_ERROR,
	REST_RECORD_FORMAT_ERROR,
	REST_RECORD_NO_DATA_ERROR,
	REST_RECORD_LENGTH_ERROR,
	REST_RECORD_COPY_ERROR,
	REST_RECORD_ARRAY_FORMAT_ERROR,
	REST_RECORD_ARRAY_SIZE_ERROR,
} REST_ERROR;

typedef enum
{
	REST_LAST_RECORD,
	REST_CONT_RECORD
} RESTrecordPos;

typedef enum
{
	REST_CONFIGS       = 0x00,
	REST_CHARTS        = 0x01,
	REST_REQUEST_ERROR = 0xFF,
} REST_REQUEST;

typedef enum
{
	REST_NO_ADR,
	REST_YES_ADR,
} REST_ADDRESS;
/*------------------------------ Default -------------------------------------*/
#define	QUOTES_ANCII            0x22U

#define	REST_BUFFER_SIZE        700U
#define	REST_DIGIT_BUFFER_SIZE  6U

#define	REST_REQUEST_NUMBER     2U
#define	REST_REQUEST_CONFIGS    "configs"
#define	REST_REQUEST_CHARTS     "charts"
/*------------------------------ Extern --------------------------------------*/
extern const char 	*restRequeststr[REST_REQUEST_NUMBER];
/*----------------------------- Functions ------------------------------------*/
uint32_t     uRESTmakeConfig( eConfigReg* reg, char* output );
uint32_t     uRESTmakeChart( char* output, eChartData* chart );
REST_ERROR   eRESTparsingConfig( char* input, eConfigReg* reg );
REST_ADDRESS eRESTgetRequest( char* path, REST_REQUEST* request, uint16_t* adr );
REST_ERROR   eRESTparsingChart( char* input, eChartData* chart );
/*---------------------------------------------------------------------------------------------------*/

#endif /* INC_REST_H_ */

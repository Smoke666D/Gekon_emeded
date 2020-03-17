/*
 * chart.h
 *
 *  Created on: 16 мар. 2020 г.
 *      Author: mikhail.mikhailov
 */
#ifndef INC_CHART_H_
#define INC_CHART_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "fix16.h"
/*------------------------ Define --------------------------------------*/
#define	CHART_LABEL_LENGTH			3U
#define	CHART_CHART_MAX_SIZE		128U
/*----------------------- Structures -----------------------------------*/
typedef struct
{
	fix16_t x;
	fix16_t y;
} CHART_DOT;

typedef struct
{
	fix16_t 	xmin;
	fix16_t 	xmax;
	fix16_t 	ymin;
	fix16_t 	ymax;
	char			xlabel[CHART_LABEL_LENGTH];
	char			ylabel[CHART_LABEL_LENGTH];
	uint16_t	size;
	CHART_DOT	dots[CHART_CHART_MAX_SIZE];
} CHART_DATA;
/*----------------------------------------------------------------------*/
#endif /* INC_CHART_H_ */
